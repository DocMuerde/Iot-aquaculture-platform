void Get_EC_value()
{
  /*  https://wiki.dfrobot.com/SKU_SEN0451_Gravity_Analog_Electrical_Conductivity_Sensor_PRO_K_1 
      https://github.com/cdjq/DFRobot_ECPRO
  #define ph_pin 0                     // Pin sensor pH (analógico), Son las direcciones del nmódulo ADC115 
  #define ec_pin 2                     // Pin sensor de conductividad electrica Son las direcciones del nmódulo ADC115 
  #define o2_pin 3                     // Pin sensor oxigeno disuelto Son las direcciones del nmódulo ADC115 
  #define te_pin 3                     // Pin sensor de temperatura incorporado en el de conductividad. Son las direcciones del nmódulo ADC115 
  */
  static float te_fifo[10]={0,0,0,0,0,0,0,0,0,0};  // Voy a sumar los 10 últimos valores y esos los doy como medida de TW1
  float te_fil=0;

  for(int i = 8; i >= 0; i--){
    te_fifo[i+1]=te_fifo[i];
    te_fil+=te_fifo[i];
  }
  te_voltage = ads.computeVolts(ads.readADC_SingleEnded(te_pin));       // en V
  te_fifo[0]=te_voltage;
  te_fil+=te_fifo[0];
  Tw1=te_fil/10.0;

  Tw1= EC_convVoltagetoTemperature_C(te_voltage);

  ec_voltage = 1000.0*ads.computeVolts(ads.readADC_SingleEnded(ec_pin));  // en mV,Debería funcionar
  EC = EC_getEC_us_cm(ec_voltage,Tw1);

}
/*
float volCal(float input){
  return input;
}*/

/*
float DFRobot_ECPRO_PT1000::convVoltagetoTemperature_C(float voltage)
{
  voltage = volCal(voltage);
  float Rpt1000 = (voltage/GDIFF+VR0)/I/G0;
  float temp = (Rpt1000-1000)/3.85;
  return temp;
}*/
float EC_convVoltagetoTemperature_C(float voltage)
{
  float Rpt1000 = (voltage/GDIFF+VR0)/I/G0;
  float temp = (Rpt1000-1000)/3.85;
  return temp;
}
/*
float DFRobot_ECPRO::getEC_us_cm(float voltage, float temperature)
{
  voltage = volCal(voltage);
  float ecvalueRaw = 100000 * voltage / RES2 / ECREF * this->_kvalue;  // Supongo que this->kvalue es la ganancia K del sensor
  float value = ecvalueRaw / (1.0 + 0.02 * (temperature - 25.0));      // Este 25 es la temperatura de calibración, y si pongo otra? 
  return value;
}*/
float EC_getEC_us_cm(float voltage, float temperature)
{
  float ecvalueRaw = 100000 * voltage / RES2 / ECREF * config.Kec;  
  float value = ecvalueRaw / (1.0 + 0.02 * (temperature - config.Tec));  // ¿NO ha de ser 25?, la librería 25 siempre
  return value;
} // ¿No se podría sacar de aquí el valor de K con el dato de la temperatura y el valor de la conductividad?



/*bool DFRobot_ECPRO::setCalibration(float calibration)
{
  if((calibration>=0.5)&&(calibration<=1.5))
  {
    this->_kvalue = calibration;
    EEPROM_write(KVALUEADDR, this->_kvalue);
    return 1;
  }else
    return 0;
}*/
// En el programa hace la siguiente calibración:   ec.setCalibration(ec.calibrate(EC_voltage));
/*float DFRobot_ECPRO::getCalibration()
{
  EEPROM_read(KVALUEADDR, this->_kvalue);
  if (this->_kvalue == 0xff)
  {
    this->_kvalue = 1.0; // For new EEPROM, write default value( K = 1.0) to EEPROM
    EEPROM_write(KVALUEADDR, this->_kvalue);
  }
  return this->_kvalue;
}*/
/*float DFRobot_ECPRO::calibrate(float voltage). // Es la función que calcula K con el líquido de referencia mS=1413
{
  voltage = volCal(voltage);
  float KValueTemp = RES2*ECREF*1413/100000.0/voltage;    // 1413 es la conductibidad del líquido 
  return KValueTemp;
}*/
/*float DFRobot_ECPRO::calibrate(float voltage, float reference) // Es la función que calcula K con el líquido de referencia la que sea. Es la que vamos a usar
{                                                                // Para calibrar no tiene en cuenta la temperatura ... debe ser 25 por narices
  voltage = volCal(voltage);
  float KValueTemp = RES2*ECREF*reference/100000.0/voltage;
  return KValueTemp;
}*/
float EC_calibrate(float voltage, float reference)               // Es la función que calcula K con el líquido de referencia la que sea. Es la que vamos a usar
{                                                                // Para calibrar no tiene en cuenta la temperatura. Así que lo que ontenemos es la K a la temeratura que sea
  float KValueTemp = RES2*ECREF*reference/100000.0/voltage;      // reference es el valor de la disolución de referencia, (Ojo que los botes han de usarse a 25º)
  return KValueTemp;
}

