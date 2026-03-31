  /* Preferencias -> Aditional boards manager URLs copiamos este link
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
herramientas -> placas -> gestor de tarjetas en el buscador ponemos esp32
instalamos:esp32 by Espressif Systems*/
// Ya no usamos la placa firebeetle esp32, ahora usamos esp32 dev module que si tiene soporte (es el esp32 dentro del firebeetle)
/* En esta nueva versión:
- Vamos a implementar el control de PH controlando el tiempo que tiene que estar la bomba ON en función de la diferencai de PH que se quiera corregir (listo)
- ¿se puede programar una seguridad para que el PH no supere el valor de 7.5 ? Habrá que usar un fedforward .... interesante hacerlo
- Repasar porque en la tarjeta SD los voltages de calibración del PGH se guardan en voltios
- Pantalla KPH el valor ronda los 50000 ms/PH cambiar a segundos mejor

*/
/*****************************************************************************************/
/*                                  Librerías.                                           */
/*****************************************************************************************/
//#include <Arduino.h>
#include <Wire.h>
//#include <LiquidCrystal_I2C.h> // Solo encuentro en el reppositorio la de franK Brabander 
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include "RTClib.h"             // Librería del reloj RTC                           [Adafruit]
#include "OneWire.h"            // Librería OneWire para los sensores DS18B20       [Paul Stoffregen]
#include "DallasTemperature.h"  // Librería para lectura de sensores DS18B20        [Miles Burton]
#include "SPI.h"                // Librería para bus comunicación SPI               [Hristo Gochkov]
#include "SD.h"                 // Libraría para gestión de memoria SD              [Arduino, SparkFun]. He tenido que meterme en la librería de Arduino y borrar la que trae
#include "DHT.h"                // Librería para sensores ambientales DHT22         [Adafruit]
#include "WiFi.h"               // Librería para comunicación WIFI                  [Hristo Gochkov]
#include "time.h"               // librería para el manejo del tiempo (seguramente la instala RTCLib) 
#include "PubSubClient.h"       // Librería para comunicación MQTT                  [Nicholas O'Leary]
#include "ArduinoJson.h"        // Librería para mandar y recibir datos Json        [Benoit Blanchon]
#include "TCA9555.h"            // Librería para trabajar con el modulo de expasion de entradas y salidas digitales I2C TCA9555
#include "Adafruit_ADS1X15.h"   // Libreía para trabajar con el convertidor de entradas analógicsa ADC115
//#include "DFRobot_ECPRO.h"    // Libreía para trabajar con el sensor SEN0451 de conductividad de DFRobot, incluyo las funciones modificadas en Get_ec_value
#include <ESP32Servo.h>         // Include the ESP32 Arduino Servo Library instead of the original Arduino Servo Library
#include <HTTPClient.h>           // Para envío HTTPS a Firebase
#include "config_firebase.h"      // Configuración Firebase
/*****************************************************************************************/
/*                                  Pines.                                               */
/*****************************************************************************************/

