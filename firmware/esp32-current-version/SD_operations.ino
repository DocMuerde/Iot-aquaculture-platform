void Write_SD(void)
{
  String data_str = "";                           // Genera string vacio
  data_str += String(timestamp) + ";";           // Concatena todas las medidas
  data_str += String(DATE) + ";";           // Concatena todas las medidas
  data_str += String(TIME) + ";";               
  data_str += String(Tw1) + ";";
  data_str += String(Tw2) + ";";
  data_str += String(PH) + ";";
  data_str += String(O2) + ";";
  data_str += String(EC) + ";";
  data_str += String(Ta1) + ";";
  data_str += String(Ha1) + ";";
  data_str += String(Ta2) + ";";
  data_str += String(Ha2) + ";";
  data_str += String(config.ContPH) + ";";
  data_str += String(TPH) + ";";
  data_str += String(!rele2_st) + ";";  // Si está la bomba en ON o no ya sea manual o en automático
  data_str += String(MARCA);   // marca =0 , dato mandado, =1 problemas wifi, =2 problemas mqtt
  appendFile(SD, filedata, data_str);  // Añade las medidas a "data_sensors.csv" de la SD
}
void appendFile(fs::FS &fs, const char * path, String message){
  Serial.printf("Añadiendo dato a SD: ");
  //SerialString(message);       // ¿porque no puedo imprimir por el puerto serie variables de tipo String?
  File file = fs.open(path, FILE_APPEND);                     // Abrir el archivo en modo añadir datos
  if(!file){                                                  // Error abriendo archivo. Salir de la función
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.println(message)){                                  // Archivo abierto correctamente. Añadir datos
    Serial.println(" OK");                       // Datos añadidos correctamente
  }
  else{
    Serial.println(" FAILED!!!");                          // Fallo añadiendo datos
  }
  file.close();
}

void loadConfiguration(const char *filename) {
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Cargando configuración ...");
  // Open file for reading
  File file = SD.open(filename);
  // Allocate the memory pool on the stack.
  // Don't forget to change the capacity to match your JSON document.
  // Use arduinojson.org/assistant to compute the capacity.
  // StaticJsonBuffer<512> jsonBuffer;
  JsonDocument root;
  // Parse the root object
  DeserializationError error = deserializeJson(root, file);
  if (error)
  {
    lcd.setCursor(0,1); lcd.print("Cargando ");
    lcd.setCursor(0,2); lcd.print("configuracion");
    lcd.setCursor(0,3); lcd.print("por defecto");
    Serial.println("");
    Serial.println(F("Failed to read file, using default configuration"));
    // Copy values from the JsonObject to the Config
    strlcpy(config.ssid,"MiFibra-8980",sizeof(config.ssid));
    strlcpy(config.password,"nmH9Rxkt",sizeof(config.password));   
    //strlcpy(config.ssid,"TP-Link_AE0C",sizeof(config.ssid));          
    //strlcpy(config.password,"48586818",sizeof(config.password));   
    config.PH1 = 7.3;
    config.mV1 = 1715;
    config.PH2 = 9.8;
    config.mV2 = 1218;
    config.CAL1_V = 256;
    config.CAL1_T = 25.75;
    config.CAL2_V = 1507;
    config.CAL2_T = 18.4;
    config.Kec = 0.9569;  // Debe ser muy proxima a 1, valor determinado en una pegatina en la sonda
    config.Tec = 25;      // Temperatura de referencia a la que se calibró la última vez
    config.mS = 1413;     // mS de la disolución de referencia
    config.id = -1;       // instalación de prueba -1
    config.KPH=(50/1.5)*(16/10.0)*1000; // hay que decidir el valor en milisegundos 
    /* 50/1.5 son los mililitros para corregir 1 de PH
       16/10. son los segundos para hechar 1 ml aprox
       1000 para pasar a milisefundos*/
    /* 50ml una vez por semana corrige 1.5 de PH. La bomba da 10ml en 16s*/
    config.ContPH=false;  // Control de PH desactivado por defecto
    config.PHref=16.0;    // Setpoint de PH 
    delay(2000);
  }
  else
  {
    lcd.setCursor(0,1); lcd.print("Sensores ");
    lcd.setCursor(0,2); lcd.print("configurados !!!");
    Serial.println("");
    Serial.println(F("Loaded configuration"));
    strlcpy(config.ssid,root["ssid"],sizeof(config.ssid));
    strlcpy(config.password,root["password"],sizeof(config.password));   
    config.PH1 = root["PH1"];
    config.mV1 = root["mV1"];
    config.PH2 = root["PH2"];
    config.mV2 = root["mV2"];
    config.CAL1_V = root["CAL1_V"];
    config.CAL1_T = root["CAL1_T"];
    config.CAL2_V = root["CAL2_V"];
    config.CAL2_T = root["CAL2_T"];
    config.Kec = root["Kec"];
    config.mS = root["mS"];
    config.Tec = root["Tec"];
    config.KPH = root["KPH"];
    config.ContPH = root["ContPH"];
    config.PHref = root["PHref"];
    config.id = root["id"];
    delay(2000);
  }
  p_config();   // vemos la configuracion por puerto serie
  // Close the file (File's destructor doesn't close the file)
  file.close();
}

