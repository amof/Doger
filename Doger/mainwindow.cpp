#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sqlite = new SqLite;
    model = new QSqlQueryModel();
    filter = new QSortFilterProxyModel();
    modelFood = new QSqlQueryModel();
    filterFood = new QSortFilterProxyModel();
    modelList = new QSqlQueryModel();
    dynList = new QSqlQueryModel();
    filterdynList = new QSortFilterProxyModel();
    listwindow = new ListWindow(0, sqlite);

    id_list=0;

    ui->stackedWidget->setCurrentIndex(0);
    ui->frame_title->hide();
    ui->btn_config_categorie_delete->hide();
    ui->btn_config_categorie_save->hide();
    ui->le_config_categoryName->hide();
    ui->btn_config_brand_delete->hide();
    ui->btn_config_brand_save->hide();
    ui->le_config_brandName->hide();

    ui->tw_material->setCurrentIndex(0);

    ui->lbl_backpack->setPixmap(QPixmap(":/images/backpack_2.png").scaled(75,75,Qt::KeepAspectRatio));
    ui->lbl_self->setPixmap(QPixmap(":/images/self.png").scaled(75,75,Qt::KeepAspectRatio));

    ui->tw_listDetail->setColumnCount(6);
    ui->tw_listDetail->setHorizontalHeaderItem(0, new QTableWidgetItem("Catégorie"));
    ui->tw_listDetail->setHorizontalHeaderItem(1, new QTableWidgetItem("Marque"));
    ui->tw_listDetail->setHorizontalHeaderItem(2, new QTableWidgetItem("Référence"));
    ui->tw_listDetail->setHorizontalHeaderItem(3, new QTableWidgetItem("Poids"));
    ui->tw_listDetail->setHorizontalHeaderItem(4, new QTableWidgetItem("Quantité"));
    ui->tw_listDetail->setHorizontalHeaderItem(5, new QTableWidgetItem("SAD/Soi"));

    loadConfig();
    loadDatabase();
    initActionsConnections();


}

MainWindow::~MainWindow()
{
    sqlite->closeDB();
    delete sqlite;
    delete model;
    delete modelFood;
    delete filter;
    delete filterFood;
    delete modelList;
    delete filterdynList;
    delete dynList;
    delete listwindow;

    delete ui;
}

/************************************************************************/
/* Base functions                                                       */
/************************************************************************/

void MainWindow::initActionsConnections()
{
    connect(ui->actionA_propos, SIGNAL(triggered()), this, SLOT(on_btn_about_clicked()));
    connect(ui->actionConfiguration, SIGNAL(triggered()), this, SLOT(on_btn_config_clicked()));
    connect(ui->actionPlacard, SIGNAL(triggered()), this, SLOT(on_btn_materiel_clicked()));
    connect(ui->actionListes, SIGNAL(triggered()), this, SLOT(on_btn_liste_clicked()));
    connect(ui->actionPlans_alimentation, SIGNAL(triggered()), this, SLOT(on_btn_alimentation_clicked()));
    connect(ui->actionQuitter, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::loadConfig()
{
    if(QFile("config.ini").exists())
    {
        QSettings settings("config.ini",QSettings::IniFormat);

        settings.sync();
        settings.beginGroup("BODY");
        int gender = settings.value("gender",0).toInt();
        int age = settings.value("age",0).toInt();
        int weight = settings.value("weight",75).toInt();
        int height = settings.value("height",180).toInt();
        int physical_activity = settings.value("physical_activity",1).toInt();
        settings.endGroup();
        settings.beginGroup("BEE");
        quint16 energy= settings.value("energy",2000).toUInt();
        quint16 fat= settings.value("fat",70).toUInt();
        quint16 carbohydrates= settings.value("carbohydrates",270).toUInt();
        quint16 fibres= settings.value("fibres",25).toUInt();
        quint16 protein= settings.value("protein",50).toUInt();
        quint16 salt= settings.value("salt",6).toUInt();
        settings.endGroup();

        ui->cb_config_gender->setCurrentIndex(gender);
        ui->sb_config_age->setValue(age);
        ui->sb_config_weight->setValue(weight);
        ui->sb_config_height->setValue(height);
        ui->cb_config_pa->setCurrentIndex(physical_activity);

        ui->le_config_energy->setText(QString::number(energy));
        ui->le_config_fat->setText(QString::number(fat));
        ui->le_config_carbohydrates->setText(QString::number(carbohydrates));
        ui->le_config_fibres->setText(QString::number(fibres));
        ui->le_config_protein->setText(QString::number(protein));
        ui->le_config_salt->setText(QString::number(salt));

    }
    else{
        QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),tr("Impossible de charger \"config.ini\"."));
    }
}

