#include "quote.h"

constexpr char* Quote::dir;
const std::string Quote::base_url{"https://query1.finance.yahoo.com/v7/finance/download/"};

Quote::Quote(){

}

//download the latest CSV from yahoo finance for the given ticker
std::string Quote::getLatestCSV(const std::string &url, const std::string &ticker)
{
    CURL *curl;
    FILE *file;
    CURLcode res;
    std::time_t now = std::time(0);

    //create the name for the temporary .CSV file from yahoo
    std::string outputFilename{"C:/Programming/GitHub/investmenttracker/data/temp/"};
    outputFilename.append(ticker);
    outputFilename.append("_");
    outputFilename.append(std::to_string(now));
    outputFilename.append(".csv");

    //download csv file from given url
    curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        //curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        file = fopen(outputFilename.c_str(),"wb");

        if(file)
        {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
            curl_easy_perform(curl);
            fclose(file);
        }

        curl_easy_cleanup(curl);
        curl_global_cleanup();
        
        //return filename and directory path
        return outputFilename;
    }

    return 0;
}

size_t Quote::write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

//download the historical date since a given date for a the given ticker
void Quote::getHistoricalData(const std::string &ticker, const std::string &startDate)
{
    if(ticker.empty() == false && startDate.empty() == false)
    {
        //build the url from which to download the stock information
        std::string url{base_url};
        url.append(ticker);
        url.append("?period1="); //start date
        url.append(Date::convert(startDate));
        url.append("&period2="); //date today
        url.append(Date::now());
        url.append("&interval=1d");

        //create custom filename to reflect historical data
        std::string tempFileName{ticker};
        tempFileName.append("_historical");

        std::string tempfile{getLatestCSV(url, tempFileName)};

        io::CSVReader<7> in(tempfile);
        in.read_header(io::ignore_extra_column, "Date", "Open", "High", "Low", "Close", "Adj Close", "Volume");
        
        std::string Date{};
        float Open{};
        float High{};
        float Low{};
        float Close{};
        float Adj_Close{};
        float Volume{};

        std::string output{};

        //Stock stock;
        //stock.ticker = ticker;

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
        sql = "CREATE TABLE IF NOT EXISTS ";
        sql.append(ticker);
        sql.append("("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "DATE     TEXT NOT NULL, "
            "UNIXDATE     INTEGER NOT NULL, "
            "OPEN     REAL NOT NULL, "
            "HIGH     REAL NOT NULL, "
            "LOW     REAL NOT NULL, "
            "ADJ_CLOSE     REAL NOT NULL, "
            "CLOSE     REAL NOT NULL, "
            "VOLUME   REAL NOT NULL);");
        
        exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
        if (exit != SQLITE_OK) {
            std::cerr << "Error in createTable function. :" << messageError << std::endl;
            sqlite3_free(messageError);
        } else {
            std::cout << "Table created Successfully" << std::endl;
        }

        while(in.read_row(Date, Open, High, Low, Close, Adj_Close, Volume))
        {      
            std::string unixDate = Date::convert(Date);

            //check if date already exists
            sql = "SELECT EXISTS(SELECT 1 FROM '";
            sql.append(ticker);
            sql.append("' WHERE DATE='");
            sql.append(Date);
            sql.append("');");

            exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);
            if (exit != SQLITE_OK) {
                std::cerr << "Error in selectData function. :" << messageError << std::endl;
                sqlite3_free(messageError);
            } else {
                std::cout << "Records selected Successfully!" << std::endl;
            }

            if(positionExists == true){
                //delte old row
                //DELETE FROM ticker WHERE DATE = date
                
                sql = "DELTE FROM '";
                sql.append(ticker);
                sql.append("' WHERE DATE = '");
                sql.append(Date);
                sql.append("'");

            } else {
                sql = "INSERT INTO ";
                sql.append(ticker);
                sql.append(" (DATE, UNIXDATE, OPEN, HIGH, LOW, ADJ_CLOSE, CLOSE, VOLUME)");
                sql.append(" VALUES('");
                sql.append(Date);
                sql.append("', '");
                sql.append(unixDate);
                sql.append("', ");
                sql.append(std::to_string(Open));
                sql.append(", ");
                sql.append(std::to_string(High));
                sql.append(", ");
                sql.append(std::to_string(Low));
                sql.append(", ");
                sql.append(std::to_string(Adj_Close));
                sql.append(", ");
                sql.append(std::to_string(Close));
                sql.append(", ");
                sql.append(std::to_string(Volume));
                sql.append(");");

                exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
                if (exit != SQLITE_OK) {
                    std::cerr << "Error in insertData function. :" << messageError << std::endl;
                    sqlite3_free(messageError);
                } else {
                    std::cout << "Records inserted Successfully!" << std::endl;
                }
            }

        }

        sqlite3_close(DB);

    } else if(ticker.empty() == true)
    {
        throw "given ticker is empty";
    } else if(startDate.empty() == true)
    {
        throw "given start date is empty";
    }
}

