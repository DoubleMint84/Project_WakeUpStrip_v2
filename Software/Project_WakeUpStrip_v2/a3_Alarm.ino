void alarmTick() {
  if (alarmRaise == -1 and change_time == 0) {
    for (int i = 0; i < al_kol; i++) {
      if ((alarms[i].hour == t_now.hour()) and (alarms[i].minute == t_now.minute()) and (t_now.second() == 0) and alarms[i].isActive) {
        alarmRaise = i;
        myOLED.clrScr();
        myOLED.print("Alarm " + String(i), CENTER, 0);
        myOLED.print(F("Get up!"), CENTER, 16);
        myOLED.print(F("Push the OK button"), CENTER, 24);
        myOLED.print(F("twice to turn off"), CENTER, 32);
        myOLED.update();
        timerAlarm = millis();
        blinkBuzz = true;
        digitalWrite(buzz, blinkBuzz);
        break;
      }
    }
  }
  if (alarmRaise != -1) {
    if(millis() - timerAlarm > 1000) {
      blinkBuzz = !blinkBuzz;
      digitalWrite(buzz, blinkBuzz);
      timerAlarm = millis();
    }
  }
}
