#include <ESP32Servo.h>
#include <WiFi.h>
#include <esp_now.h>

#define IR 13

Servo f;
Servo br;
Servo bl;

int command = 0;
int x = 0;
int y = 0;
unsigned long irLowStart = 0;
int i = 100;

void forward() {
  br.write(5);
  bl.write(180);
  f.write(0);
}
void reverse() {
  f.write(180);
  br.write(180);
  bl.write(24);
}
void left() {
  br.write(0);
  bl.write(0);
  f.write(180);
}
void right() {
  br.write(180);
  bl.write(180);
  f.write(180);
}
void stationary() {
  br.write(94);
  bl.write(92);
  f.write(93);
}
void onReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  memcpy(&command, incomingData, sizeof(command));
  Serial.print("Received command: ");
  Serial.println(command);
}

void setup() {
  pinMode(IR, INPUT);

  br.attach(27);
  bl.attach(26);
  f.attach(25);

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);

  }
void loop() {
  int IRvalue = digitalRead(IR);
  if (irLowStart == 0) irLowStart = millis();
  else if (millis() - irLowStart >= i) {
    reverse();
    delay(1000);
    stationary();
    delay(250);
    if (y == 1) {
      int number = random(1,3);
      if (number == 1) {
        right();
        delay(random(1000,2000));
        stationary();
        delay(250);
      }
      else {
        left();
        delay(random(1000,2000));
        stationary();
        delay(250);
      }
    }
    irLowStart = millis();
  }
  else if (IRvalue == LOW) {
    if (irLowStart == 0) irLowStart = millis();
  }
  else {
    if (command == 1) {
      stationary();
      y = 0;
      i = 100;
    }
    else if (command == 2) {
      forward();
      i = 10;
      y = 1;
    }
    else if (command == 3) {
      forward();
    }
    else if (command == 4) {
      reverse();
    }
    else if (command == 5) {
      left();
    }
    else if (command == 6) {
      right();
    }
    else if (command == 7) {
      stationary();
    }
    irLowStart = 0;
  }
}