#define b1_pin 25  // U14 es el botón (U12,U13, no se pueden usar pues están conectados al puerto serie). Conectado al pin de interrupción del módulo de expasión PFC8574
/* I2C
Vamos a añadir otro dispositivo I2C al bus tenemos 2, el reloj y el LCD hay que saber cuales con las diercciones I2C
RTC   0x57            // Real TIme Clock
LCD   0x27            // Display
ADC1115   0x48         // módulo de entradas analogicas la dirección es la 0x48 pero no se establece en ningún lado
TCA9535   0x20        // modulo de apliacion de 16 señales digitales, tiene 3 bits para configurar la dirección que va de 0x20 hasta 0x27 (Ojo con el LCD)
/* SALIDAS *
P00 - RELE_1
P01 - RELE_2
P02 - LED_SD
P03 - LED_WIFI
P04 - LED_MQTT
P05 - LED_DATA
P06 - DIG_OUT_1
P07 - DIG_OUT_2
 * ENTRADAS *
P8 - BOYA 1. No se porqué en el micro viene como P10, pero a nivel de programación es el pin 8
P9 - BOYA2.  Igual para todos los demás a partir del 8
P10 - MODE 
P11 - UP
P12 - DOWN
P13 - OK
P14 - DIG_IN_1
P15 - DIG_IN_2
*/
#define ph_pin 0   // Pin sensor pH (analógico), Son las direcciones del nmódulo ADC115
#define o2_pin 1   // Pin sensor oxigeno disuelto Son las direcciones del nmódulo ADC115
#define ec_pin 2   // Pin sensor de conductividad electrica Son las direcciones del nmódulo ADC115
#define te_pin 3  // Pin sensor de temperatura incorporado en el de conductividad (En la placa es AUX)
#define sd_cs 2    // Pin conectado al SD_CS
/*
Miso - IO19 (MISO)
Mosi - IO23 (MOSI)
SCK - IO18 (SCK)
CS - I02/D9
VCC y GND
*/
#define dht22_pin1 26    // Pin sensor DHT22 numero 1 (digital)
#define dht22_pin2 27    // Pin sensor DHT22 numero 2 (digital)
#define float_sup_pin 8  // Pin del sensor float switch superior (digital). Conectado el módulo TCA9535
#define float_inf_pin 9  // Pin del sensor float switch inferior (digital). Conectado el módulo TCA9535
#define ev_pin 0         // Pin del canal 1 del relé. Conectado al P00 el módulo TCA9535
#define bomb_pin 1       // Pin del canal 2 del relé. Conectado al P01 el módulo TCA9535
//#define bomb_pin 7       // Pin del canal 2 del relé. Conectado al P07 el módulo TCA9535. La bomba dosificadora puede tirar directamente de una fuente de aliomentacion de 5V que de más de 2 A
#define mode_pin 10      // Pin del boton mode. Conectado al P10 el módulo TCA9535
#define up_pin 11        // Pin del boton up.   Conectado al P11 el módulo TCA9535
#define down_pin 12      // Pin del boton down. Conectado al P12 el módulo TCA9535
#define ok_pin 13        // Pin del boton ok.   Conectado al P13 el módulo TCA9535
#define LED_SD 2         // Pin del LED_SD.   Conectado el módulo TCA9535
#define LED_WIFI 3       // Pin del LED_WIFI. Conectado el módulo TCA9535
#define LED_MQTT 4       // Pin del LED_MQTT. Conectado el módulo TCA9535
#define LED_DATA 5       // Pin del LED_DATA. Conectado el módulo TCA9535
#define servoPin 15      // GPIO pin used to connect the servo control. Es el pin A4 usado como salida. La BOMBA de DFRobot se controla con la librería servo

#define onewire_pin 16         // Pin de bus OneWire. Cada sensor tiene su propia dirección que hay q obtener
#define MODE_CALIBRATION_O2 0  // MODE_CALIBRATION_O2 = 0 --> Calibracion O2 a un punto / MODE_CALIBRATION = 1 --> Calibracion O2 a dos puntos

#define MQTT_MAX_MESSAGE_SIZE 500  // tamaño máximo del paquete que se manda por mqtt
#define mqtt_conn_lim 15000        // Límite de tiempo de espera para la conexión MQTT
#define wifi_conn_lim 15000        // Límite de tiempo de espera para la conexión WiFi
#define MQTT_PORT 1883

// Constantes que usa la librería DFRobot_ECPRO.h. no se si quitar la librería y reprogramar
#define RES2 820.0
#define ECREF 200.0
#define GDIFF (30/1.8)
#define VR0  0.223
#define G0  2
#define I  (1.24 / 10000)

/*****************************************************************************************/
/*                                  GENERACIÓN DE OBJETOS                                */
/*****************************************************************************************/

