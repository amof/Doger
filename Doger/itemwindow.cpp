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

    if(index!=0)whatToDo=TO_UPDATE; id_item=index;

    sqlite->getCategoryBrand(sqlite_CATEGORY, &idCategories, ui->cb_item_categorie);

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
    sqlite->getCategoryBrand(sqlite_BRAND, &idBrand, ui->cb_item_brand);
    sqlite->getCategoryBrand(sqlite_BRAND, &idBrand, ui->cb_alim_brand);
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

void ItemWindow::on_btn_item_save_clicked()
{
    if(ui->cb_item_brand->currentIndex()!=0){
        ItemStruct itemStruct ;
        itemStruct.id_item=0;
        itemStruct.id_category=idCategories.at(ui->cb_item_categorie->currentIndex());
        itemStruct.id_brand=idBrand.at(ui->cb_item_brand->currentIndex()-1);
        itemStruct.reference=ui->le_item_reference->text();
        itemStruct.weight=ui->le_item_weight->text().toDouble();
        itemStruct.quantity=ui->sb_item_quantity->text().toInt();
        itemStruct.volume=ui->le_item_volume->text().toDouble();
        itemStruct.dateOfAcquisition=ui->de_item_date->text();
        itemStruct.price=ui->le_item_price->text().toDouble();
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

    }else{
            QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),tr("Veuillez sélectionner une marque valide."));
        }
}

void ItemWindow::on_btn_item_saveClose_clicked()
{
    if(ui->cb_item_brand->currentIndex()!=0){
        on_btn_item_save_clicked();
        close();
    }
}

void ItemWindow::on_cb_item_brand_activated(int index)
{
    if(index==0){
        ui->le_item_brand->show();
        ui->btn_item_newBrand->show();
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

/************************************************************************/
/* FOOD                                                                 */
/************************************************************************/
void ItemWindow::on_btn_alim_save_clicked()
{

    FoodStruct food;
    food.id_food=0;
    food.id_category=idCategories.at(ui->cb_item_categorie->findText("Alimentation"));
    food.id_brand=idBrand.at(ui->cb_alim_brand->currentIndex()-1);
    food.reference=ui->le_alim_reference->text();
    food.weight=ui->le_alim_weight->text().toDouble();
    food.quantity=ui->sb_alim_quantity->text().toInt();
    food.expirationDate=ui->de_alim_date->text();
    food.price=ui->le_alim_price->text().toDouble();
    food.url=ui->le_alim_url->text();
    food.energy=ui->le_alim_energy->text().toDouble();
    food.fat=ui->le_alim_fat->text().toDouble();
    food.carbohydrates=ui->le_alim_carbohydrates->text().toDouble();
    food.fibres=ui->le_alim_fibres->text().toDouble();
    food.protein=ui->le_alim_protein->text().toDouble();
    food.salt= ui->le_alim_salt->text().toDouble();
    food.note=ui->te_alim_note->toPlainText();

    if(whatToDo==TO_INSERT&&ui->cb_alim_brand->currentIndex()!=0){
        sqlite->addModifyFood(food);
        close();
    }else if (whatToDo==TO_UPDATE&&ui->cb_alim_brand->currentIndex()!=0){
        food.id_food=id_item;
        sqlite->addModifyFood(food);
        sqlite->addModifyFood(food);
        close();
    }else{
        QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),tr("Veuillez sélectionner une marque valide."));
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
    double val = (ui->le_alim_energy->text().toInt()*100)/(myBee.energy);
    QString percentage ;
    percentage = percentage.setNum(val, 'f', 1);
    ui->l_alim_energy->setText(percentage%"%");
    ui->s_alim_energy->setValue(int(val));
}

void ItemWindow::on_le_alim_fat_editingFinished()
{
    double val = (ui->le_alim_fat->text().toInt()*100)/(myBee.fat);
    QString percentage ;
    percentage = percentage.setNum(val, 'f', 1);
    ui->l_alim_fat->setText(percentage%"%");
    ui->s_alim_fat->setValue(int(val));
}

void ItemWindow::on_le_alim_carbohydrates_editingFinished()
{
    double val = (ui->le_alim_carbohydrates->text().toInt()*100)/(myBee.carbohydrates);
    QString percentage ;
    percentage = percentage.setNum(val, 'f', 1);
    ui->l_alim_carbohydrates->setText(percentage%"%");
    ui->s_alim_carbohydrates->setValue(int(val));
}

void ItemWindow::on_le_alim_fibres_editingFinished()
{
    double val = (ui->le_alim_fibres->text().toInt()*100)/(myBee.fibres);
    QString percentage ;
    percentage = percentage.setNum(val, 'f', 1);
    ui->l_alim_fibres->setText(percentage%"%");
    ui->s_alim_fibres->setValue(int(val));
}

void ItemWindow::on_le_alim_protein_editingFinished()
{
    double val = (ui->le_alim_protein->text().toInt()*100)/(myBee.protein);
    QString percentage ;
    percentage = percentage.setNum(val, 'f', 1);
    ui->l_alim_protein->setText(percentage%"%");
    ui->s_alim_protein->setValue(int(val));
}

void ItemWindow::on_le_alim_salt_editingFinished()
{
    double val = (ui->le_alim_salt->text().toInt()*100)/(myBee.salt);
    QString percentage ;
    percentage = percentage.setNum(val, 'f', 1);
    ui->l_alim_salt->setText(percentage%"%");
    ui->s_alim_salt->setValue(int(val));
}
