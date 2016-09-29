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
    ui->s_alim_energy->setEnabled(false);
    ui->s_alim_carbohydrates->setEnabled(false);
    ui->s_alim_fat->setEnabled(false);
    ui->s_alim_protein->setEnabled(false);
    ui->s_alim_fibres->setEnabled(false);
    ui->s_alim_salt->setEnabled(false);
    ui->de_item_date->setDate(QDate::currentDate());
    loadDatabase(index);
    if(index!=0)whatToDo=TO_UPDATE; id_item=index;
}

ItemWindow::~ItemWindow()
{
    delete ui;
}

void ItemWindow::loadDatabase(int index){
    sqlite->getCategoryBrand(CATEGORY, &idCategories, ui->cb_item_categorie);
    sqlite->getCategoryBrand(BRAND, &idBrand, ui->cb_item_brand);
    ui->cb_item_brand->insertItem(0,"Nouvelle");

    if(index!=0){
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

}

void ItemWindow::setPage(quint8 page){
    ui->stackedWidget->setCurrentIndex(page);
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

void ItemWindow::on_btn_item_save_clicked()
{
    if(whatToDo==TO_INSERT){
        QString Squery = "INSERT INTO Items VALUES (NULL , :id_category, :id_brand, :reference, :weight, :quantity, :volume, :dateOfAcquisition, :price, :desired, :url_manufacturer, :url_RL, :note)";
        QSqlQuery query;

        query.prepare(Squery);
        query.bindValue(":id_category", idCategories.at(ui->cb_item_categorie->currentIndex()-1));
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
    }else if (whatToDo==TO_UPDATE){
        QString Squery="UPDATE Items SET reference=:reference,"
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

        if(!query.exec()) qDebug()<<"[SQLite] Erreur dans la modification d'un item ";
    }
    close();


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
    if(!ui->cb_item_brand->findText(ui->le_item_brand->text())){
        ui->le_item_brand->hide();
        ui->btn_item_newBrand->hide();

    }else{
        QMessageBox::warning(this, QGuiApplication::applicationDisplayName(),
                                 tr("Une marque portant le nom de %1 existe déjà.")
                                 .arg(ui->le_item_brand->text()));
    }

}
