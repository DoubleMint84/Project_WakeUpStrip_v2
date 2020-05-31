//----------------------НАСТРОЙКИ-----------------------
#define CLK 9
#define DT 10
#define CLK_tm 7
#define DIO 6
#define SW 11
#define maxArrSize 28
#define kolArrMenu 3
#define kolArrSettings 4
#define oled_adr 0x78
#define clock_adr 0x68
#define al_kol 5
//-------------------КОНЕЦ-НАСТРОЕК---------------------

//---------------------БИБЛИОТЕКИ-----------------------
#include <OLED_I2C.h>
#include "DS3231.h"
#include "GyverEncoder.h"
#include <Wire.h>
#include "GyverTM1637.h"
//------------------КОНЕЦ-БИБЛИОТЕК---------------------

//---------------------СТРУКТУРЫ------------------------
struct oneAlarm {
  int8_t hour;
  int8_t minute;
  bool isActive;
};
//-------------------КОНЕЦ-СТРУКТУР---------------------

Encoder enc1(CLK, DT, SW);  // для работы c кнопкой
OLED  myOLED(SDA, SCL);
RTClib rtc;
GyverTM1637 disp(CLK_tm, DIO);

extern uint8_t SmallFont[];

int value = 0, change = 0;
byte level = 0, change_time = 0;
bool inMenu = false, dots = true;
DateTime t_now, t_prev;
oneAlarm alarms[al_kol];

const char menu[][maxArrSize] = {"Alarm On/Off", "Settings", "Light"};
const char settngs_menu[][maxArrSize] = {"Time", "Date", "Alarm set", "Dawn time"};

void setup() {
  Serial.begin(9600);
  Wire.begin();
  t_now = rtc.now();
  t_prev = t_now;
  disp.clear();
  disp.brightness(7);
  disp.displayClock(byte(t_now.hour()), byte(t_now.minute()));
  if (!myOLED.begin(SSD1306_128X64))
    while (1);  // In case the library failed to allocate enough RAM for the display buffer...

  myOLED.setFont(SmallFont);
  enc1.setTickMode(MANUAL);
  enc1.setType(TYPE2);    // тип энкодера TYPE1 одношаговый, TYPE2 двухшаговый. Если ваш энкодер работает странно, смените тип
  myOLED.clrScr();
  /* myOLED.print(F("Main menu"), LEFT, 0);
    for (int i = 0; i < ((kolArr - change > 6) ? 6 : kolArr - change); i++) {
     if (i == 0) {
       myOLED.print(String("->") + String(menu[change + i]), LEFT, 8 * (i + 1));
     } else {
       myOLED.print(String("  ") + String(menu[change + i]), LEFT, 8 * (i + 1));
     }
    }*/
  myOLED.update();
  disp.point(dots);
}

void loop() {
  inputTick();
  setupTick();
  
  t_now = rtc.now();
  /*if (t_now.second() != t_prev.second()){
    disp.point(!dots);
    dots = !dots;
    }*/
  if ((t_now.minute() != t_prev.minute()) and (change_time == 0)) {
    disp.displayClock(byte(t_now.hour()), byte(t_now.minute()));
    t_prev = t_now;
  }
}

void setupTick() {
  if (enc1.isTurn() && inMenu) {       // если был совершён поворот (индикатор поворота в любую сторону)
    switch (level) {
      case 0:
        myOLED.clrScr();
        myOLED.print(F("Main menu"), CENTER, 0);
        for (int i = 0; i < ((kolArrMenu - change > 6) ? 6 : kolArrMenu - change); i++) {
          if (i == 0) {
            myOLED.print(String("->") + String(menu[change + i]), LEFT, 8 * (i + 1));
          } else {
            myOLED.print(String("  ") + String(menu[change + i]), LEFT, 8 * (i + 1));
          }
        }
        myOLED.update();
        break;
      case 1:
        myOLED.clrScr();
        myOLED.print(F("Settings"), CENTER, 0);
        for (int i = 0; i < ((kolArrSettings - change > 6) ? 6 : kolArrSettings - change); i++) {
          if (i == 0) {
            myOLED.print(String("->") + String(settngs_menu[change + i]), LEFT, 8 * (i + 1));
          } else {
            myOLED.print(String("  ") + String(settngs_menu[change + i]), LEFT, 8 * (i + 1));
          }
        }
        myOLED.update();
        break;
    }


  } else if (/*enc1.isTurn()&& */ change_time == 1) {
    disp.displayClock(byte(alarms[0].hour), byte(alarms[0].minute));
  }
}

