#include <Wire.h>
#include "SensorObjeto.h"
#define    MPU9250_ADDRESS            0x68
#define    MAG_ADDRESS                0x0C
#define    GYRO_FULL_SCALE_250_DPS    0x00
#define    GYRO_FULL_SCALE_500_DPS    0x08
#define    GYRO_FULL_SCALE_1000_DPS   0x10
#define    GYRO_FULL_SCALE_2000_DPS   0x18
#define    ACC_FULL_SCALE_2_G        0x00
#define    ACC_FULL_SCALE_4_G        0x08
#define    ACC_FULL_SCALE_8_G        0x10
#define    ACC_FULL_SCALE_16_G       0x18
#include <TinyGPS++.h>  //Librería del GPS
#include <SoftwareSerial.h>
#define RX_PIN  12 // GPS RXI
#define TX_PIN  13 // GPS TX0
#define INIT_PIN 15 // Pin para  Inicializar el GPS
#define GPS_BAUD  4800  //  velocidad de comunicación serie 

// Constantes y variables globales
const int OscuridadValue = 0.027;
const int SombraValue = 1.70;
const int LuzValue = 2.40;
const int MuchaLuzValue = 3.75;
const byte interruptPin = 4;
const int sleepTimeS = 5;

// Constantes Salinidad Analógicas
const int SinSalValue = 1300;  // Medimos valor sin sal 
const int SalValue = 19300;  // Medimos valor con sal

// Constantes Humedad Analógicas
const int AirValue = 20470;  // Medimos valor aire
const int WaterValue = 9600;  // Medimos valor con Agua

// ADC que hemos definido nosotros en el Hadware
int adcHumedad = 0;
int adcSalinidad = 1;
int adcTemperatura = 2;

// Iniciar un objeto Sensor
Sensor sensores;

// Definimos el objeto gps
TinyGPSPlus gps; 

// Pin de salinidad
int salinity_power_pin = 5;

// Creamos una UART software para comunicación con el GPS
SoftwareSerial ss(RX_PIN,TX_PIN); 
       
        /*FUNCIONES*/
        
//Funcion auxiliar lectura
void I2Cread(uint8_t Address, uint8_t Register, uint8_t Nbytes, uint8_t* Data)
{
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.endTransmission();

  Wire.requestFrom(Address, Nbytes);
  uint8_t index = 0;
  while (Wire.available())
    Data[index++] = Wire.read();
}


// Funcion auxiliar de escritura
void I2CwriteByte(uint8_t Address, uint8_t Register, uint8_t Data)
{
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.write(Data);
  Wire.endTransmission();
}
//Wake on motion
void handleInterrupt(){
  uint8_t intStatus[1];
  I2Cread(MPU9250_ADDRESS,0x3A, 1, intStatus);
 // Serial.println("interrupción");
}

void medicionLuminosidad(){

  double adc3, lum, luminosidad;

  adc3 = ads1115.readADC_SingleEnded(3);
  
  lum=calcularLuminosidad(adc3);
  luminosidad = 100*OscuridadValue/(OscuridadValue-MuchaLuzValue)-lum*100/(OscuridadValue-MuchaLuzValue);
  mostrarLuminosidad(luminosidad);
  //Serial.print("AIN0 Luminosidad: ");
  //Serial.println(lum);
}

double calcularLuminosidad(double adcval){
  double vol;
 
  vol = ((adcval*4096)/32767)/1000;
  return vol;
}

void  mostrarLuminosidad(double luminosidad){

   Serial.print("Hay una Luminosidad del ");

   if(luminosidad <= 100){
    
    Serial.print(luminosidad);
   
   }else{
    
    luminosidad = 100;
    Serial.print(luminosidad);
   
   }
   Serial.println(" %");

  if(luminosidad < 1.50){

    Serial.println("Por tanto o es de noche o hay mucha oscuridad.");
  }
  
  if(luminosidad > 1.50 && luminosidad < 50){

    Serial.println("Por tanto es muy posible que esté ligeramente nublado o el dispositivo se encuentre bajo sombra.");
  }
  
  if(luminosidad > 50 && luminosidad < 90){

    Serial.println("Por tanto el disposotivo se encuentra expuesto a una luz ambiente estándar.");
  }
  
  if(luminosidad >= 90 ){

    Serial.println("Por tanto el dispositivo se encuentra expuesto a mucha luz.");
  }

  delay(5000);
  }
// Función espera 1s para leer del GPS
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while(ss.available())
    {
      gps.encode(ss.read());  // leemos del gps
    }
  } while(millis() - start < ms);
}
// Función para encender/apagar mediante un pulso
void switch_on_off()
{
   // Power on pulse
  digitalWrite(INIT_PIN,LOW);
  delay(200);
  digitalWrite(INIT_PIN,HIGH);
  delay(200); 
  digitalWrite(INIT_PIN,LOW);
 }

