#include "listwindow.h"
#include "ui_listwindow.h"
#include <QDebug>

ListWindow::ListWindow(QWidget *parent, SqLite *sqlitepointer, int index) :
    QDialog(parent),
    ui(new Ui::ListWindow)
{
    ui->setupUi(this);

    sqlite = sqlitepointer;

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
    headerLabels.push_back(tr("ID"));
    headerLabels.push_back(tr("Poids individuel"));
    ui->tw_list->setHeaderLabels(headerLabels);

    ui->lbl_backpack->setPixmap(QPixmap(":/images/backpack.png").scaled(100,100,Qt::KeepAspectRatio));
    ui->lbl_self->setPixmap(QPixmap(":/images/self.png").scaled(100,100,Qt::KeepAspectRatio));

    ui->de_hikeDate->setDate(QDate::currentDate());
    ui->de_listDate->setDate(QDate::currentDate());

    numberOfCategoriesInList=0;

    if(index!=0){
        whatToDo=TO_UPDATE;
        id_list=index;
        getExistingList();
    }
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
                int id_item = decodeByteArray(ba).at(qItemsView(i_id)).toInt();
                if(obj==ui->qw_backpack){insertItemInQTree(id_item, qListWidget(l_weightBackpack),1);}
                else if(obj==ui->qw_self){ insertItemInQTree(id_item, qListWidget(l_weightSelf),1);}

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
            if (ui->tw_list->hasFocus() && !ui->tw_list->selectedItems().isEmpty()&&ui->tw_list->currentItem()->childCount()==0){
                QVector<QString> vector;
                for(int i=0;i<length_qListWidget;i++){
                  vector.append(ui->tw_list->selectedItems()[0]->text(i));
                }
                removeItemInQTree(vector);
            }
        }
    }
}

void ListWindow::getExistingList(){
    QElapsedTimer timer;
    timer.start();
    list_id_item_toDelete.clear();

    QString qry="SELECT Lists.name, Lists.hikeDate, Lists.creationDate, Lists.id_foodPlan, Lists.weightBackpack, Lists.weightSelf, Lists.note FROM Lists WHERE Lists.id_list=:id_list";
    QSqlQuery query;
    query.prepare(qry);
    query.bindValue(":id_list",id_list);
    query.exec();
    query.next();

    ui->le_listName->setText(query.value(0).toString());
    ui->de_hikeDate->setDate(QDate::fromString(query.value(1).toString(), "dd-MM-yy"));
    ui->de_listDate->setDate(QDate::fromString(query.value(2).toString(), "dd-MM-yy"));
    ui->te_note->setText(query.value(6).toString());

    qry="SELECT ItemsLists.id_item, ItemsLists.backpackOrSelf, ItemsLists.quantity FROM ItemsLists WHERE ItemsLists.id_list=:id_list";
    query.clear();
    query.prepare(qry);
    query.bindValue(":id_list",id_list);
    query.exec();

    while(query.next()){
        list_id_item.append(query.value(0).toInt());
        if(query.value(1)=="backpack"){
            insertItemInQTree(query.value(0).toInt(), qListWidget(l_weightBackpack),query.value(2).toInt());
        }else if(query.value(1)=="self"){
            insertItemInQTree(query.value(0).toInt(), qListWidget(l_weightSelf),query.value(2).toInt());
        }
    }

    qDebug() <<"[ListWindow] Time Elapsed in (getExistingList):" <<timer.elapsed()<<" msec";
}

