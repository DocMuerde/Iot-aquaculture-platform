void Show_LCD(){
  lcd.clear();
  int esp32ano,esp32mes,esp32dia,esp32hora, esp32minu, esp32segu;
  struct tm timeinfo;
  String Shora="",Smin="",Sseg="",Smes="",Sdia=""; // Para rellenar con ceros a la izq
  if(String(hora).length()<2)
    Shora="0"+String(hora); 
  else
    Shora=String(hora); 

  if(String(minu).length()<2)
    Smin="0"+String(minu); 
  else
    Smin=String(minu); 
  if(String(segu).length()<2)
    Sseg="0"+String(segu);
  else
    Sseg=String(segu); 
  if(String(mes).length()<2)
    Smes="0"+String(mes);
  else
    Smes=String(mes);  
  if(String(dia).length()<2)
    Sdia="0"+String(dia); 
  else
    Sdia=String(dia); 
  if (pantalla==0)
  {
    if (flag_LCD == 0)
    { 
      lcd.setCursor(0,0); lcd.print(Shora); lcd.print(":"); lcd.print(Smin);lcd.print(":"); lcd.print(Sseg);lcd.print("  ");lcd.print(Sdia);lcd.print("/");lcd.print(Smes);lcd.print("/");lcd.print(ano);
      lcd.setCursor(0,1); lcd.print("Tw1:"); lcd.print(Tw1, 2);lcd.print("  Tw2:"); lcd.print(Tw2, 2);
      lcd.setCursor(0,2); lcd.print("EC:"); lcd.print(EC, 2); lcd.print(" us/cm");
      lcd.setCursor(0,3); lcd.print("PH:"); lcd.print(PH, 2);lcd.print(" O2:"); lcd.print(O2, 2); lcd.print("mg/L");
      flag_LCD = 1;
    }
    else{
      lcd.setCursor(0,0); lcd.print(Shora); lcd.print(":"); lcd.print(Smin);lcd.print(":"); lcd.print(Sseg);lcd.print("  ");lcd.print(Sdia);lcd.print("/");lcd.print(Smes);lcd.print("/");lcd.print(ano);                           // Imprimir medidas ambientales (LCD)
      lcd.setCursor(0,1); lcd.print("VAR AMB");
      lcd.setCursor(0,2); lcd.print("Ta1:"); lcd.print(Ta1, 2); lcd.print("C");lcd.setCursor(11,2); lcd.print("H1:"); lcd.print(Ha1, 2); lcd.print("%");
      lcd.setCursor(0,3); lcd.print("Ta2:"); lcd.print(Ta2, 2); lcd.print("C");lcd.setCursor(11,3); lcd.print("H2:"); lcd.print(Ha2, 2); lcd.print("%");
      flag_LCD = 0;
    }
  }
  if(pantalla==1) // Pantalla para calibrar el PH1
  {
    lcd.setCursor(0,0); lcd.print("Cal PH1");
    // lcd.setCursor(0,1); lcd.print("Establezca PH con botones"); 
    // Hayque enseñar el voltage sin filtrar para ver el valor actual 
    lcd.setCursor(0,1); lcd.print("V:"); lcd.setCursor(8,1); lcd.print(ph_voltage_cal, 2);
    lcd.setCursor(0,2); lcd.print("PH:"); lcd.setCursor(8,2); lcd.print(PH1aux, 2);
    //lcd.setCursor(0,2); lcd.print("V:"); lcd.print(ph_voltage, 2);lcd.print("  raw:"); lcd.print(pH_raw, 2);
    lcd.setCursor(0,3); lcd.print("PH UP DOWN OK"); 
  }
  if(pantalla==2)
  {
    lcd.setCursor(0,0); lcd.print("Cal PH2");
    // lcd.setCursor(0,1); lcd.print("Establezca PH con botones"); 
    // Hayque enseñar el voltage sin filtrar para ver el valor actual 
    lcd.setCursor(0,1); lcd.print("V:"); lcd.setCursor(8,1); lcd.print(ph_voltage_cal, 2);
    lcd.setCursor(0,2); lcd.print("PH:"); lcd.setCursor(8,2); lcd.print(PH2aux, 2);
    //lcd.setCursor(0,2); lcd.print("V:"); lcd.print(ph_voltage, 2);lcd.print("  raw:"); lcd.print(pH_raw, 2);
    lcd.setCursor(0,3); lcd.print("PH UP DOWN OK"); 
  }
  if(pantalla==3) // No se si por el método de calibracion con un solo punto sacando la proveta del agua ... hay que usar la temperatura del aire
  {
    lcd.setCursor(0,0); lcd.print("Cal 02 T aire");
    lcd.setCursor(0,1); lcd.print("V:"); lcd.setCursor(8,1); lcd.print(o2_voltage_cal, 2);
    lcd.setCursor(0,2); lcd.print("Ta:"); lcd.setCursor(8,2); lcd.print(Ta1, 2);
    lcd.setCursor(0,3); lcd.print("OK para acabar"); 
  }
  if(pantalla==4) // Método de calibracion con un solo punto utilizando agua saturada de O2 , medainte una bomba de acuario ... hay que usar la temperatura del agua
  {
    lcd.setCursor(0,0); lcd.print("Cal 02 T agua");
    lcd.setCursor(0,1); lcd.print("V:"); lcd.setCursor(8,1); lcd.print(o2_voltage_cal, 2);
    lcd.setCursor(0,2); lcd.print("Tw:"); lcd.setCursor(8,2); lcd.print(Tw2, 2);
    lcd.setCursor(0,3); lcd.print("OK para acabar"); 
  }
  if(pantalla==5) // Calibración EC. 
  {
    lcd.setCursor(0,0); lcd.print("Calc EC"); lcd.setCursor(8,0); lcd.print("T:"); lcd.print(Tw1, 2);
    lcd.setCursor(0,1); lcd.print("Kec:"); lcd.setCursor(8,1); lcd.print(Kecaux, 2);
    lcd.setCursor(0,2); lcd.print("mS:"); lcd.setCursor(8,2); lcd.print(mSaux, 2);
    lcd.setCursor(0,3); lcd.print("mS UP DOWN OK"); 
  }
  if(pantalla==6) // Control manual o automatico de la bomba. 
  {
    lcd.setCursor(0,0); lcd.print("PH control"); 
    lcd.setCursor(0,1); lcd.print("Aut control:"); 
    if(ContPHaux)
    {
      lcd.setCursor(12,1); lcd.print("ON");
    }
    else
    {
      lcd.setCursor(12,1); lcd.print("OFF");
    }
    lcd.setCursor(0,2); lcd.print("UP-ON  DOWN-OFF"); 
    lcd.setCursor(0,3); lcd.print("OK para acabar"); 
  }
  if(pantalla==7 &&  !config.ContPH) // Control manual de la EV 
  {
    lcd.setCursor(0,0); lcd.print("PH Man Control");
    lcd.setCursor(0,1); lcd.print("Pump:"); 
    if(!rele2_st)
    {
      lcd.setCursor(8,1); lcd.print("ON");
    }
    else
    {
      lcd.setCursor(8,1); lcd.print("OFF");
    }
    lcd.setCursor(0,2); lcd.print("UP-ON  DOWN-OFF"); 
    lcd.setCursor(0,3); lcd.print("OK para acabar"); 
  }
  if(pantalla==7 &&  config.ContPH) // Ganancia del control automático de PH 
  {
    lcd.setCursor(0,0); lcd.print("PH AUT Control");
    lcd.setCursor(0,1); lcd.print("KPH: "); lcd.setCursor(8,1); lcd.print(KPHaux);
    lcd.setCursor(0,2); lcd.print("UP=K+0.1 DOWN=K-0.1"); 
    lcd.setCursor(0,3); lcd.print("OK para acabar"); 
  }
  if(pantalla==8) // Valor objetivo de PH en el control automático 
  {
    lcd.setCursor(0,0); lcd.print("PH objetivo");
    lcd.setCursor(0,1); lcd.print("PH: "); lcd.setCursor(8,1); lcd.print(PHrefaux);
    lcd.setCursor(0,2); lcd.print("UP=PH+0.1 DWN=PH-0.1"); 
    lcd.setCursor(0,3); lcd.print("OK para acabar"); 
  }
}

/*if(pantalla==6) // Calibración EC. 
  {
    configTime(0, 0, ntpServer);    // configTime(gmtoffset_seec, daylightOffset_sec, ntpServer)
    esp32ano=timeinfo.tm_year + 1900;
    Serial.println(esp32ano);
    esp32mes=timeinfo.tm_mon;
    Serial.println(esp32mes);
    esp32dia=timeinfo.tm_mday;
    Serial.println(esp32dia);
    esp32hora=timeinfo.tm_hour;
    Serial.println(esp32hora);
    esp32minu=timeinfo.tm_min;
    Serial.println(esp32minu);
    esp32segu=timeinfo.tm_sec;
    Serial.println(esp32segu);
    lcd.setCursor(0,0); lcd.print("Ajuste reloj RTC");
    lcd.setCursor(0,1); lcd.print(esp32hora); lcd.print(":"); lcd.print(esp32minu);lcd.print(":"); lcd.print(esp32segu);lcd.print(" ");lcd.print(esp32dia);lcd.print("-");lcd.print(esp32mes);lcd.print("-");lcd.print(esp32ano);
    lcd.setCursor(0,2); lcd.print("RTC configurado GMT0");
    lcd.setCursor(0,3); lcd.print("Presiona OK"); 
  }*/