hd44780_I2Cexp lcd(0x27, 20, 4);
RTC_DS3231 rtc;                       // Reloj en tiempo real, tb se podría usar el interno del esp32 pero no guarda la hora en caso de corte de corriente
OneWire ourWire(onewire_pin);         // Objeto OneWire. Bus OneWire en pin 16
DallasTemperature DS18B20(&ourWire);  // Objeto para sensores de temperatura DS18B20 en bus OneWire
DHT dht_1(dht22_pin1, DHT22);         // Objeto para sensor ambiental DHT22 numero 1
DHT dht_2(dht22_pin2, DHT22);         // Objeto para sensor ambiental DHT22 numero 2
WiFiClient espClient;                 // Objeto de cliente wifi
PubSubClient client(espClient);       // Publicador subscriptor MQTT
TCA9555 TCA(0x20);                    // Expansor E/S digitales I2C
Adafruit_ADS1115 ads;                 // modulo de entradas analógicas. Use this for the 16-bit version 
// DFRobot_ECPRO ec;                  // Para medir conductividad, Uso las funciones sin librería
// DFRobot_ECPRO_PT1000 ecpt;         // la corregir la conductividad con la temperatura 
Servo myservo;                        // create servo object to control a servo (Bomba de DFRobot)

/*****************************************************************************************/
/*                              Variables globales                                       */
/*****************************************************************************************/

const int N=2;                 // Número de veces que intento reconectar a la WIFI y mandar un dato
unsigned long timestamp;       // Variable de instante de adquisición de datos de sensores para su almacenamiento
float Ta1;                     // Temperatura ambiente 1 (DHT22 1)
float Ha1;                     // Humedad ambiente 1     (DHT22 1)
float Ta2;                     // Temperatura ambiente 2 (DHT22 2)
float Ha2;                     // Humedad ambiente 2     (DHT22 2)
String DATE, TIME;             // Variables para guardar el tiempo
float Tw1 = 0, Tw2 = 0;        // Tempraturas del agua. TW1 nos la dá el sensor de conductividad y TW2 el Sensor DS18B20
float ph_voltage, PH;          // Variables para la obtencion del PH
float o2_voltage = 0, O2 = 0;  // Variables para la obtencion de la concentración de Oxigeno
float ec_voltage = 0, EC = 0;  // Variables para la obtencion de la conductividad
float te_voltage = 0, TE = 0;  // Variables para la obtencion de la conductividad
float ph_voltage_cal = 0;      // Voltage actual sin filtro para calibrar y no tener que esperar. Se muestra por la pantalla de calibración
float o2_voltage_cal = 0;      // Voltage actual sin filtro para calibrar y no tener que esperar. Se muestra por la pantalla de calibración
float ec_voltage_cal = 0;      // Voltage actual sin filtro para calibrar y no tener que esperar. Se muestra por la pantalla de calibración
float pH_raw = 0;              // Valor de salida del ADC para el pH (0-4095)
float pH_fil = 0;
float o2_raw = 0;
float o2_fil = 0;
volatile int pantalla = 0;  // Dependiendo del valor enseñará unas cosas u otras por la pantalla

// Definimos una estructura para guadar la configuración de los sensores ¿de la wifi tb?
// Se actualiza en load configuration
struct Config {
  char ssid[20];
  char password[20];
  // PH
  float PH1, PH2, mV1, mV2;
  // O2
  float CAL1_V, CAL1_T, CAL2_V, CAL2_T;  //
  //// EC
  float Kec,Tec,mS;  // Se va a guardar la K y la temperatura a la que se midió la misma (Para la corrección por temperatura) y los mS de la disolución patrón que se uso la última vez ( esto es más para comodidad de la calibración)
  int id;            // este va a ser un parámetro que se usa para identificar la instalación  1 será agronomos, -1 es la de prueba 
  float KPH,PHref;    // Para el control, tiempo que está la bomba en ON por 1 de PH de direrencia respecto a la consigna que PHref            // Implementar estas dos cosas
  bool ContPH;        // Control automatico del PH activo
};

