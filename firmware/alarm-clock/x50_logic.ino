Task taskLogic(100, TASK_FOREVER, &logicCb, &scheduler);
OneButton logicButtonMode(PIN_BTN1, true);
OneButton logicButtonSet(PIN_BTN2, true);


#define LOGIC_MODE_TIME 0
#define LOGIC_MODE_DATE 1
#define LOGIC_MODE_COUNTDOWN 2
#define LOGIC_MODE_MAX 3

#define LOGIC_MODE_SET_AL_MINS 3
#define LOGIC_MODE_SET_AL_HRS 4
#define LOGIC_MODE_SET_AL_ON 5

#define LOGIC_MODE_SET_CD_SEC 6
#define LOGIC_MODE_SET_CD_MIN 7
#define LOGIC_MODE_CD_RUN 8
#define LOGIC_MODE_CD_FINISH 9

#define LOGIC_MODE_TIME_ALTRIG 10


uint8_t logicMode;
uint8_t logicCntdnMin;
uint8_t logicCntdnSec;
uint32_t logicCntdnStart;

void logicTick() {
  logicButtonMode.tick();
  logicButtonSet.tick();
}

void logicSetup() {
  logicCntdnStart = 0;
  logicCntdnMin = 3;
  logicCntdnSec = 0;
  logicMode = LOGIC_MODE_TIME;

  logicButtonMode.setDebounceTicks(30);
  logicButtonMode.setPressTicks(5000);
  logicButtonMode.attachClick(logicHandleMode);

  logicButtonSet.setDebounceTicks(30);
  logicButtonSet.setPressTicks(1000);
  logicButtonSet.attachClick(logicHandleSet);

  //logicButtonMode.attachLongPressStop(wifiResetAndRestart);
}

void logicSetMode(const uint8_t mode) {
  logicMode = mode;
  if (mode == LOGIC_MODE_CD_FINISH) {
    beep(50, 200, 255);
  } else if (mode == LOGIC_MODE_CD_RUN) {
    logicCntdnStart = millis();
  } else if (mode == LOGIC_MODE_TIME_ALTRIG) {
    beep(100, 300, 255);
  }
  logicEvaluate();
}

void logicHandleMode() {
  switch (logicMode) {
    case LOGIC_MODE_TIME:
    case LOGIC_MODE_DATE:
    case LOGIC_MODE_COUNTDOWN:
      logicMode++;
      if (logicMode >= LOGIC_MODE_MAX) logicMode = 0;
      logicEvaluate();
      break;

    case LOGIC_MODE_SET_AL_MINS:
      logicSetMode(LOGIC_MODE_SET_AL_HRS);
      break;
    case LOGIC_MODE_SET_AL_HRS:
      logicSetMode(LOGIC_MODE_SET_AL_ON);
      break;
    case LOGIC_MODE_SET_AL_ON:
      configWrite();
      logicSetMode(LOGIC_MODE_TIME);
      break;

    case LOGIC_MODE_SET_CD_SEC:
      logicSetMode(LOGIC_MODE_SET_CD_MIN);
      break;
    case LOGIC_MODE_SET_CD_MIN:
      logicSetMode(LOGIC_MODE_CD_RUN);
      break;
    case LOGIC_MODE_CD_FINISH:
      logicSetMode(LOGIC_MODE_COUNTDOWN);
      break;

    case LOGIC_MODE_TIME_ALTRIG:
      logicSetMode(LOGIC_MODE_TIME);
      break;
    default:
      return;
  }
  beep(10, 0, 1);
}

void logicHandleSet() {
  switch (logicMode) {
    case LOGIC_MODE_TIME:
      logicSetMode(LOGIC_MODE_SET_AL_MINS);
      break;

    case LOGIC_MODE_SET_AL_ON:
      config.alarmOn = config.alarmOn ? 0 : 1;
      break;

    case LOGIC_MODE_SET_AL_MINS:
      config.alarmMin++;
      if (config.alarmMin >= 60) config.alarmMin = 0;
      logicEvaluate();
      break;

    case LOGIC_MODE_SET_AL_HRS:
      config.alarmHr++;
      if (config.alarmHr >= 24) config.alarmHr = 0;
      logicEvaluate();
      break;

    case LOGIC_MODE_COUNTDOWN:
      logicSetMode(LOGIC_MODE_SET_CD_SEC);
      break;

    case LOGIC_MODE_SET_CD_SEC:
      logicCntdnSec++;
      if (logicCntdnSec >= 60) logicCntdnSec = 0;
      logicEvaluate();
      break;

    case LOGIC_MODE_SET_CD_MIN:
      logicCntdnMin++;
      if (logicCntdnMin >= 30) logicCntdnMin = 0;
      logicEvaluate();
      break;

    case LOGIC_MODE_CD_FINISH:
      logicSetMode(LOGIC_MODE_COUNTDOWN);
      break;

    case LOGIC_MODE_TIME_ALTRIG:
      logicSetMode(LOGIC_MODE_TIME);
      break;

    default:
      return;
  }
  beep(10, 0, 1);
}

