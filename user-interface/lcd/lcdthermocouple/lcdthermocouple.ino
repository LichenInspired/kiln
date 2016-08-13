/*************************************************** 
  This is an example for the Adafruit Thermocouple Sensor w/MAX31855K

  Designed specifically to work with the Adafruit Thermocouple Sensor
  ----> https://www.adafruit.com/products/269

  These displays use SPI to communicate, 3 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_MAX31855.h"
#include <LiquidCrystal.h>

int thermoCLK = 2;
int thermoCS = 3;
int thermoDO = 4;

int relay = 6;

double temp = 95.0; // This is the desired temperature

// What needs to be built is the ability to guage the reaction to the heat or cooling being applied, and to use
// that knowledge to ensure that the thermal results follow the defined schedule. This will be usefull in many ways.
// Think about the sitation where the device being monitored physically cannot cool itself or heat itself to the
// specified temperature. The thermo controller would notice that lack of response to more heating/cooling and would
// determine that the max temperature had been reached and that the schedule could not be followed. Think about trying
// to fire a kiln to cone 6 with the lid open. Even if you kept the coils on all the time you still would not be able to
// reach temperature. The thermo controller should be able to recognize that and cancel the firing, or fall back on another
// operation, based on the user's preferences. Think about that for a second. If you had a refridgerator, and you scheduled
// it to follow a specific outline of temperatures at certain times, or rates of increase etc. if the thermo controller
// could not follow that schedule, you would not want it to turn off completely and ruin whatever you had stored in there.
// You would instead want it to fall back to another temperature goal, lets say, maintain the fridge at the coolest temperature
// possible, while notifying me of the failure to follow the schedule.

// Also what one also must consider when developing this tool is the ability to use the responsiveness to heating to ensure that
// over heating, under heating and over usage of the elements is minimized or eliminated. If the temperature needs to increase
// quickly, but the device controlled is responding slowly, the heat must be maintained in order to achieve the maximum response and
// increase. However if the increase were to be gradual, or the temperature maintained at a certain temp, it would be unwise to turn
// the heating elements on for a long duration; overshooting your targeted rate increase or target temperature. Instead the controller
// must be intelligent in selecting the right durration of heating/cooling in order to properly follow the chosen schedule.


int longDuration = 1000; // Climb fast
int shortDuration = 100; // This is the duration of time the power relay is kept on, or off, in ms. As it stands,
// the delay for the loop adds a time interval by which the whole loop operates. I believe this delay is present to
// accomodate retrieval of data from the thermocouple amplifier or LCD. 


// Create a function that gets the length of each of the lines on the display, and fills in the rest with spaces.
// That way it doesn't cause screen flicker by initiallizing the clear function each time the screen changes.
// Ideally the function would check what was on the screen and change it only if the character being written was different.
// This would function while clearing any space from the point of the cursor on to the end of the screen.

// Integrate control through Cosmos

// Develop user interface: This will be able to be presented through a character LCD screen, graphical LCD screen, graphical TFT LCD
// screen, and over serial. Control of the unit will also be over serial connection as well.

// Figure out user input options


// Initialize the Thermocouple
Adafruit_MAX31855 thermocouple(thermoCLK, thermoCS, thermoDO);
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

byte degree[8] = {
  B01100,
  B10010,
  B10010,
  B01100,
  B00000,
  B00000,
  B00000,
};

void setup() {
  Serial.begin(9600);
  // set up the LCD's number of columns and rows: 
  lcd.createChar(0, degree);
  lcd.begin(16, 2);
  delay(500);
  pinMode(relay, OUTPUT);
}

void loop() {
  
   lcd.setCursor(0, 0);

   double internal = thermocouple.readInternal();
   
   if (internal > 25.0) {

     lcd.print("MAX31855 TOO HOT");
     lcd.write("                ");
     lcd.setCursor(0,1);
     lcd.print("TEMP C: ");
     lcd.print(internal);
     lcd.write(byte(0));
     lcd.write("                ");     

   } else {
     
     double c = thermocouple.readCelsius();
     double f = thermocouple.readFarenheit();
     
     lcd.setCursor(0, 0);
     
     if (isnan(c)) 
     {
       lcd.print("THERMO CPL ERROR");
       lcd.write("                ");
       lcd.setCursor(0, 1);
       lcd.write("                ");
     } 
     else 
     {
       lcd.print("TEMP C: "); 
       lcd.print(c);
       lcd.write(byte(0));
       lcd.write("                ");
       
       lcd.setCursor(0,1);
       lcd.print("TEMP F: ");
       lcd.print(f);
       lcd.write(byte(0));
       lcd.write("                ");
     }
   
     if (f < temp * .99 ) {
       
       Serial.print("INCREASE TEMP\n");
       digitalWrite(relay, HIGH);
       delay(longDuration);
       digitalWrite(relay, LOW);
       
     } else if ( f >= temp * .99 && f < temp ) {
     
       Serial.print("INCREASE TEMP, THREASHOLD CLOSE\n");
       digitalWrite(relay, HIGH);
       delay(shortDuration);
       digitalWrite(relay, LOW);
       
     } else if (f >= temp ) {
       
       Serial.print("DEREASE TEMP\n");
       digitalWrite(relay, LOW);
       
     }     
     
   }
   
   delay(500);
}
