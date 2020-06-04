

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
#define sd_pin 53
#define PARSE_AMOUNT 6  
#define butPin 22
//-------------------КОНЕЦ-НАСТРОЕК---------------------

//---------------------БИБЛИОТЕКИ-----------------------
#include <OLED_I2C.h>
#include "RTClib.h"
#include <GyverButton.h>
#include "GyverEncoder.h"
#include <Wire.h>
#include "GyverTM1637.h"
#include <SPI.h>
#include <SD.h>
//#include <SoftwareSerial.h>
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
RTC_DS3231 rtc;
GyverTM1637 disp(CLK_tm, DIO);
GButton but(butPin);

extern uint8_t SmallFont[];

int value = 0, change = 0;
int intData[PARSE_AMOUNT];
byte level = 0, change_time = 0;
bool inMenu = false, dots = true;
DateTime t_now, t_prev;
oneAlarm alarms[al_kol];
boolean recievedFlag = false;
boolean getStarted = false;
byte index;
String string_convert = "";
//SoftwareSerial mySerial(2, 3);

const char menu[][maxArrSize] = {"Alarm On/Off", "Settings", "Light"};
const char settngs_menu[][maxArrSize] = {"Time", "Date", "Alarm set", "Dawn time"};

void setup() {
  Serial.begin(9600);
  Serial.println("Starting up");
  //mySerial.begin(9600);
  Serial1.begin(9600);
  rtc.begin();
  t_now = rtc.now();
  t_prev = t_now;
  Serial.print("Initializing SD card...");

  if (!SD.begin(sd_pin)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  dataSdRead();
  disp.clear();
  disp.brightness(7);
  disp.displayClock(byte(t_now.hour()), byte(t_now.minute()));
  if (!myOLED.begin(SSD1306_128X64)){
    Serial.println("OLED failed");
    //while (1);
  } else {
      // In case the library failed to allocate enough RAM for the display buffer...
  Serial.println("OLED has inited");  
  } 
  myOLED.setFont(SmallFont);
  but.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  but.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  but.setClickTimeout(600); 
  but.setType(LOW_PULL);
  but.setDirection(NORM_OPEN);
  enc1.setTickMode(MANUAL);
  enc1.setType(TYPE2);    // тип энкодера TYPE1 одношаговый, TYPE2 двухшаговый. Если ваш энкодер работает странно, смените тип
  myOLED.clrScr();
  Serial.println("All systems clear");
  Serial.print(t_now.hour());
  Serial.print(' ');
  Serial.print(t_now.minute());
  Serial.print(' ');
  Serial.print(t_now.second());
  Serial.print(' ');
  Serial.print(t_now.year());
  Serial.print(' ');
  Serial.print(t_now.month());
  Serial.print(' ');
  Serial.print(t_now.day());
  Serial.println(' ');
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
  parsing();
  command_parse();
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

void command_parse(){
  if (recievedFlag) {                           // если получены данные
    recievedFlag = false;
    for (int i = 0; i < index; i++) {
      Serial.print(intData[i]); Serial.print(" ");
    
    }Serial.println();
    switch(intData[0]) {
      case 0:

        break;

      case 1:
        switch(intData[1]) {
          case 0:
            t_now = rtc.now();
            rtc.adjust(DateTime(t_now.year(), t_now.month(), t_now.day(), intData[2], intData[3], intData[4]));
            t_now = rtc.now();
            disp.displayClock(byte(t_now.hour()), byte(t_now.minute()));
            t_prev = t_now;
            break;
          case 1:
            t_now = rtc.now();
            rtc.adjust(DateTime(intData[4], intData[3], intData[2], t_now.hour(), t_now.minute(), t_now.second()));
            break;
        }
        break;
    }
  }
}

void parsing() {
  if (Serial1.available() > 0) {
    Serial.println("Data detected");
    char incomingByte = Serial1.read();        // обязательно ЧИТАЕМ входящий символ
    if (getStarted) {                         // если приняли начальный символ (парсинг разрешён)
      if (incomingByte != ' ' && incomingByte != ';') {   // если это не пробел И не конец
        string_convert += incomingByte;       // складываем в строку
      } else {                                // если это пробел или ; конец пакета
        intData[index] = string_convert.toInt();  // преобразуем строку в int и кладём в массив
        string_convert = "";                  // очищаем строку
        index++;                              // переходим к парсингу следующего элемента массива
      }
    }
    if (incomingByte == '$') {                // если это $
      getStarted = true;                      // поднимаем флаг, что можно парсить
      index = 0;                              // сбрасываем индекс
      string_convert = "";                    // очищаем строку
    }
    if (incomingByte == ';') {                // если таки приняли ; - конец парсинга
      getStarted = false;                     // сброс
      recievedFlag = true;                    // флаг на принятие
    }
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
  but.tick();
  if (enc1.isClick()) {
    if (inMenu) {
      switch (level) {
        case 0:
          if (change == 0) {
            alarms[0].isActive = !alarms[0].isActive;
            writeAlarmToSd(1);
            inMenu = false;
            myOLED.clrScr();
            myOLED.update();
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
    if (but.isClick()) {
      writeAlarmToSd(0);
      disp.displayClock(byte(t_now.hour()), byte(t_now.minute()));
      myOLED.clrScr();
      myOLED.update();
      change_time = 0;
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
