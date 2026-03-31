/* parece que esta función ya no se usa manda datos filtrados y sin filtrar */
void write_serial(void)
{
  Serial.print("PH: ");
  Serial.print(" Sin fil: ");
  Serial.print(pH_raw);
  Serial.print(" Con fil: ");
  Serial.println(pH_fil);
  Serial.print("O2: ");
  Serial.print(" Sin fil: ");
  Serial.print(o2_raw);
  Serial.print(" Con fil: ");
  Serial.println(o2_fil);
  Serial.print("EC: ");
  Serial.print(" Sin fil: ");
  Serial.println(EC);
  //Serial.print(" Con fil: ");
  //Serial.println(ec_fil);
}