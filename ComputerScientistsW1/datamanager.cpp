#include "DataManager.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <QSqlQuery>
#include <QSqlRecord>

// having problems with queries? try this
//    auto error = query.lastError();
//    if (error.isValid()) {
//        cout << "ERROR: " << error.text().toStdString() << endl;
//        cout << query.lastQuery().toStdString() << endl;
//    }

void DataManager::initializeTables()
{
    query.exec("create table IF NOT EXISTS scientists ("
               "ID INTEGER primary key NOT NULL,"
               "name varchar(100) NOT NULL,"
               "sex char(1) NOT NULL,"
               "birth INT NOT NULL,"
               "death INT,"
               "About text)"
              );

    query.exec("create table IF NOT EXISTS pctype ("
               "ID INTEGER primary key NOT NULL,"
               "type varchar(100) NOT NULL)"
              );

    query.exec("create table IF NOT EXISTS computers ("
               "ID INTEGER primary key NOT NULL,"
               "name varchar(255) NOT NULL,"
               "buildyear INT,"
               "type INT REFERENCES pctype(ID) NOT NULL,"
               "wasbuilt INT,"
               "About text)"
              );

    query.exec("create table users ("
               "ID INTEGER primary key NOT NULL,"
               "scientistID INT REFERENCES scientists(ID) NOT NULL,"
               "computerID INT REFERENCES computers(ID) NOT NULL)"
              );

    query.exec("INSERT OR REPLACE INTO pctype VALUES (1, 'Electronic')");
    query.exec("INSERT OR REPLACE INTO pctype VALUES (2, 'Mecanic')");
    query.exec("INSERT OR REPLACE INTO pctype VALUES (3, 'Transistor Machine')");
}

DataManager::DataManager(string dataBaseLocation)
{
    this->fileName = dataBaseLocation;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QString(fileName.c_str()));
    db.open();

    query = QSqlQuery(db);
    initializeTables();
}

vector<Computer> DataManager::getComputersFromScientistId(int scientistId)
{
    vector<Computer> allRelatedComputers;
    QSqlQuery query(db);
    query.prepare("SELECT computers.*"
                  " FROM computers"
                  " JOIN users on computers.ID = users.computerID"
                  " WHERE users.scientistID = :sciID");
    query.bindValue(":sciID", scientistId);
    query.exec();

    while(query.next())
    {
        allRelatedComputers.push_back(getNextComputerQuery(query));
    }
    return allRelatedComputers;
}

vector<Scientist> DataManager::getScientistsFromComputerId(int computerId)
{
    vector<Scientist> allRelatedScientist;
    QSqlQuery query(db);
    query.prepare("SELECT scientists.*"
                  " FROM scientists"
                  " JOIN users on scientists.ID = users.scientistID"
                  " WHERE users.computerID = :comID");
    query.bindValue(":comID", computerId);
    query.exec();
    while(query.next())
    {
        allRelatedScientist.push_back(getNextScientistQuery(query));
    }
    return allRelatedScientist;
}

vector<TypeOfComputer> DataManager::getTypeOfComputers()
{
    vector<TypeOfComputer> returnTypes;

    QSqlQuery record("SELECT * FROM pctype");
    while (record.next())
    {
        int id = record.value(0).toUInt();
        string name = record.value(1).toString().toStdString();
        TypeOfComputer newType(id,name);
        returnTypes.push_back(newType);
    }
    return returnTypes;
}

string DataManager::getTypeOfComputerFromId(int id)
{
    string toQstring = "SELECT type FROM pctype WHERE id == " + to_string(id);
    QSqlQuery record(toQstring.c_str());
    record.next();
    return record.value(0).toString().toStdString();
}

Scientist DataManager::getScientistFromId(int id)
{
    //QSqlQuery query(db); // may or maynot be needed uncomment if database seems flaky
    query.prepare("SELECT * FROM scientists WHERE id == :id");
    query.bindValue(":id", id);
    query.exec();
    query.next();
    return getNextScientistQuery(query);
}

Computer DataManager::getComputerFromId(int id)
{
    //QSqlQuery query(db); // may or maynot be needed uncomment if database seems flaky
    query.prepare("SELECT * FROM computers WHERE id == :id");
    query.bindValue(":id", id);
    query.exec();
    query.next();
    return getNextComputerQuery(query);
}

