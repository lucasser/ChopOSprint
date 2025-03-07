/*
Main code for printer
*/

#include <Arduino.h>
#include <unordered_map>
#include <config.h>
#include <printer.h>

Printer printer;

void setup() {
  //load config document from SPIFFS
  JsonDocument config;
  loadConfig(config);

  //Serial
  Serial.begin(115200);
  Serial.setTimeout(1);

  //register appropriate axis
  printer.loadConfig(config);
  Serial.println("on");
}

void loop() {
  while (Serial.available()) {
    String in = Serial.readString();
    Serial.println(in);
    printer.processCommand(in);
  }
  printer.tick();
}