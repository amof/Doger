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

private slots:
    void on_dockWidget_visibilityChanged(bool visible);
    void populatetw_Matos();
    QList<QStandardItem *> prepareRow(const QString &first, const QString &second, const QString &third, const QString &forth);
    bool eventFilter(QObject* obj, QEvent* event);
    void on_dockWidget_topLevelChanged(bool topLevel);
    QVector<QString> decodeByteArray(QByteArray ba);
    void insertItemInQTree(QVector<QString> vector);

private:
    Ui::ListWindow *ui;
    enum qtableview{brand, reference, weight, id};
};

#endif // LISTWINDOW_H
