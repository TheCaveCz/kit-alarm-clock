Task taskLogic(100, TASK_FOREVER, &logicCb, &scheduler);
OneButton logicButtonMode(PIN_BTN1, true);
OneButton logicButtonSet(PIN_BTN2, true);


#define LOGIC_MODE_TIME 0
#define LOGIC_MODE_DATE 1
#define LOGIC_MODE_SET_AL_ON 2
#define LOGIC_MODE_COUNTDOWN 3
#define LOGIC_MODE_MAX 4

#define LOGIC_MODE_CD_RUN 5
#define LOGIC_MODE_CD_FINISH 6
#define LOGIC_MODE_TIME_ALTRIG 7
#define LOGIC_MODE_RESET 8


uint8_t logicMode = LOGIC_MODE_TIME;
uint8_t logicCntdnMin = 3;
uint8_t logicCntdnSec = 0;

uint8_t logicAlarmHr = 9;
uint8_t logicAlarmMin = 0;
uint8_t logicAlarmSec = 0;
uint8_t logicAlarmOn = 0;
uint8_t logicAlarmDays = 0b01111111;

uint32_t logicCntdnStart;
uint8_t logicCntdnRunMin;
uint8_t logicCntdnRunSec;


void logicTick() {
  logicButtonMode.tick();
  logicButtonSet.tick();
}

void logicSetup() {
  logicButtonMode.setDebounceTicks(30);
  logicButtonMode.setPressTicks(5000);
  logicButtonMode.attachClick(logicHandleBtnMode);
  logicButtonMode.attachLongPressStop(wifiReset);
  logicButtonMode.attachLongPressStart(logicShowReset);

  logicButtonSet.setDebounceTicks(30);
  logicButtonSet.setPressTicks(1000);
  logicButtonSet.attachClick(logicHandleBtnSet);
}

void logicShowReset() {
  logicSetMode(LOGIC_MODE_RESET);
}

void logicSetAlarmTime(const uint8_t h, const uint8_t m, const uint8_t s, const uint8_t dow) {
  logicAlarmHr = h;
  logicAlarmMin = m;
  logicAlarmSec = s;
  logicAlarmDays = dow;

  logValue("Alarm hour: ", h);
  logValue("Alarm minute: ", m);
  logValue("Alarm second: ", s);
  logValue("Alarm dow: ", dow);
}

bool logicDowMatch(const uint8_t dow) {
  // day of week in time library starts with different day than in Blynk
  switch (dow) {
    case 2: return (logicAlarmDays & 0b00000001) != 0; // monday
    case 3: return (logicAlarmDays & 0b00000010) != 0; // tuesday
    case 4: return (logicAlarmDays & 0b00000100) != 0; // wednesday
    case 5: return (logicAlarmDays & 0b00001000) != 0; // thursday
    case 6: return (logicAlarmDays & 0b00010000) != 0; // friday
    case 7: return (logicAlarmDays & 0b00100000) != 0; // saturday
    case 1: return (logicAlarmDays & 0b01000000) != 0; // sunday
    default: return false;
  }
}

void logicSetMode(const uint8_t mode) {
  logicMode = mode;
  beepStop();
  if (mode == LOGIC_MODE_CD_FINISH) {
    beep(50, 200, 255);
    if (Blynk.connected()) {
      Blynk.notify("Countdown finished!");
    }

  } else if (mode == LOGIC_MODE_CD_RUN) {
    logicCntdnStart = millis();
    logicCntdnRunMin = logicCntdnMin;
    logicCntdnRunSec = logicCntdnSec;
    logValue("Starting countdown at: ", logicCntdnStart);

  } else if (mode == LOGIC_MODE_TIME_ALTRIG) {
    beep(100, 300, 255);
    if (Blynk.connected()) {
      Blynk.notify("Wake up!");
    }

  }
  logicEvaluate();
}

