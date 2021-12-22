#include "userAction.h"

void UserAction::inputTrade(Position &position){

    bool pass = false;
    
    //Ticker
    //need input as 5 char
    while(pass == false){
        std::string temp_ticker;
        std::size_t found;

        std::cout << "Input ticker: ";
        std::cin >> temp_ticker;

        found = temp_ticker.find("$");
        if(found != std::string::npos){
            temp_ticker.erase(found);
        }

        if(temp_ticker.size() <= 5){
            position.ticker = temp_ticker;
            pass = true;
        } else{
            std::cout << std::endl << "Wrong ticker input!" << std::endl;
        }
    }
    pass = false;

    //Date
    //need input as DD/MM/YYYY
    while(pass == false){
        std::string temp_date;

        
        std::cout << "Input the date in the DD/MM/YYYY format: ";
        std::cin >> temp_date;

        if (temp_date.size() == 10)
        {
            position.Date = temp_date;
            pass = true;
        } else{
            std::cout << std::endl << "Wrong date input!" << std::endl;
        }
    }
    pass = false;

    //Amount
    //needed as uint number
    while(pass == false){
        int temp_amount;

        std::cout << "Input the amount: ";
        std::cin >> temp_amount;

        if(temp_amount > 0){
            position.amount = static_cast<uint16_t>(temp_amount);
            pass = true;
        } else {
            std::cout << std::endl << "Wrong amount input!" << std::endl;
        }
    }
    pass = false;

    //Action Type
    //'b' is buy action, 's' is sell action
    while(pass == false){
        char temp_action;

        std::cout << "Input the type of order: ";
        std::cin >> temp_action;

        switch (temp_action)
        {
        case 'b':
            position.action = actionType::buy;
            pass = true;
            break;

        case 's':
            position.action = actionType::sell;
            pass = true;
            break;
        
        default:
            std::cout << std::endl << "Wrong order type input!" << std::endl;
            break;
        }
    }
    pass = false;

    //price
    //need as positive float
    while(pass == false){
        float temp_price;

        std::cout << "Input the stock price: ";
        std::cin >> temp_price;

        if(temp_price > 0){
            position.price = temp_price;
            pass = true;
        } else {
            std::cout << std::endl << "Wrong price input!" << std::endl;
        }
    }
}