//download the latest stock data since the last saved date for a given ticker
void Quote::updateStockData(const std::string &ticker){
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

    //check if ticker_historical exists
    // SELECT name FROM sqlite_master WHERE type='table' AND name='{table_name}';

    sql = "SELECT EXISTS(SELECT 1 FROM sqlite_master WHERE type='table' AND name='";
    sql.append(ticker);
    sql.append("')");

    exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);
    if (exit != SQLITE_OK) {
		std::cerr << "Error in selectData function. :" << messageError << std::endl;
		sqlite3_free(messageError);
	} else {
		std::cout << "Records selected Successfully!" << std::endl;
    }

    if(positionExists == true){
        //get last date    
        //  SELECT Date FROM 'tsla' WHERE ID=(SELECT MAX(ID) FROM 'tsla')

        sql = "SELECT Date FROM '";
        sql.append(ticker);
        sql.append("' WHERE ID=(SELECT MAX(ID) FROM '");
        sql.append(ticker);
        sql.append("')");

        exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);
        if (exit != SQLITE_OK) {
            std::cerr << "Error in selectData function. :" << messageError << std::endl;
            sqlite3_free(messageError);
        } else {
            std::cout << "Records selected Successfully!" << std::endl;
        }

        //flip date and replace "-"" with "/""
        std::string tempDateReverse;
        tempDateReverse = lastDate.substr(8,2) + "/" + lastDate.substr(5,2) + "/" + lastDate.substr(0,4);

        //use last date as input date like historical
        getHistoricalData(ticker, tempDateReverse);
 
    } else {
        std::cerr << "Error: table doesn't exist" << std::endl;
    }
}

int Quote::callback(void* NotUsed, int argc, char** argv, char** azColName){
    /* argc: holds the number of results, argv: holds each value in array, azColName: holds each column returned in array, */
    positionExists = false;
    lastDate.clear();

    for (int i = 0; i < argc; i++) {
		// column name and value
		std::cout << azColName[i] << ": " << argv[i] << std::endl;
        if(*argv[i] == '1'){
            positionExists = true;
        }
        lastDate = argv[i];
	}

	std::cout << std::endl;

	return 0;
}

std::string Quote::getLatestData(const std::string &ticker, const std::string &column){
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

    //check if ticker_historical exists
    // SELECT name FROM sqlite_master WHERE type='table' AND name='{table_name}';

    sql = "SELECT EXISTS(SELECT 1 FROM sqlite_master WHERE type='table' AND name='";
    sql.append(ticker);
    sql.append("')");

    exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);
    if (exit != SQLITE_OK) {
		std::cerr << "Error in selectData function. :" << messageError << std::endl;
		sqlite3_free(messageError);
	} else {
		std::cout << "Records selected Successfully!" << std::endl;
    }

    if(positionExists == true){
        //get last positon    
        //  SELECT colum FROM 'tsla' WHERE ID=(SELECT MAX(ID) FROM 'tsla')

        sql = "SELECT ";
        sql.append(column); //variable column selection
        sql.append(" FROM '");
        sql.append(ticker);
        sql.append("' WHERE ID=(SELECT MAX(ID) FROM '");
        sql.append(ticker);
        sql.append("')");

        exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);
        if (exit != SQLITE_OK) {
            std::cerr << "Error in selectData function. :" << messageError << std::endl;
            sqlite3_free(messageError);
        } else {
            std::cout << "Records selected Successfully!" << std::endl;
        }

        if(column == "Date"){
            //flip date and replace "-"" with "/""
            std::string tempDateReverse;
            tempDateReverse = lastDate.substr(8,2) + "/" + lastDate.substr(5,2) + "/" + lastDate.substr(0,4);
            return tempDateReverse;
        } else {
            return lastDate;
        }
    } else {
        std::cerr << "Error: table doesn't exist" << std::endl;
        return "error fetching data";
    }
}