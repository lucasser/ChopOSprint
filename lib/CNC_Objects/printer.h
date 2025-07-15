#ifndef PRINTER_H
#define PRINTER_H

//[TODO]: return error if config is invalid

#include "arduinoJsonChar.h"
#include "unordered_map"
#include <vector>
#include "../../config/config.h"
#include <leveling.h>
#include <axis.h>
#include <cmath>

using std::vector;
using std::unordered_map;
using std::array;

#define AXISBYID axismap.find(id)->second //lookup the correct axis by char id

struct moveCommand; //forward declaration

class Printer {
    public:
        //configure the printer
        void loadConfig(JsonDocument& config);

        //processes the input string and figures out what to do
        void processCommand(String& in);

        //prints out the structure of the printer
        String toString();

        //tick function for all axis and tools
        void tick();
    private:
        //configures all the axis
        void setupAxis(JsonVariant config);

        //converts input string for a move into a moveCommand
        moveCommand parseMove(String& in);

        /**
         * @brief Request a pointer to a specific axis from %Printer::AXIS.
         * @param id Char (x,y,z,e) specifying which axis to return.
         * @return A pointer to the requested %Axis if that axis is initialized, and nullptr if it isn't.
        **/
        Axis* getAxis(char id);
        /**
         * @brief Request a pointer to a specific axis from %Printer::AXIS.
         * @param id int specifying which axis to return.
         * @return A pointer to the requested %Axis if that axis is initialized, and nullptr if it isn't.
        **/
        Axis* getAxis(int id);

        const unordered_map<char, int> axismap = AXISORDER; //maps axis name to array index
        array<Axis*, AXISAMOUNT> AXIS = {}; //Stores all the axis controlled by the esp. Do not pass Axis by value, only by reference.
        vector<char> activeAxis = {};
};

//contains 4 coordinates for 4 axis, and time for the move. Absolute positions only
struct moveCommand{
    moveCommand() {
        for (auto i : coords) {
            i = NAN;
        }
    };
    char type; //relative or absolute
    std::array<float, AXISAMOUNT> coords; //stores positions for a move. size configurable in config.h
    float time = 0;
    String toString() {
        String out = "time: ";
        out += time;
        out += ", type: ";
        out += type;
        out += ", coords: ";
        for (float x : coords) {
            out += x;
            out += ", ";
        }
        return out;
    }
};

#endif //PRINTER_H