const char* filename = "/config.txt";  // <- SD library uses 8.3 filenames. fichero en el que se almacenan la configuración
const char* fileunsended = "/unsended.txt";  // <- SD library uses 8.3 filenames. fichero en el que se almacenan los datos que no se han podido mandar
const char* filedata ="/data_sensors.csv";   // fichero con los datos formateados para leerlos con un excel
Config config;                         // <- global configuration object

// Addresses of 2 DS18B20s Hay que ejecutar el ejemplo OneWire->DS18Bx20_Temperature (creo que he hecho unprograma que saca las direcciones como deben ser)
// hay q poner en que pin está el bus OneWire
// nos devuelve por pantalla, depues de la palabra ROM la dirección en hexadecimal
// ROM: 28 D2 89 78 54 9 11
// En el vector que guarda la dirección hay que poner 0x seguiodo del número (con dos cifras) 28 14 5D 67 54 22 9 DF
// uint8_t sensor1[8] = {0xF7, 0xFF, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //OJO completar con el 0 antes del 9
// uint8_t sensor1[8] = { 0x28, 0xD2, 0x89, 0x78, 0x54, 0x22, 0x09, 0x11 };  //OJO completar con el 0 antes del 9
// uint8_t sensor2[8] = { 0x28, 0xFF, 0x64, 0x1F, 0x42, 0x76, 0x7A, 0x00 };


const char* mqtt_server = "20.123.83.216";  // Broker remoto MQTT
const char* clientId = "ESP32Client";
const char* mqtt_user_name = "aquacolvhost:us_aquacol";
const char* mqtt_password = "aquacol";
const char* ntpServer = "in.pool.ntp.org";

bool SD_st = false, WIFI_st = false, MQTT_st = false, DATA_st = false;  // Estados de los leds
bool rele1_st = true, rele2_st = true;                                  // Estados de los relés, Inicialmente a true para que se desactiven
int level_sup, level_inf;                                               // Estado de sensores float switch
int flag_LCD = 0;                                                       // Bandera para alternar la impresión de los datos en el LCD
volatile bool flag = false, flag2 = false;                              // Baderas para ver el estado de los botones flag para ver que se ha producido una pulsación , flag2 si esta es válida
float TPH=0;                                                            // Tiempo que está la bomba de PH en ON 

/*****************************************************************************************/
/*                                   Tiempos.                                            */
/*****************************************************************************************/

const unsigned int T_p = 5000;  // Cada T_p milisegundos se actuliza los valores de la pantalla en caso de q pantalla=0
const unsigned int T_p2 = 500;  // Cada T_p2 milisegundos se actuliza los valores de la pantalla en caso de q pantalla!=0 (calibrando)
//const int SD_mul = 12 * 15;     // Cada SD_mul*T_p guardamos datos en la sd, en ppio cada 15 min. 
const int SD_mul = 12;
const int PH_mul = 4 ;          // cada PH_mul*SD_mul_Tp se controlará el valor del PH 
int MARCA = 0;                  // Si MARCA =0 todo OK.  Almacenaremos marca en la SD para saber q datos no se han enviado
                                // Si MARCA =1 wifi no disponible
                                // Si MARCA =2 No se ha podido conectar con servidor mqtt
                                // Si MARCA=3  No se ha podido mandar el dato
// Variables para almacenar instantes de tiempo anteriores, Objeto para el reloj RTC
unsigned long prev_time1 = 0;           // Marca para leer sensores
unsigned long prev_time2 = 0;           // Para mandar Datos
volatile unsigned long prev_time3 = 0;  // Para quitar rebotes en la lectura de los sensores
unsigned long prev_time6 = 0;           // Si supera el tiempo límite (wifi_conn_lim): conexión wifi no establecida
unsigned long prev_time7 = 0;
unsigned long prev_time10 = 0;  // para controlar el tiempo en el que se manda el byte de control (para que no se interrunpa la conexion wifi). Pensar mejor
unsigned long timePH = 0;       // para controlar el tiempo que la bomba dosificadora de PH está en ON

