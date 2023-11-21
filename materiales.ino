#include <LiquidCrystal_I2C.h>
#include <DIYables_IRcontroller.h> // Library for IR Receiver
#include <Array.h>

const int ELEMENT_COUNT_MAX = 20000;
Array<int, ELEMENT_COUNT_MAX> array;

#define IR_RECEIVER_PIN 19 // The ESP32 pin GPIO19 connected to IR controller
#define RELE_PIN 14

DIYables_IRcontroller_21 irController(IR_RECEIVER_PIN, 200); // debounce time is 200ms
LiquidCrystal_I2C lcd(0x27, 16, 2);                          // Dirección I2C 0x27, 16 columnas y 2 filas

const char *serverName = "https://materiales.vercel.app/api/ensayo";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
// unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
int button;
int counter = 0;
int pos = 90;
float last_value = 0;
float current_value = 0;
float g = 9.81;
float m = 0.271;
float H_i = 0.16;

float H_f = 0;
int req_status = 0;

bool measuring = false;
bool releState = false;

float EP_i = g * m * H_i;
float EP_f = 0;
float sigma = 0;

// Replace with your network credentials
const char *ssid = "iPhone de santiago";
const char *password = "arduino12";

void setup()
{
  Serial.begin(115200);
  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, releState)
  lcd.init();      // Inicializar la pantalla LCD
  lcd.backlight(); // Encender la retroiluminación (si la pantalla la tiene)
  lcd.setCursor(0, 0);
  lcd.print("IP address:");
  lcd.setCursor(0, 1);
  lcd.print("WiFi.localIP()");

  // IrReceiver.begin(19);
  irController.begin();
}

void reset()
{
  array.clear();
  lcd.clear();
  lcd.print("RESET");
  delay(3000);
  lcd.clear();
  lcd.print("EP inicial ");
  lcd.setCursor(0, 1);
  lcd.print(EP_i);
  Serial.println("Pressed on button reset");
}


void activate()
{
  
  counter = 0;
  Serial.println("Pressed on button play/pause");
  lcd.clear();
  lcd.print("LEYENDO");
  releState = false;
  digitalWrite(RELE_PIN, releState);
  measuring = true;
}

void stopMeasuring()
{
  Serial.println("5");
  measuring = false;
}

int largest()
{

  int i;

  // Initialize maximum element
  int max = array[0];

  // Traverse array elements
  // from second and compare
  // every element with current max
  for (i = 1; i < array.size(); i++)
    if (array[i] > max)
      max = array[i];
  Serial.println("Maximo valor:");
  Serial.println(max);
  lcd.clear();
  lcd.print("Maximo valor:");
  lcd.setCursor(0, 1);
  lcd.print(max);
  return max;
}

void readControl()
{
  Key21 key = irController.getKey();
  if (key != Key21::NONE)
  {
    switch (key)
    {
    case Key21::KEY_PLAY_PAUSE:
    {
      Serial.println("KEY_PLAY_PAUSE");
      if (measuring)
      {
        /* code */
        stopMeasuring();
        largest();
        
      }
      else
      {
        activate();
      }

      key = Key21::NONE;
      break;
    }

    case Key21::KEY_PREV:
    {
      Serial.println("KEY_PREV");
      reset();
      releState = true;
      digitalWrite(RELE_PIN, releState);
      key = Key21::NONE;

      break;
    }

    case Key21::KEY_3:
    {
      Serial.println("3");
      // req_status = post();

      key = Key21::NONE;

      break;
    }

    default:
    {
      Serial.println("WARNING: undefined key:");
      key = Key21::NONE;
      break;
    }
    }
  }
}

void loop()
{
  
  while (measuring && !(counter == ELEMENT_COUNT_MAX - 1))
  {
    counter = counter + 1;
    int valor = analogRead(4);
    readControl();
    array.push_back(valor);
    Serial.println(valor);
  };

  readControl();
}
