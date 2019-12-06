#include <Wire.h>
#include "SensorObjeto.h"

// Iniciar un objeto Sensor
Sensor sensores;

// Constante que ponemos para decir el tiempo que va ha estar dormido (en segundos)
const int sleepTimeS = 5;

// Pin de salinidad
int salinity_power_pin = 5;

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

/**************************************************************************************
 * 
 * SETUP
 * 
 **************************************************************************************/

void setup() {
  
  Serial.begin(9600); // Iniciamos el monitor
  Serial.println("\n\n---------DESPIERTO----------"); // Título para saber que se ha despertado
  pinMode(salinity_power_pin, OUTPUT); // Le decimos al pin 5 que va ha se de salida..

}

/**************************************************************************************
 * 
 * LOOP
 * 
 **************************************************************************************/

void loop() {
  
  Serial.print("Humedad: ");
  
  // Llamamos a la función calcularHumedad y nos calculará la humedad que recibe el sensor
  Serial.println(sensores.calcularHumedad(adcHumedad, AirValue, WaterValue)); // entra el adc 0, valor en aire y valor en agua

  Serial.print("Salinidad: ");

  // Llamamos a la función calcularSalinidad y nos calculará la salinidad que recibe el sensor
  Serial.println(sensores.calcularSalinidad(adcSalinidad, SinSalValue, SalValue, salinity_power_pin)); // entra el adc 1, valor sin sal, valor con sal y el pin 5
  
  Serial.print("Temperatura: ");

  // Llamamos a la función calcularTemperatura y nos calculará la temperatura que recibe el sensor
  Serial.println(sensores.calcularTemperatura(adcTemperatura)); // entra el adc 2

  Serial.print("-----------DORMIR----------");
  ESP.deepSleep(sleepTimeS * 1000000); // Ponemos la placa en reposo para que no gaste batería.
}
