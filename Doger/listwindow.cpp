#include "listwindow.h"
#include "ui_listwindow.h"
#include <QDebug>

ListWindow::ListWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ListWindow)
{
    ui->setupUi(this);
    populatetw_Matos();
    ui->qw_backpack->setAcceptDrops(true);
    ui->qw_backpack->installEventFilter(this);
    ui->qw_self->setAcceptDrops(true);
    ui->qw_self->installEventFilter(this);
    ui->qw_delete->setAcceptDrops(true);
    ui->qw_delete->installEventFilter(this);
    ui->tw_list->setColumnCount(length_qListWidget);
    ui->tw_list->installEventFilter(this);

    QStringList headerLabels;
    headerLabels.push_back(tr("Marque"));
    headerLabels.push_back(tr("Modèle"));
    headerLabels.push_back(tr("Poids Sac"));
    headerLabels.push_back(tr("Poids Soi"));
    headerLabels.push_back(tr("Quantité"));
    ui->tw_list->setHeaderLabels(headerLabels);

    ui->lbl_backpack->setPixmap(QPixmap(":/images/backpack.png").scaled(100,100,Qt::KeepAspectRatio));
    ui->lbl_self->setPixmap(QPixmap(":/images/self.png").scaled(100,100,Qt::KeepAspectRatio));
}

ListWindow::~ListWindow()
{
    delete ui;
}

void ListWindow::on_dockWidget_visibilityChanged(bool visible)
{
    if(visible==false){
        ui->dockWidget->setFloating(false);
        ui->dockWidget->raise();
        ui->dockWidget->show();
    }
}

void ListWindow::populatetw_Matos()
{
    QString qry="SELECT Categories.name, Items.id_item, Brands.name, Items.reference, Items.weight FROM Categories, Brands INNER JOIN Items ON Items.id_category = Categories.id_category AND Items.id_brand = Brands.id_brand ORDER BY Categories.name ASC, Brands.name ASC, Items.reference ASC";
    QSqlQuery query;
    query.prepare(qry);
    query.exec();

    QStandardItemModel *m = new QStandardItemModel(this);
    QString prevCategoryName="";
    QList<QStandardItem *> preparedRow;
    int children = 0;

    while(query.next()) {
        QString categoryName = query.value(0).toString();
        int id = query.value(1).toInt();
        QString brandName = query.value(2).toString();
        QString reference = query.value(3).toString();
        QString weight = query.value(4).toString();

        if (prevCategoryName != categoryName) {
           children = 0;
           preparedRow=prepareRow(categoryName, "", "","");
           m->appendRow(preparedRow);
           prevCategoryName = categoryName;
        }

        QList<QStandardItem *> secondRow =prepareRow(brandName, reference, weight, QString::number(id));
        preparedRow.first()->appendRow(secondRow);;
        children++;
    }

    m->setHeaderData(0, Qt::Horizontal, tr("Marque"));
    m->setHeaderData(1, Qt::Horizontal, tr("Modèle"));
    m->setHeaderData(2, Qt::Horizontal, tr("Poids"));
    ui->tw_items->setModel(m);
    //ui->tw_matos->hideColumn(3);

    for(int i=0;i++;i<m->columnCount()){
        ui->tw_items->resizeColumnToContents(i);
    }
}


QList<QStandardItem *> ListWindow::prepareRow(const QString &first,
                                                const QString &second,
                                                const QString &third,
                                                const QString &forth)
{
    QList<QStandardItem *> rowItems;
    rowItems << new QStandardItem(first);
    rowItems << new QStandardItem(second);
    rowItems << new QStandardItem(third);
    rowItems << new QStandardItem(forth);
    return rowItems;
}



void ListWindow::on_dockWidget_topLevelChanged(bool topLevel)
{
    if(topLevel==true){
        ui->dockWidget->setMaximumWidth(16777215);
        ui->gb_items->hide();
    }else{
        ui->dockWidget->setMaximumWidth(300);
        ui->gb_items->show();
    }
}

bool ListWindow::eventFilter(QObject* obj, QEvent* event){

    if (obj == ui->qw_backpack || obj==ui->qw_self) {
            if (event->type() == QEvent::DragEnter) {
                QDragEnterEvent* ev = (QDragEnterEvent*)event;
                if(ev->source()==ui->tw_items){
                     ev->acceptProposedAction();
                }

            }
            else if (event->type() == QEvent::Drop) {
                QDropEvent* ev = (QDropEvent*)event;
                ev->setDropAction(Qt::CopyAction);
                ev->accept();
                QByteArray ba = ev->mimeData()->data("application/x-qabstractitemmodeldatalist");

                if(obj==ui->qw_backpack){insertItemInQTree(decodeByteArray(ba), qListWidget(l_weightBackpack));}
                else if(obj==ui->qw_self){ insertItemInQTree(decodeByteArray(ba), qListWidget(l_weightSelf));}

        }
    }
    else if (obj==ui->qw_delete){
        if (event->type() == QEvent::DragEnter) {
            QDragEnterEvent* ev = (QDragEnterEvent*)event;
            if(ev->source()==ui->tw_list){
                 ev->acceptProposedAction();
            }

        }
        else if (event->type() == QEvent::Drop) {
            QDropEvent* ev = (QDropEvent*)event;
            ev->setDropAction(Qt::CopyAction);
            ev->accept();
            QByteArray ba = ev->mimeData()->data("application/x-qabstractitemmodeldatalist");
            removeItemInQTree(decodeByteArray(ba));
        }
    }
    else if (event->type()==QEvent::KeyPress){
        QKeyEvent* pKeyEvent=static_cast<QKeyEvent*>(event);
        if (pKeyEvent->key() == Qt::Key_Delete){
            if (ui->tw_list->hasFocus() && !ui->tw_list->selectedItems().isEmpty()){
                QVector<QString> vector;
                for(int i=0;i<length_qListWidget;i++){
                  vector.append(ui->tw_list->selectedItems()[0]->text(i));
                }
                removeItemInQTree(vector);
            }
        }
    }
}

