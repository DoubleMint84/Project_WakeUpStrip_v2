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
  } else {
    Serial.println("ERROR: Could not read command file. Program stopped.");
    return;
  }
  Serial.println("Data has written");
}
