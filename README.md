# Battery-Capacity-Measuring
Measuring the capacity of one (or more) 1850 type Li-Ion battery with an Arduino nano and an INA219 sensor

## **TODO**
[ ] Check SD card if it is available during write

[ ] Read files from SD card and name the files accordingly

[ ] Make changes to the Serial and I2C comm so it does not wait for the I2C to start Serial comm.

[ ] Make multiple files. So the main.cpp will not look like a dumbster

**SIGNALS**

* Card opening error
* SD file open/write error
* INA219 comm error
* Done signal

## **REQUIREMENTS**

[ON START]

* load necessary comm ports
* create new folder for measurement
* create measurement cycle file
* read and log starting values (voltage)
* set charge cycle (depending on starting voltage value)

**This might need some changing**  
* if charged (> 4.00V):
  * Discharge, and measure capacitance
 * Charge                 [Full charge]
 * Discharge              [Full discharge]
 * Charge                 [Full charge to be safe]

[LOOP DESCRIPTION]
    
Every cycle should be measured and saved in a different txt file

* [ ] check if SD card is still present
* [ ] read and calculate amp and mAh
* [ ] try to open file
* [ ] write data to file
* [ ] if the charging or discharging is done write the output to the test file and colse the test.
  * go to the next test

  * OR end the whole test