

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>
#include <IRremote.h>
#include <Servo.h>


#define IR_BUTTON_1 12
#define IR_BUTTON_2 24
#define IR_BUTTON_3 94
#define IR_BUTTON_PLAY_PAUSE 67

#define TRIGGER_PIN  11  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     12  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
int button;
int pos = 0;
float last_value = 0;
float current_value = 0;
float g = 9.81;
float m = 0.271;
float H_i = 0.20;
float H_f = 0 ;

bool measuring = false;

float EP_i = g*m*H_i;
float EP_f = 0;



NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 
Servo servo_9;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección I2C 0x27, 16 columnas y 2 filas

void setup() {
  Serial.begin(9600);
  servo_9.attach(9, 500, 2500);
  servo_9.write(pos);
  lcd.init();                      // Inicializar la pantalla LCD
  lcd.backlight();                 // Encender la retroiluminación (si la pantalla la tiene)
  lcd.setCursor(0, 0);
  lcd.print( "EP inicial " );
  lcd.setCursor(0, 1);
  lcd.print( String(EP_i) + " J");
  IrReceiver.begin(2);

  
   // Send ping, get distance in cm and print result (0 = outside set distance range)

}


int reset(){
        pos = 0;
        servo_9.write(pos); 
        lcd.clear();
        lcd.print( "RESET" );
        delay(3000);
        lcd.clear();
        lcd.print( "EP inicial " );
        lcd.setCursor(0, 1);
        lcd.print( EP_i );
        Serial.println("Pressed on button 1");
        return 0;
}



int activate () {
        pos = 90;
        servo_9.write(pos); 
        Serial.println("Pressed on button play/pause");
        lcd.clear();
        lcd.print( "ACTIVADO" );
        measuring = true;
        last_value = 0;
        lcd.clear();
        lcd.print( "cm: " );
        lcd.setCursor(0, 1);
        lcd.print( sonar.ping_cm());
        return 0;

}
void loop() {
  // Tu código aquí
    
    while(measuring) {
        current_value = static_cast< float >(sonar.ping_cm());
        if((current_value < 30 )) {
          Serial.println("False: " + String(current_value) + " " + String(last_value));
          if(current_value < last_value && last_value < 30){
            Serial.println("Current: " + String(current_value) );
            if(last_value < 20){
              H_f = (last_value/100) ;
              EP_f = H_f*m*g;
              Serial.println("Ultimo valor : " + String(last_value));
              Serial.println("Altura final : " + String(H_f));
              Serial.println("Energia potencial final: " + String(EP_f));
              lcd.clear();
              lcd.print( "σ" );
              lcd.setCursor(0, 1);
              lcd.print( EP_f);
              delay(3000);
              measuring = false;
              
              }
              
          }
          else {
              Serial.println("False: " + String(current_value) + " " + String(last_value));
              last_value = current_value;

          }
         
        }
        

    };


  if (IrReceiver.decode()) {
    IrReceiver.resume();
    int command = IrReceiver.decodedIRData.command;
    switch (command) {
      case IR_BUTTON_1: {
        reset();
        break;
      }
      case IR_BUTTON_2: {
        Serial.println("Pressed on button 2");
        break;
      }
      case IR_BUTTON_3: {
        Serial.println("Pressed on button 3");
        break;
      }
      case IR_BUTTON_PLAY_PAUSE: {
        activate();
        break;
      }
      default: {
        Serial.println(command);
      }
    }
  }
  delay(100);
  


}
