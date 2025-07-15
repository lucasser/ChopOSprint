#include <printer.h>

void Printer::loadConfig(JsonDocument& config) {
  setupAxis(config);
}

void Printer::setupAxis(JsonVariant config) {
  JsonArray axis(config["axis"]);
  for (JsonVariant i : axis) {
    char id = i["id"];
    AXIS.at(AXISBYID) = new Axis(i);
    activeAxis.push_back(id);
  }
  //[TODO]: setup heating stuff
}

Axis* Printer::getAxis(int id) {
  if (AXIS.at(id)->init) {
    return AXIS.at(id);
  }
  return nullptr;
}

void Printer::tick() {
  for (auto i : activeAxis) {
    getAxis(i)->tick();
  }
}

Axis* Printer::getAxis(char id) {
  if (AXIS.at(AXISBYID)->init) {
    return AXIS.at(AXISBYID);
  }
  return nullptr;
}

void Printer::processCommand(String& in) {
  char command = in.charAt(0);
  Serial.println(in.length());
  if (in.length() < 3) {
    return;
  }
  if (command == 'l') { //Level
    Serial.println("level");
	Axis* tolevel = getAxis(in.charAt(2));
    tolevel->level();
  } else if (command == 't') { //Toolhead
    Serial.println("tool");
  } else if (command == 'u') { //Configs
    Serial.println("config updated");
  } else if (command == 's') { //Stop
    Serial.println("stopped");
    for (auto i : activeAxis) {
      if (in.indexOf(i) != -1 || in.indexOf('a') != -1) {
        getAxis(i)->suspend();
      }
    }
  } else if (command == 'r') { //Resume
    bool trash = in.charAt(1) != 'k';
    Serial.println("moving");
    for (auto i : activeAxis) {
      if (in.indexOf(i) != -1 || in.indexOf('a') != -1) {
        getAxis(i)->resume(trash);
      }
    }
  } else if (command == 'm') { //Move
    moveCommand go = parseMove(in);
    Serial.println(go.toString());
    if (go.type != 'r' && go.type != 'a') {
      return;
    }
    for (auto i : activeAxis) {
      int id = i;
      if (isnanf(go.coords.at(AXISBYID))) {
        getAxis(i)->delay(go.time);
      } else {
        getAxis(i)->generalMove({go.type, go.coords.at(AXISBYID), go.time});
      }
    }
  } else if (command == 'p') { //Print
    Serial.println(toString());
  }
}

String Printer::toString() {
  String out = "axis: [{";
  for (auto i : activeAxis) {
    out += "\nid: ";
    out += i;
    out += "\naxis: ";
    out += getAxis(i)->toString();
  }
  out += "}]";
  return out;
}

//[TODO?]: revisit and optimize logic
moveCommand Printer::parseMove(String& in) {
  moveCommand go; //where we moving next
  String temp = "";

  go.type = in.charAt(1);
  go.time = (in.indexOf("t") == -1) ? 0 : in.substring(in.indexOf('t') + 1, in.indexOf(' ', in.indexOf('t'))).toFloat();

  for (auto i : activeAxis) {
    int index = in.indexOf(i);
    if (index != -1) {
      temp = in.substring(index + 1, in.indexOf(' ', index));
    }
    int id = i;
    go.coords.at(AXISBYID) = temp.toFloat();
  }
  return go;
}