void DataManager::addTypeOfComputer(string type)
{
    string newType = "INSERT OR REPLACE INTO pctype (type) VALUES ('" + type + "')";
    query.exec(newType.c_str());
    db.commit();
}

void DataManager::addUser(int userId, int computerId)
{
    string newType = "INSERT OR REPLACE INTO users (scientistID,computerID) VALUES ('"
            + to_string(userId) + "','"
            + to_string(computerId) + "' )";

    query.exec(newType.c_str());
    db.commit();
}

// edit type?
//void DataManager::editTypeOfComputer(TypeOfComputer type)
//{
//    query.exec("");
//}

void DataManager::removeFromScientist(int id)
{
    string deleteSci = "DELETE FROM scientists WHERE ID = " + to_string(id);

    query.exec(deleteSci.c_str());
    db.commit();
}

void DataManager::removeFromComputer(int id)
{
    string deleteComp = "DELETE FROM computer WHERE ID = " + to_string(id);

    query.exec(deleteComp.c_str());
    db.commit();
}

DataManager::~DataManager(){}

/*!
 * \brief DataManager::addScientist Adds a scientist to the cvs file
 * \param Scientist
 */
void DataManager::addScientist(Scientist scientis)
{
    string currScientist = "";
    if (scientis.getDeathYear() == 0)
    {
         currScientist = "INSERT INTO scientists(name,sex,birth,about)"
              "VALUES('" + scientis.getName()
                 + "','" + scientis.getSex()
                 + "','" + to_string(scientis.getBirthYear())
                 + "','" + scientis.getAbout() + "')";
    }
    else
    {
        currScientist = "INSERT INTO scientists(name,sex,birth,death,about)"
             "VALUES('" + scientis.getName()
                + "','" + scientis.getSex()
                + "','" + to_string(scientis.getBirthYear())
                + "','" + to_string(scientis.getDeathYear())
                + "','" + scientis.getAbout() + "')";
    }

    query.exec(currScientist.c_str());
}

void DataManager::addComputer(Computer comp)
{
    query.prepare("INSERT INTO computers(name, buildyear, type, wasbuilt, about)"
                  "VALUES(:name, :buildyear, :type, :wasbuilt, :about)");
    query.bindValue(":name", comp.getName().c_str());
    query.bindValue(":buildyear", comp.getBuildYear());
    query.bindValue(":type", comp.getType());
    query.bindValue(":wasbuilt", comp.getWasItBuilt());
    query.bindValue(":about", comp.getAbout().c_str());
    query.exec();
}

vector<Scientist> DataManager::getAllScientists(ScientistSortOrder sort)
{
    vector<Scientist> allScientists;

    QSqlQuery query(db);

    // Otputs the list as the user wants it sorted
    string command = "SELECT * FROM scientists ORDER BY " + sort.getSortByString() + " " + sort.getDirectionString();
    query.exec(command.c_str());

    // Creates a scientist from the values and inserts the scientist to a vector
    while(query.next())
    {
        allScientists.push_back(getNextScientistQuery(query));
    }

    return allScientists;
}
// query.next MUST be called before or it will fetch old/wrong data
Scientist DataManager::getNextScientistQuery(QSqlQuery query)
{
    int id = query.value("ID").toUInt();
    string name = query.value("name").toString().toStdString();
    string sex = query.value("sex").toString().toStdString();
    int birth = query.value("birth").toUInt();
    int death = query.value("death").toUInt();
    string about = query.value("About").toString().toStdString();

    return Scientist(name, sex[0], birth, death, about, id);
}

vector<Computer> DataManager::getAllComputers(ScientistSortOrder sort)
{
    vector<Computer> allComputers;
    QSqlQuery query(db);

    // Otputs the list as the user wants it sorted
    string command = "SELECT * FROM computers ORDER BY " + sort.getSortByString() + " " + sort.getDirectionString();
    query.exec(command.c_str());

    // Creates a computer from the values and inserts the scientist to a vector
    while(query.next())
    {
        allComputers.push_back(getNextComputerQuery(query));
    }
    return allComputers;
}

