#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Button pins
#define UP_BUTTON     5
#define DOWN_BUTTON   2
#define LEFT_BUTTON   14
#define RIGHT_BUTTON  12
#define MODE_BUTTON   27

// Buzzer pin
#define BUZZER_PIN    18

// OLED pins
#define OLED_SDA 21
#define OLED_SCL 22

// Struct to send data
typedef struct struct_message {
  int command;
} struct_message;

struct_message myData;

// Receiver MAC address (replace with your robot's ESP32 MAC)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

esp_now_peer_info_t peerInfo;

bool manualMode = true;

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Init buttons
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(MODE_BUTTON, INPUT_PULLUP);

  // Init buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Init OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Remote Starting...");
  display.display();

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Mode toggle
  if (digitalRead(MODE_BUTTON) == LOW) {
    manualMode = !manualMode;
    delay(300); // debounce
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(manualMode ? "Mode: Manual" : "Mode: Auto");
    display.display();
  }

  if (manualMode) {
    // Manual mode commands
    if (digitalRead(UP_BUTTON) == LOW) {
      myData.command = 1; // Forward
      sendData();
      digitalWrite(BUZZER_PIN, HIGH);
    } 
    else if (digitalRead(DOWN_BUTTON) == LOW) {
      myData.command = 2; // Backward
      sendData();
      digitalWrite(BUZZER_PIN, HIGH);
    } 
    else if (digitalRead(LEFT_BUTTON) == LOW) {
      myData.command = 3; // Left
      sendData();
      digitalWrite(BUZZER_PIN, HIGH);
    } 
    else if (digitalRead(RIGHT_BUTTON) == LOW) {
      myData.command = 4; // Right
      sendData();
      digitalWrite(BUZZER_PIN, HIGH);
    } 
    else {
      myData.command = 0; // Stop
      sendData();
      digitalWrite(BUZZER_PIN, LOW);
    }
  } 
  else {
    // Auto mode
    myData.command = 5; // Auto
    sendData();
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void sendData() {
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
}
