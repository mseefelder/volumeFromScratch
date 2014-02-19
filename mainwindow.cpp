#include "mainwindow.h"
#include "ui_mainwindow.h"

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

void MainWindow::on_layer_valueChanged(const QString &arg1)
{
    cout << ui->layer->value() << endl;
    ui->widget->setLayer(ui->layer->value());
    ui->widget->update();
}
