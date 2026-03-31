void Send_MQTT()
{ 
    int j=0;  // numero de intentos de envío
    JsonDocument doc;
    char out[500];
    char inst[20];
    sprintf(inst,"Aquacol.INST.inst%d", config.id);
    char Cnull[]="null";
    char CnullC[]="\"null\"";
    doc["ft_ins_inst"]=config.id;          // dependiendo del numero es una einstalación u otra. Eso se gestiona en la base de datos
    doc["ft_ins_timestamp"]=timestamp;     // mandamos el tiempo EPOCH sin corrección de la hora por el lugar. El programa al arrancar se coenta a un servidor de tiempo y actuaiza la hora
    doc["ft_ins_t1"]=round2(Tw1);          // Temperatura del agua de la sonda 1 que es la que está con el sensor de conductibidad
    doc["ft_ins_t2"]=round2(Tw2);          // Temperatura el agua 2, porporcionada por un sensor DS18B20
    doc["ft_ins_ph"]=round2(PH);           // PH
    doc["ft_ins_o2"]=round2(O2);           // Concentración de oxigeno
    doc["ft_ins_ec"]=round2(EC);           // Conductibidad ()
    doc["ft_ins_ta1"]=round2(Ta1);         // Temperatura del aire del DTH22 1
    doc["ft_ins_ha1"]=round2(Ha1);         // humedad del aire del DTH22 1
    doc["ft_ins_ta2"]=round2(Ta2);         // Temperatura del aire del DTH22 2
    doc["ft_ins_ha2"]=round2(Ha2);         // Humedad del aire del DTH22 2
    doc["ft_ins_ns"]=level_sup;            // valor del sensor de nivel 1 (no se usa por ahora)
    doc["ft_ins_ni"]=level_inf;            // valor del sensor de nivel 2 (no se usa por ahora)
    serializeJson(doc, out);
    reemplaza(out,CnullC,Cnull);  // sustituimos en la cadena null por "null"  para que la cadena quede correcta.
    //char out[]="{\"ft_ins_inst\":-1,\"ft_ins_timestamp\":1699978282,\"ft_ins_t1\":20.125,\"ft_ins_t2\":0,\"ft_ins_ph\":6.221940517,\"ft_ins_o2\":1.028320313,\"ft_ins_ec\":null,\"ft_ins_ta1\":10.89999962,\"ft_ins_ha1\":0,\"ft_ins_ta2\":11.30000019,\"ft_ins_ha2\":0,\"ft_ins_ns\":0,\"ft_ins_ni\":0}";
    SerialString(out);
    //while (!client.publish("Aquacol.INST.inst3", out) && j<3)
    while (!client.publish(inst,out) && j<N)   //Aquacol.INST.inst-1   hay que ver si se puede poner otra instalación a partir de un número en la tabal config
    {
      Serial.print("Published failled!!!! Attempt: ");
      Serial.println(j+1);
      j++;
      TCA.write1(LED_DATA, LOW);                                        // EL cuarto led de la placa se apaga en caso de NO poder mandar datos al servidor MQTT
      delay(100); // esperamos unpelo paar volver a intentrarlo.
    }
    if (j<N)
    {
      Serial.println("Published !!!!");
      TCA.write1(LED_DATA, HIGH);  
      // Chequeamos que hay datos que no se han podido mandar
      if (SD.exists(fileunsended)) 
      {
          if(Send_Stored_DATA(inst,fileunsended)) // Send_Stored_DATA devuelve 0 si no ha podido mandar todos los datos 1 si los ha mandado todos
          {
            SD.remove(fileunsended);
            Serial.println("Fichero con datos no enviados, enviado y borrado !!!!");
          }
      }                                     // EL cuarto led de la placa se enciende en caso de poder mandar datos al servidor MQTT
    }
    else
    { 
      MARCA=3;   // Aunque estamos conectados al servidor mqtt no puedo publicar 
    }

}

void Store_MQTT()
{
    JsonDocument doc;
    char out[500];
    char Cnull[]="null";
    char CnullC[]="\"null\"";
    doc["ft_ins_inst"]=config.id;          // -1 de prueba, 1 es la instalación de agronomos
    doc["ft_ins_timestamp"]=timestamp;
    doc["ft_ins_t1"]=round2(Tw1);
    doc["ft_ins_t2"]=round2(Tw2);
    doc["ft_ins_ph"]=round2(PH);
    doc["ft_ins_o2"]=round2(O2);
    doc["ft_ins_ec"]=round2(EC);
    doc["ft_ins_ta1"]=round2(Ta1);
    doc["ft_ins_ha1"]=round2(Ha1);
    doc["ft_ins_ta2"]=round2(Ta2);
    doc["ft_ins_ha2"]=round2(Ha2);
    doc["ft_ins_ns"]=level_sup;
    doc["ft_ins_ni"]=level_inf;
    serializeJson(doc, out);
    reemplaza(out,CnullC,Cnull);  // sustituimos en la cadena null por "null"  para que la cadena quede correcta.
    //char out[]="{\"ft_ins_inst\":-1,\"ft_ins_timestamp\":1699978282,\"ft_ins_t1\":20.125,\"ft_ins_t2\":0,\"ft_ins_ph\":6.221940517,\"ft_ins_o2\":1.028320313,\"ft_ins_ec\":null,\"ft_ins_ta1\":10.89999962,\"ft_ins_ha1\":0,\"ft_ins_ta2\":11.30000019,\"ft_ins_ha2\":0,\"ft_ins_ns\":0,\"ft_ins_ni\":0}";
    appendFile(SD, fileunsended, out);  // Añade el documento serializado a "unsended.txt" de la SD
}

int Send_Stored_DATA(char * inst,const char * path)
{
    char mensaje[500];           // Parece q el mensaje tiene 230 caracteres
    File file = SD.open(path);   // Abrir el archivo
    int i=0;
    char aux;
    while(file.available())
    {
        aux=file.read();
        if (aux != '\n')
        {
          mensaje[i]=aux;
          i++;
        }
        else
        {
          mensaje[i]='\0';  // mensaje terminado falta mandarlo
          if(!client.publish(inst,mensaje))   //Aquacol.INST.inst-1   hay que ver si se puede poner otra instalación a partir de un número en la tabal config
          {
              Serial.println("Error mandando dato almacenado!!! ");
              file.close();
              return 0;
          }
          else  // el dato se ha enviado con éxito
          {
              Serial.println("Dato almacenado mandado !!! ");
              delay(10); // paro un paco antes de mandar otro dato
              i=0;
          }

        }
    }
    file.close();
    return 1;
}