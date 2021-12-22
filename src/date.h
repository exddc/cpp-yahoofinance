#pragma once

#include <string>
#include <ctime>
#include <sstream>
#include <time.h>

class Date{
private:

public:
    //outputs the unix timestamp for now
    static std::string now();

    //converts and outputs the unix timestamp for a given date
    static std::string convert(const std::string &refDate);
};