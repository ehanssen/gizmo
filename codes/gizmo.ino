
/*
   Arduino and 2 * MPU6050 Accelerometers for ultramicrotome angle reading
   very bad code (but works) from Eric Hanssen and Phil Francis inspired from https://howtomechatronics.com/tutorials/arduino/arduino-and-mpu6050-accelerometer-and-gyroscope-tutorial/
   Ian Holmes Imaging Centre, Bio21 Institute, The University of Melbourne, Australia
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MPU6050_tockn.h>
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const int mpu1 = 0x69; //MPU6050 on sample chuck

MPU6050 mpu2(Wire); // MPU6050 I2C addresses on knife block
long timer = 0;

//Pushbutton definition start

#define ResetPin 4

int lastButtonState=0;
int currentButtonState;

float zeroX1=0;
float zeroY1=0;
float zeroZ2=0;


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
  
  lcd.init();                      // initialize the lcd 
  lcd.init();
  lcd.backlight();

  Serial.begin(19200);
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(mpu1);       // Start communication with MPU6050 // MPU=0x69 (sample)
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  Wire.begin();                      // Initialize comunication

  mpu2.begin();                      //MPU on 0x68 (knife)
  mpu2.calcGyroOffsets(true);
}

//Reset button def starts
void ResetMPU6050(float x, float y, float z){
  //Center the Joystick at current position
  zeroX1=roll;
  zeroY1=pitch;
  zeroZ2=yaw2;
}
//Reset button def ends

void loop() {

  // === Read acceleromter data === //
  //MPU1
  Wire.beginTransmission(mpu1);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(mpu1, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - 0.58; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) + 1.58; // AccErrorY ~(-1.58)
   
  // === Read gyroscope data === //
  //MPU1 sample
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(mpu1);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(mpu1, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  //MPU2 Knife
  mpu2.update();
   
  //=== calculate roll, pitch and yaw === //
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by seconds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw2 = mpu2.getGyroAngleZ();

  // Complementary filter - combine acceleromter and gyro angle values
  gyroAngleX = 0.96 * gyroAngleX + 0.04 * accAngleX;
  gyroAngleY = 0.96 * gyroAngleY + 0.04 * accAngleY;

  //define roll (Sample rotation), pitch (sample tilt) and yaw (knife rotation)
  roll = gyroAngleX;
  pitch =gyroAngleY;
  yaw2=yaw2; //maybe remove

  //reset the values to zero on pressing Pushbutton
  if (!digitalRead(ResetPin)) {
  ResetMPU6050(roll,pitch,yaw);
  ResetMPU6050(roll2,pitch2,yaw2);
  }

  roll-=zeroX1;
  pitch-=zeroY1;
  yaw2-=zeroZ2;

	// Print a message to the LCD.
  lcd.print("ROT");
  lcd.setCursor(4,0);
  lcd.print(roll,1);
  lcd.setCursor(12,0);
  lcd.print("TLT");
  lcd.setCursor(0,1);
  lcd.print(pitch,1);
  lcd.setCursor(7,1);
  lcd.print("KNF");
  lcd.setCursor(11,1);
  lcd.print(yaw2,1);
  lcd.print("  ");

  //print angles to computer if needed
 /* Serial.print(" Xnorm = ");
  Serial.print(roll,1);
  Serial.print(" Ynorm = ");
  Serial.print(pitch,1);
  Serial.print(" Znorm = ");
  Serial.println(yaw2,1);*/

  delay(100);
}
