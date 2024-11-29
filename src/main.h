/**
 * @brief Header file of projetc
 * @author Myron Kukhta (xkukht01) 
 * 
 */

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_VL53L0X.h>

#include <Wire.h>

// display
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGTH 64
#define OLED_REST -1
Adafruit_SSD1306 app_display(DISPLAY_WIDTH, DISPLAY_HEIGTH, &Wire, OLED_REST);
void display_measurement();

// laser
//Adafruit_VL53L0X laser_sensor;
//VL53L0X_RangingMeasurementData_t measurement_buffer;

/**
 * @brief First initializations
 * 
 */
void setup();

/**
 * @brief Behavioral of programm
 * 
 */
void loop();