void MainWindow::loadDatabase(){

    sqlite->openDB("database.db");
    refreshDatabase();

}

void MainWindow::refreshDatabase(){

    sqlite->getCategoryBrand(sqlite_CATEGORY, &idCategories, ui->cb_config_categories);
    sqlite->getCategoryBrand(sqlite_BRAND, &idBrand, ui->cb_config_brand);
    sqlite->getCategoryBrand(sqlite_CATEGORY, &idCategories, ui->cb_material_category);
    sqlite->getCategoryBrand(sqlite_BRAND, &idBrand, ui->cb_material_brand);

    ui->cb_config_categories->insertItem(0,"Nouvelle");
    ui->cb_config_brand->insertItem(0,"Nouvelle");
    ui->cb_material_category->insertItem(0,"Tous");
    ui->cb_material_category->setCurrentIndex(0);
    ui->cb_material_brand->insertItem(0,"Tous");
    ui->cb_material_brand->setCurrentIndex(0);

    QString maxWeight = "SELECT Items.weight FROM Categories, Brands INNER JOIN Items ON Items.id_category = Categories.id_category AND Items.id_brand = Brands.id_brand ORDER BY Items.weight DESC LIMIT 1";
    QSqlQuery query;
    query.prepare(maxWeight);
    query.exec();
    query.next();
    ui->s_material_weight->setMaximum(query.value(0).toInt());

    QString req="SELECT Items.id_item, Categories.name, Brands.name, Items.reference, Items.weight, Items.quantity, Items.desired FROM Categories, Brands INNER JOIN Items ON Items.id_category = Categories.id_category AND Items.id_brand = Brands.id_brand ORDER BY Categories.name ASC, Brands.name ASC, Items.reference ASC";

    model->setQuery(req);
    model->setHeaderData(1, Qt::Horizontal, tr("Catégorie"));
    model->setHeaderData(2, Qt::Horizontal, tr("Marque"));
    model->setHeaderData(3, Qt::Horizontal, tr("Modèle"));
    model->setHeaderData(4, Qt::Horizontal, tr("Poids[gr]"));
    model->setHeaderData(5, Qt::Horizontal, tr("Quantité"));
    model->setHeaderData(6, Qt::Horizontal, tr("Liste de souhait"));

    filter->setSourceModel(model);

    ui->tw_itemsList->setSortingEnabled(true);
    ui->tw_itemsList->setModel(filter);
    ui->tw_itemsList->hideColumn(0);
    ui->tw_itemsList->resizeColumnsToContents();

    QString reqFood="SELECT Food.id_food, Brands.name, Food.reference, Food.quantity, Food.expirationDate, Food.energy FROM Food INNER JOIN Categories ON Food.id_category = Categories.id_category INNER JOIN Brands ON Food.id_brand = Brands.id_brand";
    modelFood->setQuery(reqFood);
    modelFood->setHeaderData(1, Qt::Horizontal, tr("Marque"));
    modelFood->setHeaderData(2, Qt::Horizontal, tr("Modèle"));
    modelFood->setHeaderData(3, Qt::Horizontal, tr("Quantité"));
    modelFood->setHeaderData(4, Qt::Horizontal, tr("Date d'expiration"));
    modelFood->setHeaderData(5, Qt::Horizontal, tr("Energie[kcal]"));

    filterFood->setSourceModel(modelFood);

    ui->tw_alimentation->setSortingEnabled(true);
    ui->tw_alimentation->setModel(filterFood);
    ui->tw_alimentation->hideColumn(0);
    ui->tw_alimentation->resizeColumnsToContents();

    QString reqList="SELECT Lists.id_list, Lists.name, Lists.weightBackpack, Lists.weightSelf FROM Lists";
    modelList->setQuery(reqList);
    modelList->setHeaderData(1, Qt::Horizontal, tr("Nom"));

    ui->tv_list->setSortingEnabled(true);
    ui->tv_list->setModel(modelList);
    ui->tv_list->hideColumn(0);
    ui->tv_list->hideColumn(2);
    ui->tv_list->hideColumn(3);
    ui->tv_list->setColumnWidth(1, 300);

}