void logicEvaluate() {
  uint32_t t = ntpGetWalltime();
  if (t == TIME_UNKNOWN) {
    dispShowStatic(DISP_NOTIME);
    return;
  }

  tmElements_t tt;
  switch (logicMode) {
    case LOGIC_MODE_TIME:
      breakTime(t, tt);
      if (config.alarmOn && config.alarmMin == tt.Minute && config.alarmHr == tt.Hour && tt.Second == 0) {
        logicSetMode(LOGIC_MODE_TIME_ALTRIG);
        return;
      }
      dispTime = tt.Hour * 100 + tt.Minute;
      dispDots = (millis() - lastTickTime < 500UL ? 0b0100 : 0) | (config.alarmOn ? 0b0001 : 0);
      dispMask = 0xf;
      dispShowBuffer();
      break;
    case LOGIC_MODE_TIME_ALTRIG:
      breakTime(t, tt);
      dispTime = tt.Hour * 100 + tt.Minute;
      dispDots = (millis() - lastTickTime < 500UL ? 0b0100 : 0) | (config.alarmOn ? 0b0001 : 0);
      dispMask = taskLogic.getRunCounter() & 1 ? 0xf : 0;
      dispShowBuffer();
      break;
    case LOGIC_MODE_DATE:
      breakTime(t, tt);
      dispTime = tt.Day * 100 + tt.Month;
      dispDots = 0b1001;
      dispMask = 0xf;
      dispShowBuffer();
      break;
    case LOGIC_MODE_SET_AL_ON:
      dispShowStatic2(DISP_ALARM, dispDigit(config.alarmOn ? 1 : 0));
      break;
    case LOGIC_MODE_SET_AL_MINS:
      dispTime = config.alarmHr * 100 + config.alarmMin;
      dispDots = 0b0100;
      dispMask = 0b1100 | (taskLogic.getRunCounter() & 1 ? 0b11 : 0);
      dispShowBuffer();
      break;
    case LOGIC_MODE_SET_AL_HRS:
      dispTime = config.alarmHr * 100 + config.alarmMin;
      dispDots = 0b0100;
      dispMask = 0b11 | (taskLogic.getRunCounter() & 1 ? 0b1100 : 0);
      dispShowBuffer();
      break;
    case LOGIC_MODE_COUNTDOWN:
      dispTime = logicCntdnMin * 100 + logicCntdnSec;
      dispDots = 0b1000;
      dispMask = logicCntdnMin < 10 ? 0b0111 : 0xf;
      dispShowBuffer();
      break;
    case LOGIC_MODE_SET_CD_SEC:
      dispTime = logicCntdnMin * 100 + logicCntdnSec;
      dispDots = 0b1000;
      dispMask = 0b0100 | (taskLogic.getRunCounter() & 1 ? 0b11 : 0) | (logicCntdnMin >= 10 ? 0b1000 : 0);
      dispShowBuffer();
      break;
    case LOGIC_MODE_SET_CD_MIN:
      dispTime = logicCntdnMin * 100 + logicCntdnSec;
      dispDots = 0b1000;
      dispMask = 0b11 | (taskLogic.getRunCounter() & 1 ? (0b100 | (logicCntdnMin >= 10 ? 0b1000 : 0)) : 0);
      dispShowBuffer();
      break;
    case LOGIC_MODE_CD_RUN: {
        uint32_t delta = millis() - logicCntdnStart;
        uint32_t cntdn = (logicCntdnMin * 60UL + logicCntdnSec) * 1000UL;
        if (delta > cntdn) {
          logicSetMode(LOGIC_MODE_CD_FINISH);
          return;
        }
        cntdn -= delta;
        bool b = cntdn % 1000UL < 500UL;
        cntdn /= 1000;
        uint8_t min = cntdn / 60;
        uint8_t sec = cntdn % 60;
        dispTime = min * 100 + sec;
        dispDots = b ? 0 : 0b1000;
        dispMask = min < 10 ? 0b0111 : 0xf;
        dispShowBuffer();
        break;
      }
    case LOGIC_MODE_CD_FINISH:
      dispTime = 0;
      dispDots = 0b1000;
      dispMask = taskLogic.getRunCounter() & 1 ? 0b111 : 0;
      dispShowBuffer();
      break;
  }
}

void logicCb() {
  logicEvaluate();

  if (logicButtonSet.isLongPressed() && taskLogic.getRunCounter() % 2 == 0) {
    logicHandleSet();
  }
}