// query.next MUST be called before or it will fetch old/wrong data
Computer DataManager::getNextComputerQuery(QSqlQuery query)
{
    int id = query.value("ID").toUInt();
    string name = query.value("name").toString().toStdString();
    int buildYear = query.value("buildyear").toUInt();
    int type = query.value("type").toUInt();
    bool wasItBuilt = query.value("wasbuilt").toUInt();
    string about = query.value("about").toString().toStdString();

    return Computer(name, buildYear, type, wasItBuilt, about, id);
}

vector<Scientist> DataManager::findScientistByName(string subString, ScientistSortOrder sort)
{
    vector<Scientist> allScientists = getAllScientists(sort);
    vector<Scientist> matchingScientists;
    string temp;
    string name;

    //sets substring to all lowercase
    for(unsigned int i = 0; i < subString.length(); i++)
    {
        subString[i] = tolower(subString[i]);
    }

    //Checks all scientists and inserts the ones who's name matches name parameter into the vector matchingScientists
    for(unsigned int i = 0; i < allScientists.size(); i++)
    {
        name = allScientists[i].getName();

        //sets the scientist name to all lowercase
        for(unsigned int j = 0; j < name.length(); j++)
        {
            temp += tolower(name[j]);
        }

        //inserts the matching scientists to a vector
        if(temp.find(subString) != std::string::npos)
        {
             matchingScientists.push_back(allScientists[i]);
        }
        temp = "";
    }
    return matchingScientists;
}

vector<Computer> DataManager::findComputerByName(string subString, ScientistSortOrder sort)
{
    vector<Computer> allComputers = getAllComputers(sort);
    vector<Computer> matchingComputers;
    string temp;
    string name;

    //sets substring to all lowercase
    for(unsigned int i = 0; i < subString.length(); i++)
    {
        subString[i] = tolower(subString[i]);
    }

    //Checks all scientists and inserts the ones who's name matches name parameter into the vector matchingScientists
    for(unsigned int i = 0; i < allComputers.size(); i++)
    {
        name = allComputers[i].getName();

        //sets the scientist name to all lowercase
        for(unsigned int j = 0; j < name.length(); j++)
        {
            temp += tolower(name[j]);
        }

        //inserts the matching scientists to a vector
        if(temp.find(subString) != std::string::npos)
        {
             matchingComputers.push_back(allComputers[i]);
        }
        temp = "";
    }
    return matchingComputers;
}

vector<Scientist> DataManager::findByBirthYear(int yearFrom, int yearTo, ScientistSortOrder sort)
{
    vector<Scientist> allScientists = getAllScientists(sort);
    vector<Scientist> matchingScientists;

    //Checks all scientists and inserts the ones who's birth year matches year parameter into the vector matchingScientists
    for(unsigned int i = 0; i < allScientists.size(); i++)
    {
        if(allScientists[i].getBirthYear() > yearFrom && allScientists[i].getBirthYear() < yearTo)
        {
            matchingScientists.push_back(allScientists[i]);
        }
    }
    return matchingScientists;
}

vector<Scientist> DataManager::findByDeathYear(int yearFrom, int yearTo, ScientistSortOrder sort)
{
    vector<Scientist> allScientists = getAllScientists(sort);
    vector<Scientist> matchingScientists;

    //Checks all scientists and inserts the ones who's death year matches year parameter into the vector matchingScientists
    for(unsigned int i = 0; i < allScientists.size(); i++)
    {
        if(allScientists[i].getDeathYear() > yearFrom && allScientists[i].getDeathYear() < yearTo)
        {
            matchingScientists.push_back(allScientists[i]);
        }
    }
    return matchingScientists;
}

vector<Scientist> DataManager::findBySex(string sex, ScientistSortOrder sort)
{
    vector<Scientist> allScientists = getAllScientists(sort);
    vector<Scientist> matchingScientists;

    //Checks all scientists and inserts the ones who's sex matches sex parameter into the vector matchingScientists
    for(unsigned int i = 0; i < allScientists.size(); i++)
    {
        if(allScientists[i].getSex() == sex[0])
        {
            matchingScientists.push_back(allScientists[i]);
        }
    }
    return matchingScientists;
}

/*!
 * \brief DataManager::getage Calculates age of scientist.
 * \param oneScientist
 * \returns the age of the given scientist
 */
int DataManager::getage(Scientist oneScientist)
{
    if(oneScientist.getDeathYear() == 0)
    {
        return 2015 - oneScientist.getBirthYear();
    }
    return oneScientist.getDeathYear() - oneScientist.getBirthYear();
}
