#include "listwindow.h"
#include "ui_listwindow.h"
#include <QDebug>

ListWindow::ListWindow(QWidget *parent, SqLite *sqlitepointer, int index) :
    QDialog(parent),
    ui(new Ui::ListWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Widget);

    sqlite = sqlitepointer;
    numberOfCategoriesInList=0;

    // Authorize drag/drop events
    ui->qw_backpack->setAcceptDrops(true);
    ui->qw_backpack->installEventFilter(this);
    ui->qw_self->setAcceptDrops(true);
    ui->qw_self->installEventFilter(this);
    ui->qw_delete->setAcceptDrops(true);
    ui->qw_delete->installEventFilter(this);
    ui->tw_list->setColumnCount(length_qListWidget);
    ui->tw_list->installEventFilter(this);

    // Put labels in header of list
    QStringList headerLabels;
    headerLabels.push_back(tr("Marque"));
    headerLabels.push_back(tr("Modèle"));
    headerLabels.push_back(tr("Poids Sac[gr]"));
    headerLabels.push_back(tr("Poids Soi[gr]"));
    headerLabels.push_back(tr("Quantité"));
    headerLabels.push_back(tr("ID"));
    headerLabels.push_back(tr("Poids individuel[gr]"));
    ui->tw_list->setHeaderLabels(headerLabels);
    ui->tw_list->setColumnWidth(5,0);

    // Set scale to pictures
    ui->lbl_backpack->setPixmap(QPixmap(":/images/backpack.png").scaled(100,100,Qt::KeepAspectRatio));
    ui->lbl_self->setPixmap(QPixmap(":/images/self.png").scaled(100,100,Qt::KeepAspectRatio));

    // Set current date
    ui->de_hikeDate->setDate(QDate::currentDate());
    ui->de_listDate->setDate(QDate::currentDate());

    // Retrieve all the existing items
    populatetw_Matos();

    // In case of a modification of the list
    if(index!=0){
        whatToDo=TO_UPDATE;
        id_list=index;
        getExistingList();
    }
}

ListWindow::~ListWindow()
{
    sqlite=NULL;
    delete sqlite;
    delete ui;
}

/************************************************************************/
/* Retrieve items/list                                                  */
/************************************************************************/

void ListWindow::populatetw_Matos()
{
    QString qry="SELECT Categories.name, Items.id_item, Brands.name, Items.reference, Items.weight FROM Categories, Brands INNER JOIN Items ON Items.id_category = Categories.id_category AND Items.id_brand = Brands.id_brand ORDER BY Categories.name ASC, Brands.name ASC, Items.reference ASC";
    QSqlQuery query;
    query.prepare(qry);
    query.exec();

    // Retrieve all existing items
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
        preparedRow.first()->appendRow(secondRow);
        children++;
    }

    m->setHeaderData(0, Qt::Horizontal, tr("Marque"));
    m->setHeaderData(1, Qt::Horizontal, tr("Modèle"));
    m->setHeaderData(2, Qt::Horizontal, tr("Poids[gr]"));
    m->setHeaderData(3, Qt::Horizontal, tr("ID"));
    ui->tw_items->setModel(m);

    // Nice presentation
    ui->tw_items->expandAll();
    for(int i=0;i<m->columnCount()-1;i++){
        ui->tw_items->resizeColumnToContents(i);
    }
    ui->tw_items->collapseAll();

    m=NULL;
    delete m;
}

void ListWindow::getExistingList(){
    QElapsedTimer timer;
    timer.start();

    // Protection against filled list
    list_id_item_toDelete.clear();

    // Retrive list properties
    QString qry="SELECT Lists.name, Lists.hikeDate, Lists.creationDate, Lists.id_foodPlan, Lists.weightBackpack, Lists.weightSelf, Lists.note FROM Lists WHERE Lists.id_list=:id_list";
    QSqlQuery query;
    query.prepare(qry);
    query.bindValue(":id_list",id_list);
    query.exec();

    if(query.next()){
        // Display values retrieved
        ui->le_listName->setText(query.value(0).toString());
        ui->de_hikeDate->setDate(QDate::fromString(query.value(1).toString(), "dd-MM-yy"));
        ui->de_listDate->setDate(QDate::fromString(query.value(2).toString(), "dd-MM-yy"));
        ui->te_note->setText(query.value(6).toString());

        // Retrieve elements in list
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
    }

    qDebug() <<"[ListWindow] Time Elapsed in (getExistingList):" <<timer.elapsed()<<" msec";
}