/************************************************************************/
/* Actions in HomeScreen                                                */
/************************************************************************/

void MainWindow::on_btn_home_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->frame_title->hide();
}

void MainWindow::on_btn_materiel_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->frame_title->show();
    ui->lbl_title->setText("Gestion du placard");
}

void MainWindow::on_btn_liste_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->frame_title->show();
    ui->lbl_title->setText("Gestion des listes");
}

void MainWindow::on_btn_alimentation_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->frame_title->show();
    ui->lbl_title->setText("Plans alimentaires");
}

void MainWindow::on_btn_config_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    ui->frame_title->show();
    ui->lbl_title->setText("Configuration");
}

void MainWindow::on_btn_about_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
    ui->frame_title->show();
    ui->lbl_title->setText("A propos");
}

/************************************************************************/
/* Gestion du placard                                                   */
/************************************************************************/

// Materiel
void MainWindow::on_btn_materiel_add_clicked()
{
    displayItemWindow(PAGE_MATERIEL, 0);

}

void MainWindow::on_tw_itemsList_doubleClicked(const QModelIndex &index)
{
    displayItemWindow(PAGE_MATERIEL, index.sibling(index.row(),0).data().toInt());
}

void MainWindow::on_btn_materiel_delete_clicked()
{
    deleteQuestion(ui->tw_itemsList->currentIndex().sibling(ui->tw_itemsList->currentIndex().row(),2).data().toString(),
                   sqlite_ITEM,
                   ui->tw_itemsList->currentIndex().sibling(ui->tw_itemsList->currentIndex().row(),0).data().toInt());
}

void MainWindow::on_btn_materiel_modify_clicked()
{
    on_tw_itemsList_doubleClicked(ui->tw_itemsList->currentIndex());
}

// Alimentation

void MainWindow::on_btn_alimentation_add_clicked()
{
    displayItemWindow(PAGE_ALIMENTATION, 0);
}

void MainWindow::on_tw_alimentation_doubleClicked(const QModelIndex &index)
{
    displayItemWindow(PAGE_ALIMENTATION, index.sibling(index.row(),0).data().toInt());
}

void MainWindow::on_btn_alimentation_modify_clicked()
{
    on_tw_itemsList_doubleClicked(ui->tw_alimentation->currentIndex());
}

void MainWindow::on_btn_alimentation_delete_clicked()
{
    deleteQuestion(ui->tw_alimentation->currentIndex().sibling(ui->tw_alimentation->currentIndex().row(),2).data().toString(),
                   sqlite_FOOD,
                   ui->tw_alimentation->currentIndex().sibling(ui->tw_alimentation->currentIndex().row(),0).data().toInt());
}

