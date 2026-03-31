int setup_mqtt()
{
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Config MQTT...");
  delay(1000);
  
  client.setServer(mqtt_server, MQTT_PORT);                                  // Establecer broker y puerto MQTT
  Serial.println("Conectando con servidor MQTT...");
  prev_time7 = millis();
  while(!client.connected()){                                       // Mientras no se establezca la conexión:
    client.connect(mqtt_user_name,mqtt_user_name,mqtt_password);                                   // Conectar con cliente
    client.setBufferSize(MQTT_MAX_MESSAGE_SIZE);
    //Serial.println(client.getBufferSize());
    delay(100);
    if(millis() - prev_time7 > mqtt_conn_lim){ 
      lcd.setCursor(0,1); lcd.print(">>> MQTT ERROR");                         // Conteo de tiempo
      Serial.println("Tiempo superado. Conexion MQTT no establecida");  // Tiempo superado. Conexión MQTT no establecida
      TCA.write1(LED_MQTT, LOW); // EL tercer led de la placa se apaga en caso de NO poder conectarse al servidor MQTT
      delay(2000);                                       
      return -1;                                                           // Salir de setup_mqtt
    }
  }
  lcd.setCursor(0,1); lcd.print(">>> MQTT OK");
  Serial.println("Conexion MQTT establecida");
  TCA.write1(LED_MQTT, HIGH);                                           // EL tercer led de la placa se enciende en caso de poder conectarse al servidor MQTT
  delay(2000);
  return 1;
}