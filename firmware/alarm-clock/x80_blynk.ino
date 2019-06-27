Task blynkTask(50, TASK_FOREVER, &blynkConnectCb, &scheduler);
WidgetRTC blynkRtc;

BLYNK_CONNECTED() {
  logInfo("Blynk connected");
  Blynk.syncAll();
  blynkRtc.begin();
  taskLogic.enableIfNot();
}

BLYNK_DISCONNECTED() {
  logInfo("Blynk disconnected");
}

BLYNK_WRITE(BLYNK_PIN_TIMEINPUT) {
  TimeInputParam t(param);
  if (!t.hasStartTime()) {
    logInfo("Blynk: no start time");
    return;
  }
  
  logInfo("Blynk: got new alarm time");
  uint8_t dow = 0;
  for (int i = 1; i <= 7; i++) {
    if (t.isWeekdaySelected(i)) {
      dow |= 1 << (i - 1);
    }
  }
  logicSetAlarmTime(t.getStartHour(), t.getStartMinute(), t.getStartSecond(), dow);
}

BLYNK_WRITE(BLYNK_PIN_ALARMENABLE) {
  logValue("Alarm enable: ", param.asInt());
  logicAlarmOn = param.asInt() ? 1 : 0;
}

BLYNK_WRITE(BLYNK_PIN_COUNTDOWN_MIN) {
  int t = param.asInt();
  logValue("Set countdown min: ", t);
  if (t >= 0 && t <= 255) {
    logicCntdnMin = t;
  }
}

BLYNK_WRITE(BLYNK_PIN_COUNTDOWN_SEC) {
  int t = param.asInt();
  logValue("Set countdown sec: ", t);
  if (t >= 0 && t < 60) {
    logicCntdnSec = t;
  }
}

BLYNK_WRITE(BLYNK_PIN_COUNTDOWN_START) {
  if (param.asInt() == 1) {
    if (logicMode == LOGIC_MODE_CD_RUN || logicMode == LOGIC_MODE_CD_FINISH) {
      logInfo("Stopping countdown");
      logicSetMode(LOGIC_MODE_TIME);
    } else {
      logInfo("Starting countdown");
      logicSetMode(LOGIC_MODE_CD_RUN);
    }
  }
}

bool blynkIsConfigured() {
  return strlen(config.blynkToken) != 0;
}

void blynkConnectCb() {
  if (WiFi.isConnected() && !Blynk.connected()) {
    logInfo("Blynk connect attempt");
    Blynk.connect();
  }

  if (Blynk.connected()) {
    blynkTask.setInterval(50);
    blynkTask.setCallback(&blynkCheckCb);
  }
}

void blynkCheckCb() {
  if (Blynk.connected()) {
    Blynk.run();
  } else {
    blynkTask.setInterval(RECONNECT_INTERVAL);
    blynkTask.setCallback(&blynkConnectCb);
  }
}

void blynkSetup() {
  if (blynkIsConfigured()) {
    Blynk.config(config.blynkToken);
    blynkTask.enable();
  }
}
