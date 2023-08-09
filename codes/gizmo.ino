
/*
   Arduino and 2 * MPU6050 Accelerometers for ultramicrotome angle reading
   very bad code (but works) from Eric Hanssen and Phil Francis inspired from https://howtomechatronics.com/tutorials/arduino/arduino-and-mpu6050-accelerometer-and-gyroscope-tutorial/
   Ian Holmes Imaging Centre, Bio21 Institute, The University of Melbourne, Australia
   Gizmo v1.1
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MPU605069_tockn.h>
#include <MPU6050_tockn.h>
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

MPU605069 mpu1(Wire); //MPU6050 on sample chuck
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

  lcd.print("ROT");
  lcd.setCursor(12,0);
  lcd.print("TLT");
  lcd.setCursor(7,1);
  lcd.print("KNF");

  mpu1.begin();                      //MPU on 0x69 (sample)
  mpu1.calcGyroOffsets(true);
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
 
  // === Read data === //
  //MPU1 sample
  mpu1.update();
  //MPU2 Knife
  mpu2.update();
   
  //=== calculate roll, pitch and yaw === //
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by seconds (s) to get the angle in degrees
  roll = mpu1.getGyroAngleX();
  pitch= mpu1.getGyroAngleY();
  yaw2 = mpu2.getGyroAngleZ();


  //define roll (Sample rotation), pitch (sample tilt) and yaw (knife rotation)
  roll = roll;
  pitch =pitch;
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
  
  lcd.setCursor(4,0);
  lcd.print(roll,1);
  
  lcd.setCursor(0,1);
  lcd.print(pitch,1);
  
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
