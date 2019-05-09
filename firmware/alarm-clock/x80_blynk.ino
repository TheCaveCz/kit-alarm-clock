Task blynkTask(50, TASK_FOREVER, &blynkConnectCb, &scheduler);


BLYNK_CONNECTED() {
  logInfo("Blynk connected");
  Blynk.syncAll();
}

BLYNK_DISCONNECTED() {
  logInfo("Blynk disconnected");
}

//BLYNK_WRITE(BLYNK_PIN_LIMIT) {
//  logicSetLimit(param.asInt());
//}

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
