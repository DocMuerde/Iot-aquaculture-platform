void Get_env_values()
{
  float h = dht_1.readHumidity();
  float t = dht_1.readTemperature();
// Check if any reads failed and exit early ( to try again ) .
  if  (isnan(h) || isnan(t)) 
  {
    Serial.println(F("Failed to read from DHT sensor 1 !"));
  }
  else
  {
    Ta1=t;
    Ha1=h;
  } 
  h = dht_2.readHumidity () ;
  t = dht_2.readTemperature();
// Check if any reads failed and exit early ( to try again ) .
  if  (isnan(h) || isnan(t)) 
  {
    Serial.println(F("Failed to read from DHT sensor 2 !"));
  }
  else
  {
    Ta2=t;
    Ha2=h;
  } 
}