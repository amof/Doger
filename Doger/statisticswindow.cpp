#include "statisticswindow.h"
#include "ui_statisticswindow.h"

Statisticswindow::Statisticswindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Statisticswindow)
{
    ui->setupUi(this);

    QPieSeries *series = new QPieSeries();
    series->append("Couchage", 50);
    series->append("Vêtements", 20);
    series->append("Cuisine", 10);
    series->append("Divers", 10);
    series->append("Pharmacie", 10);

    QPieSlice *slice = series->slices().at(1);
    slice->setLabelVisible();
    slice->setBrush(Qt::green);

    slice = series->slices().at(0);
    slice->setLabelVisible();

    slice = series->slices().at(2);
    slice->setLabelVisible();

    slice = series->slices().at(3);
    slice->setLabelVisible();

    slice = series->slices().at(4);
    slice->setLabelVisible();

    QChart *chart = new QChart();
    chart->addSeries(series);
    //chart->setTitle("Répartition du matériel");
    chart->legend()->hide();

    ui->chartview->setChart(chart);
    ui->chartview->setRenderHint(QPainter::Antialiasing);
}

Statisticswindow::~Statisticswindow()
{
    delete ui;
}

