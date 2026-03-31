void pantallas(void)
{
/* 
#define ev_pin 0                      // Pin del canal 1 del relé. Conectado al P00 el módulo TCA9535
#define bomb_pin 1                    // Pin del canal 2 del relé. Conectado al P01 el módulo TCA9535
P8 - BOYA 1. No se porqué en el micro viene como P10, pero a nivel de programación es el pin 8
P9 - BOYA2.  Igual para todos los demás a partir del 8
P10 - MODE 
P11 - UP
*/
  bool Bmode,Bup,Bdown,Bok;

  Bmode= !TCA.read1(mode_pin);
  Bup=   !TCA.read1(up_pin);
  Bdown= !TCA.read1(down_pin);
  Bok=   !TCA.read1(ok_pin);

  Get_time();
  Get_Sensors();

  if (Bmode)
  {
    pantalla++;
    flag2=true;
  }
  // Sensor de PH
  // para la configuración del ph nos hacen falta 2 puntos uno es el valor del PH y el otro de los milivoltios medidos  (PH1-PH2)/(mV1-mV2); 
  if (pantalla==1 && Bup) // la pantalla 1 sirne para ajusta el valor del PH1 
  {
    PH1aux=PH1aux+0.1;
    mV1aux=ph_voltage_cal; 
    flag2=true;
  }
  if (pantalla==1 && Bdown)
  {
    PH1aux=PH1aux-0.1;
    mV1aux=ph_voltage_cal; 
    flag2=true;
  }
  if (pantalla==2 && Bup) // calibracion del segundo punto de PH
  {
    PH2aux=PH2aux+0.1;
    mV2aux=ph_voltage_cal; 
    flag2=true;
  }
  if (pantalla==2 && Bdown)
  {
    PH2aux=PH2aux-0.1;
    mV2aux=ph_voltage_cal; 
    flag2=true;
  }

  // sensor de O2 //
  if (pantalla==3 ) // pantalla para la calibración de la concentración de oxígeno   V_saturation = (float)(CAL1_V + 35 * Tw1 - CAL1_T * 35) hay que esperar a que se estabilicen y entonces se guardan
  {
    CAL1_Vaux=o2_voltage_cal;
    CAL1_Taux=Ta1;  //Calibro con temepartura aire, si se saca la probeta 
    flag2=true;
  }
  if (pantalla==4 ) // Calibro con la temperatura del agua si calibro con agua saturada
  {
    CAL1_Vaux=o2_voltage_cal;
    CAL1_Taux=Tw2;  // Calibro con la temperatura del agua si calibro con agua saturada
    flag2=true;
  }
  if (pantalla==5 && Bup ) // Ajusto arriba disolución patrón
  {
    mSaux=mSaux+1;
    Kecaux=EC_calibrate(ec_voltage,mSaux);  // funcion definida en Get_EC_value
    flag2=true;
  }
  if (pantalla==5 && Bdown ) // Ajusto arriba disolución patrón
  {
    mSaux=mSaux-1;
    Kecaux=EC_calibrate(ec_voltage,mSaux);  // funcion definida en Get_EC_value
    flag2=true;
  }
  if (pantalla==6) // Control Manual o automático la pantalla va a depender
  {
    if(Bup)
      ContPHaux = true;  // Bomba apagada
    if(Bdown)
      ContPHaux = false; // Bomba encendida
    flag2=true;
  }
  if (pantalla==7 && !ContPHaux) // Control manual de la bomba 
  {
    if(Bdown)
      rele2_st = true;  // Bomba apagada
    if(Bup)
      rele2_st = false; // Bomba encendida
    TCA.write1(bomb_pin, rele2_st);
    TCA.write1(7, rele2_st);
    Serial.print("bomba ");Serial.println(rele2_st);
    if(!rele2_st)
      myservo.write(180); 
    else
      myservo.write(90); 
  }
  if (pantalla==7 && ContPHaux) // Control automático de la bomba de la bomba 
  {
    if(Bup)
      KPHaux+= 1;  // aunmentamos tiempo de encendido
    if(Bdown)
      KPHaux-= 1;  // disminuimos tiempo de encendido
    flag2=true;
  }
  if (pantalla==8) // Control automático de la bomba de la bomba 
  {
    if(Bup)
      PHrefaux+= 0.1;  // aunmentamos tiempo de encendido
    if(Bdown)
      PHrefaux-= 0.1;  // disminuimos tiempo de encendido
    flag2=true;
  }

  if(pantalla>8)
  {
    pantalla=0;    
  }
  if (Bok && !(pantalla == 7 && !ContPHaux)) // Bok and the pump not being Controlled manually
  {
    /* Si hemos hecho cualquier cambio en la cofiguación delos parámetros, tenemos que actualizarlos*/
    config.PH1=PH1aux;
    config.mV1=mV1aux;
    config.PH2=PH2aux;
    config.mV2=mV2aux;
    config.CAL1_V=CAL1_Vaux;
    config.CAL1_T=CAL1_Taux;
    config.CAL2_V=CAL2_Vaux;
    config.CAL2_T=CAL2_Taux;
    config.Kec=Kecaux;
    config.Tec=Tecaux;
    config.mS=mSaux;
    config.id=idaux;
    config.KPH=KPHaux;             // Implementar estas dos cosas
    config.ContPH=ContPHaux;
    config.PHref=PHrefaux;
    Serial.println("Parámetros actualizados");
    /* tendremos que guardarlos en la tarjeta SD para la proxima ejecuación que cargue los últimos parámetros usados */
    saveConfiguration(filename);
    lcd.clear();
    lcd.setCursor(0,1); lcd.print("PARAMETROS"); 
    lcd.setCursor(0,2); lcd.print("ACTUALIZADOS");
    delay(2000); 
    pantalla=0;
    flag2=true;
  }
  if (Bok && (pantalla == 7 && !ContPHaux))
  {
    pantalla=0;
  }

}