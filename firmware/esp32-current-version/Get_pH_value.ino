void Get_pH_value()
{
  float ph_slope = (config.PH1-config.PH2)/(config.mV1-config.mV2);             // pendiente de la recta que calcula el PH, valor inicial 3.5, Se recalcula en el setup
  float ph_offset= config.PH1 - ph_slope * config.mV1;            // Offset para calcular el PH
  int buf[10], t;  // Buffer de 10 muestras de pH y variable temporal
  int N=20;       // Tamaño del buffer              
  static float ph_fifo[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  // Voy a sumar los 20 últimos valores y esos los doy como medida del pH
  
  
  for(int i = 0; i < 10; i++){          // Lectura de 10 valores almacenándolos en array buf[]
    buf[i] = ads.readADC_SingleEnded(ph_pin);        // Lectura señal analógica de pH
    delay(10);
  }
  for(int i = 0; i < 9; i++){           // Ordenar el array en orden ascendente
    for(int j = i + 1; j < 10; j++){
      if(buf[i] > buf[j]){
        t = buf[i];
        buf[i] = buf[j];
        buf[j] = t;
      }
    }
  }
  pH_raw=0;
  for(int i = 2; i < 8; i++){                                               // Eliminar los valores extremos y calcular la media
    pH_raw += buf[i];                         
  }                
  pH_raw = pH_raw / 6.0;
  // metemos la medida en en una cola fifo que almacena los 10 últimos valores, ultimo valor en la componente cero 
  pH_fil=0;
  for(int i = N-2; i >= 0; i--){
    ph_fifo[i+1]=ph_fifo[i];
    pH_fil+=ph_fifo[i];
  }
  ph_fifo[0]=pH_raw;
  //for(int i = 0; i < N; i++)
  //{
  //  Serial.print(ph_fifo[i]);
  //  Serial.print(",");
  //}
  //Serial.println(" ");
    
  pH_fil+=ph_fifo[0];
  pH_fil=pH_fil / N;
	ph_voltage = ads.computeVolts(pH_fil);       // Obtener la tensión en mV con el ADC calibrado
  ph_voltage_cal = ads.computeVolts(pH_raw);   // Este es el valor sin filtar para la calibracion 
  PH = ph_slope * ph_voltage + ph_offset;                            // Introduzco el ofset y la pendiente a partir de dos puntos
}