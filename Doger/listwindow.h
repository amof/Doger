#ifndef LISTWINDOW_H
#define LISTWINDOW_H

#include <QDialog>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSqlQuery>
#include <QDropEvent>
#include <QMimeData>
#include <QVector>
#include <QDataStream>
#include <QTreeWidgetItem>
#include <sqlite.h>

namespace Ui {
class ListWindow;
}

class ListWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ListWindow(QWidget *parent = 0, SqLite *sqlitepointer = NULL, int index=0);
    ~ListWindow();

private:
    Ui::ListWindow *ui;
    enum qItemsView{i_brand, i_reference, i_weight, i_id};
    enum qListWidget{l_brand, l_reference, l_weightBackpack, l_weightSelf, l_quantity, l_id, l_weight};
    #define length_qListWidget 7
    int numberOfCategoriesInList;
    SqLite *sqlite;
    int id_list=0;
    #define TO_INSERT 0
    #define TO_UPDATE 1
    bool whatToDo=TO_INSERT;

private slots:
    void on_dockWidget_visibilityChanged(bool visible);
    void populatetw_Matos();
    QList<QStandardItem *> prepareRow(const QString &first, const QString &second, const QString &third, const QString &forth);
    bool eventFilter(QObject* obj, QEvent* event);
    void on_dockWidget_topLevelChanged(bool topLevel);
    QVector<QString> decodeByteArray(QByteArray ba);
    void insertItemInQTree(int id_item, qListWidget place, int defaultQuantity);
    void removeItemInQTree(QVector<QString> vectorFromList);
    void getExistingList();
    void on_btn_saveList_clicked();
};

#endif // LISTWINDOW_H