void logicHandleBtnMode() {
  if (!taskLogic.isEnabled()) return;

  switch (logicMode) {
    case LOGIC_MODE_TIME:
    case LOGIC_MODE_DATE:
    case LOGIC_MODE_SET_AL_ON:
    case LOGIC_MODE_COUNTDOWN:
      logicMode++;
      if (logicMode >= LOGIC_MODE_MAX) logicMode = 0;
      logicEvaluate();
      break;

    case LOGIC_MODE_CD_FINISH:
    case LOGIC_MODE_CD_RUN:
    case LOGIC_MODE_TIME_ALTRIG:
      logicSetMode(LOGIC_MODE_TIME);
      break;

    default:
      return;
  }
  beep(10, 0, 1);
}

void logicHandleBtnSet() {
  if (!taskLogic.isEnabled()) return;

  switch (logicMode) {
    case LOGIC_MODE_SET_AL_ON:
      logicAlarmOn = logicAlarmOn ? 0 : 1;
      if (Blynk.connected()) {
        Blynk.virtualWrite(BLYNK_PIN_ALARMENABLE, logicAlarmOn);
      }
      break;

    case LOGIC_MODE_COUNTDOWN:
      logicSetMode(LOGIC_MODE_CD_RUN);
      break;

    case LOGIC_MODE_CD_FINISH:
    case LOGIC_MODE_CD_RUN:
    case LOGIC_MODE_TIME_ALTRIG:
      logicSetMode(LOGIC_MODE_TIME);
      break;

    default:
      return;
  }
  beep(10, 0, 1);
}

void logicEvaluate() {
  uint32_t t = now();
  tmElements_t tt;
  switch (logicMode) {
    case LOGIC_MODE_TIME:
      breakTime(t, tt);
      if (logicAlarmOn && logicAlarmMin == tt.Minute && logicAlarmHr == tt.Hour && logicAlarmSec == tt.Second && logicDowMatch(tt.Wday)) {
        logicSetMode(LOGIC_MODE_TIME_ALTRIG);
        return;
      }
      dispTime = tt.Hour * 100 + tt.Minute;
      dispDots = !(tt.Second & 1) ? 0b0011 : 0;
      dispMask = 0xf;
      dispShowBuffer();
      break;
    case LOGIC_MODE_TIME_ALTRIG:
      breakTime(t, tt);
      dispTime = tt.Hour * 100 + tt.Minute;
      dispDots = 0b0011;
      dispMask = taskLogic.getRunCounter() & 1 ? 0xf : 0;
      dispShowBuffer();
      break;
    case LOGIC_MODE_DATE:
      breakTime(t, tt);
      dispTime = tt.Day * 100 + tt.Month;
      dispDots = 0b0001;
      dispMask = 0xf;
      dispShowBuffer();
      break;
    case LOGIC_MODE_SET_AL_ON:
      dispShowStatic2(DISP_ALARM, taskLogic.getRunCounter() & 1 ? dispDigit(logicAlarmOn ? 1 : 0) : 0);
      break;
    case LOGIC_MODE_COUNTDOWN:
      dispTime = logicCntdnMin * 100 + logicCntdnSec;
      dispDots = 0b0010;
      dispMask = taskLogic.getRunCounter() & 1 ? (logicCntdnMin < 10 ? 0b0111 : 0b1111) : 0;
      dispShowBuffer();
      break;
    case LOGIC_MODE_CD_RUN: {
        uint32_t delta = millis() - logicCntdnStart;
        uint32_t cntdn = (logicCntdnRunMin * 60UL + logicCntdnRunSec) * 1000UL;
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
        dispDots = b ? 0 : 0b0010;
        dispMask = min < 10 ? 0b0111 : 0xf;
        dispShowBuffer();
        break;
      }
    case LOGIC_MODE_CD_FINISH:
      dispTime = 0;
      dispDots = 0b0010;
      dispMask = taskLogic.getRunCounter() & 1 ? 0b111 : 0;
      dispShowBuffer();
      break;
    case LOGIC_MODE_RESET:
      dispShowStatic(DISP_RESET);
      break;
  }
}

void logicCb() {
  logicEvaluate();
}
