#include "itemwindow.h"
#include "ui_itemwindow.h"

BEE myBee;

ItemWindow::ItemWindow(QWidget *parent, SqLite *sqlitepointer, int index) :
    QDialog(parent),
    ui(new Ui::ItemWindow)
{
    ui->setupUi(this);

    sqlite = sqlitepointer;

    ui->lbl_item_picture->setPixmap(QPixmap(":/images/noImage.png").scaled(100,100,Qt::KeepAspectRatio));
    ui->lbl_alim_picture->setPixmap(QPixmap(":/images/noImage.png").scaled(100,100,Qt::KeepAspectRatio));

    ui->le_item_category->hide();
    ui->btn_item_newCategory->hide();
    ui->le_item_brand->hide();
    ui->btn_item_newBrand->hide();
    ui->le_alim_brand->hide();
    ui->btn_alim_newBrand->hide();
    ui->de_item_date->setDate(QDate::currentDate());

    ui->s_alim_energy->setEnabled(false);
    ui->s_alim_carbohydrates->setEnabled(false);
    ui->s_alim_fat->setEnabled(false);
    ui->s_alim_protein->setEnabled(false);
    ui->s_alim_fibres->setEnabled(false);
    ui->s_alim_salt->setEnabled(false);
    ui->de_alim_date->setDate(QDate::currentDate());

    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->le_item_category, SIGNAL(returnPressed()),ui->btn_item_newCategory,SIGNAL(clicked()));
    connect(ui->le_item_brand, SIGNAL(returnPressed()),ui->btn_item_newBrand,SIGNAL(clicked()));
    connect(ui->le_alim_brand, SIGNAL(returnPressed()),ui->btn_alim_newBrand,SIGNAL(clicked()));

    if(index!=0)whatToDo=TO_UPDATE; id_item=index;
}

ItemWindow::~ItemWindow()
{
    sqlite=NULL;
    delete sqlite;
    delete ui;
}

void ItemWindow::setPage(quint8 page){
    ui->stackedWidget->setCurrentIndex(page);
    currentPage=page;
    loadDatabase(id_item);
}

void ItemWindow::setBEE(BEE beeReceived){
    myBee=beeReceived;
}

