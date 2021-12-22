#include "portfolio.h"

Portfolio::Portfolio(){}

//enter a new trade
void Portfolio::enterTrade(){

    //get the new positon and details from the user
    Position newPosition;
    UserAction::inputTrade(newPosition);

    
    sqlite3* DB;
    char* messageError;
    std::string sql;
    int exit = 0;

    //open or create the Portfolio Database
    exit = sqlite3_open(dir, &DB);
  
    if (exit) {
        std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
    } else {
        std::cout << "Opened Database Successfully!" << std::endl;
    }

    //create the table if it doesn't exist
    sql = "CREATE TABLE IF NOT EXISTS TRADINGHISTORY("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "TICKER      TEXT NOT NULL, "
        "DATE     TEXT NOT NULL, "
        "TYPE     TEXT NOT NULL, "
        "AMOUNT       INT  NOT NULL, "
        "PRICE   REAL NOT NULL);";
    
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "Error in createTable function. :" << messageError << std::endl;
        sqlite3_free(messageError);
    } else {
        std::cout << "Table created Successfully" << std::endl;
    }

    //insert the given trade
    sql = "INSERT INTO TRADINGHISTORY (TICKER, DATE, TYPE, AMOUNT, PRICE)";
    sql.append(" VALUES('");
    sql.append(newPosition.ticker);
    sql.append("', '");
    sql.append(newPosition.Date);
    sql.append("', ");
    if(newPosition.action == buy){
        sql.append("'buy', ");
    } else {
        sql.append("'sell', ");
    }
    sql.append(std::to_string(newPosition.amount));
    sql.append(", ");
    sql.append(std::to_string(newPosition.price));
    sql.append(");");

    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
	if (exit != SQLITE_OK) {
		std::cerr << "Error in insertData function. :" << messageError << std::endl;
		sqlite3_free(messageError);
	} else {
		std::cout << "Records inserted Successfully!" << std::endl;
    }

    //close the portfolio database
    sqlite3_close(DB);

    updatePositions(newPosition);
}

//update the positions
void Portfolio::updatePositions(const Position &position){
    sqlite3* DB;
    char* messageError;
    std::string sql;
    int exit = 0;

    //open or create the Portfolio Database
    exit = sqlite3_open(dir, &DB);
  
    if (exit) {
        std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
    } else {
        std::cout << "Opened Database Successfully!" << std::endl;
    }

    //create the table if it doesn't exist
    sql = "CREATE TABLE IF NOT EXISTS POSITIONS("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "NAME      TEXT NOT NULL, "
        "TICKER      TEXT NOT NULL, "
        "AMOUNT       INT  NOT NULL, "
        "PRICE     REAL NOT NULL, "
        "POSITIONVALUE     REAL NOT NULL, "
        "ENTRYDATE     TEXT NOT NULL, "
        "ENTRYPRICE   REAL NOT NULL);";
    
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "Error in createTable function. :" << messageError << std::endl;
        sqlite3_free(messageError);
    } else {
        std::cout << "Table created Successfully" << std::endl;
    }

    //check if ticker already exists
    sql = "SELECT EXISTS(SELECT 1 FROM POSITIONS WHERE TICKER='";
    sql.append(position.ticker);
    sql.append("');");

    exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);
    if (exit != SQLITE_OK) {
		std::cerr << "Error in selectData function. :" << messageError << std::endl;
		sqlite3_free(messageError);
	} else {
		std::cout << "Records selected Successfully!" << std::endl;
    }

    if(positionExists == true){
        //insert changes to existing positon
        sql = "UPDATE POSITIONS SET AMOUNT = AMOUNT";
        if(position.action == buy){
            sql.append("+");
        } else {
            sql.append("-");
        }
        sql.append(std::to_string(position.amount));
        sql.append(" WHERE TICKER = '");
        sql.append(position.ticker);
        sql.append("';");

        exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
        if (exit != SQLITE_OK) {
            std::cerr << "Error in updateData function. :" << messageError << std::endl;
            sqlite3_free(messageError);
        } else {
            std::cout << "Records updated Successfully!" << std::endl;
        }

        //UPDATE POSITIONS SET AMOUNT = 'A' WHERE TICKER = 'Cooper'
        //"UPDATE PRODUCTTABLE SET Product = Product +1;"

    } else {
        //insert new position

        sql = "INSERT INTO POSITIONS (NAME, TICKER, AMOUNT, PRICE, POSITIONVALUE, ENTRYDATE, ENTRYPRICE)";
        sql.append(" VALUES('");
        sql.append(position.ticker);
        sql.append("', '");
        sql.append(position.ticker);
        sql.append("', '");
        sql.append(std::to_string(position.amount));
        sql.append("', '");
        sql.append(std::to_string(position.price));
        sql.append("', '");
        sql.append(std::to_string(position.price * position.amount));
        sql.append("', '");
        sql.append(position.Date);
        sql.append("', '");
        sql.append(std::to_string(position.price));
        sql.append("');");

        exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
        if (exit != SQLITE_OK) {
            std::cerr << "Error in insertData function. :" << messageError << std::endl;
            sqlite3_free(messageError);
        } else {
            std::cout << "Records inserted Successfully!" << std::endl;
        }

        //call quote::getHistoricalData
        Quote::getHistoricalData(position.ticker, position.Date);

        setLatestPrice(position.ticker, Quote::getLatestData(position.ticker, "CLOSE"));
    }

    sqlite3_close(DB);
}

