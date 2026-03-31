void SerialString(String cad)
{
  int i=0;
  while(cad[i]!='\0')
  {
    Serial.print(cad[i]);
    i++;
  }
  Serial.println("");
  //Serial.print("Longitud de la cadena: ");
  //Serial.println(i);
} 

void reemplaza(char * cad, char * in, char * rep)  // quiero buscar en cad la cadena in, y reemplazarla por rep
{
    int ncad=0,nin=0,nrep=0; //longitudes de las cadenas
    int j,k;                 // contadores
    int flag=0;  // 1 cuando hayamos encontrado la cadena
    while(cad[ncad]!='\0')
      ncad++;
    while(in[nin]!='\0')
      nin++;
    while(rep[nrep]!='\0')
      nrep++;
    for(j=0;j<ncad-nin;j++) // buscamos toda la cadena meno sel trozo del final, puede haber varios in
    {
      flag=0;
      if(in[0]==cad[j]) // He encontrado la primera letra
      {
          flag=1;       // supongo que está toda la palabra.
          for(k=1;k<nin && flag==1;k++) // la primera posición ya sabemos que es igual
          {
            if(cad[j+k]!=in[k])
              flag=0;                   // coincidencia parcial
          }
          if(flag==1) // Coincidencia total borramos palabra e insertamos nueva
          {
              for(k=0;k<nin;k++)
                Borra_char(cad,j); // borramos nin caracteres en la posicioón j. cada vez que borro se desplaza
              for(k=0;k<nrep;k++) // nrep < nin
                inserta_char(cad, j+k, rep[k]);
          }      
      }
    }

} 
void Borra_char(char * in, int i)  // Borro el caractar i de la cadena i
{
  int n=0; //longitud de la cadena
  while(in[n]!='\0')
    n++;
  for(int j=i;j<n;j++) // puede dejar \0 repetidos al final
  {
    in[j]=in[j+1];
  }
} 
void inserta_char(char * in, int i, char c)  // inserto en la posición i  el caracter c supongo tamaño suficiente
{
  int n=0; //longitud de la cadena
  while(in[n]!='\0')
    n++;
  for(int j=n;j>=i;j--)
  {
    in[j+1]=in[j];
  }
  in[i]=c;
}