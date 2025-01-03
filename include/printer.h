#ifndef PRINTER_H
#define PRINTER_H

#include <unordered_map>

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
        //[TODO]: array amount should be set by a header
        //this only works for 4 axis. need to change these and moveCommand::coords for more
        const std::unordered_map<char, int> axismap{{'x', 0}, {'y', 1}, {'z', 2}, {'e', 3}}; //maps axis name to array index
        std::array<Axis, 4> AXIS = {}; //Stores all the axis controlled by the esp. Do not pass Asis by value, only by refference.
};

#endif //PRINTER_H