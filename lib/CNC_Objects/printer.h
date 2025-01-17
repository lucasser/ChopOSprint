#ifndef PRINTER_H
#define PRINTER_H

//[TODO]: return error if config is invalid

#include "arduinoJsonChar.h"
#include "unordered_map"
#include <vector>
#include "../../config/config.h"
#include <axis.h>

using std::vector;
using std::unordered_map;
using std::array;

#define AXISBYID axismap.find(id)->second //lookup the correct axis by char id

class Printer {
    public:
        //configure the printer
        void loadConfig(JsonDocument& config);

        //processes the input string and figures out what to do
        void processCommand(String& in);

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

        //[TODO?]: dynamically allocate a vector to store the axis. then any identifier will work.
        const unordered_map<char, int> axismap = AXISORDER; //maps axis name to array index
        array<Axis, AXISAMOUNT> AXIS = {}; //Stores all the axis controlled by the esp. Do not pass Axis by value, only by refference.
        vector<char> activeAxis = {};
};

#endif //PRINTER_H