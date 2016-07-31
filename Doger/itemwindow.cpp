#include "itemwindow.h"
#include "ui_itemwindow.h"

Itemwindow::Itemwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Itemwindow)
{
    ui->setupUi(this);

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
}

Itemwindow::~Itemwindow()
{
    delete ui;
}

void Itemwindow::setPage(quint8 page){
    ui->stackedWidget->setCurrentIndex(page);
}

bool Itemwindow::loadFile(const QString &fileName, QLabel *label)
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


void Itemwindow::setImage(const QImage &newImage, QLabel *label)
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

void Itemwindow::on_btn_alim_loadImage_clicked()
{
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first(), ui->lbl_alim_picture)) {}
}

void Itemwindow::on_btn_alim_deleteImage_clicked()
{
    ui->lbl_alim_picture->setPixmap(QPixmap(":/images/noImage.png").scaled(100,100,Qt::KeepAspectRatio));
}

void Itemwindow::on_btn_item_loadImage_clicked()
{
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first(), ui->lbl_item_picture)) {}
}

void Itemwindow::on_btn_item_deleteImage_clicked()
{
    ui->lbl_item_picture->setPixmap(QPixmap(":/images/noImage.png").scaled(100,100,Qt::KeepAspectRatio));
}
