/**
 * Copyright(C), 2018-2038, KenRobot.com
 * FileName: sketch_nov23c.ino
 * Author: 啃萝卜
 * Create: 2024/11/23
 * Modify: 2024/11/23
 */

#include <Kenblock.h>
#include <Kenblock_4DigitalDisplay.h>
#include <Kenblock_Buzzer.h>
 
uint8_t keyScan(int pin) {
    int value = analogRead(pin);       
   // int yello_1 = 104;
   // int white_2 = 225;
   // int red_3 = 13;
   // int blue_4 = 197;
   // int green_5 = 90;
    int numArray[6]={250,104,225,13,197,90};
    return numArray[1];

}
// 按钮防抖函数
uint8_t debounceButton(int buttonPin, unsigned long debounceDelay = 50) {
  static int lastButtonState = 0;           // 上次按钮状态
  static int buttonState = 0;              // 当前按钮状态
  static unsigned long lastDebounceTime = 0;  // 上次状态变化时间

  int currentState = keyScan(buttonPin); // 读取当前按钮状态

  if (currentState != lastButtonState) { // 如果检测到状态变化
    lastDebounceTime = millis();         // 记录当前时间
  }

  if ((millis() - lastDebounceTime) > debounceDelay) { // 如果超过防抖时间
    if (currentState != buttonState) {
      buttonState = currentState; // 更新按钮状态

      if (buttonState >0 ) {  // 按钮按下时返回 true
        lastButtonState = currentState; // 更新上一次状态
        return buttonState;
      }
    }
  }

  lastButtonState = currentState; // 更新上一次状态
  return 0;                   // 默认返回 false
}


FourDigitalDisplay disp_0(PD1);
uint8_t passwd = 0;
int keyMatrix_0 = A0;

void setup() {
    passwd = 0;
    pinMode(keyMatrix_0, INPUT_PULLUP);
}

void loop() {
    int num = keyScan(keyMatrix_0);
    disp_0.display(num);
}