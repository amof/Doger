#ifndef ITEMWINDOW_H
#define ITEMWINDOW_H

#include <QWidget>
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

namespace Ui {
class Itemwindow;
}

class Itemwindow : public QWidget
{
    Q_OBJECT

public:
    explicit Itemwindow(QWidget *parent = 0);
    ~Itemwindow();
    bool loadFile(const QString &, QLabel *label);
    void setPage(quint8 page);

    #define PAGE_MATERIEL 0
    #define PAGE_ALIMENTATION 1

private slots:
    void setImage(const QImage &newImage, QLabel *label);

    void on_btn_alim_loadImage_clicked();

    void on_btn_alim_deleteImage_clicked();

    void on_btn_item_loadImage_clicked();

    void on_btn_item_deleteImage_clicked();

private:
    Ui::Itemwindow *ui;
    QImage image;
};

#endif // ITEMWINDOW_H