/**************************************************************************************
 * 
 * SETUP
 * 
 **************************************************************************************/

void setup() {
  
  Serial.begin(9600); // Iniciamos el monitor
  Serial.println("\n\n---------DESPIERTO----------"); // Título para saber que se ha despertado
  Serial.println("Inicializando...");
 Serial.println("Configurando acelerómetro...");
  ads1115.begin(); //Inicializamos ads1115
 Serial.println("Ajustando la ganancia...");
  ads1115.setGain(GAIN_ONE);
  
  //SALINIDAD
  
  pinMode(salinity_power_pin, OUTPUT); // Le decimos al pin 5 que va ha se de salida..
 
  //GPS
  
  ss.begin(GPS_BAUD); // Inicializar la comunicación con el GPS
 
  pinMode(INIT_PIN,OUTPUT); 
  switch_on_off(); // Pulso para encender el GPS

  Serial.println("Fecha      Hora       Latitud   Longitud   Alt    Rumbo   Velocidad");
  Serial.println("(MM/DD/YY) (HH/MM/SS)     (deg)       (deg)  (ft)                   (mph)");
  Serial.println("-------------------------------------------------------------------------"); 

  // ACCELEROMTERO
  I2CwriteByte(MPU9250_ADDRESS, 28, ACC_FULL_SCALE_16_G);

  // WAKE ON MOTION
  I2CwriteByte(MPU9250_ADDRESS, 0x6B, 0x00);
  I2CwriteByte(MPU9250_ADDRESS, 0x6C, 0x07);
  I2CwriteByte(MPU9250_ADDRESS, 0x1D, 0x09);
  I2CwriteByte(MPU9250_ADDRESS, 0x38, 0x40);
  I2CwriteByte(MPU9250_ADDRESS, 0x69, 0xC0);
  I2CwriteByte(MPU9250_ADDRESS, 0x1F, 0x09);
  I2CwriteByte(MPU9250_ADDRESS, 0x1E, 0x08);
  I2CwriteByte(MPU9250_ADDRESS, 0x37, 0x80);
  I2CwriteByte(MPU9250_ADDRESS, 0x6B, 0x20);

  Serial.println("Configurando el dispositivo ");
  
  pinMode(interruptPin, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, CHANGE);
}

/**************************************************************************************
 * 
 * LOOP
 * 
 **************************************************************************************/

void loop() {
  
    

  
  //GPS
  
  char gpsDate[10]; 
  char gpsTime[10];

  if(gps.location.isValid()){ // Si el GPS está recibiendo los mensajes NMEA

    sprintf(gpsDate,"%d/%d/%d", gps.date.month(),gps.date.day(),gps.date.year()); // Construimos string de datos fecha
    sprintf(gpsTime,"%d/%d/0%d", gps.time.hour(),gps.time.minute(),gps.time.second());  // Construimos string de datos hora

    Serial.print(gpsDate);
    Serial.print('\t');
    Serial.print(gpsTime);
    Serial.print('\t');
    Serial.print(gps.location.lat(),6);
    Serial.print('\t');
    Serial.print(gps.location.lng(),6);
    Serial.print('\t');
    Serial.print(gps.altitude.feet());
    Serial.print('\t');
    Serial.print(gps.course.deg(),2);
    Serial.print('\t');
    Serial.println(gps.speed.mph(),2);
  }
  else  // Si no recibe los mensajes
  {
    
    Serial.print("Satellites in view: ");
    Serial.println(gps.satellites.value());
  }
  smartDelay(1000);
  
    //LUMINOSIDAD
  
  medicionLuminosidad();
  delay(2000);
  
  //HUMEDAD
  
  Serial.print("Humedad: ");
  
  // Llamamos a la función calcularHumedad y nos calculará la humedad que recibe el sensor
  Serial.println(sensores.calcularHumedad(adcHumedad, AirValue, WaterValue)); // entra el adc 0, valor en aire y valor en agua
  
  //SALINIDAD
  
  Serial.print("Salinidad: ");

  // Llamamos a la función calcularSalinidad y nos calculará la salinidad que recibe el sensor
  Serial.println(sensores.calcularSalinidad(adcSalinidad, SinSalValue, SalValue, salinity_power_pin)); // entra el adc 1, valor sin sal, valor con sal y el pin 5

  //TEMPERATURA
  
  Serial.print("Temperatura: ");

  // Llamamos a la función calcularTemperatura y nos calculará la temperatura que recibe el sensor
  Serial.println(sensores.calcularTemperatura(adcTemperatura)); // entra el adc 2

  //DEEP SLEEP
  //Serial.print("-----------DORMIR----------");
  //ESP.deepSleep(sleepTimeS * 1000000); // Ponemos la placa en reposo para que no gaste batería.
  delay(2000);
}