/*****************************************************************************************/
/*                                   Calibracion.                                        */
/*****************************************************************************************/
// Variables auxiliares para guardar los valores temporales de calibración.
float PH1aux, PH2aux, mV1aux, mV2aux;              // se inican con el valor acual por si no se modifican que conserven el valor al almacenarlas
float CAL1_Vaux, CAL2_Vaux, CAL1_Taux, CAL2_Taux;  
float Kecaux,mSaux,Tecaux;
float KPHaux,PHrefaux;                              // Ganancia del control y PH objetivo
bool ContPHaux;
int idaux;                               
/*****************************************************************************************/
/*                                   Zona Horaria                                        */
/*****************************************************************************************/
// España tiene GMT +1 en invierno y GMT +2 en verano
// Colombia GMT -5 y no cambia zona horaria en todo el año
typedef struct {
  int dia;
  int mes;
  int GMTmas;
} Fecha;
typedef struct {
  Fecha ppio;
  Fecha fin;
} ZonaHoraria;

ZonaHoraria ZH, ZHSPA, ZHCOL;

int hora,minu,segu,dia,mes,ano;

/*****************************************************************************************/
/*                                    Setup.                                             */
/*****************************************************************************************/

void setup() 
{
  // En españa el horario de verano es GMT+2 y el de invierno es GMT+1
  ZHSPA.ppio.dia = 31;
  ZHSPA.ppio.mes = 3;
  ZHSPA.ppio.GMTmas = 1;
  ZHSPA.fin.dia = 27;
  ZHSPA.fin.mes = 10;
  ZHSPA.fin.GMTmas = 2;

  ZHCOL.ppio.dia = 28;
  ZHCOL.ppio.mes = 3;
  ZHCOL.ppio.GMTmas = -5;
  ZHCOL.fin.dia = 28;
  ZHCOL.fin.mes = 10;
  ZHCOL.fin.GMTmas = -5;
  ZH = ZHSPA;

  //******************
  //*  E/S digitales *
  //******************
  Serial.begin(115200);
  pinMode(b1_pin, INPUT_PULLUP);          // Interrupcion cuando se pulsa un botón del ADC1115
  attachInterrupt(digitalPinToInterrupt(b1_pin), ISR, FALLING);  // Que se hace con el botón
  //  ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.setGain(GAIN_ONE);  // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV. (podemos medir hasta 4.096 V con lo que mejoramos la precisión)
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADC1115.");
    while (1);
  }
  Wire.begin(21,22); // pines SDA, SCL en tu FireBeetle/ESP32
  lcd.begin(20, 4);     // columnas, filas
  TCA.begin();  // Iniciamos el módulo TCA9535 8 primeros como salidas y los siguientes como entradas
  for (int i = 0; i < 8; i++)
    TCA.pinMode1(i, OUTPUT);
  for (int i = 0; i < 8; i++)
    TCA.pinMode1(i + 8, INPUT);
  // Establecemos valor inicial de las salidas reles a true, leds a false y auxiliares a false en ppio
  TCA.write1(ev_pin, HIGH);    // el módulo de reles funciona activandose a nivel bajo
  TCA.write1(bomb_pin, HIGH);  // el módulo de reles funciona activandose a nivel bajo
  TCA.write1(LED_SD, LOW);
  TCA.write1(LED_WIFI, LOW);
  TCA.write1(LED_MQTT, LOW);
  TCA.write1(LED_DATA, LOW);
  TCA.write1(6, LOW);  // SALIDAS AUXILIARES
  TCA.write1(7, LOW);
  // January 21, 2014 at 3am you would call:

  lcd.backlight();  // Reset display LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Config devices...");

  //********
  //*  SD  *
  //********

  delay(100);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Config SD...");
  delay(1000);
  if (SD.begin(sd_cs)) {  // Una vez inicializada, incluir nombres de sensores en CSV
    //String header = "TimeStamp; Oxigeno; pH; EC; Temp1; Temp2; Ta1; Ha1; Ta2; Ha2";
    //writeFile(SD, "/data_sensors.csv", header);
    lcd.setCursor(0, 1);
    lcd.print(">>> SD OK");
    Serial.println("SD configurada correctamente");
    TCA.write1(LED_SD, HIGH);  // EL primer led de la placa se enciende en caso de tener configuarada la tarjeta SD correctamente
    delay(1000);
  } else {
    lcd.setCursor(0, 1);
    lcd.print(">>> SD ERROR");
    Serial.println("ERROR: SD no detectada");
    TCA.write1(LED_SD, LOW);  // EL primer led de la placa se apaga en caso NO de tener configuarada la tarjeta SD correctamente
    delay(1000);
  }

  //Cargamos configuración sensores del archivo /config.txt
  loadConfiguration(filename);    //"/config.txt";   Está en la carpeta SD operations
  // Inicializamos las variables para configurar con los valores cargados
  PH1aux = config.PH1, PH2aux = config.PH2, mV1aux = config.mV1, mV2aux = config.mV2;
  CAL1_Vaux = config.CAL1_V, CAL2_Vaux = config.CAL2_V, CAL1_Taux = config.CAL1_T, CAL2_Taux = config.CAL2_T;
  Kecaux = config.Kec,Tecaux = config.Tec,mSaux = config.mS,idaux = config.id;
  KPHaux= config.KPH, PHrefaux=config.PHref, ContPHaux=config.ContPH;
  setup_wifi();  // Configuración de la conexión wifi
  // Aquí hay que iniciar el reloj RTC conectandose a internet
  rtc.begin();  // Inicializa el reloj a tiempo real
  Cal_RTC();
  // rtc.adjust(DateTime(2024, 2, 23, 14, 18, 0));  // para ajustar la hora del RTC de forma manual

  setup_mqtt();  // Configuración de la conexión MQTT

  dht_1.begin();              // Inicializa sensores de temperatura y humedad DHT22 1
  dht_2.begin();              // Inicializa sensores de temperatura y humedad DHT22 2
  DS18B20.begin();            // Inicializa sensores de temperatura DS18B20
  DS18B20.setResolution(10);  // cambiamos de 8 bits a 10 mejoramos precisión de 1 grado a 0.25 
  // Si no existe el firchero if (SD.exists(filedata))  añadimos una cabecera para que luego se entiende lo que hay dentro
  if (!SD.exists(filedata)) // Cabecera del fichero que almacena datos
  {
    String data_str2 = "timestamp;DATE;TIME;Tw1;Tw2;PH;O2;EC;Ta1;Ha1;Ta2;Ha2;Control_PH;TPH;BombaON;MARCA";      // marca =0 , dato mandado, =1 problemas wifi, =2 problemas mqtt =3 dato incorrecto 
    appendFile(SD, filedata, data_str2);  // Añade las medidas a "data_sensors.csv" de la SD
  }
  // Para el funcionamiento de la bomba 
  myservo.setPeriodHertz(50);// Standard 50hz servo
  myservo.attach(servoPin, 500, 2400);   // attaches the servo on pin 18 to the servo object
                                         // using SG90 servo min/max of 500us and 2400us
                                         // for MG995 large servo, use 1000us and 2000us,
                                         // which are the defaults, so this line could be
                                         // "myservo.attach(servoPin);"
}

