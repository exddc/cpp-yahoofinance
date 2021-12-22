#include "date.h"

//outputs the unix timestamp for now
std::string Date::now(){
    std::time_t timeNow = std::time(nullptr);
    std::stringstream result;
    result << timeNow;
    return result.str();
}

//converts and outputs the unix timestamp for a given date
std::string Date::convert(const std::string &refDate){
    //needed input as DD/MM/YYYY
    //todo: make date input prompt with checks if date is correct

    if(refDate.empty() == false){
        std::string DD{};
        std::string MM{};
        std::string YYYY{};

        size_t found{refDate.find("/")};
        if (int(found) > 0 ){
            DD = refDate.substr(0,found);
            found = refDate.find("/", found + 1);

            if(int(found) > 0){
                MM = refDate.substr((DD.length() + 1), found - DD.length() - 1);
                YYYY = refDate.substr(DD.length() + MM.length() + 2);
            }
        }

        int year{atoi(YYYY.c_str())};
        int month{atoi(MM.c_str())};
        int day{atoi(DD.c_str())};

        struct tm dateConvert;

        dateConvert.tm_year = year - 1900;
        dateConvert.tm_mon = month - 1;
        dateConvert.tm_mday = day;

        //set time after market close
        dateConvert.tm_hour = 23;
        dateConvert.tm_min = 59;
        dateConvert.tm_sec = 0;

        time_t unixTime{mktime(&dateConvert)};

        std::stringstream result;
        result << unixTime;
        return result.str();
    } else {
        throw "false date input";
    }
}   
