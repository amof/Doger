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

void SqLite::deleteRecordInItemsLists(int id_list, QVector<int> id_item){

    QString Squery="DELETE FROM ItemsLists WHERE id_list=:id_list and id_item=:id_item";
    QSqlQuery query;
    db.transaction();

    for(int i=0;i<id_item.count();i++){
        query.clear();
        query.prepare(Squery);
        query.bindValue(":id_list", id_list);
        query.bindValue(":id_item", id_item.at(i));
        if(!query.exec()) qDebug()<<"[SQLite] Erreur lors de la suppression de id_list= "<< QString::number(id_list)<<" id_item:"<<id_item<<" : "<<query.lastError();

    }
    db.commit();

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

void SqLite::addModifyFood(FoodStruct food){
    QString Squery = "";
    QSqlQuery query;

    if(food.id_food==0){
        Squery = "INSERT INTO Food VALUES (NULL , :id_category, :id_brand, :reference, :weight, :quantity, :expirationDate, :price,"
                                 " :url, :energy, :fat,:carbohydrates,:fibres,:protein,:salt, :note)";;
    }else if(food.id_food>0){
        Squery="UPDATE FOOD SET id_brand=:id_brand,"
                               "reference=:reference,"
                               "weight=:weight,"
                               "quantity=:quantity,"
                               "expirationDate=:expirationDate,"
                               "price=:price,"
                               "url=:url,"
                               "energy=:energy,"
                               "fat=:fat,"
                               "carbohydrates=:carbohydrates,"
                               "fibres=:fibres,"
                               "protein=:protein,"
                               "salt=:salt,"
                               "note=:note "
                               "WHERE id_food=:id_food";
    }

    query.prepare(Squery);

    query.bindValue(":id_category", food.id_category);
    query.bindValue(":id_brand", food.id_brand);
    query.bindValue(":reference", food.reference);
    query.bindValue(":weight", food.weight);
    query.bindValue(":quantity", food.quantity);
    query.bindValue(":expirationDate",food.expirationDate);
    query.bindValue(":price", food.price);
    query.bindValue(":url", food.url);
    query.bindValue(":energy", food.energy);
    query.bindValue(":fat", food.fat);
    query.bindValue(":carbohydrates", food.carbohydrates);
    query.bindValue(":fibres", food.fibres);
    query.bindValue(":protein", food.protein);
    query.bindValue(":salt", food.salt);
    query.bindValue(":note", food.note);

    if(food.id_food>0){
        query.bindValue(":id_food", food.id_food);
    }

    if(!query.exec()) qDebug()<<"[SQLite] Erreur dans l'ajout d'un nouvel item :"<<query.lastError();
}

void SqLite::addModifyList(ListStruct list, int numberOfCategoriesInList, QTreeWidget *listDetail, QVector<int> itemAlreadyExisting){

    QString Squery="";
    QSqlQuery query;

    // Add/Update list in List table

    if(list.id_list==0){
        Squery="INSERT INTO Lists VALUES (NULL , :name, :hikeDate, :creationDate, :id_foodPlan, :weightBackpack, :weightSelf, :note)";
    }else if(list.id_list>0){
        Squery="UPDATE Lists SET name=:name,"
                               "hikeDate=:hikeDate,"
                               "creationDate=:creationDate,"
                               "id_foodPlan=:id_foodPlan,"
                               "weightBackpack=:weightBackpack,"
                               "weightSelf=:weightSelf,"
                               "note=:note "
                               "WHERE id_list=:id_list";
    }

    query.prepare(Squery);
    query.bindValue(":name",list.name);
    query.bindValue(":hikeDate",list.hikeDate);
    query.bindValue(":creationDate",list.creationDate);
    query.bindValue(":id_foodPlan",list.id_foodPlan);
    query.bindValue(":weightBackpack",list.weightBackpack);
    query.bindValue(":weightSelf",list.weightSelf);
    query.bindValue(":note",list.note);

    if(list.id_list>0){
        query.bindValue(":id_list", list.id_list);
    }

    if(query.exec()){
        int id_list=list.id_list;

        if(list.id_list==0){
            id_list = query.lastInsertId().toInt();
        }else{
            qDebug()<<"[SqLite] error last insertID";
        }

        // Update items in ItemsLists
        if(itemAlreadyExisting.count()>0){
            Squery="UPDATE ItemsLists SET quantity=:quantity,"
                                   "totalWeight=:totalWeight,"
                                   "backpackOrSelf=:backpackOrSelf "
                                   "WHERE id_list=:id_list and id_item=:id_item";
            db.transaction();

            QList<QTreeWidgetItem*> itemFound ;
            QTreeWidgetItem *parent ;

            for(int i=0;i<itemAlreadyExisting.count();i++){
                itemFound = listDetail->findItems(QString::number(itemAlreadyExisting.at(i)), Qt::MatchExactly|Qt::MatchRecursive, qListWidget(l_id));

                QString backpackOrSelf="";
                double weight=0;

                if(itemFound[0]->text(qListWidget(l_weightBackpack)).isEmpty()){
                    backpackOrSelf="self";
                    weight = itemFound[0]->text(qListWidget(l_weightSelf)).toDouble();
                }
                else if(itemFound[0]->text(qListWidget(l_weightSelf)).isEmpty()){
                    backpackOrSelf="backpack";
                    weight = itemFound[0]->text(qListWidget(l_weightBackpack)).toDouble();
                }

                query.clear();
                query.prepare(Squery);
                query.bindValue(":quantity",itemFound[0]->text(qListWidget(l_quantity)).toInt());
                query.bindValue(":totalWeight",weight);
                query.bindValue(":backpackOrSelf",backpackOrSelf);
                query.bindValue(":id_list",list.id_list);
                query.bindValue(":id_item",itemAlreadyExisting.at(i));

                if(!query.exec()) qDebug()<<"[SQLite] Erreur dans l'update d'un item de liste "<<query.lastError();

                parent = itemFound[0]->parent();
                parent->removeChild(itemFound[0]);

                if(parent->childCount()==0){
                    listDetail->takeTopLevelItem(listDetail->indexOfTopLevelItem(parent));
                    if(numberOfCategoriesInList>0)numberOfCategoriesInList--;
                }

            }

            if(!db.commit())qDebug()<<"[SQLite] Erreur dans le commit de l'update de la liste "<<db.lastError();

        }


        if(id_list!=0){

            Squery="INSERT INTO ItemsLists VALUES (:id_item , :id_list, :quantity, :totalWeight, :backpackOrSelf)";
            db.transaction();

            QTreeWidgetItem *item = new QTreeWidgetItem;

            for(int i=0;i<numberOfCategoriesInList;i++){

                item = listDetail->topLevelItem(i);
                QString backpackOrSelf="";
                double weight=0;

                for(int j=0;j<item->childCount();j++){
                    if(item->child(j)->text(qListWidget(l_weightBackpack)).isEmpty()){
                        backpackOrSelf="self";
                        weight = item->child(j)->text(qListWidget(l_weightSelf)).toDouble();
                    }
                    else if(item->child(j)->text(qListWidget(l_weightSelf)).isEmpty()){
                        backpackOrSelf="backpack";
                        weight = item->child(j)->text(qListWidget(l_weightBackpack)).toDouble();
                    }
                    query.clear();
                    query.prepare(Squery);

                    query.bindValue(":id_item",item->child(j)->text(qListWidget(l_id)).toInt());
                    query.bindValue(":id_list",id_list);
                    query.bindValue(":quantity",item->child(j)->text(qListWidget(l_quantity)).toInt());
                    query.bindValue(":totalWeight",weight);
                    query.bindValue(":backpackOrSelf",backpackOrSelf);

                    if(!query.exec()) qDebug()<<"[SQLite] Erreur dans l'ajout d'un item de liste "<<query.lastError();

                }
            }
            if(!db.commit())qDebug()<<"[SQLite] Erreur dans le commit de l'ajout de la liste "<<db.lastError();

            item = NULL;
            delete item;

        }
    }
    else{
        qDebug()<<"[SQLite] Erreur dans l'ajout d'une liste "<<query.lastError();
    }
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
    if(!query.exec()) qDebug()<<"[SQLite] Erreur dans la recupération de la catégorie/marque "<<query.lastError();


    while(query.next())
    {
        vector->append(query.value(0).toInt());
        cb->addItem(query.value(1).toString());
    }
}

