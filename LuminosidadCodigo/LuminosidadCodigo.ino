#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads1115(0x48);

const int OscuridadValue = 0.027;
const int SombraValue = 1.70;
const int LuzValue = 2.40;
const int MuchaLuzValue = 3.75;


void setup() {
  
  Serial.begin(9600);
  Serial.println("Inicializando...");
  ads1115.begin(); //Inicializamos ads1115
  Serial.println("Ajustando la ganancia...");
  ads1115.setGain(GAIN_ONE);

}

void loop(){

  medicionLuminosidad();
  delay(2000);

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
  
