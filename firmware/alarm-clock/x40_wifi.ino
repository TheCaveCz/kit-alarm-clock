
WiFiEventHandler wifiIpHandler;
bool wifiShouldSaveConfig;
bool wifiGotIpFlag;

void wifiSetup() {
  wifiGotIpFlag = false;
  if (WiFi.getAutoConnect()) WiFi.setAutoConnect(false);
  if (WiFi.getMode() != WIFI_STA) WiFi.mode(WIFI_STA);

  logInfo("Connecting...");
  dispShowStatic(DISP_CON);

  wifiIpHandler = WiFi.onStationModeGotIP([&](const WiFiEventStationModeGotIP & evt) {
    // this executes when module reconnects and gets IP from DHCP
    // can be called multiple times
    wifiGotIpFlag = true;
    logValue("Got IP: ", evt.ip);
  });

  if (WiFi.SSID() == "") {
    logInfo("No saved credentials");
    dispShowStatic(DISP_AP);

    WiFiManager wifiManager;
    wifiShouldSaveConfig = false;
    wifiManager.setSaveConfigCallback([]() {
      logInfo("Config should be saved");
      wifiShouldSaveConfig = true;
    });


    WiFiManagerParameter tokenParam("blynkToken", "Blynk token", config.blynkToken, 32);
    wifiManager.addParameter(&tokenParam);

    String ap = WIFI_AP_NAME;
    ap += chipId;
    if (!wifiManager.startConfigPortal(ap.c_str())) {
      logInfo("Failed to connect or setup, rebooting...");
      delay(1000);
      ESP.reset();
      while (1) delay(1);
    }

    strcpy(config.blynkToken, tokenParam.getValue());

    if (wifiShouldSaveConfig) {
      configWrite();
    }

  } else if (!WiFi.isConnected()) { // calling WiFi.begin when already connected does all kind of weird stuff
    logValue("Stored SSID: ", WiFi.SSID());
    WiFi.begin();
  }
}

void wifiReset() {
  WiFi.disconnect(true);
  delay(1000);
  ESP.reset();
  while (1) delay(1);
}
