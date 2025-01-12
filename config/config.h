#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#define RESETTIME 5 //the time all axis will take to move to saved position after a resume

#define AXISAMOUNT 5 //number of axis this printer supports
#define AXISORDER {{'x', 0}, {'y', 1}, {'z', 2}, {'e', 3}, {'f', 4}} //order in which the axis get mapped to indexes

#endif //config