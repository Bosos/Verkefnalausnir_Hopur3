#ifndef dataManager
#define dataManager
#include "stdlib.h"
#include <vector>
#include "Scientist.h"
#include "Computer.h"
#include <SortBy.h>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <TypeOfComputer.h>

using namespace std;

/*!
 * \brief The DataManager class
 * It's responsible for comunicating with the data storage
 */
class DataManager
{
public:
    DataManager(string fileName);

    // Scientists
    void addScientist(Scientist scientis);
    void updateScientist(Scientist scientis);
    void removeFromScientist(int id);
    Scientist getScientistFromId(int id);
    vector<Scientist> getAllScientists (ScientistSortOrder sort);
    vector<Scientist> getScientistsFromComputerId(int computerId);
    vector<Scientist> findScientistByName (string name, ScientistSortOrder sort);
    vector<Scientist> findByBirthYear (int yearFrom, int yearTo, ScientistSortOrder sort);
    vector<Scientist> findByDeathYear (int yearFrom, int yearTo, ScientistSortOrder sort);
    vector<Scientist> findBySex(string sex, ScientistSortOrder sort);
    int getAge(Scientist oneScientist);

    // Computers
    void addComputer(Computer comp);
    void updateComputer(Computer comp);
    void removeFromComputer(int id);
    void addTypeOfComputer(string type);
    Computer getComputerFromId(int id);
    vector<Computer> getAllComputers (ComputerSortOrder sort);
    vector<Computer> findComputerByName(string subString, ComputerSortOrder sort);
    vector<Computer> findComputerByBuildYear (int yearFrom, int yearTo, ComputerSortOrder sort);
    vector<Computer> findComputerByType (int type, ComputerSortOrder sort);
    vector<Computer> findComputerByWasItBuilt (bool wasBuilt ,ComputerSortOrder sort);
    vector<Computer> getComputersFromScientistId(int scientistId);

    // Computer-Scientists relations
    void addCSRelation(int userId, int computerId);
    void removeCSRelation(int userId, int computerId);
    vector<TypeOfComputer> getAllTypesOfComputers();
    string getTypeOfComputerFromId(int id);

private:
    void initializeTables();
    Scientist getNextScientistQuery(QSqlQuery query);
    Computer getNextComputerQuery(QSqlQuery query);

    string fileName;
    QSqlDatabase db;
    QSqlQuery query;
};

#endif // dataManager



