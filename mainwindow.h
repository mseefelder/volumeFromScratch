#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:

    void on_tf_A_valueChanged(int value);

    void on_tf_B_valueChanged(int value);

    void on_tf_C_valueChanged(int value);

    void on_resetTF_clicked();

    void on_saveImage_clicked();

    void on_reloadShader_clicked();

    void on_openVolume_clicked();

    void on_loadVolume_clicked();

private:
    Ui::MainWindow *ui;

    QString fileName;
};

#endif // MAINWINDOW_H