void ItemWindow::loadDatabase(int index){

    sqlite->getCategoryBrand(sqlite_CATEGORY, &idCategories, ui->cb_item_categorie);
    sqlite->getCategoryBrand(sqlite_BRAND, &idBrand, ui->cb_item_brand);
    sqlite->getCategoryBrand(sqlite_BRAND, &idBrand, ui->cb_alim_brand);
    ui->cb_item_categorie->insertItem(0,"Nouvelle");
    ui->cb_item_brand->insertItem(0,"Nouvelle");
    ui->cb_alim_brand->insertItem(0,"Nouvelle");

    if(index!=0 && currentPage==PAGE_MATERIEL){
        QString qry="SELECT Categories.name,"
                    "Brands.name,"
                    "Items.reference,"
                    "Items.weight,"
                    "Items.volume,"
                    "Items.quantity,"
                    "Items.dateOfAcquisition,"
                    "Items.price,"
                    "Items.desired,"
                    "Items.url_manufacturer,"
                    "Items.url_RL,"
                    "Items.note "
                    "FROM Brands, Categories INNER JOIN Items ON Items.id_category = Categories.id_category AND Items.id_brand = Brands.id_brand WHERE Items.id_item=:id";
        QSqlQuery query;
        query.prepare(qry);
        query.bindValue(":id", index );
        query.exec();
        query.next();

        ui->cb_item_categorie->setCurrentIndex(ui->cb_item_categorie->findText(query.value(0).toString()));
        ui->cb_item_brand->setCurrentIndex(ui->cb_item_brand->findText(query.value(1).toString()));
        ui->le_item_reference->setText(query.value(2).toString());
        ui->le_item_weight->setText(query.value(3).toString());
        ui->le_item_volume->setText(query.value(4).toString());
        ui->sb_item_quantity->setValue(query.value(5).toInt());
        ui->de_item_date->setDate(QDate::fromString(query.value(6).toString(),"dd-MM-yy"));
        ui->le_item_price->setText(query.value(7).toString());
        ui->cb_item_desired->setChecked(query.value(8).toBool());
        ui->le_item_url->setText(query.value(9).toString());
        ui->le_item_url_rl->setText(query.value(10).toString());
        ui->te_item_note->setText(query.value(11).toString());
    }
    else if(index!=0 && currentPage==PAGE_ALIMENTATION){
        QString qry="SELECT Brands.name,"
                    "Food.reference, "
                    "Food.weight, "
                    "Food.quantity, "
                    "Food.expirationDate, "
                    "Food.price, "
                    "Food.url, "
                    "Food.energy, "
                    "Food.fat, "
                    "Food.carbohydrates, "
                    "Food.fibres, "
                    "Food.protein, "
                    "Food.salt, "
                    "Food.note FROM Food INNER JOIN Categories ON Food.id_category = Categories.id_category INNER JOIN Brands ON Food.id_brand = Brands.id_brand WHERE id_food=:id";
        QSqlQuery query;
        query.prepare(qry);
        query.bindValue(":id", index );
        query.exec();
        query.next();

        ui->cb_alim_brand->setCurrentIndex(ui->cb_alim_brand->findText(query.value(0).toString()));
        ui->le_alim_reference->setText(query.value(1).toString());
        ui->le_alim_weight->setText(query.value(2).toString());
        ui->sb_alim_quantity->setValue(query.value(3).toInt());
        ui->de_alim_date->setDate(QDate::fromString(query.value(4).toString(),"dd-MM-yy"));
        ui->le_alim_price->setText(query.value(5).toString());
        ui->le_alim_url->setText(query.value(6).toString());
        ui->le_alim_energy->setText(query.value(7).toString());
        ui->le_alim_fat->setText(query.value(8).toString());
        ui->le_alim_carbohydrates->setText(query.value(9).toString());
        ui->le_alim_fibres->setText(query.value(10).toString());
        ui->le_alim_protein->setText(query.value(11).toString());
        ui->le_alim_salt->setText(query.value(12).toString());
        ui->te_alim_note->setText(query.value(13).toString());

        displayPercentage(ui->le_alim_energy, myBee.energy, ui->s_alim_energy, ui->l_alim_energy);
        displayPercentage(ui->le_alim_fat, myBee.fat, ui->s_alim_fat, ui->l_alim_fat);
        displayPercentage(ui->le_alim_carbohydrates, myBee.carbohydrates, ui->s_alim_carbohydrates, ui->l_alim_carbohydrates);
        displayPercentage(ui->le_alim_fibres, myBee.fibres, ui->s_alim_fibres, ui->l_alim_fibres);
        displayPercentage(ui->le_alim_protein, myBee.protein, ui->s_alim_protein, ui->l_alim_protein);
        displayPercentage(ui->le_alim_salt, myBee.salt, ui->s_alim_salt, ui->l_alim_salt);
    }

}

bool ItemWindow::loadFile(const QString &fileName, QLabel *label)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }
    setImage(newImage, label);

    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
        .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
    qDebug()<<message;
    return true;
}


void ItemWindow::setImage(const QImage &newImage, QLabel *label)
{
    image = newImage;
    label->setPixmap(QPixmap::fromImage(image).scaled(100,100,Qt::KeepAspectRatio));
}

static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void ItemWindow::on_btn_item_loadImage_clicked()
{
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first(), ui->lbl_alim_picture)) {}
}

void ItemWindow::on_btn_item_deleteImage_clicked()
{
    ui->lbl_alim_picture->setPixmap(QPixmap(":/images/noImage.png").scaled(100,100,Qt::KeepAspectRatio));
}

void ItemWindow::on_btn_alim_loadImage_clicked()
{
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first(), ui->lbl_item_picture)) {}
}

void ItemWindow::on_btn_alim_deleteImage_clicked()
{
    ui->lbl_item_picture->setPixmap(QPixmap(":/images/noImage.png").scaled(100,100,Qt::KeepAspectRatio));
}

/************************************************************************/
/* Item                                                                 */
/************************************************************************/

