#ifndef ITEMWINDOW_H
#define ITEMWINDOW_H

#include <QDialog>
#include <sqlite.h>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <QString>
#include <QLabel>
#include <QSlider>
#include <QVector>

struct BEE{
    double energy;
    double fat;
    double carbohydrates;
    double fibres;
    double protein;
    double salt;
};

namespace Ui {
class ItemWindow;
}

class ItemWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ItemWindow(QWidget *parent = 0, SqLite *sqlitepointer = NULL, int index=0);
    ~ItemWindow();

    bool loadFile(const QString &, QLabel *label);
    void setPage(quint8 page);
    void setBEE(BEE beeReceived);

    #define PAGE_MATERIEL 0
    #define PAGE_ALIMENTATION 1

private slots:
    void setImage(const QImage &newImage, QLabel *label);

    void loadDatabase(int index);
    void displayPercentage(QLineEdit *le, double refValue, QSlider *slider, QLabel *labelPercentage);
    double replaceDotWithComma(QString value);

    void on_btn_item_loadImage_clicked();

    void on_btn_item_deleteImage_clicked();

    void on_btn_alim_loadImage_clicked();

    void on_btn_alim_deleteImage_clicked();

    bool on_btn_item_save_clicked();

    void on_cb_item_brand_activated(int index);

    void on_btn_item_newBrand_clicked();

    void on_btn_alim_save_clicked();

    void on_btn_alim_newBrand_clicked();

    void on_cb_alim_brand_activated(int index);

    void on_btn_item_saveClose_clicked();

    void on_le_alim_energy_editingFinished();

    void on_le_alim_fat_editingFinished();

    void on_le_alim_carbohydrates_editingFinished();

    void on_le_alim_fibres_editingFinished();

    void on_le_alim_protein_editingFinished();

    void on_le_alim_salt_editingFinished();

private:
    Ui::ItemWindow *ui;
    QImage image;
    QVector<int> idCategories;
    QVector<int> idBrand;
    SqLite *sqlite;
    quint8 currentPage=0;
    int id_item=0;
    #define TO_INSERT 0
    #define TO_UPDATE 1
    bool whatToDo=TO_INSERT;

};

#endif // ITEMWINDOW_H
