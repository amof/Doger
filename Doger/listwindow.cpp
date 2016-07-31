#include "listwindow.h"
#include "ui_listwindow.h"
#include <QDebug>

Listwindow::Listwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Listwindow)
{
    ui->setupUi(this);
}

Listwindow::~Listwindow()
{
    delete ui;
}



void Listwindow::on_dockWidget_visibilityChanged(bool visible)
{
    if(visible==false){
        ui->dockWidget->setFloating(false);
        ui->dockWidget->raise();
        ui->dockWidget->show();
    }
}
