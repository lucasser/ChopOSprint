/***************************

Has two functions. load config, and update config
Load config reads a config file under /data/config.json and saves it to the JsonDocument doc
Update config [TODO] takes path to a key and a value, and sets that key to the specified value in the config.json file.

****************************/

#ifndef CONFIG_H
#define CONFIG_H

#include "SPIFFS.h"
#include "ArduinoJson.h"

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

#endif