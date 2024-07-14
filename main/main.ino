#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_FreeTouch.h"

Adafruit_FreeTouch qt_seven = Adafruit_FreeTouch(7);
Adafruit_FreeTouch qt_zero = Adafruit_FreeTouch(0);
int threshold = 350;

// SCREEN
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//#undef PIN_WIRE_SDA
#define PIN_WIRE_SDA   4
//#undef PIN_WIRE_SCL
#define PIN_WIRE_SCL   5
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define water 8
#define pump 9
#define goal_tracker 6

int cnt = 0;
int cnt2 = 0;
int setting_goal = 0;
int user_goal = 0;
float user_progress = 0;

void setup() {
  Serial.begin(9600);
  qt_zero.begin();
  qt_seven.begin();
  pinMode(water, OUTPUT); pinMode(pump, OUTPUT);
  pinMode(goal_tracker, INPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize OLED screen
  display.clearDisplay();
  drawOledBase();
}


void loop() {
  int goal = digitalRead(goal_tracker);
  Serial.println(goal);
  if(goal == 0) {
    cnt2++;
  } else {
    cnt2 = 0;
  }
  delay(10);

  if (cnt2 > 15 && setting_goal == 0) {
    setting_goal = 1;
    display.clearDisplay();
    inputGoalPage(user_goal);
    delay(1000);
    while (setting_goal == 1) {
      cnt2 = 0;
      int g = digitalRead(goal_tracker);
      if (g == 0) {
        cnt2++;
        int h = digitalRead(goal_tracker);
        while (h == 0) {
          cnt2++;
          h = digitalRead(goal_tracker);
          delay(10);
        }
      }
      if (cnt2 > 40) {
        display.clearDisplay();
        drawOledBase();
        setting_goal = 0;
      } else if (cnt2 > 2) {
        if (user_goal == 13) {
          user_goal = 0;
        } else {
          user_goal = user_goal + 1;
        }
        drawGoalOnGoalPage(user_goal);
      }
    }
    cnt2 = 0;
  }

  int qt1 = qt_zero.measure();
  int qt2 = qt_seven.measure();
  if(qt1 < threshold || qt2 < threshold){
    cnt++;
  } else {
    cnt = 0;
  }

  float old_progress = user_progress;
  if (cnt > 8){
      digitalWrite(water, HIGH); digitalWrite(pump, LOW); //flow out
      int qt1 = qt_zero.measure();
      int qt2 = qt_seven.measure();
      while(qt1 < threshold || qt2 < threshold){
        qt1 = qt_zero.measure();
        qt2 = qt_seven.measure();
        user_progress += 0.002;
        delay(200);
      }
      digitalWrite(water, LOW); digitalWrite(pump, HIGH);
      delay(2000);
      digitalWrite(water, LOW); digitalWrite(pump, LOW); 
      delay(2000);   
      cnt = 0;  
  }
  drawProgress(old_progress);
  delay(1);
}

void drawOledBase() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(27, 0);
  display.print("Water Intake!");
  display.drawRoundRect(10, 20, 20, 40, 2, WHITE);
  display.drawRoundRect(14, 14, 12, 6, 2, WHITE);
  display.setCursor(38, 20);
  display.print("Progress: ");
  display.setCursor(94, 20);
  display.print(user_progress);
  display.setCursor(38, 40);
  display.print("Goal: ");
  display.setCursor(75, 40);
  display.print(user_goal);
  display.display();
}

void drawProgress(float oldProgress) {
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setCursor(94, 20);
  display.print(oldProgress);
  display.setTextColor(WHITE);
  display.setCursor(94, 20);
  display.print(user_progress);
  display.display();

}

void inputGoalPage(int cups) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(35, 4);
  display.print("Click to");
  display.setCursor(25, 16);
  display.print("Increase Goal");
  display.setTextSize(2);
  display.setCursor(24, 35);
  display.print("Goal: ");
  display.setCursor(90, 35);
  display.print(cups);
  display.display();
}

void drawGoalOnGoalPage(int cups) {
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.setCursor(90, 35);
  if (cups == 0) {
    display.print(13);
  } else {
    display.print(cups - 1);
  }
  display.setTextColor(WHITE);
  display.setCursor(90, 35);
  display.print(cups);
  display.display();
}