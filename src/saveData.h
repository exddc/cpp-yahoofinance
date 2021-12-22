#pragma once

#include <string>

#include "../libs/tinyxml2.h"
#include "dataHandler.h"

class Save{

public:

    //save latest data in xml
    static void saveLatestQuote(const std::string &ticker, const std::string &data);

    //save data in xml
    static void saveData(const Stock& stock);

    //amend the given position to the portfolio.xml file
    static void savePosition(const Position &position);
};