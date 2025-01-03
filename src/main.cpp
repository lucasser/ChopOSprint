/*
Main code for printer
*/

#include <Arduino.h>
#include "stepper.h"
#include <unordered_map>
#include "config.h"
#include "printer.h"

//[TODO]: move to dedicated file REWRITE!!! currently wont parse more than one axis
//problems arise if given just r10. Always send at least 1 coordinate
moveCommand parse(String in) {
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

void setup() {
  //load config document from SPIFFS
  JsonDocument config;
  loadConfig(config);

  //Serial
  Serial.begin(115200);
  Serial.setTimeout(1);

  //register appropriate axis
  Printer(config);
  Serial.println("on");
}

// void strProccess(String in) {
//   char command = in.charAt(0);
//   moveCommand go;
//   switch (command) {
//     case 'l': //level
//       Serial.println("level");
//       break;
//     case 'a': //absolute move, fall through
//     case 'r': {//relative move
//         go = parse(in);
//         float dist = go.coords[axismap.find(AXIS)->second];
//         //call correct move function
//         if (!isnanf(dist)) { //make sure this coord was specified. if it wasn't it'll be NAN, then stay in place
//           switch (command) {
//             case 'a':
//               axis.moveAbsolute(dist, go.time);
//               break;
//             case 'r':
//               axis.moveRelative(dist, go.time);
//               break;
//           }
//         } else {
//           axis.delay(go.time);
//         }
//       }
//     default:
//       break;
//   }
// }

void loop() {
  while (Serial.available()) {
    String in = Serial.readString();
    //Serial.println(in);
    //strProccess(in);
  }

  //axis.tick();
}