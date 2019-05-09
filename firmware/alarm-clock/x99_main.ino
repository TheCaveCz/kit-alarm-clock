
//OneButton resetButton(PIN_BTN, true);

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
  //  resetButton.setDebounceTicks(30);
  //  resetButton.setPressTicks(5000);
  //  resetButton.attachLongPressStop(wifiReset);

  configSetup();
  otaSetup(); // so we can OTA when config portal is up
  ntpSetup();
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
    taskNtpSync.enable();
    taskNtpSync.forceNextIteration();
    taskLogic.enable();
  }



  //  if (resetButton.isLongPressed()) {
  //    ledSetState(LED_ON);
  //  } else if (logicAlarm) {
  //    ledSetState(LED_TWO_BLINKS);
  //  } else if (WiFi.isConnected() && (Blynk.connected() || !blynkIsConfigured())) {
  //    ledSetState(LED_OFF);
  //  } else {
  //    ledSetState(LED_FAST_BLINK);
  //  }
}
