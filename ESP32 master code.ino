#include <MPU6050_tockn.h>
#include <Wire.h>

#define I2C_SLAVE_ADDR 0x04 // Address 4

// HC-SR04 ultrasonic sensor.
#include <NewPing.h>

#define TRIGGER_PIN  18  // ESP32 pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     34  // ESP32 pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 100 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

//MPU9250 CODE
MPU6050 mpu6050(Wire);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();                // join I2C bus with address #4
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}

void loop() {
  // put your main code here, to run repeatedly:
  double endangle = mpu6050.getAngleZ() + 180;
  int temp = 0;
  
  MPU();
  ultrasonicSensor();

  //SECTION 0
  drivingMovement(255, 255, 90);
  delay(1000);
  drivingMovement(0, 0, 90);
  delay(1000);
  
  //SECTION 1
  while (temp == 0)
  {
    while(mpu6050.getAngleZ() < endangle)
    {
      drivingMovement(255, 255, 0);
      MPU ();
      ultrasonicSensor ();
      temp = breakLoop(mpu6050.getAngleZ());
      break;   
    }
  }
  
  //SECTION 2
  while (temp == 1)
  {
    drivingMovement(-200, -200, 90);
    MPU ();
    ultrasonicSensor ();
    while (sonar.ping_cm() < 10 && sonar.ping_cm() != 0)
    {
    drivingMovement(0, 0, 90);
    temp = loop (sonar.ping_cm());
    }   
  }
  
  //SECTION 3
  while (temp == 2)
  {
    double endangle2 = mpu6050.getAngleZ() + 90;
    while(mpu6050.getAngleZ() < endangle2 )
    {
      drivingMovement(255, 255, 160);
      MPU ();
      ultrasonicSensor ();
      temp = breakLoop2 (mpu6050.getAngleZ());
      break; 
    }
  }
  
  //SECTION 4
  while (temp == 3)
  {
    drivingMovement(-200, -200, 90);
    MPU ();
    ultrasonicSensor ();
    while (sonar.ping_cm() < 10 && sonar.ping_cm() != 0)
    {
    drivingMovement(0, 0, 90);
    temp = loop2 (sonar.ping_cm());
    }
  }

  //SECTION 5
  while (temp == 4)
  {
    drivingMovement(0, 0, 90);
  }
}

void MPU ()
{
  mpu6050.update();
  Serial.print("angleX : ");
  Serial.print(mpu6050.getAngleX());
  Serial.print("\tangleY : ");
  Serial.print(mpu6050.getAngleY());
  Serial.print("\tangleZ : ");
  Serial.println(mpu6050.getAngleZ());
}

void ultrasonicSensor ()
{
  Serial.print("Ping: ");
  Serial.print(sonar.ping_cm()); // Send ping, get distance in cm and print result (0 = outside set distance range)
  Serial.println("cm");  
}
void drivingMovement (int leftMotor_speed, int rightMotor_speed, int steeringAngle)
{
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  Wire.write((byte)((leftMotor_speed & 0x0000FF00) >> 8));    // first byte of x, containing bits 16 to 9
  Wire.write((byte)(leftMotor_speed & 0x000000FF));           // second byte of x, containing the 8 LSB - bits 8 to 1
  Wire.write((byte)((rightMotor_speed & 0x0000FF00) >> 8));    // first byte of y, containing bits 16 to 9
  Wire.write((byte)(rightMotor_speed & 0x000000FF));           // second byte of y, containing the 8 LSB - bits 8 to 1
  Wire.write((byte)((steeringAngle & 0x0000FF00) >> 8));    // first byte of y, containing bits 16 to 9
  Wire.write((byte)(steeringAngle & 0x000000FF));           // second byte of y, containing the 8 LSB - bits 8 to 1
  Wire.endTransmission(); // stop transmitting 
}

int breakLoop (float angle)
{
  while (angle > 170 && angle < 210)
  {
    return 1;
  }
  return 0;
}

int loop (float distance)
{
  while (distance < 10)
  {
    return 2;
  }
  return 1;
}

int breakLoop2 (float angle2)
{
  while (angle2 > 80 && angle2 < 120)
  {
    return 3;
  }
  return 2;
}

int loop2 (float distance2)
{
  while (distance2 < 10)
  {
    return 4;
  }
  return 3;
}