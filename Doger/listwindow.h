#ifndef LISTWINDOW_H
#define LISTWINDOW_H

#include <QDialog>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSqlQuery>

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

    void on_dockWidget_topLevelChanged(bool topLevel);

private:
    Ui::ListWindow *ui;
};

#endif // LISTWINDOW_H
