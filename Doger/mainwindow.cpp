#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    listwindow = new Listwindow;
    statisticswindow = new Statisticswindow;
    sqlite = new SqLite;

    ui->stackedWidget->setCurrentIndex(0);
    ui->frame_title->hide();
    ui->btn_config_categorie_delete->hide();
    ui->btn_config_categorie_save->hide();
    ui->le_config_categoryName->hide();
    ui->btn_config_brand_delete->hide();
    ui->btn_config_brand_save->hide();
    ui->le_config_brandName->hide();

    loadConfig();
    loadDatabase();
    initActionsConnections();


}

MainWindow::~MainWindow()
{
    sqlite->closeDB();
    delete sqlite;
    delete ui;
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

    sqlite->getCategoryBrand(CATEGORY, &idCategories, ui->cb_config_categories);
    sqlite->getCategoryBrand(BRAND, &idBrand, ui->cb_config_brand);
    ui->cb_config_categories->insertItem(0,"Nouvelle");
    ui->cb_config_brand->insertItem(0,"Nouvelle");

    QString req="SELECT Items.id_item, Categories.name, Brands.name, Items.reference, Items.weight, Items.quantity, Items.desired FROM Brands, Categories INNER JOIN Items ON Items.id_category = Categories.id_category AND Items.id_brand = Brands.id_brand WHERE Brands.id_brand = Items.id_brand AND Categories.id_category = Categories.id_category";
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(req);
    model->setHeaderData(1, Qt::Horizontal, tr("Catégorie"));
    model->setHeaderData(2, Qt::Horizontal, tr("Marque"));
    model->setHeaderData(3, Qt::Horizontal, tr("Modèle"));
    model->setHeaderData(4, Qt::Horizontal, tr("Poids"));
    model->setHeaderData(5, Qt::Horizontal, tr("Quantité"));
    model->setHeaderData(6, Qt::Horizontal, tr("Liste de souhait"));

    QSortFilterProxyModel *filter = new QSortFilterProxyModel();
    filter->setSourceModel(model);

    ui->tw_itemsList->setModel(filter);
    ui->tw_itemsList->setSortingEnabled(true);
    //ui->tw_itemsList->hideColumn(0);
}

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

void MainWindow::on_btn_materiel_add_clicked()
{
    ItemWindow *itemwindow = new ItemWindow(0,sqlite,0);
    if(!itemwindow->exec())
    {
        itemwindow->setPage(PAGE_MATERIEL);
        itemwindow->show();
    }
    delete itemwindow;
    refreshDatabase();

}

void MainWindow::on_btn_alimentation_add_clicked()
{
    /*itemwindow->show();
    itemwindow->setPage(PAGE_ALIMENTATION);*/
}



void MainWindow::on_btn_list_add_clicked()
{
    listwindow->show();
}

void MainWindow::on_btn_matos_stats_clicked()
{
    statisticswindow->show();
}


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

void MainWindow::initActionsConnections()
{
    connect(ui->actionA_propos, SIGNAL(triggered()), this, SLOT(on_btn_about_clicked()));
    connect(ui->actionConfiguration, SIGNAL(triggered()), this, SLOT(on_btn_config_clicked()));
    connect(ui->actionPlacard, SIGNAL(triggered()), this, SLOT(on_btn_materiel_clicked()));
    connect(ui->actionListes, SIGNAL(triggered()), this, SLOT(on_btn_liste_clicked()));
    connect(ui->actionPlans_alimentation, SIGNAL(triggered()), this, SLOT(on_btn_alimentation_clicked()));
    connect(ui->actionQuitter, SIGNAL(triggered()), this, SLOT(close()));
}


void MainWindow::on_btn_config_categorie_save_clicked()
{
    QString text = "";
    if(ui->cb_config_categories->currentIndex()==0){
        sqlite->addCategoryBrand(CATEGORY,ui->le_config_categoryName->text());
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
    sqlite->deleteCategoryBrand(CATEGORY, idCategories.at(ui->cb_config_categories->currentIndex()-1));

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
        sqlite->addCategoryBrand(BRAND, ui->le_config_brandName->text());
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
    sqlite->deleteCategoryBrand(BRAND, idBrand.at(ui->cb_config_brand->currentIndex()-1));

    ui->btn_config_brand_delete->hide();
    ui->btn_config_brand_save->hide();
    ui->le_config_brandName->hide();

    refreshDatabase();

    ui->statusBar->showMessage("Catégorie supprimée avec succès.", 3000);
}

void MainWindow::on_tw_itemsList_doubleClicked(const QModelIndex &index)
{
    ItemWindow *itemwindow = new ItemWindow(0,sqlite, index.sibling(index.row(),0).data().toInt() );
    if(!itemwindow->exec())
    {
        itemwindow->setPage(PAGE_MATERIEL);
        itemwindow->show();
    }
    delete itemwindow;

    refreshDatabase();
}

void MainWindow::on_btn_materiel_delete_clicked()
{
    int reponse = QMessageBox::question(this, "Supression?", tr("Supprimer %1 ?")
                                        .arg(ui->tw_itemsList->currentIndex().sibling(ui->tw_itemsList->currentIndex().row(),3).data().toString())
                                        ,QMessageBox::Yes | QMessageBox::No);

            if(reponse == QMessageBox::Yes){
                QString Squery = "DELETE FROM Items WHERE id_item=:id_item";
                QSqlQuery query;
                query.prepare(Squery);
                query.bindValue(":id_item", ui->tw_itemsList->currentIndex().sibling(ui->tw_itemsList->currentIndex().row(),0).data().toString());

                if(!query.exec()) qDebug()<<"[SQLite] Erreur lors de la suppression d'un item.";
            }
     refreshDatabase();

}

void MainWindow::on_btn_materiel_modify_clicked()
{
    on_tw_itemsList_doubleClicked(ui->tw_itemsList->currentIndex());
}
