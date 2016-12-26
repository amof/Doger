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

    QString qry="SELECT Categories.name, Sum(ItemsLists.totalWeight), Lists.weightBackpack FROM Lists, Categories INNER JOIN ItemsLists ON ItemsLists.id_list = Lists.id_list INNER JOIN Items ON Items.id_category = Categories.id_category AND ItemsLists.id_item = Items.id_item WHERE ItemsLists.backpackOrSelf = \"backpack\" AND Lists.id_list = :id_list GROUP BY Categories.name ORDER BY Sum(ItemsLists.totalWeight) ASC";
    QSqlQuery query;
    query.prepare(qry);
    query.bindValue(":id_list", id_list);
    query.exec();

    fillChart(ui->cv_backpack, query);
    ui->cv_backpack->chart()->setTitle("Répartition du sac à dos");

}

void StatisticsWindow::fillChartSelf(int id_list){

    QString qry="SELECT Categories.name, Sum(ItemsLists.totalWeight), Lists.weightSelf FROM Lists, Categories INNER JOIN ItemsLists ON ItemsLists.id_list = Lists.id_list INNER JOIN Items ON Items.id_category = Categories.id_category AND ItemsLists.id_item = Items.id_item WHERE ItemsLists.backpackOrSelf = \"self\" AND Lists.id_list = :id_list GROUP BY Categories.name ORDER BY Sum(ItemsLists.totalWeight) ASC";
    QSqlQuery query;
    query.prepare(qry);
    query.bindValue(":id_list", id_list);
    query.exec();

    fillChart(ui->cv_self, query);
    ui->cv_self->chart()->setTitle("Répartition sur soi");

}

void StatisticsWindow::fillChart(QtCharts::QChartView *chartView, QSqlQuery query){
    QPieSeries *series = new QPieSeries();
    QVector<int> vector;
    QVector<QString> vectorName;

    while(query.next()){
        vector.append(qRound((query.value(1).toDouble()/query.value(2).toDouble())*100));
        vectorName.append(query.value(0).toString());
    }

    for(int i=0;i<vector.size();i++){
        if(i%2==0){
            //qDebug()<<"ici"<<vectorName[i/2]<<i/2;
            series->append(vectorName[i/2]+"("+QString::number(vector[i/2])+"%)", vector[i/2]);
        }else{
            //qDebug()<<"là"<<vectorName[vector.size()-(i/2)-1]<<i<<QString::number(4-(i/2)-1);
            series->append(vectorName[vector.size()-(i/2)-1]+"("+QString::number(vector[vector.size()-(i/2)-1])+"%)", vector[vector.size()-(i/2)-1]);
        }
    }

    QPieSlice *slice = new QPieSlice();

    for(int i=0;i<series->count();i++){
        slice=series->slices().at(i);
        slice->setLabelVisible();
    }


    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->hide();
    chart->setTitleFont(QFont("Segoe UI", 14));


    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}

void StatisticsWindow::fillMostHeavyItem(int id_list){
    QString qry="SELECT ItemsLists.totalWeight, Items.reference, Lists.weightBackpack FROM Lists INNER JOIN ItemsLists ON ItemsLists.id_list = Lists.id_list INNER JOIN Items ON ItemsLists.id_item = Items.id_item WHERE Lists.id_list = :id_list and ItemsLists.backpackOrSelf = \"backpack\" ORDER BY ItemsLists.totalWeight DESC LIMIT 5";
    QSqlQuery query;
    query.prepare(qry);
    query.bindValue(":id_list", id_list);
    query.exec();

    QHorizontalBarSeries *series = new QHorizontalBarSeries();
    QStringList categories;
    QBarSet *set = new QBarSet("");
    set->setColor(QColor(Qt::green));

    double backpackWeight=0;
    while(query.next()){
        double percentage = query.value(0).toDouble()/query.value(2).toDouble();
        backpackWeight=query.value(2).toDouble();
        QString label =query.value(1).toString()+" ("+QString::number(query.value(0).toDouble()) +"gr-"+QString::number(qRound(percentage*100))+"%)";
        categories.insert(0,label);
        set->insert(0,query.value(0).toDouble());
    }

    series->append(set);

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->createDefaultAxes();
    chart->legend()->hide();
    chart->axisX()->setMax(QVariant(backpackWeight));
    chart->setAxisY(axis, series);

    chart->setTitle("Items les plus lourd dans le sac à dos");
    chart->setTitleFont(QFont("Segoe UI", 14));

    ui->cv_itemWeight->setChart(chart);
    ui->cv_itemWeight->setRenderHint(QPainter::Antialiasing);
}
