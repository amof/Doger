#ifndef LISTWINDOW_H
#define LISTWINDOW_H

#include <QWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSqlQuery>
#include <QDropEvent>
#include <QMimeData>
#include <QVector>
#include <QDataStream>
#include <QTreeWidgetItem>
#include <sqlite.h>
#include <QElapsedTimer>

namespace Ui {
class ListWindow;
}

class ListWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ListWindow(QWidget *parent = 0, SqLite *sqlitepointer = NULL);
    setIndex(int index);
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
    QVector<int> list_id_item_toDelete;
    QVector<int> list_id_item;

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
    void on_tw_items_doubleClicked(const QModelIndex &index);

};

#endif // LISTWINDOW_H