bool ItemWindow::on_btn_item_save_clicked()
{
    bool insertSuccessfully = false;
    if(ui->cb_item_brand->currentIndex()!=0 && !ui->le_item_reference->text().isEmpty()){
        qDebug()<<"[ItemWindow] Saving !";
        ItemStruct itemStruct ;
        itemStruct.id_item=0;
        itemStruct.id_category=idCategories.at(ui->cb_item_categorie->currentIndex());
        itemStruct.id_brand=idBrand.at(ui->cb_item_brand->currentIndex()-1);
        itemStruct.reference=ui->le_item_reference->text();
        itemStruct.weight=replaceDotWithComma(ui->le_item_weight->text());
        itemStruct.quantity=ui->sb_item_quantity->text().toInt();
        itemStruct.volume=ui->le_item_volume->text().toDouble();
        itemStruct.dateOfAcquisition=ui->de_item_date->text();
        itemStruct.price=replaceDotWithComma(ui->le_item_price->text());
        itemStruct.desired=ui->cb_item_desired->isChecked();
        itemStruct.url_manufacturer=ui->le_item_url->text();
        itemStruct.url_RL=ui->le_item_url_rl->text();
        itemStruct.note=ui->te_item_note->toPlainText();

        if(whatToDo==TO_UPDATE){
            itemStruct.id_item=id_item;
        }

        sqlite->addModifyItem(itemStruct);

        ui->le_item_reference->clear();
        ui->le_item_weight->clear();
        ui->sb_item_quantity->setValue(1);
        ui->le_item_volume->clear();
        ui->de_item_date->setDate(QDate::currentDate());
        ui->le_item_price->clear();
        ui->cb_item_desired->setChecked(false);
        ui->le_item_url->clear();
        ui->le_item_url_rl->clear();
        ui->te_item_note->clear();

        insertSuccessfully = true;

    }else{
            QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),tr("Veuillez sélectionner une marque valide et indiquer une référence valide."));
        }

    return insertSuccessfully;
}

void ItemWindow::on_btn_item_saveClose_clicked()
{
    if(ui->cb_item_brand->currentIndex()!=0){
       if(on_btn_item_save_clicked()){
            close();
        }
    }
}

void ItemWindow::on_cb_item_brand_activated(int index)
{
    if(index==0){
        ui->le_item_brand->show();
        ui->btn_item_newBrand->show();
        ui->le_item_brand->setFocus();
    }else{
        ui->le_item_brand->hide();
        ui->btn_item_newBrand->hide();
    }
}

void ItemWindow::on_btn_item_newBrand_clicked()
{
    if(ui->cb_item_brand->findText(ui->le_item_brand->text())==-1){
        ui->le_item_brand->hide();
        ui->btn_item_newBrand->hide();
        if(ui->cb_item_brand->currentIndex()==0){
            sqlite->addCategoryBrand(sqlite_BRAND, ui->le_item_brand->text());
        }
        loadDatabase(id_item);
        ui->cb_item_brand->setCurrentText(ui->le_item_brand->text());
        ui->le_item_brand->clear();

    }else{
        QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),
                                 tr("Une marque portant le nom de %1 existe déjà.")
                                 .arg(ui->le_item_brand->text()));
    }

}

void ItemWindow::on_cb_item_categorie_activated(int index)
{
    if(index==0){
        ui->le_item_category->show();
        ui->btn_item_newCategory->show();
        ui->le_item_category->setFocus();
    }else{
        ui->le_item_category->hide();
        ui->btn_item_newCategory->hide();
    }
}

void ItemWindow::on_btn_item_newCategory_clicked()
{
    if(ui->cb_item_categorie->findText(ui->le_item_category->text())==-1){
        ui->le_item_category->hide();
        ui->btn_item_newCategory->hide();
        if(ui->cb_item_categorie->currentIndex()==0){
            sqlite->addCategoryBrand(sqlite_CATEGORY, ui->le_item_category->text());
        }
        loadDatabase(id_item);
        ui->cb_item_categorie->setCurrentText(ui->le_item_category->text());
        ui->le_item_category->clear();

    }else{
        QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),
                                 tr("Une marque portant le nom de %1 existe déjà.")
                                 .arg(ui->le_item_category->text()));
    }
}

