#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sqlite.h"
#include <QMessageBox>
#include "itemwindow.h"
#include "listwindow.h"
#include "statisticswindow.h"

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

public:


private:
    Ui::MainWindow *ui;
    Listwindow *listwindow;
    Statisticswindow *statisticswindow;
    QVector<int> idCategories;
    QVector<int> idBrand;
    SqLite *sqlite;
    QMenu* contextMenu;
};

#endif // MAINWINDOW_H
