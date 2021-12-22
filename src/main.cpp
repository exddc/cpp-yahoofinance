// Basic main file to populate the project and to iterate on

/* #include "functions.h"
#include "yahoofinance.h" */

//#include "YahooAPI.h"
#include "quote.h"
#include "portfolio.h"
#include <iostream>

//#include "../libs/sqlite3.h"
#include <sqlite3.h>


using namespace std;

int main()
{
    Quote::updateStockData("tsla");
    //Portfolio portfolio;
    //std::string ticker{"tsla"};

    //portfolio.enterTrade();




    
    


    return 0;
}