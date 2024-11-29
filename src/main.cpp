/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/
#include "main.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)


void display_measurement(){
  app_display.clearDisplay();
  app_display.setTextColor(WHITE);
  app_display.setCursor(0, 10);
  //app_display.println(measurement_buffer.RangeMilliMeter);
  app_display.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if(!app_display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  //if(!laser_sensor.begin()){
  //  Serial.println(F("VL53LOX allocation failed"));
  //  for(;;);
  //}
  
  delay(2000);
  app_display.clearDisplay();

  app_display.setTextSize(1);
  app_display.setTextColor(WHITE);
  app_display.setCursor(0, 10);
  // Display static text
  app_display.println("Hahaha");
  app_display.display();
  
}


void loop() {
  //laser.rangingTest(&measurement_buffer);
  //display_measurement();
}
