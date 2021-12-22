#pragma once

#include <string>
#include <sqlite3.h>
#include <iostream>

#include "saveData.h"
#include "dataHandler.h"
#include "userAction.h"
#include "quote.h"


class Portfolio{
private:
    //location of the Portfolio Database
    const char* dir{"C:/Programming/GitHub/investmenttracker/data/portfolio.db"};

    //update the positions
    void updatePositions(const Position &position);

    //retrieve contents from select method
    static int callback(void* NotUsed, int argc, char** argv, char** azColName);

    inline static bool positionExists = false;

public:
    Portfolio();

    //return the ticker and amount for all positions
    std::string getPositons();

    //return the and amount for a given ticker
    std::string getPositon(const std::string &ticker);

    //return all active positions
    std::string getActivePositions();

    //enter a new trade
    void enterTrade();

    //edit an positon
    void editPosition();

    //update the position with the latest price
    void setLatestPrice(const std::string &ticker, const std::string &pPrice);
};

    