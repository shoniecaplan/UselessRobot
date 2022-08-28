/* ♥ ◦ ❀ ◦ ♥ ◦ ❀ |  shonie_caplan official  | ❀ ◦ ♥ ◦ ❀ ◦ ♥
♡ ◦ ❀ ◦ ♡ ◦ ❀ ◦ ♡   |  𝕾. 𝕮𝖆𝖕𝖑𝖆𝖓 .ᵒᶠᶠ  |   ♡ ◦ ❀ ◦ ♡ ◦ ❀ ◦ ♡
⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄
Project:
The Useless Robot v1.0
⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄
v1.0 of project finished: 2022-08-27
Contact: shonie4caplan@gmail.com
⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄⑄
*/
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Servo.h>

// Objects
Adafruit_MPU6050 mpu;
Servo s;

// Pins
const unsigned int servo_pin = 3; // Connect servo's input pin to DIGITAL PIN 3, or change this number to the pin you want

//==============================
const int cycles_b4_change = 50; // Change this number to increase/decrease the 'delay' before the robot falls over
//==============================

// With this variable, we will count the number of consecutive times the MPU-6050 has read that it is standing up
int fall_counter = 0;


void setup_servo() {
  s.attach(servo_pin);
  s.write(100);
}

void check_mpu() {
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
}

void print_accelerations(float a_x, float a_y, float a_z) {
  Serial.print("AX:");
  Serial.print(a_x);
  Serial.print(" | AY:");
  Serial.print(a_y);
  Serial.print(" | AZ:");
  Serial.print(a_z);
  Serial.println();
}

void fall_over() {
  Serial.println("TURNING SERVO =====\n");
  // Fall over
  s.write(170);
  delay(100);
  s.write(25);
  delay(300);

  // Twitch leg
  for ( int i=1; i < 3; i++ ) {
    s.write(120);
    delay(i * 2 * 100);
    s.write(100);
    delay(i * 2 * 200);
  }
  s.write(120);
  delay(700);
  
  // Droop Leg
  for (int i=120; i > 30; i--) {
    s.write(i);
    delay( (121-i)/10 );
  }
  delay(5000);
  
  // Reset leg
  s.write(100);
  delay(500);
}


void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  }
  
  setup_servo();
  check_mpu();

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println();
  delay(100);
}

void loop() {

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /*
    Function to print out accelerations to serial monitor. Comment-out this function after it is working, to improve efficiency.
    When the table is standing, you should be getting something close to:
    ax = 0, ay = 0, az = -9.8 (± 0.5 for all of them)
  */
   print_accelerations( a.acceleration.x, a.acceleration.y, a.acceleration.z );

  float ax,ay,az; ax=ay=az=0;
  ax = a.acceleration.x;
  ay = a.acceleration.y;
  az = a.acceleration.z;

  // Convert the x & y accelerations to a positive value, if negative
  ax < 0 ? ax *= (-1) : ax=ax;
  ay < 0 ? ay *= (-1) : ay=ay;

  // If the MPU reads that its standing up, increase fall_counter, if not, reset fall_counter to 0.
  ( ax < 1.5 && ay < 1.5 && az < 0 ) ? fall_counter++ : fall_counter = 0;

  /*
    If the number of reads that the MPU-6050 has been standing, is equal to cycles_b4_change
    then the robot falls over. If not, do nothing.
    You can set the amount of times the MPU has to read a 'standing' value, in a row, by changing 'cycles_b4_change'.
  */
  ( fall_counter == cycles_b4_change ) ? ( fall_over(), fall_counter = 0 ) : ( fall_counter = fall_counter );

  delay(64);
}