/************************************************************************/
/* Manage the dockWidget                                                */
/************************************************************************/

void ListWindow::on_dockWidget_visibilityChanged(bool visible)
{
    if(visible==false){
        ui->dockWidget->setFloating(false);
        ui->dockWidget->raise();
        ui->dockWidget->show();
    }
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

/************************************************************************/
/* Handle insertion/deletion in QTreeWidget                             */
/************************************************************************/

bool ListWindow::eventFilter(QObject* obj, QEvent* event){

    // If an item is dragged and dropped into one of the two areas permitted, add it to the list
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
    // If an item is dragged and dropped into suppresion area, remove it from the list
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

    // If the user interacts with tw_list
    if(obj==ui->tw_list){
        QKeyEvent* pKeyEvent=static_cast<QKeyEvent*>(event);

        if(ui->tw_list->currentItem()!=NULL&&((event->type()==QEvent::KeyRelease && pKeyEvent->key() == Qt::Key_Enter)||event->type()==QEvent::FocusIn)){
                updateQuantity(ui->tw_list->currentItem());
        }
        if (pKeyEvent->key() == Qt::Key_Delete && event->type()==QEvent::KeyRelease){
            if (!ui->tw_list->selectedItems().isEmpty()&&ui->tw_list->currentItem()->childCount()==0){
                QVector<QString> vector;
                for(int i=0;i<length_qListWidget;i++){
                  vector.append(ui->tw_list->selectedItems()[0]->text(i));
                }
                removeItemInQTree(vector);
            }
        }

    }

}

void ListWindow::on_tw_items_doubleClicked(const QModelIndex &index)
{
    insertItemInQTree(index.sibling(index.row(),3).data().toInt(), qListWidget(l_weightBackpack), 1);
}

void ListWindow::on_tw_list_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(column==qListWidget(l_quantity)&&item->text(qListWidget(l_quantity))!=""){
        item->setFlags(item->flags()|Qt::ItemIsEditable);
    }else{
        item->setFlags(Qt::ItemIsEnabled|Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled);
    }
}

void ListWindow::insertItemInQTree(int id_item, qListWidget place, int defaultQuantity){

    QString qry="SELECT Categories.name, Brands.name, Items.reference, Items.weight, Items.quantity FROM Brands, Categories INNER JOIN Items ON Items.id_category = Categories.id_category AND Items.id_brand = Brands.id_brand WHERE Items.id_item=:id_item";
    QSqlQuery query;
    query.prepare(qry);
    query.bindValue(":id_item", id_item);
    query.exec();

    if(query.next()){
        bool addItemWeightToTotal=false;
        //Child Search
        QList<QTreeWidgetItem*> childSearch = ui->tw_list->findItems(QString::number(id_item), Qt::MatchExactly|Qt::MatchRecursive,qListWidget(l_id));
        QTreeWidgetItem *child = new QTreeWidgetItem();

        if(childSearch.isEmpty()){
            addItemWeightToTotal=true;
            child->setText(qListWidget(l_brand),query.value(1).toString());
            child->setText(qListWidget(l_reference),query.value(2).toString());
            child->setText(qListWidget(place),QString::number(query.value(3).toDouble()*defaultQuantity));
            child->setText(qListWidget(l_quantity),QString::number(defaultQuantity));
            child->setText(qListWidget(l_id),QString::number(id_item));
            child->setText(qListWidget(l_weight), query.value(3).toString());

            QTreeWidgetItem *root = new QTreeWidgetItem();
            root->setText(0, query.value(0).toString());
            root->setText(qListWidget(place), query.value(3).toString());
            ui->tw_list->addTopLevelItem(root);
            root->addChild(child);
            numberOfCategoriesInList++;
            root = NULL;
            delete root;

        }else if(!childSearch.isEmpty() && childSearch[0]->text(qItemsView(place))!=""){
            childSearch[0]->setText(qListWidget(l_quantity), QString::number(childSearch[0]->text(qListWidget(l_quantity)).toInt()+1));
            updateQuantity(childSearch[0]);
        }

        // Allow nice presentation
        ui->tw_list->expandAll();
        for(int i=0;i<ui->tw_list->columnCount();i++){
            ui->tw_list->resizeColumnToContents(i);

        }
        ui->tw_list->setColumnWidth(5,0);

        // Display total weight value
        if(qListWidget(place)==qListWidget(l_weightBackpack)&&addItemWeightToTotal==true){
            ui->lbl_weightBackpack->setText(QString::number(ui->lbl_weightBackpack->text().toDouble()+(query.value(3).toDouble()*defaultQuantity)));
        }else if(qListWidget(place)==qListWidget(l_weightSelf)&&addItemWeightToTotal==true){
            ui->lbl_weightSelf->setText(QString::number(ui->lbl_weightSelf->text().toDouble()+(query.value(3).toDouble()*defaultQuantity)));
        }

        // Delete pointer
        child = NULL;
        delete child;
    }

}

