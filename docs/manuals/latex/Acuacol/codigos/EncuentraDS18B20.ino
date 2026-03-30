#include <OneWire.h>

OneWire ourWire(16);                //Se establece el pin 2  como bus OneWire

void setup(void) {
  Serial.begin(115200);
}

void loop(void) {
  byte addr[8];  
  Serial.println("Obteniendo direcciones:");
  while (ourWire.search(addr)) 
  {  
  Serial.print("Address = ");
  for( int i = 0; i < 7; i++) {
    Serial.print(" 0x");
    Serial.print(addr[i], HEX);
    Serial.print(",");
  }
  Serial.print(" 0x");
  Serial.print(addr[7], HEX);
  Serial.println();
}

Serial.println();
ourWire.reset_search();
delay(2000);
}