void MainWindow::displayItemWindow(quint8 page, int index){
    ItemWindow *itemwindow = new ItemWindow(0,sqlite, index);
    itemwindow->setPage(page);
    if(page==PAGE_ALIMENTATION){
        BEE bee;
        bee.energy=ui->le_config_energy->text().toDouble();
        bee.carbohydrates=ui->le_config_carbohydrates->text().toInt();
        bee.fat=ui->le_config_fat->text().toInt();
        bee.fibres=ui->le_config_fibres->text().toInt();
        bee.protein=ui->le_config_protein->text().toInt();
        bee.salt=ui->le_config_salt->text().toInt();
        itemwindow->setBEE(bee);
    }

    if(!itemwindow->exec()){
        itemwindow->show();
    }
    delete itemwindow;
    refreshDatabase();
}

void MainWindow::deleteQuestion(QString name, int toDelete, int index){
    int reponse = QMessageBox::question(this, "Supression?", tr("Supprimer %1 ?").arg(name),QMessageBox::Yes | QMessageBox::No);

    if(reponse == QMessageBox::Yes){
        sqlite->deleteRecord(toDelete, index);
        if(toDelete==sqlite_ITEM){
            sqlite->deleteRecord(sqlite_ITEMLIST, index);
        }
        refreshDatabase();
    }
}

// Liste dynamique

void MainWindow::on_s_material_weight_valueChanged(int value)
{
    ui->l_material_weight->setText(QString::number(value)%"gr");
}

void MainWindow::on_btn_material_filter_clicked()
{
    QString req="SELECT Items.id_item, Categories.name, Brands.name, Items.reference, Items.weight, Items.quantity FROM Categories, Brands INNER JOIN Items ON Items.id_category = Categories.id_category AND Items.id_brand = Brands.id_brand WHERE ";

    if(ui->cb_material_weight->currentIndex()==0){
        req= req %"Items.weight>="%QString::number(ui->s_material_weight->sliderPosition())%" ";
    }else if(ui->cb_material_weight->currentIndex()==1){
        req= req %"Items.weight<="%QString::number(ui->s_material_weight->sliderPosition())%" ";
    }
    if(ui->cb_material_category->currentIndex()!=0){
        req= req %"AND Categories.id_category="%QString::number(idCategories.at(ui->cb_material_category->currentIndex()-1))%" ";
    }
    if(ui->cb_material_brand->currentIndex()!=0){
        req= req %"AND Brands.id_brand="%QString::number(idBrand.at(ui->cb_material_brand->currentIndex()-1))%" ";
    }
    if(ui->cb_material_wishlist->currentIndex()==1){
        req= req %"AND Items.desired=1 ";
    }
    req = req %"ORDER BY Categories.name ASC, Brands.name ASC, Items.reference ASC";

    dynList->setQuery(req);
    dynList->setHeaderData(1, Qt::Horizontal, tr("Catégorie"));
    dynList->setHeaderData(2, Qt::Horizontal, tr("Marque"));
    dynList->setHeaderData(3, Qt::Horizontal, tr("Modèle"));
    dynList->setHeaderData(4, Qt::Horizontal, tr("Poids[gr]"));
    dynList->setHeaderData(5, Qt::Horizontal, tr("Quantité"));

    filterdynList->setSourceModel(dynList);

    ui->tw_dynList->setSortingEnabled(true);
    ui->tw_dynList->setModel(filterdynList);
    ui->tw_dynList->hideColumn(0);
    ui->tw_dynList->resizeColumnsToContents();
}


/************************************************************************/
/* Listes                                                               */
/************************************************************************/

