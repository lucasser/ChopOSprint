#include "printer.h"
#include "axis.h"

Printer::Printer(JsonDocument& config) {
  setupAxis(config);
}

void Printer::setupAxis(JsonVariant config) {
  JsonArray axis(config["axis"]);
  for (JsonVariant i : axis) {
    char id = i["id"];
    AXIS.at(AXISBYID).loadConfig(i);
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
  if (AXIS.at(AXISBYID).init) {
    return &AXIS.at(AXISBYID);
  }
  return nullptr;
}

void Printer::processCommand(String in) {
  char command = in.charAt(0);
  if (in.length() < 3) {
    return;
  }
  switch (command) {
    case 'l': //level
      Serial.println("level");
      char id = in.charAt(2);
      AXIS.at(AXISBYID).level();
      break;
    case 't': //tool
      Serial.println("tool");
      break;
    case 'u': //update internal config
      Serial.println("config updated");
      break;
    case 's': //stop
      Serial.println("stopped");
      int j = 0;
      for (auto i : AXIS) {
        if (!i.init) {
          continue;
        }
        if (in.indexOf(axismap.find(j)->first) != -1 || in.indexOf('a') != -1) {
          i.suspend();
        }
        j++;
      }
      break;
    case 'r': //resume
      Serial.println("moving");
      break;
    case 'm': //move
      moveCommand go = parseMove(in);
      int j = 0;
      for (auto i : AXIS) {
        if (!i.init || isnanf(go.coords.at(j))) {
          continue;
        }
        i.generalMove({in.charAt(2), go.coords.at(j), go.time});
        j++;
      }
    default:
      break;
  }
}

//[TODO]: move to dedicated file REWRITE!!! currently wont parse more than one axis
//problems arise if given just r10. Always send at least 1 coordinate
moveCommand Printer::parseMove(String in) {
  float* coord = nullptr; //the coordinate in moveCommand::axis to write to
  moveCommand go; //where we moving next
  String temp = "";

  go.time = in.substring(1, in.indexOf(' ')).toFloat();

  for (int i = in.indexOf(' '); i < in.length(); i++) {
    char c = in.charAt(i);
    if (c == ' ') { //new value
      if (coord != nullptr) {*coord = temp.toFloat(); /*record position*/}
      temp = "";
      coord = &go.coords[axismap.find(in.charAt(i+1))->second];
      i++; //skip over identifier letter
    } else {
      temp += c;
    }
  }
  *coord = temp.toFloat();

  Serial.println(go.toString());
  return go;
}