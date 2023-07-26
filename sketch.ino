/*
   Arduino and 2 * MPU6050 Accelerometers
   by Eric Hanssen inspired from https://howtomechatronics.com/tutorials/arduino/arduino-and-mpu6050-accelerometer-and-gyroscope-tutorial/
*/

#include <Wire.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(12,11,10,9,8,7);
const int MPU1 = 0x68, MPU2 = 0x69; // MPU6050 I2C addresses


//Pushbutton definition start

#define ResetPin 4
int lastButtonState=0;
int currentButtonState;
float zeroX1=0;
float zeroY1=0;
float zeroZ1=0;
float zeroX2=0;
float zeroY2=0;
float zeroZ2=0;
//Pushbutton definition start

 // MPU6050 I2C address
float AccX, AccY, AccZ, AccX2, AccY2,AccZ2;
float GyroX, GyroY, GyroZ, GyroX2, GyroY2, GyroZ2;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ, accAngleX2, accAngleY2, gyroAngleX2, gyroAngleY2, gyroAngleZ2;
float roll, pitch, yaw, roll2, pitch2, yaw2, x1,y1,z1,x2,y2,z2;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;

void setup() {
  pinMode(ResetPin, INPUT_PULLUP);    // initialise PushButton
  lcd.begin(16,2);                    // initialise the LCD
  Serial.begin(19200);
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU1);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU2);       // Start communication with MPU6050 // MPU=0x69
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
 
}

//Reset button def starts
void ResetMPU6050(float x, float y, float z){
  //Center the Joystick at current position
  zeroX1=roll;
  zeroY1=pitch;
  zeroZ1=yaw;
  zeroX2=roll2;
  zeroY2=pitch2;
  zeroZ2=yaw2;
}
//Reset button def ends

void loop() {

  // === Read acceleromter data === //
  //MPU1
  Wire.beginTransmission(MPU1);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU1, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - 0.58; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) + 1.58; // AccErrorY ~(-1.58)
  //MPU2
  Wire.beginTransmission(MPU2);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU2, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX2 = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY2 = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ2 = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX2 = (atan(AccY2 / sqrt(pow(AccX2, 2) + pow(AccZ2, 2))) * 180 / PI) - 0.58; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  accAngleY2 = (atan(-1 * AccX2 / sqrt(pow(AccY2, 2) + pow(AccZ2, 2))) * 180 / PI) + 1.58; // AccErrorY ~(-1.58)

  
  // === Read gyroscope data === //
  //MPU1
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU1);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU1, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  //MPU2
  Wire.beginTransmission(MPU2);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU2, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX2 = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY2 = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ2 = (Wire.read() << 8 | Wire.read()) / 131.0;

  //=== calculate roll, pitch and yaw === //
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by seconds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw =  yaw + GyroZ * elapsedTime;
  gyroAngleX2 = gyroAngleX2 + GyroX2 * elapsedTime; // deg/s * s = deg
  gyroAngleY2 = gyroAngleY2 + GyroY2 * elapsedTime;
  yaw2 =  yaw2 + GyroZ2 * elapsedTime;
  // Complementary filter - combine acceleromter and gyro angle values
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  roll2 = 0.96 * gyroAngleX2 + 0.04 * accAngleX2;
  pitch2 = 0.96 * gyroAngleY2 + 0.04 * accAngleY2;
  
  //reset the values to zero on pressing Pushbutton
  if (!digitalRead(ResetPin)) {
  ResetMPU6050(roll,pitch,yaw);
  ResetMPU6050(roll2,pitch2,yaw2);
  }

  roll-=zeroX1;
  pitch-=zeroY1;
  yaw-=zeroZ1;
  roll2-=zeroX2;
  pitch2-=zeroY2;
  yaw2-=zeroZ2;

	// Print a message to the LCD.
	lcd.print("  ROT  TLT   KNF ");
  lcd.setCursor(0,1);
  lcd.print(roll,1);
  lcd.setCursor(6,1);
  lcd.print(pitch,1);
  lcd.setCursor(11,1);
  lcd.print(yaw2,1);
  //print angles to computer
  Serial.print(" Xnorm = ");
  Serial.print(roll,1);
  Serial.print(" Ynorm = ");
  Serial.print(pitch,1);
  Serial.print(" Znorm = ");
  Serial.println(yaw2,1);

  delay(10);
}
