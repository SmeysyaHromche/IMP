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
int actual_dimension = MM;
float min_laser_output = 0.0;
float max_laser_output = 0.0;

// encoder
#define ENCODER_CLK 14  // GPIO_14
#define ENCODER_DT 27   // GPIO_27
#define ENCODER_SW 16   // GPIO_16
volatile int ecnoder_rotate_cnt = 0;
volatile int clkState;
volatile int dtState;
volatile int clkStateLast = HIGH;

int buttonState;
unsigned long buttonPressTimeLast = 0;
bool buttonPressed = false;
int mixin = 0;

enum MenuTools{
  StatisticTool = 0,
  MixinTool = 1,
  GoOutTool= 2,
  MenuTool = 1000,
};
int menu_tool = MenuTool;

/**
 * @brief Transform number in correct range by base division
 * 
 * @param num input number
 * @param base base of division
 * @return transformed number
 */
int set_num_in_correct_range(int num, int base){
  int aux = num % base;
  if(aux < 0){
    aux+=base;
  }
  return aux;
}

// programm core
/**
 * @brief Menu with selection tools
 * 
 */
void menu_display(){
  if(buttonPressed){  // check choosing of tools
    menu_tool = set_num_in_correct_range(ecnoder_rotate_cnt, 3);
    buttonPressed = false;
    ecnoder_rotate_cnt = 0;
    return;
  }
  app_display.clearDisplay();
  app_display.setCursor(40, 10);
  app_display.setTextColor(WHITE);
  app_display.println(F("MENU"));
  app_display.setCursor(0, 22);
  int choose = set_num_in_correct_range(ecnoder_rotate_cnt, 3);
  if(choose == StatisticTool){
    app_display.print(F(">"));
  }
  app_display.print(F("Statistics"));
  app_display.setCursor(0, 34);
  if(choose == MixinTool){
    app_display.print(F(">"));
  }
  app_display.print(F("Mixin"));
  app_display.setCursor(0, 46);
  if(choose == GoOutTool){
    app_display.print(F(">"));
  }
  app_display.print(F("Go out"));
  app_display.display();
}

/**
 * @brief Menu tools for display sensor statistics
 * 
 */
void statistic_tool(){
  if(buttonPressed){  // go out from menu
    buttonPressed = false;
    execute_status = true;
  }
  // statistics
  app_display.clearDisplay();
  app_display.setCursor(30, 10);
  app_display.setTextColor(WHITE);
  app_display.println(F("STATISTICS"));
  app_display.setCursor(0, 22);
  app_display.print(F("Min (mm):"));
  app_display.println(min_laser_output);
  app_display.setCursor(0, 34);
  app_display.print(F("Max (mm):"));
  app_display.println(max_laser_output);
  app_display.setCursor(0, 46);
  app_display.display();
}

/**
 * @brief Menu tools for set mixin of original laser output
 * 
 */
void mixin_tool(){
  if(buttonPressed){
    mixin = ecnoder_rotate_cnt;
    buttonPressed = false;
    execute_status = true;
  }
  app_display.clearDisplay();
  app_display.setCursor(40, 10);
  app_display.setTextColor(WHITE);
  app_display.println(F("MIXIN"));
  app_display.setCursor(0, 22);
  app_display.print(F("Prev (mm):"));
  app_display.println(mixin);
  app_display.setCursor(0, 34);
  app_display.print(F("New (mm):"));
  app_display.println(ecnoder_rotate_cnt);
  app_display.setCursor(0, 46);
  app_display.display();
}

/**
 * @brief Activate settings menu mode for metadata of application
 * 
 */
void settings_menu(){
  switch(menu_tool){
    case MixinTool:
      mixin_tool();
      break;
    case StatisticTool:
      statistic_tool();
      break;
    case GoOutTool:
      execute_status = true;
      break;
    case MenuTool:
    default:
      menu_display();
      break;
  }
}

/**
 * @brief Write laser sensor output on display desk
 * 
 */
void laser_sensor_display_output(){
  laser_sensor.rangingTest(&laser_sensor_output);  // read data
  
  app_display.clearDisplay();
  app_display.setCursor(30, 10);
  app_display.setTextColor(WHITE);
  app_display.println(F("DETECTION"));
  app_display.setCursor(0, 22);
  if(laser_sensor_output.RangeStatus != 4){
    output = laser_sensor_output.RangeMilliMeter + mixin;
    
    // store statistic
    if(output > max_laser_output){
      max_laser_output = output;
    }
    else if(output < min_laser_output){
      min_laser_output = output;
    }
    
    actual_dimension = set_num_in_correct_range(ecnoder_rotate_cnt, 3);
    switch (actual_dimension)  // set correct dimension
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
  else{  // undetection by out of range sensor
    app_display.println("Out of range");
  }
  app_display.display();
}


/**
 * @brief Catch on encoder rotation interrupt
 * 
 */
void catch_encoder_rotate(){
  clkState = digitalRead(ENCODER_CLK);
  dtState = digitalRead(ENCODER_DT);
  if (clkState != clkStateLast){
    if(clkState == HIGH){
      if (dtState != clkState) {  // rotate to left
        ecnoder_rotate_cnt--;  
      } 
      else{  // rotate to right
        ecnoder_rotate_cnt++;
      }
    }
    clkStateLast = clkState;
  }
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
}

/**
 * @brief Core loop
 * 
 */
void loop() {
  // catch press button
  buttonState = digitalRead(ENCODER_SW);
  if(millis()-buttonPressTimeLast > 500){
      if(buttonState == LOW){
        if(execute_status){  // from laser to menu
          buttonPressed = false;
          menu_tool = MenuTool;
          execute_status = false;
        }
        else{  // menu
          buttonPressed = true;
        }
        buttonPressTimeLast = millis();
    }
  }

  
  if(execute_status){  // laser detection execute
    laser_sensor_display_output();
  }
  else{  // menu execute
    settings_menu();
  }
}
