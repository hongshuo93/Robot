#include <Kenblock.h>
#include <Kenblock_4DigitalDisplay.h>
#include <Kenblock_Buzzer.h>
#include <Kenblock_Buzzer.h>
#include <Servo.h>
FourDigitalDisplay disp_0(PD1);
Buzzer buzzer_0(8);
Servo servo_0;

const int keyMatrix_0 = A0;
const int greenLED = A1;
const int yellowLED = A2;
const int blueLED = A3;
const int servo_0_pin = 9;

const int rightPasswd = 1234;
const unsigned long debounceDelay = 50; // 防抖延迟时间（单位：毫秒）

int digCnt = 0;
int currentNum = 0;
int errorCnt = 0;
int state = 0;

unsigned long lastChangeTime = 0;

void setup() {
    Serial.begin(9600);
    pinMode(keyMatrix_0, INPUT_PULLUP);
    pinMode(greenLED, OUTPUT);
    pinMode(yellowLED, OUTPUT);
    pinMode(blueLED, OUTPUT);

    servo_0.attach(servo_0_pin);
    servo_0.write(180);
}

void setLEDState(int state) {
    digitalWrite(greenLED, state == 1);
    digitalWrite(yellowLED, state == 2);
    digitalWrite(blueLED, state == 3);
    if (state != 0) {
        buzzer_0.didi();
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
            if (currentNum == rightPasswd) {
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
            digCnt = 0;
            currentNum = 0;
            state = 0;
        }
    }
    disp_0.display(currentNum);
    setLEDState(state);
}

void loop() {
    int num = readKey();
    processInput(num);
}
