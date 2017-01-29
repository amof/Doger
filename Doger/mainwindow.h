#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sqlite.h"
#include <QMessageBox>
#include "itemwindow.h"
#include "listwindow.h"
#include "statisticswindow.h"
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QHorizontalPercentBarSeries>
#include <QStyledItemDelegate>
#include <QInputDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void loadConfig();
    void loadDatabase();
    void refreshDatabase();
    void initActionsConnections();
    void displayItemWindow(quint8 page, int index);
    void deleteQuestion(QString name, int toDelete, int index);
    void fillListChart(int id_list);
    void cleanListDisplayed();
    void on_btn_materiel_clicked();

    void on_btn_liste_clicked();

    void on_btn_home_clicked();

    void on_btn_alimentation_clicked();

    void on_btn_config_clicked();

    void on_btn_about_clicked();

    void on_btn_alimentation_add_clicked();

    void on_btn_materiel_add_clicked();

    void on_btn_list_add_clicked();

    void on_btn_matos_stats_clicked();

    void on_cb_config_categories_activated(int index);

    void on_cb_config_brand_activated(int index);

    void on_btn_config_categorie_save_clicked();

    void on_btn_config_categorie_delete_clicked();

    void on_btn_config_brand_save_clicked();

    void on_btn_config_brand_delete_clicked();
    void on_tw_itemsList_doubleClicked(const QModelIndex &index);

    void on_btn_materiel_delete_clicked();

    void on_btn_materiel_modify_clicked();

    void on_tw_alimentation_doubleClicked(const QModelIndex &index);

    void on_btn_alimentation_modify_clicked();

    void on_btn_alimentation_delete_clicked();

    void on_tv_list_doubleClicked(const QModelIndex &index);

    void on_btn_list_delete_clicked();

    void on_btn_list_modify_clicked();

    void on_btn_save_BEE_clicked();

    void on_btn_estimate_clicked();

    void on_s_material_weight_valueChanged(int value);

    void on_btn_material_filter_clicked();

    void on_btn_list_duplicate_clicked();

    void on_tv_list_clicked(const QModelIndex &index);

public:


private:
    Ui::MainWindow *ui;
    QVector<int> idCategories;
    QVector<int> idBrand;
    SqLite *sqlite;
    QSqlQueryModel *model ;
    QSortFilterProxyModel *filter;
    QSqlQueryModel *modelFood;
    QSortFilterProxyModel *filterFood;
    QSqlQueryModel *modelList;
    QSortFilterProxyModel *filterdynList;
    QSqlQueryModel *dynList;
    int id_list;
    #define GENDER_FEMALE   0
    #define GENDER_MALE     1
    #define CAP_SEDENTAIRE  0
    #define CAP_PEUACTIF    1
    #define CAP_ACTIF       2
    #define CAP_TRESACTIF   3
};


#endif // MAINWINDOW_H
