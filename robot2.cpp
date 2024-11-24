#include <Kenblock.h>
#include <Kenblock_4DigitalDisplay.h>
#include <Kenblock_Buzzer.h>
#include <Servo.h>
#include <EEPROM.h>

#define BUZZER_PIN 8 // 蜂鸣器连接的引脚

FourDigitalDisplay disp_0(PD1);
Buzzer buzzer_0(8);
Servo servo_0;

const int keyMatrix_0 = A0;
const int greenLED = A1;
const int yellowLED = A2;
const int blueLED = A3;
const int servo_0_pin = 9;

const unsigned long debounceDelay = 50; // 防抖延迟时间（单位：毫秒）

int digCnt = 0;           // 当前输入的位数
int currentNum = 0;       // 当前输入的数字
int errorCnt = 0;         // 连续错误计数
int state = 0;            // 当前状态
int adminPasswd = 2424;   // 管理员密码
unsigned long lastChangeTime = 0; // 上一次按键时间
int rightPasswd = 1234;   // 用户密码（存储于 EEPROM）
bool isAdminMode = false; // 是否进入管理员模式
int newPasswdStage = 0;   // 密码修改阶段
int newPasswd = 0;        // 新密码临时存储

void setup() {
    Serial.begin(9600);
    pinMode(keyMatrix_0, INPUT_PULLUP);
    pinMode(greenLED, OUTPUT);
    pinMode(yellowLED, OUTPUT);
    pinMode(blueLED, OUTPUT);

    servo_0.attach(servo_0_pin);
    servo_0.write(90);

    // 从EEPROM读取初始密码
    int storedValue = 0;
    EEPROM.get(0, storedValue);
    if (storedValue > 0 && storedValue <= 9999) {
        rightPasswd = storedValue;
    } else {
        rightPasswd = 1234;
    }
    Serial.print("Value read from EEPROM: ");
    Serial.println(rightPasswd);
}

void setLEDState(int state) {
    if(state == 1) {
        servo_0.write(180);
    }
    digitalWrite(greenLED, state == 1);
    digitalWrite(yellowLED, state == 2);
    digitalWrite(blueLED, state == 3);
    if (state == 1) {
        playCorrectSound();
    } else if (state == 2) {
        playErrorSound();
    } else if (state == 3) {
        playErrorMAXSound();
    }
    if(state == 1) {
        servo_0.write(90);
    }
}

int keyScan(int pin) {
    int value = analogRead(pin);
    int numArray[] = {870, 738, 274, 453, 600};
    for (int idx = 0; idx < 5; ++idx) {
        if (abs(value - numArray[idx]) < 10) {
            return idx + 1;
        }
    }
    return -1;
}

int readKey() {
    static int lastNum = -1;
    static int stableNum = -1;

    int num = keyScan(keyMatrix_0);
    if (num != lastNum) {
        lastChangeTime = millis();
        lastNum = num;
    }
    if ((millis() - lastChangeTime) > debounceDelay && num != stableNum) {
        stableNum = num;
        return stableNum;
    }
    return -1;
}

void processInput(int num) {
    if (num > 0) {
        digCnt++;
        currentNum = currentNum * 10 + num;

        if (digCnt == 4) {
            if (isAdminMode) {
                handleAdminMode();
            } else if (currentNum == adminPasswd) {
                enterAdminMode();
            } else if (currentNum == rightPasswd) {
                errorCnt = 0;
                state = 1;
            } else {
                errorCnt++;
                state = (errorCnt < 3) ? 2 : 3;
                if (state == 3) errorCnt = 0;
            }
            disp_0.display(currentNum);
            setLEDState(state);
            delay(500);
            resetInput();
        }
    }
    disp_0.display(currentNum);
    setLEDState(state);
}

void resetInput() {
    digCnt = 0;
    currentNum = 0;
    state = 0;
}

void enterAdminMode() {
    isAdminMode = true;
    newPasswdStage = 0;
    newPasswd = 0;
    state = 0;
    Serial.println("Admin mode activated. Enter new password.");
}

void handleAdminMode() {
    if (newPasswdStage == 0) {
        newPasswd = currentNum; // 第一次输入的新密码
        newPasswdStage = 1;
        Serial.println("Re-enter new password to confirm.");
    } else if (newPasswdStage == 1) {
        if (newPasswd == currentNum) { // 第二次输入相同
            rightPasswd = newPasswd;
            EEPROM.put(0, rightPasswd); // 存储新密码到EEPROM
            Serial.println("Password updated successfully.");
            playCorrectSound();
            isAdminMode = false; // 退出管理员模式
        } else {
            Serial.println("Passwords do not match. Try again.");
            playErrorSound();
        }
        disp_0.display(currentNum);
        resetInput(); // 确保输入状态完全重置
        newPasswdStage = 0;
    }
}

void playCorrectSound() {
    tone(BUZZER_PIN, 440, 200);
    delay(150);
    tone(BUZZER_PIN, 880, 200);
    delay(150);
    noTone(BUZZER_PIN);
}

void playErrorSound() {
    for (int i = 0; i < 3; i++) {
        tone(BUZZER_PIN, 300, 150);
        delay(200);
    }
    noTone(BUZZER_PIN);
}

void playErrorMAXSound() {
    for (int i = 0; i < 5; i++) {
        tone(BUZZER_PIN, 300, 500);
        delay(250);
        noTone(BUZZER_PIN);
        delay(50);
    }
    noTone(BUZZER_PIN);
}

void loop() {
    int num = readKey();
    processInput(num);
}
