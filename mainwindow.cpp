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
