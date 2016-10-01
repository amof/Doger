#ifndef SQLITE_H
#define SQLITE_H

#include <QtSql>
#include <QComboBox>

class SqLite
{
public:
    SqLite();
    void openDB(QString dbName);
    void closeDB();

    void addCategoryBrand(int sqlite_name, QString name);
    void deleteCBIF(int sqlite_name, int id);

    void modifyCategory(int id, QString name);
    void modifyBrand(int id, QString name);

    void getCategoryBrand(int sqlite_name, QVector<int> *vector, QComboBox *cb);

public:

    #define sqlite_CATEGORY    1
    #define sqlite_BRAND       2
    #define sqlite_ITEM        3
    #define sqlite_FOOD        4

private:
    QSqlDatabase db ;
};

#endif // SQLITE_H
