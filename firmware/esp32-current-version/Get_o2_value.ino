void Get_o2_value()
{ 
  // https://wiki.dfrobot.com/Gravity__Analog_Dissolved_Oxygen_Sensor_SKU_SEN0237
  float V_saturation;                                                         // Tensión de saturación
  static float o2_fifo[10]={0,0,0,0,0,0,0,0,0,0};  // Voy a sumar los 10 últimos valores y esos los doy como medida del O2
 
  const int DO_Table[41] = {                                                  // Tabla de constantes para la obtención indirecta del oxigeno disuelto
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};

  if(MODE_CALIBRATION_O2 == 0){                                                   // Cálculo de la tensión de saturación (Calibración a 1 punto)
    V_saturation = (float)(config.CAL1_V + 35 * Tw1 - config.CAL1_T * 35);
  }
  else{                                                                           // Cálculo de la tensión de saturación (Calibración a 2 puntos)
    V_saturation = (float)((Tw1 - config.CAL2_T) * (config.CAL1_V - config.CAL2_V) / (config.CAL1_T - config.CAL2_T) + config.CAL2_V);
  }
  
  o2_raw = ads.readADC_SingleEnded(o2_pin);
  o2_fil=0;
  for(int i = 8; i >= 0; i--){
    o2_fifo[i+1]=o2_fifo[i];
    o2_fil+=o2_fifo[i];
  }
  o2_fifo[0]=o2_raw;
  o2_fil+=o2_fifo[0];
  o2_fil=o2_fil/10.0;
  o2_voltage = ads.computeVolts(o2_fil);              // Obtener la tensión en mV que ofrece el ADC calibrado
  o2_voltage_cal = ads.computeVolts(o2_raw); 
  O2 = (float)(o2_voltage * (DO_Table[(int)Tw1]) / V_saturation);   // Cálculo del oxígeno disuelto
  O2 = O2 / 1000.0;
  if (O2<0)
    O2=0;
} 