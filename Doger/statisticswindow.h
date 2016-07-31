#ifndef STATISTICSWINDOW_H
#define STATISTICSWINDOW_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

namespace Ui {
class Statisticswindow;
}

class Statisticswindow : public QWidget
{
    Q_OBJECT

public:
    explicit Statisticswindow(QWidget *parent = 0);
    ~Statisticswindow();

private:
    Ui::Statisticswindow *ui;
};

#endif // STATISTICSWINDOW_H
