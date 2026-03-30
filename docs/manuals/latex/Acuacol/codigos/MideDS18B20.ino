#include <OneWire.h>
#include "DallasTemperature.h"        // Librería para lectura de sensores DS18B20        [Miles Burton]

OneWire ourWire(16);                //Se establece el pin 16 como bus OneWire
DallasTemperature DS18B20(&ourWire);  // Objeto para sensores de temperatura DS18B20 en bus OneWire

/* Actualizar con las direcciones identificadas anteriormente,
completando con 0 los identificadores de 1 cifra */ 
uint8_t sensor1[8] = {0x28, 0xD2, 0x89, 0x78, 0x54, 0x22, 0x09, 0x11}; //OJO completar con el 0 antes del 9
uint8_t sensor2[8] = {0x28, 0xFF, 0x64, 0x1F, 0x42, 0x76, 0x7A, 0x00};  

void setup(void) {
  Serial.begin(115200);
}

void loop(void) {
  DS18B20.requestTemperatures();
  float aux1=DS18B20.getTempC(sensor1);       // sensor1 es la dirección del sensor 1
  float aux2=DS18B20.getTempC(sensor2);
  float Tw1=0,Tw2=0;
  if(aux1 != -127.0){
    Tw1 = aux1;   // En caso de que no se reciva dato no actualizamos el dato de TW
  }
  if(aux2 != -127.0){
    Tw2 = aux2;   // En caso de que no se reciva dato no actualizamos el dato de TW
  }
  Serial.print("Temperatura 1 = ");
  Serial.print(Tw1);
  Serial.print("   Temperatura 2 = ");
  Serial.println(Tw2);

delay(2000);
}