/*****************************************************************************************/
/*                                    Loop.                                              */
/*****************************************************************************************/

void loop() {
  static int cont = 1, cont2=1;  
  int i = 0, j = 0;     // i contador de intentos de conexion wifi. Vamos a intentar reconectar 3 veces, si no se puede ponemos una marca a 1 y lo guardamos en la SD
                        // j contador de intentos de conexion mqtt. Vamos a intentar reconectar 3 veces, si no se puede ponemos una marca a 2 y lo guardamos en la SD

  // cuando pulsamos un botón
  if (flag) {
    pantallas();
    flag = false;
  }

  if (flag2) {
    flag2 = false;
    // Serial.print(pantalla);  // imprime el valor correspondiente a la pantalla que se está viendo 
    Show_LCD(); // Actualizmos LCD 
  }
  // Adquisición de medidas de los sensores
  if (((millis() - prev_time1 > T_p) && pantalla == 0) || ((millis() - prev_time1 > T_p2) && pantalla != 0))  // Actualizamos la pantalla. Si estamos configurando cada medio segundo (pantalla !=0) si no cada 5
  {
    prev_time1 = millis();
    Get_time();
    Get_Sensors();
    Show_LCD();
  }
  if (millis() - prev_time2 > T_p)  // Mandar datos. independiente de actualizar pantalla , ya que con ISR modificamos prev_time1
  {
    prev_time2 = millis();
    if (cont == SD_mul) {
      Get_time();
      Get_Sensors();
      MARCA = 0;     // Suponemos que todo OK
      cont = 0;      // Cuenta número de ciclos hasta el proximo envío
      i = 0, j = 0;  // Contadoresd de intentos wifi y mqtt
      lcd.setCursor(19, 3);
      lcd.print("D");
      // esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 0, &adc_chars);   // Calibración del ADC
      // Serial.println(WiFi.status());
      while (WiFi.status() != WL_CONNECTED && i < N) {
        setup_wifi();
        i++;
        //Serial.println(i);
        delay(100);  // ¿Hacemos pausa si no podemos conectar?
      }
      if (i < N)  // hay conexion wifi
      {
        while (!client.connected() && j < N)  // si no hay conexion mqtt
        {
          if(setup_mqtt())
            j=10;    // nos hemos conectado
          else
            j++;
          //Serial.println(j);
          delay(100);
        }
      } else
        MARCA = 1;  // No se ha podido conectar WIFI
      if (j >= N && j!=10) {
        MARCA = 2;  // No se ha podido conectar MQTT
      }
      if (MARCA == 0) {
        Send_MQTT();  // Envío de datos por MQTT
      }
      else
      {
        Store_MQTT();  // Almacena el mismo dato que mandaría por MQTT en la SD en el archivo almacendo en fileunsended
      }
      // SIEMPRE ejecuta Firebase (independiente de MQTT)
      Send_Firebase();  // ← Nuevo: Envío a MI sistema
      Write_SD();  // Escribimos los datos en la SD teniendo en cuenta si se ha podido mandar
    }
    if (cont2 == SD_mul*PH_mul && config.ContPH) // Código del control
    {
        cont2=0;
        if(((config.PHref-PH)-0.5)>0)
        {
          timePH=millis()+config.KPH*(config.PHref-PH); // Tiempo en el que se pararía la bomba
          TPH=config.KPH*(config.PHref-PH);                  // Para sacar el dato 
        }
        else
        {
          TPH=0;
        }         
    }
    cont++;
    cont2++;
  }
  if(config.ContPH && timePH>millis())
  {
    rele2_st = false;  // Bomba encendida
    TCA.write1(bomb_pin, rele2_st);  // Para poner la bomba en ON
    myservo.write(180); 
  }
  if(config.ContPH && timePH<=millis())
  {
    rele2_st = true;  // Bomba apagada
    TCA.write1(bomb_pin, rele2_st);  // Para poner la bomba en OFF   
    myservo.write(90); 
  }
}
