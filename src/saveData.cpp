#include "saveData.h"

void Save::saveLatestQuote(const std::string &ticker, const std::string &data)
{    
    //extract the separate infos from the data
    std::string date{data.substr(0,10)};
    std::string price{data.substr(10,10)};
    std::string volume{data.substr(20,10)};

    //set filename and path
    std::string filename{"C:/Programming/GitHub/investmenttracker/data/"};
    filename.append(ticker);
    filename.append(".xml");

    //create the new structure
	tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* Stock = doc.NewElement("Stock");
    doc.InsertFirstChild(Stock);

    //enter the ticker 
    tinyxml2::XMLElement* Ticker = doc.NewElement("Ticker");
    Ticker->SetText(ticker.c_str());
    Stock->InsertEndChild(Ticker);

    //create dates
    tinyxml2::XMLElement* Dates = doc.NewElement("Dates");
    Stock->InsertEndChild(Dates);

    //enter the Date
    tinyxml2::XMLElement* Date = doc.NewElement(date.c_str());
    Dates->InsertEndChild(Date);

    //enter the price
    tinyxml2::XMLElement* Price = doc.NewElement("Price");
    Price->SetText(price.c_str());
    Date->InsertEndChild(Price);

    //enter the volume
    tinyxml2::XMLElement* Volume = doc.NewElement("Volume");
    Volume->SetText(volume.c_str());
    Date->InsertEndChild(Volume);



    doc.SaveFile(filename.c_str());

}

void Save::saveData(const Stock& stock){
    //set filename and path
    std::string filename{"C:/Programming/GitHub/investmenttracker/data/"};
    filename.append(stock.ticker);
    filename.append("_historical.xml");

    //create the new structure
	tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* Stock = doc.NewElement("Stock");
    doc.InsertFirstChild(Stock);

    //enter the ticker 
    tinyxml2::XMLElement* Ticker = doc.NewElement("Ticker");
    Ticker->SetText(stock.ticker.c_str());
    Stock->InsertEndChild(Ticker);

    //create dates
    tinyxml2::XMLElement* Dates = doc.NewElement("Dates");
    Stock->InsertEndChild(Dates);

    for(int i = 0; i < stock.Date.size(); ++i){
        //enter the Date
        tinyxml2::XMLElement* Date = doc.NewElement(stock.Date[i].c_str());
        Dates->InsertEndChild(Date);

        //enter the price
        tinyxml2::XMLElement* Price = doc.NewElement("Price");
        Price->SetText(stock.Close[i]);
        Date->InsertEndChild(Price);

        //enter the volume
        tinyxml2::XMLElement* Volume = doc.NewElement("Volume");
        Volume->SetText(stock.Volume[i]);
        Date->InsertEndChild(Volume);
    }

    doc.SaveFile(filename.c_str());

}

void Save::savePosition(const Position &position){
    //check if portfolio.xml file exists. create a new file if it doesn´t exit
    tinyxml2::XMLDocument doc;
	doc.LoadFile("C:/Programming/GitHub/investmenttracker/data/portfolio.xml");

	if(doc.ErrorID() != 0){
        static const char* xml = "<?xml version=\"1.0\"?>";
        doc.Parse(xml);

        doc.SaveFile("C:/Programming/GitHub/investmenttracker/data/portfolio.xml");
    }


}