#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "SPIFFS.h"
#include "ArduinoJson.h"
#include "axis.h"

//load config document from config.json to JsonDocument doc
void loadConfig(JsonDocument& doc) {
    if (!SPIFFS.begin(true)) {
        Serial.println("An error has occurred while mounting SPIFFS");
        return;
    }
    Serial.println("SPIFFS mounted successfully");
    File configFile = SPIFFS.open("/config.json");

    DeserializationError error = deserializeJson(doc, configFile);

    if (error){
        Serial.println(F("Failed to read file, using default configuration"));
    }

    configFile.close();
}

//custom converter to export ArduinoJson elements as char
namespace ArduinoJson {
  template <>
  struct Converter<char> {
    static void toJson(char c, JsonVariant var) {
      char buf[] = {c, 0};
      var.set(buf);
    }

    static char fromJson(JsonVariantConst src) {
      auto p = src.as<const char*>();
      return p ? p[0] : 0;
    }
  };
}


#endif