/************************************************************************/
/* FOOD                                                                 */
/************************************************************************/
void ItemWindow::on_btn_alim_save_clicked()
{

    if(ui->cb_alim_brand->currentIndex()>0 && !ui->le_alim_reference->text().isEmpty()){
        if(ui->cb_item_categorie->findText("Alimentation")==-1){
            sqlite->addCategoryBrand(sqlite_CATEGORY,"Alimentation");
            sqlite->getCategoryBrand(sqlite_CATEGORY, &idCategories, ui->cb_item_categorie);
        }
        FoodStruct food;
        food.id_food=0;
        food.id_category=idCategories.at(ui->cb_item_categorie->findText("Alimentation"));
        food.id_brand=idBrand.at(ui->cb_alim_brand->currentIndex()-1);
        food.reference=ui->le_alim_reference->text();
        food.weight=replaceDotWithComma(ui->le_alim_weight->text());
        food.quantity=ui->sb_alim_quantity->text().toInt();
        food.expirationDate=ui->de_alim_date->text();
        food.price=replaceDotWithComma(ui->le_alim_price->text());
        food.url=ui->le_alim_url->text();
        food.energy=replaceDotWithComma(ui->le_alim_energy->text());
        food.fat=replaceDotWithComma(ui->le_alim_fat->text());
        food.carbohydrates=replaceDotWithComma(ui->le_alim_carbohydrates->text());
        food.fibres=replaceDotWithComma(ui->le_alim_fibres->text());
        food.protein=replaceDotWithComma(ui->le_alim_protein->text());
        food.salt= replaceDotWithComma(ui->le_alim_salt->text());
        food.note=ui->te_alim_note->toPlainText();

        if(whatToDo==TO_INSERT&&ui->cb_alim_brand->currentIndex()!=0){
            sqlite->addModifyFood(food);
            close();
        }else if (whatToDo==TO_UPDATE&&ui->cb_alim_brand->currentIndex()!=0){
            food.id_food=id_item;
            sqlite->addModifyFood(food);
            close();
        }
    }
    else{
        QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),tr("Veuillez sélectionner une marque valide et indiquer une référence."));
    }


}

void ItemWindow::on_btn_alim_newBrand_clicked()
{
    if(ui->cb_alim_brand->findText(ui->le_alim_brand->text())==-1){
        ui->le_alim_brand->hide();
        ui->btn_alim_newBrand->hide();
        if(ui->cb_alim_brand->currentIndex()==0){
            sqlite->addCategoryBrand(sqlite_BRAND, ui->le_alim_brand->text());
        }
        loadDatabase(id_item);
        ui->cb_alim_brand->setCurrentText(ui->le_alim_brand->text());
        ui->le_alim_brand->clear();

    }else{
        QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),
                                 tr("Une marque portant le nom de %1 existe déjà.")
                                 .arg(ui->le_alim_brand->text()));
    }

}

void ItemWindow::on_cb_alim_brand_activated(int index)
{
    if(index==0){
        ui->le_alim_brand->show();
        ui->btn_alim_newBrand->show();
    }else{
        ui->le_alim_brand->hide();
        ui->btn_alim_newBrand->hide();
    }
}

// Display %

void ItemWindow::on_le_alim_energy_editingFinished()
{
    displayPercentage(ui->le_alim_energy, myBee.energy, ui->s_alim_energy, ui->l_alim_energy);
}

void ItemWindow::on_le_alim_fat_editingFinished()
{
    displayPercentage(ui->le_alim_fat, myBee.fat, ui->s_alim_fat, ui->l_alim_fat);
}

void ItemWindow::on_le_alim_carbohydrates_editingFinished()
{
    displayPercentage(ui->le_alim_carbohydrates, myBee.carbohydrates, ui->s_alim_carbohydrates, ui->l_alim_carbohydrates);
}

void ItemWindow::on_le_alim_fibres_editingFinished()
{
    displayPercentage(ui->le_alim_fibres, myBee.fibres, ui->s_alim_fibres, ui->l_alim_fibres);
}

void ItemWindow::on_le_alim_protein_editingFinished()
{
    displayPercentage(ui->le_alim_protein, myBee.protein, ui->s_alim_protein, ui->l_alim_protein);
}

void ItemWindow::on_le_alim_salt_editingFinished()
{
    displayPercentage(ui->le_alim_salt, myBee.salt, ui->s_alim_salt, ui->l_alim_salt);
}

void ItemWindow::displayPercentage(QLineEdit *le, double refValue, QSlider *slider, QLabel *labelPercentage){
    QString value = le->text();
    value = value.replace(",", ".");
    double calculatedPercentage = (value.toDouble()*100)/refValue;
    QString percentage ;
    percentage = percentage.setNum(calculatedPercentage, 'f', 1);
    labelPercentage->setText(percentage%"%");
    slider->setValue(int(calculatedPercentage));
}

double ItemWindow::replaceDotWithComma(QString value){
    value = value.replace(",", ".");

    return value.toDouble();
}