QVector<QString> ListWindow::decodeByteArray(QByteArray ba){
    QVector<QString> data;

    QDataStream *ds = new QDataStream(ba);

    while(!ds->atEnd()){
        int row, col;
        QMap<int,  QVariant> roleDataMap;
        *ds >> row >> col >> roleDataMap;
        //qDebug()<<QString::number(row)<<QString::number(col)<<roleDataMap<<roleDataMap[0].toString();
        data.append(roleDataMap[0].toString());
    }

    return data;
}

void ListWindow::insertItemInQTree(QVector<QString> vectorFromItems, qListWidget place){
    QString qry="SELECT Categories.name, Brands.name, Items.reference, Items.weight, Items.quantity FROM Brands, Categories INNER JOIN Items ON Items.id_category = Categories.id_category AND Items.id_brand = Brands.id_brand WHERE Items.id_item=:id_item";
    QSqlQuery query;
    query.prepare(qry);
    query.bindValue(":id_item", vectorFromItems[qItemsView(i_id)]);
    query.exec();

    if(query.next()){
        //Child Search
        QList<QTreeWidgetItem*> childSearch = ui->tw_list->findItems(vectorFromItems[qItemsView(i_id)], Qt::MatchExactly|Qt::MatchRecursive,qListWidget(l_id));
        QTreeWidgetItem *child = new QTreeWidgetItem();

        if(childSearch.isEmpty()){
            child->setText(qListWidget(l_brand),query.value(1).toString());
            child->setText(qListWidget(l_reference),query.value(2).toString());
            child->setText(qListWidget(place),query.value(3).toString());
            child->setText(qListWidget(l_quantity),"1");
            child->setText(qListWidget(l_id),vectorFromItems[qItemsView(i_id)]);
            child->setText(qListWidget(l_weight), query.value(3).toString());

        }else if(!childSearch.isEmpty() && childSearch[0]->text(qItemsView(place))!=""){
            child = childSearch[0];

            int quantity = child->text(qListWidget(l_quantity)).toInt() + 1;
            child->setText(qListWidget(l_quantity),QString::number(quantity));

            int weight = child->text(qListWidget(place)).toDouble() + query.value(3).toDouble();
            child->setText(qListWidget(place),QString::number(weight));
        }

        // Root Search
        QList<QTreeWidgetItem*> rootSearch = ui->tw_list->findItems(query.value(0).toString(), Qt::MatchExactly);

        // Add root/child
        if(rootSearch.isEmpty()){
            QTreeWidgetItem *root = new QTreeWidgetItem();
            root->setText(0, query.value(0).toString());
            root->setText(qListWidget(place), query.value(3).toString());
            ui->tw_list->addTopLevelItem(root);
            root->addChild(child);
        }else if(!rootSearch.isEmpty() && child->text(qItemsView(place))!=""){
            rootSearch[0]->addChild(child);
            int weight = rootSearch[0]->text(qListWidget(place)).toDouble() + query.value(3).toDouble();
            rootSearch[0]->setText(qListWidget(place),QString::number(weight));
        }

        ui->tw_list->expandAll();
        for(int i=0;i<ui->tw_list->columnCount();i++){
            ui->tw_list->resizeColumnToContents(i);
        }
    }

}

void ListWindow::removeItemInQTree(QVector<QString> vectorFromList){
    int quantity = vectorFromList[qListWidget(l_quantity)].toInt();
    QList<QTreeWidgetItem*> childSearch = ui->tw_list->findItems(vectorFromList[qListWidget(l_id)], Qt::MatchExactly|Qt::MatchRecursive,qListWidget(l_id));
    QTreeWidgetItem* root = childSearch[0]->parent();

    int weight = childSearch[0]->text(qListWidget(l_weight)).toDouble();

    // Update Weight
    if(childSearch[0]->text(qListWidget(l_weightSelf)).isEmpty()){
        int weightBackpack = childSearch[0]->text(qListWidget(l_weightBackpack)).toDouble();
        int totalWeightBackpack = root->text(qListWidget(l_weightBackpack)).toDouble();
        childSearch[0]->setText(qListWidget(l_weightBackpack),QString::number(weightBackpack-weight));
        root->setText(qListWidget(l_weightBackpack),QString::number(totalWeightBackpack-weight));

    }else if(childSearch[0]->text(qListWidget(l_weightBackpack)).isEmpty()){
        int weightSelf = childSearch[0]->text(qListWidget(l_weightSelf)).toDouble();
        int totalWeightSelf = root->text(qListWidget(l_weightSelf)).toDouble();
        childSearch[0]->setText(qListWidget(l_weightSelf),QString::number(weightSelf-weight));
        root->setText(qListWidget(l_weightSelf),QString::number(totalWeightSelf-weight));
    }

    // Update Quantity
    if(quantity>1){
        childSearch[0]->setText(qListWidget(l_quantity),QString::number(quantity-1) );
    }else if(quantity==1){
        root->removeChild(childSearch[0]);
    }

    // If there are no more children delete the root
    if(root->childCount()==0){
        delete ui->tw_list->takeTopLevelItem(ui->tw_list->indexOfTopLevelItem(root));
    }
}
