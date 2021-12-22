#pragma once

#include <string>
#include <curl/curl.h>
#include <stdio.h>
#include <ctime>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <ctime>
#include <sqlite3.h>


#include "../libs/csv.h"
#include "../libs/tinyxml2.h"
#include "date.h"
#include "saveData.h"
#include "dataHandler.h"

class Quote{
private:
    //location of the Portfolio Database
    static constexpr char* dir{"C:/Programming/GitHub/investmenttracker/data/StockData.db"};

    //yahoo finance api base url
    static const std::string base_url;
    
    //download the latest CSV from yahoo finance for the given ticker
    static std::string getLatestCSV(const std::string &url, const std::string &ticker);
    
    //recieve data function
    static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);

    //retrieve contents from select method
    static int callback(void* NotUsed, int argc, char** argv, char** azColName);

    //variables to handle information from callback
    inline static bool positionExists = false;
    inline static std::string lastDate;

public:
    Quote();

    //download the historical date since a given date for a the given ticker
    static void getHistoricalData(const std::string &ticker, const std::string &startDate);

    //return the latest data for a given Ticker from the selected column
    static std::string getLatestData(const std::string &ticker, const std::string &column);

    //download the latest stock data since the last saved date for a given ticker
    static void updateStockData(const std::string &ticker);

};