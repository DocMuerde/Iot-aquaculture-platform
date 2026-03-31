void Cal_RTC() {
  // Una vez conectados a la wifi
  // https://cplusplus.com/reference/ctime/tm/
  int esp32ano,esp32mes,esp32dia,esp32hora, esp32minu, esp32segu;
  configTime(0, 0, ntpServer);    // configTime(gmtoffset_seec, daylightOffset_sec, ntpServer)
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) 
  {
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    esp32ano=timeinfo.tm_year + 1900;
    //Serial.println(esp32ano);
    esp32mes=timeinfo.tm_mon +1;  // tm_mon	int	months since January	0-11
    //Serial.println(esp32mes);
    esp32dia=timeinfo.tm_mday;
    //Serial.println(esp32dia);
    esp32hora=timeinfo.tm_hour;
    //Serial.println(esp32hora);
    esp32minu=timeinfo.tm_min;
    //Serial.println(esp32minu);
    esp32segu=timeinfo.tm_sec;
    //Serial.println(esp32segu);
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    rtc.adjust(DateTime(esp32ano, esp32mes, esp32dia, esp32hora, esp32minu, esp32segu));  // establecemos la hora del modulo de reloj
    lcd.clear();
    lcd.setCursor(0,0); lcd.print("Ajustando reloj!!!");
    lcd.setCursor(0,1); lcd.print(esp32hora); lcd.print(":"); lcd.print(esp32minu);lcd.print(":"); lcd.print(esp32segu);lcd.print(" ");lcd.print(esp32dia);lcd.print("-");lcd.print(esp32mes);lcd.print("-");lcd.print(esp32ano);
    lcd.setCursor(0,2); lcd.print("RTC configurado GMT0");
    delay(2000);
  }
}