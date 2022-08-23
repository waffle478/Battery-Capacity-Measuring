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

void setup() {
  Serial.begin(115200);
  
  pinMode(comm, OUTPUT);
  pinMode(output, OUTPUT);
  pinMode(SD_error, OUTPUT);

  digitalWrite(comm, HIGH);
  digitalWrite(output, HIGH);
  digitalWrite(SD_error, HIGH);
  
  delay(1000);

  digitalWrite(comm, LOW);
  digitalWrite(output, LOW);
  digitalWrite(SD_error, LOW);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    digitalWrite(comm, LOW);
    delay(50);
    digitalWrite(comm, HIGH);
    delay(50);
  }

  if (!ina219.begin()) {
    // Megnézi, hogy rá van e csatlakozva az árammérő, ha nem, akkor villog
    Serial.println("Failed to find INA219 chip");
    while (1) {
      digitalWrite(comm, LOW);
      delay(100);
      digitalWrite(comm, HIGH);
      delay(100);
    }
  }


  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    bool flash = false;
    while (1)
    {
      digitalWrite(SD_error, flash);
      flash = !flash;
      delay(500);
    }
  }
  Serial.println("card initialized.");
  
  for(int x = 0; x < 4; x++)
  {
    digitalWrite(output, HIGH);
    delay(250);
    digitalWrite(output, LOW);
    delay(250);
  }
  
  delay(1000);
}

void loop() {
  File dataFile = SD.open("amper.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    digitalWrite(SD_error, LOW);
    shuntvoltage = ina219.getShuntVoltage_mV();
    busvoltage = ina219.getBusVoltage_V();
    volt = busvoltage + (shuntvoltage / 1000);
    amp = ina219.getCurrent_mA();
    
    Serial.print((String)count + ": ");
    
    count++;
    
    if(abs(amp) > 30)
    { 
      zero_count = 0;
      mAh += amp;
      dataFile.println((String)volt + " V\t" + (String)amp + " mA\t" + (String)(mAh/360));
      dataFile.close();
    }
    else { zero_count++; Serial.print("--"); }
    
    if(zero_count > 10){

      if(!first)
      {
        first = true;
        File mAhs = SD.open("mAh.txt", FILE_WRITE);
        if(mAhs)
        {
          mAhs.println((String)(mAh/360) + "\n");
          mAhs.close();
        }
        else
        {
          Serial.println("Error opening mAh.txt");
        }
      }

      
      digitalWrite(comm, HIGH);
      digitalWrite(output, HIGH);      
    }
    else first = false;
    Serial.println((String)volt + " V\t" + (String)amp + " mA\tCurrent mAh: " + (String)(mAh/360));
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening amper_read.txt");
    digitalWrite(SD_error, HIGH);
    
    bool flash = false;
    
    for(int i = 0; i<5; i++)
    {
      digitalWrite(SD_error, flash);
      flash = !flash;
      delay(100);
    }
  }
  delay(10000);
}
