#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

#include <SPI.h>
#include <SD.h>

#define output A1     // merítés kész
#define SD_error A2   // SD hiba
#define comm A3       // Kommunikációs hiba jelző

Adafruit_INA219 ina219;

bool first = false;   // A mérés óta az első kiírás.

int count = 0;        // Ennyiedik mérést csinálta
int zero_count = 0;   // Ha nem ad le áramot, akkor elkezdi számolni mennyi ideje nem ad áramot

const int chipSelect = 4; // SD chip select

double mAh = 0;
float amp = 0;
float volt = 0;

float shuntvoltage = 0;
float busvoltage = 0;
float loadvoltage = 0;

void blink(uint8_t pin, uint8_t del, uint8_t times){
  for (uint8_t i = 0; i < times*2; i++)
  {
    digitalWrite(pin, !digitalRead(pin));
    delay(del);
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(comm,       OUTPUT);
  pinMode(output,     OUTPUT);
  pinMode(SD_error,   OUTPUT);

  digitalWrite(comm,      HIGH);
  digitalWrite(output,    HIGH);
  digitalWrite(SD_error,  HIGH);
  
  delay(500);

  digitalWrite(comm,      LOW);
  digitalWrite(output,    LOW);
  digitalWrite(SD_error,  LOW);
  
  if (!ina219.begin()) {
    // Initiate INA219 sensor communication
    while (1) {
      //Error could not start comm
      blink(comm, 100, 1);
    }
  }

  while (!Serial) {
    // wait for serial port to connect. Needed for native USB port only
    blink(comm, 50, 1);
  }
  Serial.println("INA219 sensor initialized.");
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1)
    {
      blink(SD_error, 500, 1);
    }
  }
  Serial.println("card initialized.");

  /**
   *  ################    NEED TO DO    ################
   */
  
  // Everithing is done
  blink(output, 250, 4);
  
  delay(500);
}

void loop() {


  delay(10000);
}
