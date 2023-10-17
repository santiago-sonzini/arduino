/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>
#include <Servo.h>
#include <DIYables_IRcontroller.h> // Library for IR Receiver
#include <HTTPClient.h>


#define IR_RECEIVER_PIN 19 // The ESP32 pin GPIO19 connected to IR controller
#define IR_BUTTON_1 12
#define IR_BUTTON_2 24
#define IR_BUTTON_3 94
#define IR_BUTTON_PLAY_PAUSE 67

#define TRIGGER_PIN  25  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     26  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.


DIYables_IRcontroller_21 irController(IR_RECEIVER_PIN, 200); // debounce time is 200ms


const char* serverName = "https://materiales.vercel.app/api/ensayo";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
int button;
int pos = 90;
float last_value = 0;
float current_value = 0;
float g = 9.81;
float m = 0.271;
float H_i = 0.16;
float H_f = 0 ;
int req_status =0;

bool measuring = false;

float EP_i = g*m*H_i;
float EP_f = 0;

float sigma = 0;


LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección I2C 0x27, 16 columnas y 2 filas
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 
Servo myservo;

// Replace with your network credentials
const char* ssid = "iPhone de santiago";
const char* password = "arduino12";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  myservo.attach(27);
  myservo.write(pos);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.init();                      // Inicializar la pantalla LCD
  lcd.backlight();                 // Encender la retroiluminación (si la pantalla la tiene)
  lcd.setCursor(0, 0);
  lcd.print( "IP address:" );
  lcd.setCursor(0, 1);
  lcd.print( WiFi.localIP() );

  server.begin();
  //IrReceiver.begin(19);
  irController.begin();
}

int reset(){
        pos = 90;
        current_value = 0;
        last_value = 0;
        myservo.write(pos);        
        lcd.clear();
        lcd.print( "RESET" );
        
        delay(3000);
        lcd.clear();
        lcd.print( "EP inicial " );
        lcd.setCursor(0, 1);
        lcd.print( EP_i );
        Serial.println("Pressed on button reset");
        return 0;
}

void setInit(){
        pos = 10;
        current_value = 0;
        last_value = 0;
        myservo.write(pos);        
        lcd.clear();
        lcd.print( "RESET to 0" );
        
        delay(3000);
        lcd.clear();
        lcd.print( "EP inicial " );
        lcd.setCursor(0, 1);
        lcd.print( EP_i );
        Serial.println("Pressed on button reset");
}



int activate () {
        current_value = 0;
        last_value = 0;
        pos = 10;
        myservo.write(pos);
        Serial.println("Pressed on button play/pause");
        lcd.clear();
        lcd.print( "ACTIVADO" );
        delay(10);
        measuring = true;
        
        return 0;

}


int post () {
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
    
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
      
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "sigma="+String(sigma);           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      //http.addHeader(" Content-Type", "application/json");
      //int httpResponseCode = http.POST("{\"sigma\":\""+ String(sigma) + "\"");

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
            http.end();

      return httpResponseCode;
      // Free resources
    }
    else {
      Serial.println("WiFi Disconnected");
      return 0;
    }
    lastTime = millis();
  }
}

void loop(){
  

   while(measuring) {
        current_value = static_cast< float >(sonar.ping_cm());
        if((current_value < 30 )) {
          if(current_value < last_value && last_value < 30 ){
            Serial.println("Current: " + String(current_value) );
            if(last_value < 20 ){
              H_f = (last_value/100) + 0.04 + 0.015 ;
              EP_f = H_f*m*g;
              sigma = EP_i - EP_f;
              Serial.println("Ultimo valor : " + String(last_value));
              Serial.println("Altura final : " + String(H_f));
              Serial.println("Energia potencial final: " + String(EP_f));
              Serial.println("σ " + String(sigma));
              lcd.clear();
              lcd.print( "σ" );
              lcd.setCursor(0, 1);
              lcd.print( sigma);
              last_value = 0;
              current_value = 0;
              delay(3000);
              measuring = false;
              
              } else {
              Serial.println("False: " + String(current_value) + " " + String(last_value));
              last_value = current_value;

              }
              
          }
          else {
              //Serial.println("False: " + String(current_value) + " " + String(last_value));
              last_value = current_value;

          }
         
        }
        

    };

  Key21 key = irController.getKey();
  if (key != Key21::NONE) {
    switch (key) {
      case Key21::KEY_1:
        Serial.println("1");
        activate();
        key= Key21::NONE;
        break;

      case Key21::KEY_2:
        Serial.println("2");
        reset();
        key= Key21::NONE;

        break;

      case Key21::KEY_3:
        Serial.println("3");
        req_status = post();
        
        key= Key21::NONE;

        break;
      case Key21::KEY_4:
        Serial.println("4");
        setInit();
        
        key= Key21::NONE;

        break;

    
      default:
        Serial.println("WARNING: undefined key:");
        key= Key21::NONE;
        break;
    }
  }
   
    
  
}
