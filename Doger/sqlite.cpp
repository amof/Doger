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

void SqLite::addCategoryBrand(int catBrand, QString name){

    QString Squery="";
    QSqlQuery query;

    switch (catBrand) {
    case CATEGORY:
        Squery = "INSERT INTO Categories VALUES (NULL , :name)";
        break;

    case BRAND:
        Squery = "INSERT INTO Brands VALUES (NULL , :name)";
        break;
    default:
        break;
    }

    query.prepare(Squery);
    query.bindValue(":name", name);

    if(!query.exec()) qDebug()<<"[SQLite] Erreur dans l'ajout de "<< name<<" : "<<db.lastError();
}

void SqLite::deleteCategoryBrand(int catBrand, int id){

    QString Squery="";
    QSqlQuery query;

    switch (catBrand) {
    case CATEGORY:
        Squery = "DELETE FROM Categories WHERE id_category =:id";
        break;

    case BRAND:
        Squery = "DELETE FROM Brands WHERE id_brand =:id";
        break;
    default:
        break;
    }

    query.prepare(Squery);
    query.bindValue(":id", id);
    if(!query.exec()) qDebug("Erreur lors de la suppression.");
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

void SqLite::getCategoryBrand(int catBrand, QVector<int> *vector, QComboBox *cb) {

    cb->clear();
    vector->clear();

    QString Squery = "";
    QSqlQuery query;

    switch (catBrand) {
    case CATEGORY:
        Squery = "SELECT id_category, name FROM Categories ORDER BY name ASC";
        break;

    case BRAND:
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

