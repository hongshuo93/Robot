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
    uint8_t value = analogRead(pin);       

    int numArray[6] = {250, 104, 225, 13, 197, 90}; // 按钮对应的电压值
    for (int idx = 0; idx < 6; idx++) { // 修正循环范围，数组索引从0开始
        int absDiff = abs(value - numArray[idx]); // 计算绝对差值
        // 优化打印信息，减少内存使用
       /* Serial.print("[");
        Serial.print(idx);
        Serial.print("]: ");
        Serial.print(numArray[idx]);
        Serial.print(" value: ");
        Serial.println(value);*/
        
        if (absDiff < 5) { // 如果差值小于阈值，则返回按键索引
            return idx; // 按键编号从1开始
        }
    }
    return 0; // 如果没有匹配值，返回0
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
int passwd = 0;
int keyMatrix_0 = A0;
int led_blue = 8;
int led_yello = 9;
int led_red = 10;
void setup() {
    passwd = 0;
    Serial.begin(9600);
    pinMode(keyMatrix_0, INPUT_PULLUP);
    pinMode(led_blue, OUTPUT);
    pinMode(led_yello, OUTPUT);
    pinMode(led_red, OUTPUT);
}
uint8_t correctPasswd =1234;
uint8_t errorCnt = 0;
void loop() {

    int num = debounceButton(keyMatrix_0);
    if(num>0){
        passwd = passwd*10+num;
        Serial.print("num: ");
        Serial.print(num);
        Serial.print(" passwd: ");
        Serial.print(passwd);
        Serial.println();
    }
    if(passwd>10000||passwd<0){
        passwd = 0;
    }
    if(passwd == correctPasswd){
        digitalWrite(led_blue, HIGH);
        digitalWrite(led_yello, LOW);
        digitalWrite(led_red, LOW);
        errorCnt = 0;
    }else if(passwd>1000){
        errorCnt = errorCnt+1;
        if(errorCnt>=3){
            digitalWrite(led_blue, LOW);
            digitalWrite(led_yello, LOW);
            digitalWrite(led_red, HIGH);
        }else{
            digitalWrite(led_blue, LOW);
            digitalWrite(led_yello, HIGH);
            digitalWrite(led_red, LOW);
        }
    }else{
        digitalWrite(led_blue, LOW);
        digitalWrite(led_yello, LOW);
        digitalWrite(led_red, LOW);
    }

    disp_0.display(passwd);
}

