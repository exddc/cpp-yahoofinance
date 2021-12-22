#pragma once

#include <string>
#include <vector>

enum actionType{
    buy, 
    sell
};

class Stock{
public:
    Stock();
    //virtual ~Stock();

    std::string ticker;
    std::vector<std::string> Date;
    std::vector<float> Open;
    std::vector<float> High;
    std::vector<float> Low;
    std::vector<float> Close;
    std::vector<float> Adj_Close;
    std::vector<float> Volume;
};

class Position{
public:
    Position();

    std::string ticker;
    std::string Date;
    uint16_t amount;
    actionType action;
    float price;
    float commission;
};