void inputTick() {
  enc1.tick();
  if (enc1.isClick()) {
    if (inMenu) {
      switch (level) {
        case 0:
          if (change == 0) {
            alarms[1].isActive = !alarms[1].isActive;
            inMenu = false;
          }
          else if (change == 1) {
            level = 1;
            change = 0;
            myOLED.clrScr();
            myOLED.print(F("Settings"), CENTER, 0);
            for (int i = 0; i < ((kolArrSettings - change > 6) ? 6 : kolArrSettings - change); i++) {
              if (i == 0) {
                myOLED.print(String("->") + String(settngs_menu[change + i]), LEFT, 8 * (i + 1));
              } else {
                myOLED.print(String("  ") + String(settngs_menu[change + i]), LEFT, 8 * (i + 1));
              }
            }
            myOLED.update();
          } else {
            inMenu = false;
            myOLED.clrScr();
            myOLED.update();
          }
          break;
        case 1:
          if (change == 2) {
            change = 0;
            change_time = 1;
            inMenu = false;
            myOLED.clrScr();
            myOLED.print(F("Alarm 1 setting"), CENTER, 0);
            myOLED.print(F("Set the time"), CENTER, 16);
            myOLED.print(F("on 7-segment disp"), CENTER, 24); 
            myOLED.update();
            disp.displayClock(byte(alarms[0].hour), byte(alarms[0].minute));
          } else {
            inMenu = false;
            myOLED.clrScr();
            myOLED.update();
          }
          break;
      }

    } else if (change_time == 0) {
      inMenu = true;
      change = 0;
      level = 0;
      myOLED.clrScr();
      myOLED.print(F("Main menu"), CENTER, 0);
      for (int i = 0; i < ((kolArrMenu - change > 6) ? 6 : kolArrMenu - change); i++) {
        if (i == 0) {
          myOLED.print(String("->") + String(menu[change + i]), LEFT, 8 * (i + 1));
        } else {
          myOLED.print(String("  ") + String(menu[change + i]), LEFT, 8 * (i + 1));
        }
      }
      myOLED.update();

    } else {
      
    }
  }
  if (change_time == 1) {
    if (enc1.isRight()) {
      alarms[change_time - 1].minute++;
      if (alarms[change_time - 1].minute > 59) {
        alarms[change_time - 1].minute = 0;
        alarms[change_time - 1].hour++;
        if (alarms[change_time - 1].hour > 23) alarms[change_time - 1].hour = 0;
      }
    }
    else if (enc1.isLeft()) {
      alarms[change_time - 1].minute--;
      if (alarms[change_time - 1].minute < 0) {
        alarms[change_time - 1].minute = 59;
        alarms[change_time - 1].hour--;
        if (alarms[change_time - 1].hour < 0) alarms[change_time - 1].hour = 23;
      }
    }
    else if (enc1.isRightH()) {
      alarms[change_time - 1].hour++;
      if (alarms[change_time - 1].hour > 23) alarms[change_time - 1].hour = 0;
    }
    else if (enc1.isLeftH()) {
      alarms[change_time - 1].hour--;
      if (alarms[change_time - 1].hour < 0) alarms[change_time - 1].hour = 23;
    }
  }
  if (inMenu) {
    switch (level) {
      case 0:
        if (enc1.isLeft() and (change > 0)) change--;
        if (enc1.isRight() and (change < kolArrMenu - 1)) change++;
        break;
      case 1:
        if (enc1.isLeft() and (change > 0)) change--;
        if (enc1.isRight() and (change < kolArrSettings - 1)) change++;
        break;
    }

  }
}
