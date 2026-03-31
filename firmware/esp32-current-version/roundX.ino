double round1(double value) {
   return (int)(value * 10 + 0.5) / 10.0;
}

double round2(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}

double round3(double value) {
   return (int)(value * 1000 + 0.5) / 1000.0;
}

float roundIgnacio (float in, int n)
{
  int pot=pow(10,n);
  float resi=round(in*pot);
  float resf=resi/pot;
  return resf;
}