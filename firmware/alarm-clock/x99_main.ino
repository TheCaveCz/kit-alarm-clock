
void otaSetup() {
  String hn = WIFI_AP_NAME;
  hn += chipId;
  ArduinoOTA.setHostname(hn.c_str());
  ArduinoOTA.onStart([]() {
    dispShowStatic(DISP_OTA);
    logInfo("OTA starting");
  });
  ArduinoOTA.onEnd([]() {
    logInfo("OTA finished");
  });
  ArduinoOTA.begin();
}

void setup() {
  Serial.begin(115200);
  logInfo("\n\nStarting");
  logValue("chipId: ", chipId);

  beepSetup();
  dispSetup();
  configSetup();
  otaSetup(); // so we can OTA when config portal is up
  logicSetup();
  wifiSetup(); // will block on config portal if not configured
  blynkSetup();

  logInfo("Setup complete");
}

void loop() {
  ArduinoOTA.handle();
  logicTick();
  scheduler.execute();

  if (wifiGotIpFlag) {
    wifiGotIpFlag = false;
    if (!taskLogic.isEnabled()) {
      dispShowStatic(DISP_NOTIME);
    }
  }
}
