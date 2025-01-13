/*
Main code for printer
*/

#include <Arduino.h>
#include "axis.h"
#include <unordered_map>
#include "config.h"
#include "printer.h"

void setup() {
  //load config document from SPIFFS
  JsonDocument config;
  loadConfig(config);

  //Serial
  Serial.begin(115200);
  Serial.setTimeout(1);

  //register appropriate axis
  Printer printer(config);
  Serial.println("on");
}

void loop() {
  while (Serial.available()) {
    String in = Serial.readString();
    //Serial.println(in);
    //strProccess(in);
  }

  //axis.tick();
}