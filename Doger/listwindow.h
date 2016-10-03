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

namespace Ui {
class ListWindow;
}

class ListWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ListWindow(QWidget *parent = 0);
    ~ListWindow();

private:
    Ui::ListWindow *ui;
    enum qItemsView{i_brand, i_reference, i_weight, i_id};
    enum qListWidget{l_brand, l_reference, l_weightBackpack, l_weightSelf, l_quantity, l_id, l_weight};
    #define length_qListWidget 7
private slots:
    void on_dockWidget_visibilityChanged(bool visible);
    void populatetw_Matos();
    QList<QStandardItem *> prepareRow(const QString &first, const QString &second, const QString &third, const QString &forth);
    bool eventFilter(QObject* obj, QEvent* event);
    void on_dockWidget_topLevelChanged(bool topLevel);
    QVector<QString> decodeByteArray(QByteArray ba);
    void insertItemInQTree(QVector<QString> vectorFromItems, qListWidget place);
    void removeItemInQTree(QVector<QString> vectorFromList);

};

#endif // LISTWINDOW_H
