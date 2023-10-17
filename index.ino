#include <Adafruit_LiquidCrystal.h>
#include <Servo.h>
#include <IRremote.h>

Servo servo_9;
Adafruit_LiquidCrystal lcd_1(0);

int pos = 0;
int inches = 0;
int cm = 0;
int button = -1;

int mapCodeToButton(unsigned long code) {
  // For the remote used in the Tinkercad simulator,
  // the buttons are encoded such that the hex code
  // received is of the format: 0xiivvBF00
  // Where the vv is the button value, and ii is
  // the bit-inverse of vv.
  // For example, the power button is 0xFF00BF000

  // Check for codes from this specific remote
  if ((code & 0x0000FFFF) == 0x0000BF00) {
    // No longer need the lower 16 bits. Shift the code by 16
    // to make the rest easier.
    code >>= 16;
    // Check that the value and inverse bytes are complementary.
    if (((code >> 8) ^ (code & 0x00FF)) == 0x00FF) {
      return code & 0xFF;
    };
  };
  return -1;
};

int readInfrared() {
  int result = -1;
  // Check if we've received a new code
  if (IrReceiver.decode()) {
    // Get the infrared code
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;
    // Map it to a specific button on the remote
    result = mapCodeToButton(code);
    // Enable receiving of the next value
    IrReceiver.resume();
  };
  return result;
};

long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
};

void setup()
{
  IrReceiver.begin(2);
  servo_9.attach(9, 500, 2500);
  lcd_1.begin(16, 2);
};

void loop()
{
  if(true){
   pos = 90;
   servo_9.write(pos); 
  };
  if (button >= 0) {
      button = readInfrared();
  };
  
  cm = 0.01723 * readUltrasonicDistance(7, 7);
  
  lcd_1.setCursor(0, 0);
  lcd_1.print("cm:");
  lcd_1.setCursor(0, 1);
  lcd_1.print(cm);
  
}