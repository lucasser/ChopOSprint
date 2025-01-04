#include "printer.h"
#include "axis.h"

Printer::Printer(JsonDocument& config) {
    setupAxis(config);
}

void Printer::setupAxis(JsonVariant config) {
  JsonArray axis(config["axis"]);
  for (JsonVariant i : axis) {
    AXIS.at(axismap.find(i["id"])->second).loadConfig(i);
  }
  //[TODO]: setup heating stuff
}

Axis* Printer::getAxis(int id) {
  if (AXIS.at(id).init) {
    return &AXIS.at(id);
  }
  return nullptr;
}

Axis* Printer::getAxis(char id) {
  if (AXIS.at(axismap.find(id)->second).init) {
    return &AXIS.at(axismap.find(id)->second);
  }
  return nullptr;
}