void ListWindow::insertItemInQTree(int id_item, qListWidget place, int defaultQuantity){
    QString qry="SELECT Categories.name, Brands.name, Items.reference, Items.weight, Items.quantity FROM Brands, Categories INNER JOIN Items ON Items.id_category = Categories.id_category AND Items.id_brand = Brands.id_brand WHERE Items.id_item=:id_item";
    QSqlQuery query;
    query.prepare(qry);
    query.bindValue(":id_item", id_item);
    query.exec();

    if(query.next()){
        //Child Search
        QList<QTreeWidgetItem*> childSearch = ui->tw_list->findItems(QString::number(id_item), Qt::MatchExactly|Qt::MatchRecursive,qListWidget(l_id));
        QTreeWidgetItem *child = new QTreeWidgetItem();

        if(childSearch.isEmpty()){
            child->setText(qListWidget(l_brand),query.value(1).toString());
            child->setText(qListWidget(l_reference),query.value(2).toString());
            child->setText(qListWidget(place),QString::number(query.value(3).toDouble()*defaultQuantity));
            child->setText(qListWidget(l_quantity),QString::number(defaultQuantity));
            child->setText(qListWidget(l_id),QString::number(id_item));
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
            numberOfCategoriesInList++;
        }else if(!rootSearch.isEmpty() && child->text(qItemsView(place))!=""){
            rootSearch[0]->addChild(child);
            int weight = rootSearch[0]->text(qListWidget(place)).toDouble() + (query.value(3).toDouble()*defaultQuantity);
            rootSearch[0]->setText(qListWidget(place),QString::number(weight));
        }

        ui->tw_list->expandAll();
        for(int i=0;i<ui->tw_list->columnCount();i++){
            ui->tw_list->resizeColumnToContents(i);
        }

        // Display total weight value
        if(qListWidget(place)==qListWidget(l_weightBackpack)){
            ui->lbl_weightBackpack->setText(QString::number(ui->lbl_weightBackpack->text().toDouble()+(query.value(3).toDouble()*defaultQuantity)));
        }else if(qListWidget(place)==qListWidget(l_weightSelf)){
            ui->lbl_weightSelf->setText(QString::number(ui->lbl_weightSelf->text().toDouble()+(query.value(3).toDouble()*defaultQuantity)));
        }
    }
}

void ListWindow::removeItemInQTree(QVector<QString> vectorFromList){

    if(whatToDo==TO_UPDATE && list_id_item.contains(vectorFromList[qListWidget(l_id)].toInt())){
        list_id_item_toDelete.append(vectorFromList[qListWidget(l_id)].toInt());
        list_id_item.removeAt(list_id_item.indexOf(vectorFromList[qListWidget(l_id)].toInt()));
    }

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
        if(numberOfCategoriesInList>0)numberOfCategoriesInList--;
    }

    // Display total weight value
    if(childSearch[0]->text(qListWidget(l_weightSelf)).isEmpty()){
        ui->lbl_weightBackpack->setText(QString::number(ui->lbl_weightBackpack->text().toDouble()-weight));
    }else if(childSearch[0]->text(qListWidget(l_weightBackpack)).isEmpty()){
        ui->lbl_weightSelf->setText(QString::number(ui->lbl_weightSelf->text().toDouble()-weight));

    }
}

void ListWindow::on_btn_saveList_clicked()
{
    QElapsedTimer timer;
    timer.start();

    ListStruct list;
    list.id_list=0;
    list.name=ui->le_listName->text();
    list.creationDate=ui->de_listDate->text();
    list.hikeDate=ui->de_hikeDate->text();
    list.id_foodPlan=0;
    list.weightBackpack=ui->lbl_weightBackpack->text().toDouble();
    list.weightSelf=ui->lbl_weightSelf->text().toDouble();
    list.note=ui->te_note->toPlainText();

    if(ui->le_listName->text()!=""){
        if(whatToDo==TO_INSERT){
            QVector<int> nothing;
            sqlite->addModifyList(list, numberOfCategoriesInList, ui->tw_list, nothing);

        }else if(whatToDo==TO_UPDATE){
            qDebug()<<"To Delete : "<<list_id_item_toDelete;
            qDebug()<<"To Update : "<<list_id_item;
            list.id_list=id_list;
            // Delete
            sqlite->deleteRecordInItemsLists(id_list, list_id_item_toDelete);
            list_id_item_toDelete.clear();
            // Update & Insert
            sqlite->addModifyList(list, numberOfCategoriesInList, ui->tw_list, list_id_item);

        }

    }
    qDebug() <<"[ListWindow] Time Elapsed in (on_btn_saveList_clicked):" <<timer.elapsed()<<" msec";

}

void ListWindow::on_tw_items_doubleClicked(const QModelIndex &index)
{
    insertItemInQTree(index.sibling(index.row(),3).data().toInt(), qListWidget(l_weightBackpack), 1);
}
