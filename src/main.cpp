/**
 * @file main.cpp
 * @author Myron Kukhta (xkukht01)
 * @brief Distance measurement by laser sensor
 * @version 0.1
 * @date 2024-12-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
// lib import
#include <Wire.h>
#include <SPI.h>  // need for correct include ather lib
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_VL53L0X.h>

// program glob
volatile bool execute_status = true;

// display glob
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGTH 64
#define OLED_REST -1
Adafruit_SSD1306 app_display(DISPLAY_WIDTH, DISPLAY_HEIGTH, &Wire, OLED_REST);

// laser glob
Adafruit_VL53L0X laser_sensor = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t laser_sensor_output;
float output = 0.0;
enum Dimension{
  MM=0,
  CM=1,
  M=2
};
Dimension actual_dimension = MM;

// encoder
#define ENCODER_CLK 14  // pin CLK for detect right rotation  (GPIO_14)
#define ENCODER_DT 27    // GPIO_27 for detect left rotation   (GPIO 27)
#define ENCODER_SW 16   // GPIO_16 for detect press bottan    (GPIO 16)
#define ENCODER_ROTATE_CNT_MIN 0
#define ENCODER_ROTATE_CNT_MAX 2
volatile int ecnoder_rotate_cnt = 0;
volatile int clkState;
volatile int dtState;
volatile int clkStateLast = HIGH;
volatile int buttonState;
volatile unsigned long buttonPressTimeLast = 0;
int buttonStateLast = HIGH;
volatile int mixin = 0;

// programm core

/**
 * @brief Activate settings menu mode for metadata of application
 * 
 */
void settings_menu(){
  app_display.clearDisplay();
  app_display.setCursor(40, 10);
  app_display.setTextColor(WHITE);
  app_display.println("MENU");
  app_display.setCursor(0, 25);
  app_display.print(F("Prev mixin(mm): "));
  app_display.println(mixin);
  app_display.setCursor(0, 40);
  app_display.print(F("Reset mixin(mm): "));
  app_display.println(ecnoder_rotate_cnt);
  app_display.display();
  delay(100);
}

/**
 * @brief Write laser sensor output on display desk
 * 
 */
void laser_sensor_display_output(){
  if(ecnoder_rotate_cnt > ENCODER_ROTATE_CNT_MAX){
    ecnoder_rotate_cnt = ENCODER_ROTATE_CNT_MIN;
  }
  else if(ecnoder_rotate_cnt < ENCODER_ROTATE_CNT_MIN){
    ecnoder_rotate_cnt = ENCODER_ROTATE_CNT_MAX;
  }
  actual_dimension = (Dimension)ecnoder_rotate_cnt;
  Serial.println(actual_dimension);
  laser_sensor.rangingTest(&laser_sensor_output);  // read data
  app_display.clearDisplay();
  app_display.setCursor(0, 10);
  app_display.setTextColor(WHITE);
  if(laser_sensor_output.RangeStatus != 4){
    output = laser_sensor_output.RangeMilliMeter + mixin;
    switch (actual_dimension)
    {
    case M:
      app_display.print(F("Distance (m):"));
      output /= 1000;
      Serial.print("Distance (m): "); Serial.println(output);
      break;
    case CM:
      app_display.print(F("Distance (cm):"));
      output /=10;
      Serial.print("Distance (cm): "); Serial.println(output);
      break;
    case MM:
    default:
      Serial.print("Distance (mm): "); Serial.println(output);
      app_display.print(F("Distance (mm):"));
      break;
    }
    app_display.println(output);
  }
  else{
    app_display.println("Out of range");
    Serial.println(" out of range ");
  }
  app_display.display();
  delay(100);
}


/**
 * @brief Answer on encoder right rotation interrupt
 * 
 */
void catch_encoder_rotate(){
  clkState = digitalRead(ENCODER_CLK);
  dtState = digitalRead(ENCODER_DT);
  if (clkState != clkStateLast){
    if(clkState == HIGH){
      if (dtState != clkState) {
        Serial.println(" LEFT ");
        ecnoder_rotate_cnt--;  
      } 
      else{
        Serial.println(" RIGHT "); 
        ecnoder_rotate_cnt++;
      }
    }
    clkStateLast = clkState;
  }
}


/**
 * @brief Answer on encoder press button. Change global status.
 * 
 */
void catch_encoder_press(){
  /*Serial.println(F("Encoder pressed 1."));
  //buttonPressTime = millis();
  buttonState = digitalRead(ENCODER_SW);
  if(buttonState == LOW){
    Serial.println(F("Encoder pressed 2."));
    if(buttonPressTime-buttonPressTimeLast > 50){
      execute_status = !execute_status;
      if(execute_status){
        mixin = ecnoder_rotate_cnt;
        ecnoder_rotate_cnt = (int)actual_dimension;
      }
      else{
        ecnoder_rotate_cnt = 0;
      }
    }
    buttonPressTimeLast = buttonPressTime;
  }*/
}


/**
 * @brief Init pre-run setup of applications sub-devices
 * 
 */
void setup() {
  Serial.begin(115200);  // init serial
  Wire.begin();  // init i2c
  
  // init application display with i2c
  delay(500);
  if(!app_display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed."));
    for(;;);
  }
  else{
    Serial.println(F("SSD1306 is OK."));
  }

  // init application laser sensor with i2c
  delay(500);
  if(!laser_sensor.begin()){
    Serial.println(F("VL53LOX allocation failed."));
    for(;;);
  }
  else{
    Serial.println(F("VL53LOX is OK."));
  }

  // init GPIO (encoder output / esp input)
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);  // button in default is unpressed

  // catch encoder interruption
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), catch_encoder_rotate, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(ENCODER_SW), catch_encoder_press, FALLING);
}

/**
 * @brief Core loop
 * 
 */
void loop() {
  buttonState = digitalRead(ENCODER_SW);
  if(millis()-buttonPressTimeLast > 500){
      Serial.println(F("Encoder pressed 1."));
      if(buttonState == LOW){
        Serial.println(F("Encoder pressed 1."));
        execute_status = !execute_status;
        if(execute_status){
          mixin = ecnoder_rotate_cnt;
          ecnoder_rotate_cnt = (int)actual_dimension;
        }
        else{
          ecnoder_rotate_cnt = 0;
        }
        buttonPressTimeLast = millis();
    }
  }
  if(execute_status){
    laser_sensor_display_output();
  }
  else{
    settings_menu();
  }
}
