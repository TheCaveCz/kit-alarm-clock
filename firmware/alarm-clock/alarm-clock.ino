/*
 * Alarm clock kit firmware for Blynk app
 * 
 * The Cave, 2019
 * https://thecave.cz
 * 
 * Licensed under MIT License (see LICENSE file for details)
 * 
 * board: LOLIN(WEMOS) D1 R2 & mini
 * 
 */
#define BLYNK_PRINT Serial
#define BLYNK_NO_FANCY_LOGO

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

// https://github.com/blynkkk/blynk-library
#include <BlynkSimpleEsp8266.h>

// https://github.com/tzapu/WiFiManager
#include <WiFiManager.h>

// https://github.com/mathertel/OneButton
#include <OneButton.h>

// https://github.com/arkhipenko/TaskScheduler
#include <TaskScheduler.h>

// https://github.com/avishorp/TM1637
#include <TM1637Display.h>

// https://github.com/PaulStoffregen/Time
#include <TimeLib.h>


#define PIN_BUZZ D5
#define PIN_CLK D6
#define PIN_DATA D7
#define PIN_BTN1 D1
#define PIN_BTN2 D2

//#define BLYNK_PIN_LIMIT V0
//#define BLYNK_PIN_VALUE V1
//#define BLYNK_PIN_ALARM V2


// Logging over Serial
#define LOG_ENABLED 1

#define RECONNECT_INTERVAL 30000UL

#define WIFI_AP_NAME "alarm-clock-"

String chipId = String(ESP.getChipId(), HEX);
Scheduler scheduler;

void placeholder() {} // keep this function here
