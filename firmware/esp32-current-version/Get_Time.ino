void Get_time(void)
{
  DateTime time = rtc.now();              // Obtiene el instante actual en formate DateTime
  timestamp = time.unixtime();            // Convierte el dato anterior a tiempo epoch (segundos transcurridos desde el 01/01/1970)
  DATE=time.timestamp(DateTime::TIMESTAMP_DATE);
  TIME=time.timestamp(DateTime::TIMESTAMP_TIME);
  hora=time.hour();
  minu=time.minute();
  segu=time.second();
  mes=time.month();
  dia=time.day();
  ano=time.year();
  int GMT=ZH.ppio.GMTmas;
  int n=calculateDayOfYear(dia,mes,ano);
  int n1=calculateDayOfYear(ZH.ppio.dia,ZH.ppio.mes,ano);
  int n2=calculateDayOfYear(ZH.fin.dia,ZH.fin.mes,ano);
if  (n>=n1 && n<n2)
  GMT=ZH.fin.GMTmas;
hora=hora+GMT;
}