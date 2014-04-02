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

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
