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

 
    int numArray[10] = { 870, 728, 274, 453, 600,852,682,169,341,511}; // 按钮对应的电压值
    for (int idx = 0; idx < 10; idx++) { // 修正循环范围，数组索引从0开始
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
  static int lastNum = -1;
  static int num = -1;
  num =   keyScan(keyMatrix_0);

  static int flag = 0;
  flag ++;
  if(flag>100 || num ==0){
    flag = 0;
  }
  if(num>=0){
    if(num == lastNum){
      return -1;
    }else{
      lastNum = num;
      return num;
    }
  }
  return num;
}
int digCnt=0;
int currentNum = 0
int rightPasswd = 1234;
int errorCnt = 0;
int  blueLED = A3;
int greenLED = A1;
int yellowLED = A2;


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
  }else if(state == 2){
    digitalWrite(greenLED, LOW); 
    digitalWrite(yellowLED, HIGH); 
    digitalWrite(blueLED, LOW); 
  }else if (state == 3 ){
    digitalWrite(greenLED, LOW); 
    digitalWrite(yellowLED, LOW); 
    digitalWrite(blueLED, HIGH); 

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
          currentNum = 0;
          errorCnt = 0;
          state = 3;
        }
      }
    }
    if(digCnt>4){
      digCnt =0;
      currentNum = 0;
      state = 0;
      errorCnt = 0;
    }
    SetUpLED(state);
    disp_0.display( currentNum);
  
}