void ListWindow::updateQuantity(QTreeWidgetItem *item){

    double newWeight = item->text(qListWidget(l_weight)).toDouble()*item->text(qListWidget(l_quantity)).toInt();

    if(item->text(qListWidget(l_weightBackpack)).isEmpty()){
        double oldWeight = item->text(qListWidget(l_weightSelf)).toDouble();
        double parentWeight = item->parent()->text(qListWidget(l_weightSelf)).toDouble();
        item->parent()->setText(qListWidget(l_weightSelf),QString::number(parentWeight-oldWeight+newWeight));
        item->setText(qListWidget(l_weightSelf), QString::number(newWeight));
        ui->lbl_weightSelf->setText(QString::number(ui->lbl_weightSelf->text().toDouble()-oldWeight+newWeight));

    }else if(item->text(qListWidget(l_weightSelf)).isEmpty()){
        double oldWeight = item->text(qListWidget(l_weightBackpack)).toDouble();
        double parentWeight = item->parent()->text(qListWidget(l_weightBackpack)).toDouble();
        item->parent()->setText(qListWidget(l_weightBackpack),QString::number(parentWeight-oldWeight+newWeight));
        item->setText(qListWidget(l_weightBackpack), QString::number(newWeight));
        ui->lbl_weightBackpack->setText(QString::number(ui->lbl_weightBackpack->text().toDouble()-oldWeight+newWeight));
        ui->lbl_weightBackpack_total->setText(QString::number(ui->lbl_weightBackpack->text().toDouble()+ui->lbl_weightBackpack_food->text().toDouble()));
    }
}

void ListWindow::removeItemInQTree(QVector<QString> vectorFromList){

    // Handle suppression of item if the list already existed
    if(whatToDo==TO_UPDATE && list_id_item.contains(vectorFromList[qListWidget(l_id)].toInt())){
        list_id_item_toDelete.append(vectorFromList[qListWidget(l_id)].toInt());
        list_id_item.removeAt(list_id_item.indexOf(vectorFromList[qListWidget(l_id)].toInt()));
    }

    // Retrieve weight and quantity
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

    // Delete pointer
    root=NULL;
    delete root;
}

/************************************************************************/
/* Save list                                                            */
/************************************************************************/

void ListWindow::duplicateList(QString listName){
    whatToDo=TO_INSERT;
    ui->le_listName->setText(listName);
    on_btn_saveList_clicked();
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
            qDebug()<<"[ListWindow] Adding new list ";
            QVector<int> nothing;
            sqlite->addModifyList(list, numberOfCategoriesInList, ui->tw_list, nothing);

        }else if(whatToDo==TO_UPDATE){
            list.id_list=id_list;
            qDebug()<<"[ListWindow] Updating list with ID : "<<list.id_list;
            qDebug()<<"[ListWindow] To Delete : "<<list_id_item_toDelete;
            qDebug()<<"[ListWindow] To Update : "<<list_id_item;
            // Delete
            sqlite->deleteRecordInItemsLists(id_list, list_id_item_toDelete);
            list_id_item_toDelete.clear();
            // Update & Insert
            sqlite->addModifyList(list, numberOfCategoriesInList, ui->tw_list, list_id_item);

        }
        close();
    }else{
        QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),tr("Veuillez entrer un nom de liste."));
    }
    qDebug() <<"[ListWindow] Time Elapsed in (on_btn_saveList_clicked):" <<timer.elapsed()<<" msec";

}

/************************************************************************/
/* Various functions                                                    */
/************************************************************************/

// Decode a QByteArray and put into a QVector of QString
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

    delete ds;
    return data;
}

// Create a QList of QStandardItem to be put in QTreeView
QList<QStandardItem *> ListWindow::prepareRow(const QString &first, const QString &second, const QString &third, const QString &forth)
{
    QList<QStandardItem *> rowItems;
    rowItems << new QStandardItem(first);
    rowItems << new QStandardItem(second);
    rowItems << new QStandardItem(third);
    rowItems << new QStandardItem(forth);
    return rowItems;
}
