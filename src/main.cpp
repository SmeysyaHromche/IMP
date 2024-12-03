/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_VL53L0X.h>

#include <Wire.h>
#include <SPI.h>  // need for correct include ather lib

// display glob
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGTH 64
#define OLED_REST -1
Adafruit_SSD1306 app_display(DISPLAY_WIDTH, DISPLAY_HEIGTH, &Wire, OLED_REST);
// laser glob
Adafruit_VL53L0X laser_sensor = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t laser_sensor_output;

/**
 * @brief Write laser sensor output on display desk
 * 
 */
void laser_sensor_display_output(){
  laser_sensor.rangingTest(&laser_sensor_output);  // read data
  app_display.clearDisplay();
  app_display.setCursor(0, 10);
  if(laser_sensor_output.RangeStatus != 4){
    app_display.println("Distance:");
    app_display.print(laser_sensor_output.RangeMilliMeter);
    app_display.println("(mm)");
    Serial.print("Distance (mm): "); Serial.println(laser_sensor_output.RangeMilliMeter);
  }
  else{
    app_display.println("Out of range");
    Serial.println(" out of range ");
  }
  app_display.display();
  delay(100);
}

void display_output(){

}

/**
 * @brief Init pre-run setup of applications sub-devices
 * 
 */
void setup() {
  Serial.begin(115200);  // init serial
  Wire.begin();  // init i2c

  if(!app_display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // init application display with i2c
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  if(!laser_sensor.begin()){  // init application laser sensor with i2c
    Serial.println(F("VL53LOX allocation failed"));
    for(;;);
  }
}

/**
 * @brief Core loop
 * 
 */
void loop() {
  //laser.rangingTest(&measurement_buffer);
  //display_measurement();
  laser_sensor_display_output();
}
