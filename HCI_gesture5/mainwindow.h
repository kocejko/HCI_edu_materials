#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <videothread.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    videothread *camImg;
public slots:
    void onNewCamImg(QImage qimg);

private slots:
    void on_pushButton_clicked();

    void on_verticalSlider_valueChanged(int value);

    void on_verticalSlider_2_valueChanged(int value);

    void on_verticalSlider_3_valueChanged(int value);

    void on_verticalSlider_5_valueChanged(int value);

    void on_verticalSlider_6_valueChanged(int value);

    void on_verticalSlider_4_valueChanged(int value);

    void on_spinBox_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
