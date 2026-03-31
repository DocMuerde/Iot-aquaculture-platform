void Level_Control(){
  level_sup = TCA.read1(float_sup_pin);
  level_inf = TCA.read1(float_inf_pin);
  if(level_sup == 1 && level_inf == 1){               // Depósito al nivel máximo
    TCA.write1(ev_pin, HIGH);                         // Desactivar electroválvula
  }
  else if(level_sup == 0 && level_inf == 0){          // Depósito al nivel mínimo
    TCA.write1(ev_pin, LOW);                        // Activar electroválvula
  }
  else if(level_sup == 0 && level_inf == 1){          // Lectura no coherente. ERROR
    Serial.println("ERROR EN LECTURA DE SENSORES");   
  }
}
