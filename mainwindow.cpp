#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_tf_A_valueChanged(int value)
{
    if (ui->tf_B->value() < value) ui->tf_B->setValue(value+1);
}

void MainWindow::on_tf_B_valueChanged(int value)
{
    if (ui->tf_C->value() < value) ui->tf_C->setValue(value+1);
}

void MainWindow::on_tf_C_valueChanged(int value)
{
    if (ui->tf_D->value() < value) ui->tf_D->setValue(value+1);
}

void MainWindow::on_resetTF_clicked()
{
    ui->widget->resetTransferFunction(ui->tf_A->value(),
                                      ui->tf_B->value(),
                                      ui->tf_C->value(),
                                      ui->tf_D->value());
}

void MainWindow::on_saveImage_clicked()
{
    QString filename = QFileDialog::getSaveFileName( this, "Save File", getenv( "HOME" ), "PNG Image (*.png)" );
   if( !filename.endsWith( ".png" ))
   {
   filename.append( ".png" );
   }
    QImage image = ui->widget->grabFrameBuffer(false);
    image.save(filename, "PNG");
}

void MainWindow::on_reloadShader_clicked()
{
    ui->widget->reloadShader();
}

void MainWindow::on_openVolume_clicked()
{
    fileName = QFileDialog::getOpenFileName(this, "Open Volume", getenv( "HOME" ), "RAW File (*.raw)");
    if( !fileName.endsWith( ".raw" ))
    {
    fileName.append( ".raw" );
    }
}

void MainWindow::on_loadVolume_clicked()
{
    int * size = new int[3];
    size[0] = ui->xRes->value(); size[1] = ui->yRes->value(); size[2] = ui->zRes->value();
    Eigen::Vector3f dimensions;
    float x, y, z;
    x = ui->xRes->value()*ui->xSpace->value();
    y = ui->yRes->value()*ui->ySpace->value();
    z = ui->zRes->value()*ui->zSpace->value();
    dimensions << x, y, z;
    dimensions.normalize();

    QByteArray ba = fileName.toLocal8Bit();
    const char *filename = ba.data();

    ui->widget->resetVolume(filename, size, dimensions);

    delete [] size;
}
