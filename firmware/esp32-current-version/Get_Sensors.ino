void Get_Sensors(void)
{

  Get_Tw_value();    // En las variables Tw2 tenemos los valores de la temperatura del agua actualizados
  Get_pH_value();    // En la variable global PH tenemos el valor actualizado
  Get_EC_value();    // En la variable global EC y Tw1 obtenemos el valor actualizado
  Get_o2_value();    // En la variable global O2 tenemos el valor actualizado
  Get_env_values();  // En las variables Ta1, Ha1 Ta2 Ha2 tenmos los valores de temperatura y humedad del aire actualizados
  level_sup = TCA.read1(float_sup_pin);   // Lectura del sensor float switch superior usando TCA9535
  level_inf = TCA.read1(float_inf_pin);   // Lectura del sensor float switch inferior usando TCA9535
}