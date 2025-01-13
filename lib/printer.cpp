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

void Printer::strProccess(String in) {
  char command = in.charAt(0);
  switch (command) {
    case 'l': //level
      Serial.println("level");
      break;
    case 't': //tool
      Serial.println("tool");
      break;
    case 'u': //update internal config
      Serial.println("config updated");
      break;
    case 's': //level
      Serial.println("stopped");
      break;
    case 'r': //resume
      Serial.println("moving");
      break;
    case 'm': //move
      moveCommand go = parse(in);
      for (auto i : AXIS) {
        if (!i.init) {
          continue;
        }
        float dist = go.coords[AXISBYID];
        //call correct move function
        if (!isnanf(dist)) { //make sure this coord was specified. if it wasn't it'll be NAN, then stay in place
          switch (command) {
            case 'a':
              axis.moveAbsolute(dist, go.time);
              break;
            case 'r':
              axis.moveRelative(dist, go.time);
              break;
          }
        } else {
          axis.delay(go.time);
        }
      }
    default:
      break;
  }
}

//[TODO]: move to dedicated file REWRITE!!! currently wont parse more than one axis
//problems arise if given just r10. Always send at least 1 coordinate
moveCommand Printer::parse(String in) {
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