int Portfolio::callback(void* NotUsed, int argc, char** argv, char** azColName){
    /* argc: holds the number of results, argv: holds each value in array, azColName: holds each column returned in array, */
    positionExists = false;

    for (int i = 0; i < argc; i++) {
		// column name and value
		std::cout << azColName[i] << ": " << argv[i] << std::endl;
        if(*argv[i] == '1'){
            positionExists = true;
        }
	}

	std::cout << std::endl;

	return 0;
}

void Portfolio::setLatestPrice(const std::string &ticker, const std::string &pPrice){
    sqlite3* DB;
    char* messageError;
    std::string sql;
    int exit = 0;

    //open or create the Portfolio Database
    exit = sqlite3_open(dir, &DB);
  
    if (exit) {
        std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
    } else {
        std::cout << "Opened Database Successfully!" << std::endl;
    }

    //check if ticker already exists
    sql = "SELECT EXISTS(SELECT 1 FROM POSITIONS WHERE TICKER='";
    sql.append(ticker);
    sql.append("');");

    exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);
    if (exit != SQLITE_OK) {
		std::cerr << "Error in selectData function. :" << messageError << std::endl;
		sqlite3_free(messageError);
	} else {
		std::cout << "Records selected Successfully!" << std::endl;
    }

    if(positionExists == true){
        //insert changes to existing positon
        sql = "UPDATE POSITIONS SET PRICE = ";
        sql.append(pPrice);
        sql.append(" WHERE TICKER = '");
        sql.append(ticker);
        sql.append("';");

        exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
        if (exit != SQLITE_OK) {
            std::cerr << "Error in updateData function. :" << messageError << std::endl;
            sqlite3_free(messageError);
        } else {
            std::cout << "Price updated Successfully!" << std::endl;
        }

        //update the position value
        sql = "UPDATE POSITIONS SET POSITIONVALUE = AMOUNT * PRICE WHERE TICKER = '";
        sql.append(ticker);
        sql.append("';");

        exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
        if (exit != SQLITE_OK) {
            std::cerr << "Error in updateData function. :" << messageError << std::endl;
            sqlite3_free(messageError);
        } else {
            std::cout << "Positionvalue updated Successfully!" << std::endl;
        }
    } else {
        std::cerr << "Ticker not found!" << std::endl;
    }
}