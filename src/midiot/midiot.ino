#include <MIDI.h>
#include <WiFi101.h>
#include <Losant.h>
#include "keys.h"

const int BUTTON_PIN = 5;
const int LED_PIN = 13;
const int LED_R_PIN = 11;
const int LED_G_PIN = 10;

int note;


int led_r_state = 0;

WiFiSSLClient wifiClient;

LosantDevice device(LOSANT_DEVICE_ID);

MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  MIDI.begin();
  WiFi.setPins(8, 7, 4, 2);
  Serial.begin(115200);
  while (!Serial) { }
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);

  // Register the command handler
  device.onCommand(&handleCommand);

  connect();

}

void loop() {

  bool toReconnect = false;

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Disconnected from WiFi");
    toReconnect = true;
  }

  if (!device.connected()) {
    Serial.println("Disconnected from Losant");
    Serial.println(device.mqttClient.state());
    toReconnect = true;
  }

  if (toReconnect) {
    connect();
  }

  device.loop();

  delay(100);
}

void play_note(int note) {
  digitalWrite(LED_R_PIN, 0);
  MIDI.sendNoteOn(note, 127, 1);    // Send a Note (pitch 42, velo 127 on channel 1)
  delay(100);                // Wait for a second
  MIDI.sendNoteOff(note, 0, 1);     // Stop the note
  digitalWrite(LED_R_PIN, 1);
}

// Called whenever the device receives a command from the Losant platform.
void handleCommand(LosantCommand *command) {
  Serial.print("Command received: ");
  Serial.println(command->name);

  if (strcmp(command->name, "play") == 0) {
    JsonObject& payload = *command->payload;
    payload.printTo(Serial);
    note = payload["note"];
    Serial.println(note);
    play_note(note);
  }

}

void connect() {

  // Connect to Wifi.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  digitalWrite(LED_R_PIN, LOW);
  digitalWrite(LED_G_PIN, HIGH);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    led_r_state = !led_r_state;
    digitalWrite(LED_R_PIN, led_r_state);
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println();
  Serial.print("Connecting to Losant...");

  device.connectSecure(wifiClient, LOSANT_ACCESS_KEY, LOSANT_ACCESS_SECRET);

  digitalWrite(LED_R_PIN, LOW);
  while (!device.connected()) {
    delay(500);
    Serial.print(".");
  }

  digitalWrite(LED_R_PIN, 1);
  digitalWrite(LED_G_PIN, 0);
  Serial.println("Connected!");
}
