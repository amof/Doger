#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

    void on_actionQuitter_triggered();

    void on_actionConfiguration_triggered();

    void on_actionA_propos_triggered();

    void on_actionPlacard_triggered();

    void on_actionListes_triggered();

    void on_actionPlans_alimentation_triggered();

private:
    Ui::MainWindow *ui;
    Itemwindow *itemwindow;
    Listwindow *listwindow;
    Statisticswindow *statisticswindow;
};

#endif // MAINWINDOW_H
