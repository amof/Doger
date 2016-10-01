#include "itemwindow.h"
#include "ui_itemwindow.h"

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
    delete ui;
}

void ItemWindow::setPage(quint8 page){
    ui->stackedWidget->setCurrentIndex(page);
    currentPage=page;
    loadDatabase(id_item);
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
    if(whatToDo==TO_INSERT&&ui->cb_item_brand->currentIndex()!=0){
        QString Squery = "INSERT INTO Items VALUES (NULL , :id_category, :id_brand, :reference, :weight, :quantity, :volume, :dateOfAcquisition, :price, :desired, :url_manufacturer, :url_RL, :note)";
        QSqlQuery query;

        query.prepare(Squery);
        query.bindValue(":id_category", idCategories.at(ui->cb_item_categorie->currentIndex()));
        query.bindValue(":id_brand", idBrand.at(ui->cb_item_brand->currentIndex()-1));
        query.bindValue(":reference", ui->le_item_reference->text());
        query.bindValue(":weight", ui->le_item_weight->text().toDouble());
        query.bindValue(":quantity", ui->sb_item_quantity->text().toInt());
        query.bindValue(":volume", ui->le_item_volume->text().toDouble());
        query.bindValue(":dateOfAcquisition", ui->de_item_date->text());
        query.bindValue(":price", ui->le_item_price->text().toDouble());
        query.bindValue(":desired", ui->cb_item_desired->isChecked());
        query.bindValue(":url_manufacturer", ui->le_item_url->text());
        query.bindValue(":url_RL", ui->le_item_url_rl->text());
        query.bindValue(":note", ui->te_item_note->toPlainText());

        if(!query.exec()) qDebug()<<"[SQLite] Erreur dans l'ajout d'un nouvel item :";
        close();
    }else if (whatToDo==TO_UPDATE&&ui->cb_item_brand->currentIndex()!=0){
        QString Squery="UPDATE Items SET id_category=:id_category,"
                       "id_brand=:id_brand,"
                       "reference=:reference,"
                       "weight=:weight,"
                       "quantity=:quantity,"
                       "volume=:volume,"
                       "dateOfAcquisition=:dateOfAcquisition,"
                       "price=:price,"
                       "desired=:desired,"
                       "url_manufacturer=:url_manufacturer,"
                       "url_RL=:url_RL,"
                       "note=:note "
                       "WHERE id_item=:id_item";
        QSqlQuery query;
        query.prepare(Squery);
        query.bindValue(":id_category", idCategories.at(ui->cb_item_categorie->currentIndex()));
        query.bindValue(":id_brand", idBrand.at(ui->cb_item_brand->currentIndex()-1));
        query.bindValue(":id_item", id_item);
        query.bindValue(":reference", ui->le_item_reference->text());
        query.bindValue(":weight", ui->le_item_weight->text().toDouble());
        query.bindValue(":quantity", ui->sb_item_quantity->text().toInt());
        query.bindValue(":volume", ui->le_item_volume->text().toDouble());
        query.bindValue(":dateOfAcquisition", ui->de_item_date->text());
        query.bindValue(":price", ui->le_item_price->text().toDouble());
        query.bindValue(":desired", ui->cb_item_desired->isChecked());
        query.bindValue(":url_manufacturer", ui->le_item_url->text());
        query.bindValue(":url_RL", ui->le_item_url_rl->text());
        query.bindValue(":note", ui->te_item_note->toPlainText());

        if(!query.exec()) qDebug()<<"[SQLite] Erreur dans la modification d'un item "<<query.lastError();
        close();
    }else{
            QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),tr("Veuillez sélectionner une marque valide."));
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
    if(whatToDo==TO_INSERT&&ui->cb_alim_brand->currentIndex()!=0){
        QString Squery = "INSERT INTO Food VALUES (NULL , :id_category, :id_brand, :reference, :weight, :quantity, :expirationDate, :price,"
                         " :url, :energy, :fat,:carbohydrates,:fibres,:protein,:salt, :note)";
        QSqlQuery query;

        query.prepare(Squery);
        query.bindValue(":id_category", idCategories.at(ui->cb_item_categorie->findText("Alimentation"))); // !!!
        query.bindValue(":id_brand", idBrand.at(ui->cb_alim_brand->currentIndex()-1));
        query.bindValue(":reference", ui->le_alim_reference->text());
        query.bindValue(":weight", ui->le_alim_weight->text().toDouble());
        query.bindValue(":quantity", ui->sb_alim_quantity->text().toInt());
        query.bindValue(":expirationDate",ui->de_alim_date->text());
        query.bindValue(":price", ui->le_alim_price->text().toDouble());
        query.bindValue(":url", ui->le_alim_url->text());
        query.bindValue(":energy", ui->le_alim_energy->text().toDouble());
        query.bindValue(":fat", ui->le_alim_fat->text().toDouble());
        query.bindValue(":carbohydrates", ui->le_alim_carbohydrates->text().toDouble());
        query.bindValue(":fibres", ui->le_alim_fibres->text().toDouble());
        query.bindValue(":protein", ui->le_alim_protein->text().toDouble());
        query.bindValue(":salt", ui->le_alim_salt->text().toDouble());
        query.bindValue(":note", ui->te_alim_note->toPlainText());

        if(!query.exec()) qDebug()<<"[SQLite] Erreur dans l'ajout d'un nouvel item :";
        close();
    }else if (whatToDo==TO_UPDATE&&ui->cb_alim_brand->currentIndex()!=0){
        QString Squery="UPDATE FOOD SET id_brand=:id_brand,"
                       "reference=:reference,"
                       "weight=:weight,"
                       "quantity=:quantity,"
                       "expirationDate=:expirationDate,"
                       "price=:price,"
                       "url=:url,"
                       "energy=:energy,"
                       "fat=:fat,"
                       "carbohydrates=:carbohydrates,"
                       "fibres=:fibres,"
                       "protein=:protein,"
                       "salt=:salt,"
                       "note=:note "
                       "WHERE id_food=:id_food";
        QSqlQuery query;
        query.prepare(Squery);
        query.bindValue(":id_food", id_item);
        query.bindValue(":id_brand", idBrand.at(ui->cb_alim_brand->currentIndex()-1));
        query.bindValue(":reference", ui->le_alim_reference->text());
        query.bindValue(":weight", ui->le_alim_weight->text().toDouble());
        query.bindValue(":quantity", ui->sb_alim_quantity->text().toInt());
        query.bindValue(":expirationDate",ui->de_alim_date->text());
        query.bindValue(":price", ui->le_alim_price->text().toDouble());
        query.bindValue(":url", ui->le_alim_url->text());
        query.bindValue(":energy", ui->le_alim_energy->text().toDouble());
        query.bindValue(":fat", ui->le_alim_fat->text().toDouble());
        query.bindValue(":carbohydrates", ui->le_alim_carbohydrates->text().toDouble());
        query.bindValue(":fibres", ui->le_alim_fibres->text().toDouble());
        query.bindValue(":protein", ui->le_alim_protein->text().toDouble());
        query.bindValue(":salt", ui->le_alim_salt->text().toDouble());
        query.bindValue(":note", ui->te_alim_note->toPlainText());


        if(!query.exec()) qDebug()<<"[SQLite] Erreur dans la modification d'un item "<<query.lastError();
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
