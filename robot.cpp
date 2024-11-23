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
 

int keyScan(int pin) {
    static int flag = 0;
    flag = flag+1;
    int value = analogRead(pin);  
    if(flag>100){
        Serial.print("value:");
        Serial.println(value);
    }
    // 0 1 2 3 4 5 6 7 8 9
    int numArray[] = {0, 870, 738, 274, 453, 600,852,682,169,341,511}; // 按钮对应的电压值
    for (int idx = 1; idx <= 5; idx++) { // 修正循环范围，数组索引从0开始
        int absDiff = abs(value - numArray[idx]); // 计算绝对差值
        // 优化打印信息，减少内存使用        
        if (absDiff < 10) { // 如果差值小于阈值，则返回按键索引
            Serial.print("[");
            Serial.print(idx);
            Serial.print("]: ");
            Serial.print(numArray[idx]);
            Serial.print(" value: ");
            Serial.println(value);
            return idx; // 按键编号从1开始
        }
    }
    return -1; // 如果没有匹配值，返回0
}
// 按钮防抖函数


FourDigitalDisplay disp_0(PD1);
int passwd = 0;
int keyMatrix_0 = A0;
int led_blue = A1;
int led_yello = A2;
int led_red = A3;

int readKey() {
  static int lastNum = -1;               // 上一次按键值
  static int stableNum = -1;             // 防抖后的稳定按键值
  static unsigned long lastChangeTime = 0; // 上一次状态变化的时间
  unsigned long debounceDelay = 50;      // 防抖延迟时间（单位：毫秒）

  int num = keyScan(keyMatrix_0);        // 扫描当前按键值

  // 检查按键值是否发生变化
  if (num != lastNum) {
    lastChangeTime = millis();          // 记录变化时间
    lastNum = num;                      // 更新上一次按键值
  }

  // 如果按键值保持稳定超过 debounceDelay 时间
  if ((millis() - lastChangeTime) > debounceDelay) {
    if (num != stableNum) {             // 如果稳定值发生变化
      stableNum = num;                  // 更新防抖后的稳定值
      return stableNum;                 // 返回新的按键值
    }
  }

  return -1; // 无有效变化时返回 -1
}
int digCnt=0;
int currentNum = 0;
int rightPasswd = 1234;
int errorCnt = 0;
int  blueLED = A3;
int greenLED = A1;
int yellowLED = A2;
Buzzer buzzer_0(8);

void setup() {
    passwd = 0;
    Serial.begin(9600);
    pinMode(keyMatrix_0, INPUT_PULLUP);
    pinMode(greenLED, OUTPUT);
    pinMode(yellowLED, OUTPUT);
    pinMode(blueLED, OUTPUT);
   // pinMode(keyMatrix_1, INPUT_PULLUP);
}
void SetUpLED(int state){
  if(state == 1){
    digitalWrite(greenLED, HIGH); 
    digitalWrite(yellowLED, LOW); 
    digitalWrite(blueLED, LOW); 
    buzzer_0.didi();
  }else if(state == 2){
    digitalWrite(greenLED, LOW); 
    digitalWrite(yellowLED, HIGH); 
    digitalWrite(blueLED, LOW); 
      buzzer_0.didi();
  }else if (state == 3 ){
    digitalWrite(greenLED, LOW); 
    digitalWrite(yellowLED, LOW); 
    digitalWrite(blueLED, HIGH); 
    buzzer_0.didi();
    delay(500);
  }else if( state == 0){
    digitalWrite(greenLED, LOW); 
    digitalWrite(yellowLED, LOW); 
    digitalWrite(blueLED, LOW); 
  }
}
int state = 0;
void loop() {

    int num = readKey();
    if(num<0){
      disp_0.display( currentNum);
      SetUpLED(state);
      return;
    }

    digCnt = digCnt+1;
    currentNum = currentNum*10 + num;
    if(digCnt==4){
      if(currentNum == rightPasswd){
        errorCnt = 0;
        state = 1;
      }else{
        errorCnt++;
        if(errorCnt<3){
            state = 2;
        }else{
          errorCnt = 0;
          state = 3;
        }
      }
      disp_0.display( currentNum);
      SetUpLED(state);
      delay(500);
      digCnt =0;
      currentNum = 0;
      state = 0;
    }else{
    disp_0.display( currentNum);
      SetUpLED(state);
   
    }

  
}


// 防抖相关变量
int lastButtonState = LOW;          // 上一次按钮状态
unsigned long lastDebounceTime = 0; // 上次抖动时间
unsigned long debounceDelay = 50;  // 防抖延迟（毫秒）

/**
 * 防抖函数
 * @param currentState 当前按钮状态（HIGH 或 LOW）
 * @return 防抖后的按钮状态（HIGH 或 LOW）；无状态变化时返回 -1
 */
int debounceButton(int currentState) {
  static int stableState = LOW;       // 当前稳定状态
  unsigned long now = millis();       // 当前时间

  // 检测到状态变化
  if (currentState != stableState) {
    lastDebounceTime = now; // 更新状态变化时间
  }

  // 检查状态是否稳定超过 debounceDelay 时间
  if ((now - lastDebounceTime) > debounceDelay) {
    if (currentState != stableState) {
      stableState = currentState; // 更新稳定状态
      return stableState;         // 返回新的稳定状态
    }
  }

  return -1; // 无有效状态变化时返回 -1
}
