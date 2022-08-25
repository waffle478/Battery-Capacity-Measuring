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

float shuntvoltage = 0;
float busvoltage = 0;
float loadvoltage = 0;

struct BatteryStatus
{
  float amp = 0;
  float volt = 0;
  double mAh = 0;
};

BatteryStatus battery;

void blink(uint8_t pin, int del, uint8_t times){
  for (uint8_t i = 0; i < times*2; i++)
  {
    digitalWrite(pin, !digitalRead(pin));
    delay(del);
  }
}

void measure(BatteryStatus *batt){
  batt->amp = ina219.getBusVoltage_V() + ina219.getShuntVoltage_mV()/1000;
  batt->amp = ina219.getCurrent_mA();
}

uint8_t getDirNumber(char *name){
  uint8_t len = (uint8_t)strlen(name);
  if (NULL != strstr(name, "MEASUR"))
  {
    char number[3] = {name[len-2], name[len-1], '\0'};
    Serial.println(name);
    return atoi(number);
  }
  else
  {
    Serial.print("NO - ");
    Serial.println(name);
  }
  
  
  return 0;
}

/**
 * 
 * Dir format:
 *    Measure__
 *      
 *      __ is a number. It can be 01 - 99
 * 
 * Measure file formats:
 *    Cycle_
 * 
 *      _ is a number. It can be 1 - 9
 * 
 */

/**
 * @brief Create a new Dir for measurement
 * 
 * @return uint8_t 
 */
char *createNewMeasurementDir(char directory[13]){
  File root = SD.open("/");
  File f = root.openNextFile();
  uint8_t max = 0;
  
  while (true){
    
    if (f.isDirectory())
    {
      uint8_t dir_num = getDirNumber(f.name());
      if(dir_num > max){
        max = dir_num;
      }
    }
    f = root.openNextFile();
    if (!f) break;
  }
  f.close();
  root.close();
  char num[2] = {0};
  max = max + 1;
  if (max < 10)
  {
    strcat(directory, "0");
  }
  itoa((int)(max), num, 10);
  strcat(directory, num);
  if(SD.mkdir(directory)){
    Serial.print("SUCCESFULLY created dir: ");
    Serial.println(directory);
    Serial.print(" with number:");
    Serial.println(max);
    
    return directory;
  }
  else
  {
    Serial.print("Cannot create dir: ");
    Serial.println(directory);
    return 0;
  }
  
  return 0;
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

  while (!Serial) {
    // wait for serial port to connect. Needed for native USB port only
    blink(comm, 50, 1);
  }
  Serial.println("\n\nSerial started.");
  Serial.println("**************************************************************");
  // Make a time-out
  if (!ina219.begin()) {
    // Initiate INA219 sensor communication
    while (1) {
      //Error could not start comm
      blink(comm, 100, 1);
    }
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

  //measure(&battery);

  char dir[13] = "MEASUR";
  createNewMeasurementDir(dir);

  if (dir[0] != 0)
  {
    Serial.println("Success");
  }
  
  // Initialization DONE
  blink(output, 250, 4);
  delay(500);
}

void loop() {


  delay(10000);
}