void saveConfiguration(const char *filename) {
  // Delete existing file, otherwise the configuration is appended to the file
  SD.remove(filename);
  // Open file for writing
  File file = SD.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }
  // Allocate the memory pool on the stack
  // Don't forget to change the capacity to match your JSON document.
  // Use https://arduinojson.org/assistant/ to compute the capacity.
  // StaticJsonBuffer<256> jsonBuffer;
  JsonDocument root;

  // Set the values
  root["ssid"] = config.ssid;
  root["password"] = config.password;
  root["PH1"] = config.PH1;
  root["mV1"] = config.mV1;
  root["PH2"] = config.PH2;
  root["mV2"] = config.mV2;
  root["CAL1_V"] = config.CAL1_V;
  root["CAL1_T"] = config.CAL1_T;
  root["CAL2_V"] = config.CAL2_V;
  root["CAL2_T"] = config.CAL2_T;
  root["Kec"] = config.Kec;
  root["Tec"] = config.Tec;
  root["mS"] = config.mS;
  root["KPH"] = config.KPH;
  root["ContPH"] = config.ContPH;
  root["PHref"] = config.PHref;
  root["id"] = config.id;

  // Serialize JSON to file
  if (serializeJson(root, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file (File's destructor doesn't close the file)
  file.close();
}

// Prints the content of a file to the Serial
void printFile(const char *filename) {
  // Open file for reading
  File file = SD.open(filename);
  if (!file) {
    Serial.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }
  Serial.println();

  // Close the file (File's destructor doesn't close the file)
  file.close();
}

void p_config(void)
{
    Serial.print("SSID:   ");Serial.println(config.ssid);
    Serial.print("PSSW:   ");Serial.println(config.password);
    Serial.print("PH1:    ");Serial.println(config.PH1);
    Serial.print("mV1:    ");Serial.println(config.mV1);
    Serial.print("PH2:    ");Serial.println(config.PH2);
    Serial.print("mV2:    ");Serial.println(config.mV2);
    Serial.print("CAL1_V: ");Serial.println(config.CAL1_V);
    Serial.print("CAL1_T: ");Serial.println(config.CAL1_T);
    Serial.print("CAL2_V: ");Serial.println(config.CAL2_V);
    Serial.print("CAL2_T: ");Serial.println(config.CAL2_T);
    Serial.print("Kec:    ");Serial.println(config.Kec);
    Serial.print("Tec:    ");Serial.println(config.Tec);
    Serial.print("mS:     ");Serial.println(config.mS);
    Serial.print("KPH:     ");Serial.println(config.KPH);
    Serial.print("ContPH:  ");Serial.println(config.ContPH);
    Serial.print("PHref:  ");Serial.println(config.PHref);
    Serial.print("id:     ");Serial.println(config.id);
}
