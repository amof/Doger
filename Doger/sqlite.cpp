#include "sqlite.h"

SqLite::SqLite()
{

}

void SqLite::openDB(QString dbName){

    if(db.isOpen()){
        db.close();
        QSqlDatabase::removeDatabase("QSQLITE");
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);

    db.open();
    if(!db.open()) qDebug()<<"[SQLite] Problème lors de l'ouverture de la BD "<<db.lastError();


}

void SqLite::closeDB(){

    if(db.isOpen()){
        db.close();
        QSqlDatabase::removeDatabase("QSQLITE");
    }
    else{
        qDebug()<<"[SQLite] Problème lors de la fermeture de la BD "<<db.lastError();
    }
}

void SqLite::addCategoryBrand(int sqlite_name, QString name){

    QString Squery="";
    QSqlQuery query;

    switch (sqlite_name) {
    case sqlite_CATEGORY:
        Squery = "INSERT INTO Categories VALUES (NULL , :name)";
        break;

    case sqlite_BRAND:
        Squery = "INSERT INTO Brands VALUES (NULL , :name)";
        break;
    default:
        break;
    }

    query.prepare(Squery);
    query.bindValue(":name", name);

    if(!query.exec()) qDebug()<<"[SQLite] Erreur dans l'ajout de "<< name<<" : "<<query.lastError();
}

void SqLite::deleteRecord(int sqlite_name, int id){

    QString Squery="";
    QSqlQuery query;

    switch (sqlite_name) {
    case sqlite_CATEGORY:
        Squery = "DELETE FROM Categories WHERE id_category =:id";
        break;

    case sqlite_BRAND:
        Squery = "DELETE FROM Brands WHERE id_brand =:id";
        break;

    case sqlite_ITEM:
        Squery = "DELETE FROM Items WHERE id_item=:id";
        break;

    case sqlite_FOOD:
        Squery = "DELETE FROM Food WHERE id_food=:id";
        break;
    case sqlite_LIST:
        Squery = "DELETE FROM Lists WHERE id_list=:id";
        break;
    case sqlite_ITEMLIST:
        Squery = "DELETE FROM ItemsLists WHERE id_list=:id";
        break;

    default:
        break;
    }

    query.prepare(Squery);
    query.bindValue(":id", id);
    if(!query.exec()) qDebug()<<"[SQLite] Erreur lors de la suppression de ID= "<< QString::number(id)<<" : "<<query.lastError();
}

void SqLite::modifyCategory(int id, QString name) {

    QString Squery="UPDATE Categories SET name=:name WHERE id_category=:idCategory";
    QSqlQuery query;
    query.prepare(Squery);
    query.bindValue(":name", name);
    query.bindValue(":idCategory", id);

    if(!query.exec()) qDebug()<<"[SQLite] Erreur dans la modification d'une catégorie "<<db.lastError();
}

void SqLite::modifyBrand(int id, QString name) {

    QString Squery="UPDATE Brands SET name=:name WHERE id_brand=:idBrand";
    QSqlQuery query;
    query.prepare(Squery);
    query.bindValue(":name", name);
    query.bindValue(":idBrand", id);

    if(!query.exec()) qDebug()<<"[SQLite] Erreur dans la modification d'une marque "<<db.lastError();
}

void SqLite::addModifyItem(ItemStruct itemStruct){

    QString Squery = "";
    QSqlQuery query;

    if(itemStruct.id_item==0){
        Squery="INSERT INTO Items VALUES (NULL , :id_category, :id_brand, :reference, :weight, :quantity, :volume, :dateOfAcquisition, :price, :desired, :url_manufacturer, :url_RL, :note)";
    }else if(itemStruct.id_item>0){
        Squery="UPDATE Items SET id_category=:id_category,"
                               "id_brand=:id_brand,"
                               "reference=:reference,"
                               "weight=:weight,"
                               "quantity=:quantity,"
                               "volume=:volume,"
                               "dateOfAcquisition=:dateOfAcquisition,"
                               "price=:price,"
                               "desired=:desired,"
                               "url_manufacturer=:url_manufacturer,"
                               "url_RL=:url_RL,"
                               "note=:note "
                               "WHERE id_item=:id_item";
    }

    query.prepare(Squery);

    query.bindValue(":id_category", itemStruct.id_category);
    query.bindValue(":id_brand", itemStruct.id_brand);
    query.bindValue(":reference", itemStruct.reference);
    query.bindValue(":weight", itemStruct.weight);
    query.bindValue(":quantity", itemStruct.quantity);
    query.bindValue(":volume", itemStruct.volume);
    query.bindValue(":dateOfAcquisition", itemStruct.dateOfAcquisition);
    query.bindValue(":price", itemStruct.price);
    query.bindValue(":desired", itemStruct.desired);
    query.bindValue(":url_manufacturer", itemStruct.url_manufacturer);
    query.bindValue(":url_RL", itemStruct.url_RL);
    query.bindValue(":note", itemStruct.note);

    if(itemStruct.id_item>0){
        query.bindValue(":id_item", itemStruct.id_item);
    }

    if(!query.exec()) qDebug()<<"[SQLite] Erreur dans l'ajout d'un nouvel item :"<<query.lastError();
}

void SqLite::getCategoryBrand(int sqlite_name, QVector<int> *vector, QComboBox *cb) {

    cb->clear();
    vector->clear();

    QString Squery = "";
    QSqlQuery query;

    switch (sqlite_name) {
    case sqlite_CATEGORY:
        Squery = "SELECT id_category, name FROM Categories ORDER BY name ASC";
        break;

    case sqlite_BRAND:
        Squery = "SELECT id_brand, name FROM Brands ORDER BY name ASC";
        break;
    default:
        break;

    }

    query.prepare(Squery);
    query.exec();

    while(query.next())
    {
        vector->append(query.value(0).toInt());
        cb->addItem(query.value(1).toString());
    }
}

