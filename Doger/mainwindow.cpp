#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    itemwindow = new Itemwindow;
    listwindow = new Listwindow;
    statisticswindow = new Statisticswindow;

    ui->stackedWidget->setCurrentIndex(0);
    ui->frame_title->hide();
    ui->btn_config_categorie_delete->hide();
    ui->btn_config_categorie_save->hide();
    ui->le_config_categorie->hide();
    ui->btn_config_brand_delete->hide();
    ui->btn_config_brand_save->hide();
    ui->le_config_brand->hide();

    loadConfig();
    initActionsConnections();

}

MainWindow::~MainWindow()
{
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

void MainWindow::initActionsConnections()
{
    //connect(ui->btn_materiel_add, SIGNAL(clicked()), placard, SLOT(show()));

}
void MainWindow::on_btn_materiel_add_clicked()
{
    itemwindow->show();
    itemwindow->setPage(PAGE_MATERIEL);
}

void MainWindow::on_btn_alimentation_add_clicked()
{
    itemwindow->show();
    itemwindow->setPage(PAGE_ALIMENTATION);
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
        ui->le_config_categorie->show();
        ui->le_config_categorie->setText("");
    }
    else{
        ui->btn_config_categorie_delete->show();
        ui->btn_config_categorie_save->show();
        ui->le_config_categorie->show();
        ui->le_config_categorie->setText(ui->cb_config_categories->itemText(index));
    }
}

void MainWindow::on_cb_config_brand_activated(int index)
{
    if(index==0){
        ui->btn_config_brand_delete->hide();
        ui->btn_config_brand_save->show();
        ui->le_config_brand->show();
        ui->le_config_brand->setText("");
    }
    else{
        ui->btn_config_brand_delete->show();
        ui->btn_config_brand_save->show();
        ui->le_config_brand->show();
        ui->le_config_brand->setText(ui->cb_config_categories->itemText(index));
    }

}

void MainWindow::on_actionQuitter_triggered()
{
    close();
}

void MainWindow::on_actionConfiguration_triggered()
{
    on_btn_config_clicked();
}

void MainWindow::on_actionA_propos_triggered()
{
    on_btn_about_clicked();
}

void MainWindow::on_actionPlacard_triggered()
{
    on_btn_materiel_clicked();
}

void MainWindow::on_actionListes_triggered()
{
    on_btn_liste_clicked();
}

void MainWindow::on_actionPlans_alimentation_triggered()
{
    on_btn_alimentation_clicked();
}
