void setup_wifi()
{
  /////////////////////////////////////////////////
  //// Configuración e inicialización del WIFI ////
  /////////////////////////////////////////////////
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Configurando WIFI...");
  Serial.println("Configurando WIFI...");
  Serial.print("Conectando con: ");
  Serial.println(config.ssid);
  delay(100);
  /*if(a == 0){
    readFile(SD, "/conf_wifi.txt");                         // Leer archivo de configuración del wifi para obtener nombre y clave
    a = 1;
  }*/
  if(config.ssid != 0 && config.password != 0){                           // Si se obtiene nombre y clave del wifi:
    WiFi.mode(WIFI_STA);                                    // Configurar el modo de la conexión
    WiFi.begin(config.ssid, config.password);                             // Iniciar la conexión
    prev_time6 = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(100);
      Serial.print(".");                 // Mientras no se establezca la conexión:                  // Mientras no se establezca la conexión:
      if (millis() - prev_time6 > wifi_conn_lim){           // Conteo de tiempo. Si supera el tiempo límite: conexión no establecida
        lcd.setCursor(0,1); lcd.print(">>> Tiempo superado");
        lcd.setCursor(0,2); lcd.print(">>> WIFI ERROR");
        TCA.write1(LED_WIFI, LOW);                        // EL segundo led de la placa se apaga en caso de NO conectarse a la WIFI
        Serial.println("");
        Serial.println("Tiempo superado");
        Serial.println("Conexión WiFi no establecida");
        delay(100);
        return;                                             // Salir de la función setup_wifi
      }
    }
    lcd.setCursor(0,1); lcd.print(">>> WIFI OK");
    delay(1000);
    randomSeed(micros());                                   // Conexión wifi establecida correctamente ¿Esto para que es?
    Serial.println("");
    Serial.print("Conexion WiFi establecida con: ");
    Serial.println(config.ssid);
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());                         // Mostrar dirección IP asignada al ESP32
    TCA.write1(LED_WIFI, HIGH);                             // EL segundo led de la placa se enciende en caso de conectarse a la WIFI
  }
  else{                                                     // Si no se obtiene nombre y clave del wifi:
    lcd.setCursor(0,1); lcd.print(">>> Config error");
    lcd.setCursor(0,2); lcd.print(">>> WIFI ERROR");
    TCA.write1(LED_WIFI, LOW);                            // EL segundo led de la placa se apaga en caso de NO conectarse a la WIFI
    Serial.println("ERROR: Introducir nombre y clave WiFi en SD");  // Conexión no establecida
    Serial.println("Conexión WiFi no establecida");
    delay(1000);
  }
}