void  Get_Tw_value(void)
{
/*****************************************************************************************/
/*                          Temperatura del agua DS18B20                                 */
/*****************************************************************************************/
  float aux1,aux2;
  // Sensores de temperatura del agua
  DS18B20.requestTemperatures();
  //aux1=DS18B20.getTempC(sensor1);       // sensor1 es la dirección del sensor 1 Hay que obtener las direcciones de los sensores de temperatura
  //aux2=DS18B20.getTempC(sensor2);
  aux2 = DS18B20.getTempCByIndex(0);   // el Index 1 es el segundo sensor
  //if(aux1 != -127.0){
  //  Tw1 = aux1;   // En caso de que no se reciva dato no actualizamos el dato de TW
  //}
  if(aux2 != -127.0){
    Tw2 = aux2;   // En caso de que no se reciva dato no actualizamos el dato de TW
  }
}