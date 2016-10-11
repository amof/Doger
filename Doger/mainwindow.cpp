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
    modelListDetail = new QSqlQueryModel();

    ui->stackedWidget->setCurrentIndex(0);
    ui->frame_title->hide();
    ui->btn_config_categorie_delete->hide();
    ui->btn_config_categorie_save->hide();
    ui->le_config_categoryName->hide();
    ui->btn_config_brand_delete->hide();
    ui->btn_config_brand_save->hide();
    ui->le_config_brandName->hide();

    ui->tw_material->setCurrentIndex(0);

    ui->lbl_backpack->setPixmap(QPixmap(":/images/backpack.png").scaled(75,75,Qt::KeepAspectRatio));
    ui->lbl_self->setPixmap(QPixmap(":/images/self.png").scaled(75,75,Qt::KeepAspectRatio));

    loadConfig();
    loadDatabase();
    initActionsConnections();


}

MainWindow::~MainWindow()
{
    sqlite->closeDB();
    delete sqlite;
    delete model, modelFood, filter, filterFood;
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
        settings.beginGroup("GENERAL");
        QString units = settings.value("units",8).toString();
        settings.endGroup();
        settings.beginGroup("NUTRITION");
        quint16 energy= settings.value("energy",8).toUInt();
        quint16 fat= settings.value("fat",8).toUInt();
        quint16 carbohydrates= settings.value("carbohydrates",8).toUInt();
        quint16 fibres= settings.value("fibres",8).toUInt();
        quint16 protein= settings.value("protein",8).toUInt();
        quint16 salt= settings.value("salt",8).toUInt();
        settings.endGroup();

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
    ui->cb_config_categories->insertItem(0,"Nouvelle");
    ui->cb_config_brand->insertItem(0,"Nouvelle");

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
    modelFood->setHeaderData(4, Qt::Horizontal, tr("Date d'expriation"));
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

    if(!itemwindow->exec()){
        itemwindow->show();
    }
    delete itemwindow;
    refreshDatabase();
}

void MainWindow::deleteQuestion(QString name, int toDelete, int index){
    int reponse = QMessageBox::question(this, "Supression?", tr("Supprimer %1 ?").arg(name),QMessageBox::Yes | QMessageBox::No);

    if(reponse == QMessageBox::Yes){
        sqlite->deleteCBIF(toDelete, index);
        refreshDatabase();
    }
}

/************************************************************************/
/* Listes                                                               */
/************************************************************************/

void MainWindow::on_btn_list_add_clicked()
{
    ListWindow *listwindow = new ListWindow(0, sqlite);

    if(!listwindow->exec()){
        listwindow->show();
    }
    delete listwindow;

}

void MainWindow::on_btn_matos_stats_clicked()
{
    /*Statisticswindow *statisticswindow;

    if(!statisticswindow->exec()){
        statisticswindow->show();
    }
    delete statisticswindow;*/
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
    sqlite->deleteCBIF(sqlite_CATEGORY, idCategories.at(ui->cb_config_categories->currentIndex()-1));

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
    sqlite->deleteCBIF(sqlite_BRAND, idBrand.at(ui->cb_config_brand->currentIndex()-1));

    ui->btn_config_brand_delete->hide();
    ui->btn_config_brand_save->hide();
    ui->le_config_brandName->hide();

    refreshDatabase();

    ui->statusBar->showMessage("Catégorie supprimée avec succès.", 3000);
}



void MainWindow::on_tv_list_doubleClicked(const QModelIndex &index)
{
    QString reqListDetail="SELECT Categories.name, Brands.name, Items.reference, Items.weight, ItemsLists.backpackOrSelf FROM Brands, Categories INNER JOIN Items ON Items.id_category = Categories.id_category AND Items.id_brand = Brands.id_brand, Lists INNER JOIN ItemsLists ON ItemsLists.id_item = Items.id_item AND ItemsLists.id_list = Lists.id_list WHERE Lists.id_list=";
    reqListDetail+=index.sibling(index.row(),0).data().toString();
    modelListDetail->setQuery(reqListDetail);
    modelListDetail->setHeaderData(1, Qt::Horizontal, tr("Marque"));

    ui->tv_listDetail->setSortingEnabled(true);
    ui->tv_listDetail->setModel(modelListDetail);

    ui->lbl_weightBackpack->setText(index.sibling(index.row(),2).data().toString());
    ui->lbl_weightSelf->setText(index.sibling(index.row(),3).data().toString());
    ui->gb_listDetail->setTitle("Liste sélectionnée : "+index.sibling(index.row(),1).data().toString());

    fillListChart(index.sibling(index.row(),0).data().toInt());

    for(int i=0;i<ui->tv_listDetail->model()->rowCount();i++){
        if(ui->tv_listDetail->model()->index(i,4).data().toString()=="self"){
            qDebug()<<"yes";
        }
    }

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
}
