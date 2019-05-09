Task beepTask(20, TASK_FOREVER, &beepCb, &scheduler, false, NULL, &beepDisableCb);

uint16_t beepOn;
uint16_t beepOff;

void beepCb() {
  uint8_t on = beepTask.getRunCounter() & 1;
  digitalWrite(PIN_BUZZ, on);
  beepTask.setInterval(on ? beepOn : beepOff);
}

void beepDisableCb() {
  digitalWrite(PIN_BUZZ, LOW);
}

void beep(const uint16_t onTime, const uint16_t offTime, const uint8_t rep) {
  beepOn = onTime;
  beepOff = offTime;

  beepTask.setInterval(onTime);
  beepTask.setIterations(rep == 255 ? TASK_FOREVER : rep * 2);
  beepTask.restart();
}

void beepStop() {
  beepTask.disable();
}

void beepSetup() {
  pinMode(PIN_BUZZ, OUTPUT);
}
