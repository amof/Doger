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
#include <QElapsedTimer>
#include <QMessageBox>

namespace Ui {
class ListWindow;
}

class ListWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ListWindow(QWidget *parent = 0, SqLite *sqlitepointer = NULL, int index=0);
    void duplicateList(QString listName);
    ~ListWindow();

private:
    #define length_qListWidget 7
    #define TO_INSERT 0
    #define TO_UPDATE 1

    Ui::ListWindow *ui;
    SqLite *sqlite;

    bool whatToDo=TO_INSERT;
    int id_list=0;
    int numberOfCategoriesInList=0;
    QVector<int> list_id_item_toDelete;
    QVector<int> list_id_item;
    int lastQuantity =-1;

    enum qItemsView{i_brand, i_reference, i_weight, i_id};
    enum qListWidget{l_brand, l_reference, l_weightBackpack, l_weightSelf, l_quantity, l_id, l_weight};

private slots:
    void populatetw_Matos();
    void getExistingList();

    void on_dockWidget_visibilityChanged(bool visible);
    void on_dockWidget_topLevelChanged(bool topLevel);

    bool eventFilter(QObject* obj, QEvent* event);
    void on_tw_list_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void addCategoryToList(qListWidget place);
    void moveItemTo(QTreeWidgetItem *item, qListWidget place);
    void on_tw_items_doubleClicked(const QModelIndex &index);
    void insertItemInQTree(int id_item, qListWidget place, int defaultQuantity);
    void removeItemFromQTree(QTreeWidgetItem *item);

    void on_btn_saveList_clicked();

    void updateQuantity(QTreeWidgetItem *item, int quantity, qListWidget place);
    QList<QStandardItem *> prepareRow(const QString &first, const QString &second, const QString &third, const QString &forth);
    QVector<QString> decodeByteArray(QByteArray ba);
};

#endif // LISTWINDOW_H
