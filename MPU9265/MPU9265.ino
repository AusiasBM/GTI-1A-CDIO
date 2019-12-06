//GND - GND
//VCC - VCC
//SDA - Pin 2
//SCL - Pin 14

#include <Wire.h>

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

// Constantes y variables globales

const byte interruptPin = 4;
const int sleepTimeS = 5;


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

void handleInterrupt(){
  uint8_t intStatus[1];
  I2Cread(MPU9250_ADDRESS,0x3A, 1, intStatus);
  Serial.println("interrupción");
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Inicializando...");
  
  Serial.println("Configurando acelerómetro...");
  
  // Configurar acelerometro
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

  
 
void loop()
{
  ESP.deepSleep(sleepTimeS * 1000000);
}
