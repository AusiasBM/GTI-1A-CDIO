#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads1115(0x48); // address 0x48 conectado al GND

/**************************************************************************************

   Objeto Sensor

 **************************************************************************************/

class Sensor {

  public:
    Sensor();//Constructor basico, todo objeto tiene que tener un constructor.
    int calcularHumedad(int, double, double); //sale un int y le entran 3 variables.
    int calcularSalinidad(int, double, double, int);
    int calcularTemperatura(int);

};//Hasta aqui seria el punto.h.

/**************************************************************************************

   Metodos

 **************************************************************************************/

/*Constructor.
  Todos los metodos tienen que tener el nombre del objeto seguido
  de dos puntos para saber que ese meteodo pertenece a ese objeto. */
Sensor::Sensor() {

  Serial.println("Inicializando...");
  ads1115.begin(); //Inicializamos ads1115
  Serial.println("Ajustando la ganancia...");
  ads1115.setGain(GAIN_ONE);

}

/* Metodo para calular la humedad
   Diseño -->  int, double, doble --> calcularHumedad() --> int

   Entra el adc de este sensor, el valor en aire y valor en agua,
   y devuelve el porcentaje de humedad

*/
int Sensor::calcularHumedad(int adc, double valorSin, double valorCon) {

  // Declaramos las variables para la entrada analógica y para el porcentaje de humedad
  int lecturaAdc, humedad;

  // Leemos la entrada analógica para humedad
  lecturaAdc = ads1115.readADC_SingleEnded(adc); // entrada 0 - Humedad

  // Calculamos el porcentaje de la humedad
  humedad = 100 * valorSin / (valorSin - valorCon) - lecturaAdc * 100 / (valorSin - valorCon);

  // Hacemos un control de errores
  if (humedad <= 0) {
    humedad = 0;
  }

  if (humedad >= 100) {
    humedad = 100;
  }

  // Utilizamos los dos siguientes comandos para mirara lo que lee la entra analógica.
  //Serial.print("AIN0 Humedad: ");
  //Serial.println(lecturaAdc);

  return humedad;
}


/* Metodo para calcular la salinidad

    Diseño -->  int, double, doble, int --> calcularSalinidad() --> int

    Entra el adc de este sensor, el valor sin sal, con sal y el pin que soltará la corriente.
    Devuelve el porcentaje de salinidad en el agua.

*/

int Sensor::calcularSalinidad(int adc, double SinSalValue, double SalValue, int pin) {

  // Declaramos las variables para la entrada analógica y para el porcentaje de salinidad.
  int LecturaAdc, salinidad;

  // Activamos el pin 5
  digitalWrite( pin, HIGH );

  // Esperamos 100 ms
  delay(100);

  // Leemos la entrada analógica adc
  LecturaAdc = ads1115.readADC_SingleEnded(adc);

  // Paramos el pin 5
  digitalWrite( pin, LOW );

  // Calculamos el porcentaje de salinidad
  salinidad = 100 * SinSalValue / (SinSalValue - SalValue) - LecturaAdc * 100 / (SinSalValue - SalValue);

  // Hacemos un control de errores
  if (salinidad <= 0) {
    salinidad = 0;
  }

  if (salinidad >= 100) {
    salinidad = 100;

  }

  return salinidad;
}

/* Metodo para calcular la temperatura

    Diseño --> int --> calcularTemperatura() --> int

    Entra el adc de este sensor y devuelve la temperatura

*/

int Sensor::calcularTemperatura(int adc) {

  // Declaramos las variables para la entrada analógica y para el porcentaje de temperatura y para la transformación de voltios a bytes.
  double lecturaAdc, temp, vol;

  // Leemos la entrada analógica para humedad
  lecturaAdc = ads1115.readADC_SingleEnded(adc); // entrada 0 - Humedad

  // Pasamos los voltios que recibe el adc a bytes
  vol = ((lecturaAdc * 4096) / 32767) / 1000;

  // Pasamos los bytes a temperatura
  temp = ((vol - 0.79) / 0.033);

  return temp;
}
