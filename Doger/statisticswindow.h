#ifndef STATISTICSWINDOW_H
#define STATISTICSWINDOW_H

#include "qchartview.h"
#include <QDialog>
#include <sqlite.h>

namespace Ui {
class StatisticsWindow;
}

class StatisticsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsWindow(QWidget *parent = 0,int id_list=0);
    void fillChartBackpack(int id_list);
    void fillChartSelf(int id_list);
    void fillMostHeavyItem(int id_list);
    ~StatisticsWindow();
private slots:
    void fillChart(QChartView *chartView, QSqlQuery query);

private:
    Ui::StatisticsWindow *ui;
    int id_list;
};

#endif // STATISTICSWINDOW_H
