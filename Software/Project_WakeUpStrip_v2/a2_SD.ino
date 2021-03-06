void dataSdRead() {
  File myFile;
  myFile = SD.open("data.txt");
  if (myFile) {
    Serial.println("File has opened, reading data...");
    Serial.println("Alarm # || Hour || Minute || State");
    for (int i = 0; i < al_kol; i++) {
      alarms[i].hour = int8_t(myFile.parseInt());
      alarms[i].minute = int8_t(myFile.parseInt());
      alarms[i].isActive = bool(myFile.parseInt());
      Serial.print(i + 1);
      Serial.print(" alarm: ");
      Serial.print(alarms[i].hour);
      Serial.print(' ');
      Serial.print(alarms[i].minute);
      if (alarms[i].isActive) {
        Serial.println(" ON");
      } else {
        Serial.println(" OFF");
      }

    }
    ledPreset.r = byte(myFile.parseInt());
    ledPreset.g = byte(myFile.parseInt());
    ledPreset.b = byte(myFile.parseInt());
    Serial.print(ledPreset.r);
    Serial.print(" ");
    Serial.print(ledPreset.g);
    Serial.print(' ');
    Serial.println(ledPreset.b);
    myFile.close();
  } else {
    Serial.println("ERROR: Could not read command file. Program stopped.");
    return;
  }
  Serial.println("Data has written");
}

void writeAlarmToSd(byte event) {
  switch (event) {
    case 0:
      Serial.print(change_time);
      Serial.print(F(" alarm: set time to "));
      Serial.print(alarms[change_time - 1].hour);
      Serial.print(':');
      Serial.print(alarms[change_time - 1].minute);
      Serial.print(F(". Writing to file..."));
      break;
    case 1:
      Serial.print(F("alarm: changing state..."));
  }

  SD.remove("data.txt");
  File myFile = SD.open("data.txt", FILE_WRITE);
  if (myFile) {
    for (int i = 0; i < al_kol; i++) {
      myFile.print(alarms[i].hour);
      myFile.print(' ');
      myFile.print(alarms[i].minute);
      myFile.print(' ');
      if (alarms[i].isActive) {
        myFile.println("1");
      } else {
        myFile.println("0");
      }
    }
    myFile.print(ledPreset.r);
    myFile.print(' ');
    myFile.print(ledPreset.g);
    myFile.print(' ');
    myFile.println(ledPreset.b);
    myFile.close();
  }
  Serial.println(F("done"));
}