void MainWindow::on_tv_list_doubleClicked(const QModelIndex &index)
{
    // Clean the QtableWidget
    ui->tw_listDetail->setRowCount(0);

    id_list = index.sibling(index.row(),0).data().toInt();
    ui->lbl_weightBackpack->setText(index.sibling(index.row(),2).data().toString());
    ui->lbl_weightSelf->setText(index.sibling(index.row(),3).data().toString());
    ui->lbl_weightBackpack_total->setText(QString::number(ui->lbl_weightBackpack->text().toDouble()+ui->lbl_weightBackpack_food->text().toDouble()));
    ui->gb_listDetail->setTitle("Liste sélectionnée : "%index.sibling(index.row(),1).data().toString());

    fillListChart(index.sibling(index.row(),0).data().toInt());

    QString reqListDetail="SELECT Categories.name, Brands.name, Items.reference, ItemsLists.totalWeight, ItemsLists.quantity, ItemsLists.backpackOrSelf FROM Brands INNER JOIN Categories ON '' = '' INNER JOIN Items ON Items.id_category = Categories.id_category AND Items.id_brand = Brands.id_brand, Lists INNER JOIN ItemsLists ON ItemsLists.id_item = Items.id_item AND ItemsLists.id_list = Lists.id_list WHERE Lists.id_list=:id_list ORDER BY Categories.name DESC, Brands.name DESC, Items.reference DESC ";
    QSqlQuery query;
    query.prepare(reqListDetail);
    query.bindValue(":id_list", index.sibling(index.row(),0).data().toInt());
    query.exec();


    while(query.next()){
        ui->tw_listDetail->insertRow(0);
        ui->tw_listDetail->setItem(0, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tw_listDetail->setItem(0, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tw_listDetail->setItem(0, 2, new QTableWidgetItem(query.value(2).toString()));
        ui->tw_listDetail->setItem(0, 3, new QTableWidgetItem());
        ui->tw_listDetail->item(0,3)->setData(0,query.value(3).toDouble());
        ui->tw_listDetail->setItem(0, 4, new QTableWidgetItem());
        ui->tw_listDetail->item(0,4)->setData(0,query.value(4).toInt());

        ui->tw_listDetail->setItem(0, 5, new QTableWidgetItem());
        if(query.value(5)=="self"){
            //ui->tw_listDetail->item(0, 0)->setBackground(Qt::red);
            ui->tw_listDetail->item(0, 5)->setIcon(QIcon(QPixmap(":/images/self.png")));
            ui->tw_listDetail->item(0,5)->setData(Qt::EditRole, QVariant("Soi"));
        }else if(query.value(5)=="backpack"){
             ui->tw_listDetail->item(0, 5)->setIcon(QIcon(QPixmap(":/images/backpack.png")));
             ui->tw_listDetail->item(0,5)->setData(Qt::EditRole, QVariant("SAD"));
        }
    }

    ui->tw_listDetail->resizeColumnsToContents();

}

void MainWindow::fillListChart(int id_list){

    QString qry="SELECT Categories.name, Sum(ItemsLists.totalWeight), Lists.weightBackpack, ItemsLists.backpackOrSelf FROM Lists, Categories INNER JOIN ItemsLists ON ItemsLists.id_list = Lists.id_list INNER JOIN Items ON Items.id_category = Categories.id_category AND ItemsLists.id_item = Items.id_item WHERE Lists.id_list = :id_list GROUP BY Categories.name, ItemsLists.backpackOrSelf";
    QSqlQuery query;
    query.prepare(qry);
    query.bindValue(":id_list", id_list);
    query.exec();

    QHorizontalPercentBarSeries *series = new QHorizontalPercentBarSeries();
    QBarSet *previousSet = new QBarSet("");
    QString previousLabel="";
    while(query.next()){
        qreal value = query.value(1).toDouble()/query.value(2).toDouble();

        if(previousLabel!=query.value(0).toString()){
            QBarSet *set = new QBarSet("");
            series->append(set);
            set->setLabel(query.value(0).toString());//query.value(0).toString()+" ("+QString::number(int(value*100))+"%)"
            if(query.value(3).toString()=="backpack"){
                set->append(value);
            }else if(query.value(3).toString()=="self"){
                set->append(0);
                set->append(value);
            }

            previousSet=set;
            previousLabel=query.value(0).toString();
            set=NULL;
            delete set;

        }else if(previousLabel==query.value(0).toString()){
            previousSet->append(value);
        }

    }

    QStringList categories;
    categories << "SAD"<<"Soi";
    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->createDefaultAxes();
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setFont(QFont("Segoe UI", 10));
    chart->setAxisY(axis, series);

    chart->setTitle("Répartition du matériel");
    chart->setTitleFont(QFont("Segoe UI", 14));

    ui->chartview->setChart(chart);
    ui->chartview->setRenderHint(QPainter::Antialiasing);

    previousSet=NULL;
    axis=NULL;
    chart=NULL;
    delete previousSet;
    delete axis;
    delete chart;

}


void MainWindow::on_btn_list_add_clicked()
{

    listwindow->setIndex(0);
    listwindow->show();
	refreshDatabase();

}

void MainWindow::on_btn_list_modify_clicked()
{
    if(id_list!=0){

        listwindow->setIndex(ui->tv_list->currentIndex().sibling(ui->tv_list->currentIndex().row(),0).data().toInt());
        listwindow->show();
		refreshDatabase();

    }
}


void MainWindow::on_btn_list_delete_clicked()
{

    deleteQuestion(ui->tv_list->currentIndex().sibling(ui->tv_list->currentIndex().row(),1).data().toString(),
                   sqlite_LIST,
                   ui->tv_list->currentIndex().sibling(ui->tv_list->currentIndex().row(),0).data().toInt());

}

void MainWindow::on_btn_matos_stats_clicked()
{
    if(id_list!=0){
        StatisticsWindow *statisticswindow = new StatisticsWindow(0,ui->tv_list->currentIndex().sibling(ui->tv_list->currentIndex().row(),0).data().toInt());

        if(!statisticswindow->exec()){
            statisticswindow->show();
        }
        delete statisticswindow;
    }

}

/************************************************************************/
/* Configuration                                                        */
/************************************************************************/

void MainWindow::on_cb_config_categories_activated(int index)
{
    if(index==0){
        ui->btn_config_categorie_delete->hide();
        ui->btn_config_categorie_save->show();
        ui->le_config_categoryName->show();
        ui->le_config_categoryName->setText("");
    }
    else{
        ui->btn_config_categorie_delete->show();
        ui->btn_config_categorie_save->show();
        ui->le_config_categoryName->show();
        ui->le_config_categoryName->setText(ui->cb_config_categories->itemText(index));
    }
}

void MainWindow::on_cb_config_brand_activated(int index)
{
    if(index==0){
        ui->btn_config_brand_delete->hide();
        ui->btn_config_brand_save->show();
        ui->le_config_brandName->show();
        ui->le_config_brandName->setText("");
    }
    else{
        ui->btn_config_brand_delete->show();
        ui->btn_config_brand_save->show();
        ui->le_config_brandName->show();
        ui->le_config_brandName->setText(ui->cb_config_brand->itemText(index));
    }
}

void MainWindow::on_btn_config_categorie_save_clicked()
{
    QString text = "";
    if(ui->cb_config_categories->currentIndex()==0){
        sqlite->addCategoryBrand(sqlite_CATEGORY,ui->le_config_categoryName->text());
        text="Ajout d'une catégorie avec succès.";
    }
    else{
        sqlite->modifyCategory(idCategories.at(ui->cb_config_categories->currentIndex()-1),ui->le_config_categoryName->text());
        text="Modification d'une catégorie avec succès.";
    }

    ui->btn_config_categorie_delete->hide();
    ui->btn_config_categorie_save->hide();
    ui->le_config_categoryName->hide();

    refreshDatabase();

    ui->statusBar->showMessage(text, 3000);
}

void MainWindow::on_btn_config_categorie_delete_clicked()
{
    sqlite->deleteRecord(sqlite_CATEGORY, idCategories.at(ui->cb_config_categories->currentIndex()-1));

    ui->btn_config_categorie_delete->hide();
    ui->btn_config_categorie_save->hide();
    ui->le_config_categoryName->hide();

    refreshDatabase();

    ui->statusBar->showMessage("Catégorie supprimée avec succès.", 3000);
}

void MainWindow::on_btn_config_brand_save_clicked()
{
    QString text = "";
    if(ui->cb_config_brand->currentIndex()==0){
        sqlite->addCategoryBrand(sqlite_BRAND, ui->le_config_brandName->text());
        text="Ajout d'une marque avec succès.";
    }
    else{
        sqlite->modifyBrand(idBrand.at(ui->cb_config_brand->currentIndex()-1),ui->le_config_brandName->text());
        text="Modification d'une marque avec succès.";
    }

    ui->btn_config_brand_delete->hide();
    ui->btn_config_brand_save->hide();
    ui->le_config_brandName->hide();

    refreshDatabase();

    ui->statusBar->showMessage(text, 3000);
}

void MainWindow::on_btn_config_brand_delete_clicked()
{
    sqlite->deleteRecord(sqlite_BRAND, idBrand.at(ui->cb_config_brand->currentIndex()-1));

    ui->btn_config_brand_delete->hide();
    ui->btn_config_brand_save->hide();
    ui->le_config_brandName->hide();

    refreshDatabase();

    ui->statusBar->showMessage("Catégorie supprimée avec succès.", 3000);
}

void MainWindow::on_btn_estimate_clicked()
{
    int bee=0;
    int gender=ui->cb_config_gender->currentIndex();
    int age=ui->sb_config_age->value();
    int weight=ui->sb_config_weight->value();
    int height=ui->sb_config_height->value();
    double cap=0;

    switch (ui->cb_config_pa->currentIndex()) {
    case CAP_SEDENTAIRE:
        cap=1;
        break;
    case CAP_PEUACTIF:
        if(gender==GENDER_FEMALE)cap=1.12;
        else if(gender==GENDER_MALE)cap=1.11;
        break;
    case CAP_ACTIF:
        if(gender==GENDER_FEMALE)cap=1.27;
        else if(gender==GENDER_MALE)cap=1.25;
        break;
    case CAP_TRESACTIF:
        if(gender==GENDER_FEMALE)cap=1.45;
        else if(gender==GENDER_MALE)cap=1.48;
        break;
    default:
        break;
    }

    if(gender==GENDER_FEMALE){
        bee=qRound(354-(6.91*age)+cap*((9.36*weight)+(726*height/100)));
    }else if(gender==GENDER_MALE){
        bee=qRound(662-(9.53*age)+cap*((15.91*weight)+(539.6*height/100)));
    }
    ui->le_config_energy->setText(QString::number(bee));
}

void MainWindow::on_btn_save_BEE_clicked()
{
    if(QFile("config.ini").exists())
    {
        QSettings settings("config.ini",QSettings::IniFormat);

        settings.sync();
        settings.beginGroup("BODY");
        settings.setValue("gender",ui->cb_config_gender->currentIndex());
        settings.setValue("age",ui->sb_config_age->value());
        settings.setValue("weight",ui->sb_config_weight->value());
        settings.setValue("height",ui->sb_config_height->value());
        settings.setValue("physical_activity",ui->cb_config_pa->currentIndex());
        settings.endGroup();
        settings.beginGroup("BEE");
        settings.setValue("energy",ui->le_config_energy->text().toInt());
        settings.setValue("fat",ui->le_config_fat->text().toInt());
        settings.setValue("carbohydrates",ui->le_config_carbohydrates->text().toInt());
        settings.setValue("fibres",ui->le_config_fibres->text().toInt());
        settings.setValue("protein",ui->le_config_protein->text().toInt());
        settings.setValue("salt",ui->le_config_salt->text().toInt());
        settings.endGroup();

    }
    else{
        QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),tr("Impossible de sauvegarder \"config.ini\"."));
    }
}
