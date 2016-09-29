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

    void addCategoryBrand(int catBrand, QString name);
    void deleteCategoryBrand(int catBrand, int id);

    void modifyCategory(int id, QString name);
    void modifyBrand(int id, QString name);

    void getCategoryBrand(int catBrand, QVector<int> *vector, QComboBox *cb);

public:
    #define CATEGORY    1
    #define BRAND   2

private:
    QSqlDatabase db ;
};

#endif // SQLITE_H
