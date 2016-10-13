#ifndef SQLITE_H
#define SQLITE_H

#include <QtSql>
#include <QComboBox>

struct ItemStruct{
    int id_item;
    int id_category;
    int id_brand;
    QString reference;
    double weight;
    int quantity;
    double volume;
    QString dateOfAcquisition;
    double price;
    bool desired;
    QString url_manufacturer;
    QString url_RL;
    QString note;
};

class SqLite
{

public:

    #define sqlite_CATEGORY    1
    #define sqlite_BRAND       2
    #define sqlite_ITEM        3
    #define sqlite_FOOD        4
    #define sqlite_LIST        5
    #define sqlite_ITEMLIST    6

public:
    SqLite();
    void openDB(QString dbName);
    void closeDB();

    void addCategoryBrand(int sqlite_name, QString name);
    void deleteRecord(int sqlite_name, int id);

    void modifyCategory(int id, QString name);
    void modifyBrand(int id, QString name);

    void addModifyItem(ItemStruct itemStruct);

    void getCategoryBrand(int sqlite_name, QVector<int> *vector, QComboBox *cb);

private:
    QSqlDatabase db ;
};

#endif // SQLITE_H
