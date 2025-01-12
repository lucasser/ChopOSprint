#ifndef PRINTER_H
#define PRINTER_H

//[TODO]: return error if config is invalid

#include "../config/config.h"
#include "ArduinoJson.h"
#include "unordered_map"

#define AXISBYID axismap.find(id)->second //lookup the correct axis by char id

class Printer {
    public:
        Printer(JsonDocument& config);
        void setupAxis(JsonVariant config);
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


    private:
        const std::unordered_map<char, int> axismap = AXISORDER; //maps axis name to array index
        std::array<Axis, AXISAMOUNT> AXIS = {}; //Stores all the axis controlled by the esp. Do not pass Asis by value, only by refference.
};

#endif //PRINTER_H