void rgbSetPreset () {
  if (!ledActive) {

    pixels.fill(mRGB(ledPreset.r, ledPreset.g, ledPreset.b)); // заливаем жёлтым
    pixels.show();
  }
}
/*
  void fillColor(uint32_t c) {
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    // заполняем текущий сегмент выбранным цветом
    pixels.setPixelColor(i, c);
    pixels.show();
  }
  }*/

void calcDawn() {
  for (int i = 0; i < al_kol; i++) {
    int minutesFromMid = alarms[i].hour * 60 + alarms[i].minute, dawnMinutes;
    if (minutesFromMid >= dawnTime) {
      dawnMinutes = minutesFromMid - dawnTime;
    } else {
      dawnMinutes = minutesFromMid + 1440 - dawnTime;
    }
    alarms[i].dawnHour = dawnMinutes / 60;
    alarms[i].dawnMin = dawnMinutes % 60;
  }
}

void dawnTick() {
  if (!ledActive) {
    for (int i = 0; i < al_kol; i++) {
      if ((alarms[i].dawnHour == t_now.hour()) and (alarms[i].dawnMin == t_now.minute()) and (t_now.second() == 0) and alarms[i].isActive) {
        ledActive = true;
        lightTime = millis();
        Serial.print(i);
        Serial.println(F(" alarm: starting dawn "));
        break;
      }
    }
  }
  if (ledActive) {
    if (ledR == 0 && ledG < 255) {
      if (millis() - lightTime > dawnStep) {
        Serial.print(ledR);
        Serial.print(' ');
        Serial.println(ledG);
        //fillColor(pixels.Color(ledR, ledG, 0));
        pixels.fill(mRGB(ledR, ledG, 0)); // заливаем жёлтым
        pixels.show();
        ledG++;
        lightTime = millis();
      }
    }
    // плавно добавляем Красный
    if (ledR < 255 && ledG == 255) {
      if (millis() - lightTime > dawnStep) {
        Serial.print(ledR);
        Serial.print(' ');
        Serial.println(ledG);
        pixels.fill(mRGB(ledR, ledG, 0)); // заливаем жёлтым
        pixels.show();
        ledR++;
        lightTime = millis();
      }
    }
    // плавно гасим Зеленый
    if (ledR == 255) {
      if (millis() - lightTime > dawnStep) {

        if (ledG != 0) {
          Serial.print(ledR);
          Serial.print(' ');
          Serial.println(ledG);
          pixels.fill(mRGB(ledR, ledG, 0)); // заливаем жёлтым
          pixels.show();
          ledG--;
        }
        lightTime = millis();
      }
    }
  }
}
