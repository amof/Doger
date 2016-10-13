#include "statisticswindow.h"
#include "ui_statisticswindow.h"

StatisticsWindow::StatisticsWindow(QWidget *parent, int id_list) :
    QDialog(parent),
    ui(new Ui::StatisticsWindow)
{
    ui->setupUi(this);

    fillChartBackpack(id_list);
    fillChartSelf(id_list);
    fillMostHeavyItem(id_list);
}

StatisticsWindow::~StatisticsWindow()
{
    delete ui;
}

void StatisticsWindow::fillChartBackpack(int id_list){

    QString qry="SELECT Categories.name, Sum(ItemsLists.totalWeight), Lists.weightBackpack FROM Lists, Categories INNER JOIN ItemsLists ON ItemsLists.id_list = Lists.id_list INNER JOIN Items ON Items.id_category = Categories.id_category AND ItemsLists.id_item = Items.id_item WHERE ItemsLists.backpackOrSelf = \"backpack\" AND Lists.id_list = :id_list GROUP BY Categories.name";
    QSqlQuery query;
    query.prepare(qry);
    query.bindValue(":id_list", id_list);
    query.exec();

    QPieSeries *series = new QPieSeries();
    while(query.next()){
        double percentage = query.value(1).toDouble()/query.value(2).toDouble();
        series->append(query.value(0).toString()+"("+QString::number(int(percentage*100))+"%)", percentage);
    }

    QPieSlice *slice = new QPieSlice();

    for(int i=0;i<series->count();i++){
        slice=series->slices().at(i);
        slice->setLabelVisible();
    }


    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTitle("Répartition du sac à dos");
    chart->legend()->hide();
    chart->setTitleFont(QFont("Segoe UI", 14));


    ui->cv_backpack->setChart(chart);
    ui->cv_backpack->setRenderHint(QPainter::Antialiasing);
}

void StatisticsWindow::fillChartSelf(int id_list){

    QString qry="SELECT Categories.name, Sum(ItemsLists.totalWeight), Lists.weightSelf FROM Lists, Categories INNER JOIN ItemsLists ON ItemsLists.id_list = Lists.id_list INNER JOIN Items ON Items.id_category = Categories.id_category AND ItemsLists.id_item = Items.id_item WHERE ItemsLists.backpackOrSelf = \"self\" AND Lists.id_list = :id_list GROUP BY Categories.name";
    QSqlQuery query;
    query.prepare(qry);
    query.bindValue(":id_list", id_list);
    query.exec();

    QPieSeries *series = new QPieSeries();
    while(query.next()){
        double percentage = query.value(1).toDouble()/query.value(2).toDouble();
        series->append(query.value(0).toString()+"("+QString::number(int(percentage*100))+"%)", percentage);
    }

    QPieSlice *slice = new QPieSlice();

    for(int i=0;i<series->count();i++){
        slice=series->slices().at(i);
        slice->setLabelVisible();
    }


    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTitle("Répartition sur soi");
    chart->legend()->hide();
    chart->setTitleFont(QFont("Segoe UI", 14));


    ui->cv_self->setChart(chart);
    ui->cv_self->setRenderHint(QPainter::Antialiasing);
}

void StatisticsWindow::fillMostHeavyItem(int id_list){
    QString qry="SELECT ItemsLists.totalWeight, Items.reference, Lists.weightBackpack FROM Lists INNER JOIN ItemsLists ON ItemsLists.id_list = Lists.id_list INNER JOIN Items ON ItemsLists.id_item = Items.id_item WHERE Lists.id_list = :id_list and ItemsLists.backpackOrSelf = \"backpack\" ORDER BY ItemsLists.totalWeight DESC LIMIT 10";
    QSqlQuery query;
    query.prepare(qry);
    query.bindValue(":id_list", id_list);
    query.exec();

    QHorizontalBarSeries *series = new QHorizontalBarSeries();
    QStringList categories;
    QBarSet *set = new QBarSet("");
    set->setColor(QColor(Qt::green));

    int i=0;
    while(query.next()){
        double percentage = query.value(0).toDouble()/query.value(2).toDouble();
        QString label =query.value(1).toString()+"("+QString::number(int(percentage*100))+"%)";
        categories.insert(0,label);
        set->insert(0,query.value(0).toDouble());
        i++;

    }
    series->append(set);

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->createDefaultAxes();
    chart->legend()->hide();
    chart->setAxisY(axis, series);

    chart->setTitle("Items les plus lourd dans le sac à dos");
    chart->setTitleFont(QFont("Segoe UI", 14));

    ui->cv_itemWeight->setChart(chart);
    ui->cv_itemWeight->setRenderHint(QPainter::Antialiasing);
}
