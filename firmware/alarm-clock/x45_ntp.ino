Task taskNtpSync(5000L, TASK_FOREVER, &ntpSyncStartCb, &scheduler);
Task taskNtpTick(1000L, TASK_FOREVER, &ntpTickCb, &scheduler, true);


WiFiUDP ntpUdp;
IPAddress ntpServerIP;

#define TIME_UNKNOWN 0xffffffffUL
uint32_t currentTime;
int32_t currentTzOffset;
uint32_t lastTimeSync;
uint32_t lastTickTime;
//int32_t tzOffset;

// Europe/Prague
uint32_t tzData[] = {
  0UL, 3600,
  1521939600UL, 7200,
  1540688400UL, 3600,
  1553994000UL, 7200,
  1572138000UL, 3600,
  1585443600UL, 7200,
  1603587600UL, 3600,
  1616893200UL, 7200,
  1635642000UL, 3600,
  1648342800UL, 7200,
  1667091600UL, 3600,
  1679792400UL, 7200,
  1698541200UL, 3600,
  1711846800UL, 7200,
  1729990800UL, 3600,
  1743296400UL, 7200,
  1761440400UL, 3600,
  1774746000UL, 7200,
  1792890000UL, 3600,
  1806195600UL, 7200,
  1824944400UL, 3600,
  0xffffffffUL, 0,
};

#define NTP_PACKET_SIZE 48
byte ntpBuffer[NTP_PACKET_SIZE];


void ntpSetup() {
  currentTime = TIME_UNKNOWN;
  currentTzOffset = 0;
  lastTimeSync = 0;
  lastTickTime = 0;
  ntpUdp.begin(2390);
}


int32_t ntpCalcTzOffset() {
  int8_t cnt = 0;
  while (tzData[cnt] < currentTime) {
    cnt += 2;
  }
  return tzData[cnt - 1];
}

void ntpTickCb() {
  if (currentTime == TIME_UNKNOWN) return;

  currentTime++;
  lastTickTime = millis();
  currentTzOffset = ntpCalcTzOffset();
}

uint32_t ntpGetWalltime() {
  if (currentTime == TIME_UNKNOWN) return TIME_UNKNOWN;

  return currentTime + currentTzOffset;
}

void ntpSyncStartCb() {
  logInfo("Start NTP sync");
  if (!WiFi.hostByName("tik.cesnet.cz", ntpServerIP)) {
    logInfo(" - failed to resolve NTP IP");
    return;
  }

  // set all bytes in the buffer to 0
  memset(ntpBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  ntpBuffer[0] = 0b11100011;   // LI, Version, Mode
  ntpBuffer[1] = 0;     // Stratum, or type of clock
  ntpBuffer[2] = 6;     // Polling Interval
  ntpBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  ntpBuffer[12]  = 49;
  ntpBuffer[13]  = 0x4E;
  ntpBuffer[14]  = 49;
  ntpBuffer[15]  = 52;

  ntpUdp.beginPacket(ntpServerIP, 123);
  ntpUdp.write(ntpBuffer, NTP_PACKET_SIZE);
  ntpUdp.endPacket();

  logInfo(" - NTP request sent");

  taskNtpSync.setCallback(&ntpSyncFinishCb);
  taskNtpSync.setInterval(1000L);
  taskNtpSync.enableDelayed(0); // clears run counter
}

void ntpSyncFinishCb() {
  int success = ntpUdp.parsePacket() >= NTP_PACKET_SIZE;
  if (success) {
    ntpUdp.read(ntpBuffer, NTP_PACKET_SIZE);

    unsigned long highWord = word(ntpBuffer[40], ntpBuffer[41]);
    unsigned long lowWord = word(ntpBuffer[42], ntpBuffer[43]);
    // Unix time starts on Jan 1 1970. NTP starts in 1900, difference in seconds is 2208988800:
    currentTime = ((highWord << 16) | lowWord) - 2208988800UL;
    currentTzOffset = ntpCalcTzOffset();
    lastTimeSync = millis();
    logValue("Got NTP time: ", currentTime);
  }

  // wait for ntp response five seconds and if this fails, wait one minute and try sync again
  if (success || taskNtpSync.getRunCounter() >= 5) {
    logValue("NTP finished: ", taskNtpSync.getRunCounter());
    taskNtpSync.setCallback(&ntpSyncStartCb);
    taskNtpSync.setInterval(600000L);
    taskNtpSync.enableDelayed(0); // clears run counter
  }
}
