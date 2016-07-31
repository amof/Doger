#ifndef LISTWINDOW_H
#define LISTWINDOW_H

#include <QWidget>

namespace Ui {
class Listwindow;
}

class Listwindow : public QWidget
{
    Q_OBJECT

public:
    explicit Listwindow(QWidget *parent = 0);
    ~Listwindow();

private slots:
    void on_dockWidget_visibilityChanged(bool visible);

private:
    Ui::Listwindow *ui;
};

#endif // LISTWINDOW_H
