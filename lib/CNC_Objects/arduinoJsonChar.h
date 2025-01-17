#ifndef ARDUINO_JSON_CHAR
#define ARDUINO_JSON_CHAR

#include <ArduinoJson.h>

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

#endif //arduinoJson char exporter