#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
//#include <splash.h>
//#include <Adafruit_Microbit.h>
//Adafruit_Microbit_Matrix microbit;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (4 or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
#define REPORTING_PERIOD_MS     1000
 
// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;
 
uint32_t tsLastReport = 0;
 
// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
    Serial.println("Beat!");
    display.setCursor(100, 0);
    display.write(3);
    display.display();
    
//    microbit.show(microbit.EMPTYHEART);
//    microbit.drawPixel(2, 2, LED_ON);
//    delay(20);
//    microbit.clear();
}
 
void setup()
{
    Serial.begin(115200);
//    microbit.begin();
    Serial.print("Initializing pulse oximeter..");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
    display.clearDisplay();
    delay(1000);
//    display.setTextSize(1);      // Normal 1:1 pixel scale
//    display.setTextColor(SSD1306_WHITE); // Draw white text
//    display.setCursor(0, 0);     // Start at top-left corner
//    display.println(F("micro:bit pulse\noximeter")); 
//    display.display();
//    delay(1000);
//    display.setTextSize(2);      // Normal 1:1 pixel scale
//    display.setTextColor(SSD1306_WHITE); // Draw white text
//    display.setCursor(0, 7);     // Start at top-left corner
//    display.cp437(true);         // Use full 256 char 'Code Page 437' font
//    display.write(3);
//    display.display();
    
 
    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
 
    // The default current for the IR LED is 50mA and it could be changed
    //   by uncommenting the following line. Check MAX30100_Registers.h for all the
    //   available options.
    // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
 
    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
}
 
void loop()
{
    // Make sure to call update as fast as possible
    pox.update();
 
    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        Serial.print("Heart rate:");
        Serial.print(pox.getHeartRate());
        Serial.print("bpm / SpO2:");
        Serial.print(pox.getSpO2());
        Serial.println("%");
        display.clearDisplay();
        display.cp437(true);         // Use full 256 char 'Code Page 437' font
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.setCursor(0, 6);    
        display.println(F("SpO2       %")); 
        display.setCursor(0, 38);    
        display.println(F("Heart           bpm")); 
        display.setTextSize(2);      // Normal 1:1 pixel scale
        display.setCursor(36, 0);
        display.println(pox.getSpO2()); 
        display.setCursor(36, 32);
        display.println(pox.getHeartRate(),1); 
        display.display();
         
        tsLastReport = millis();
    }
}
