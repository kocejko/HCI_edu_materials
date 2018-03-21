#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    camImg = new videothread(this);

    ui->setupUi(this);

    //wartosci początkowe na sliderach
    int h1 = 6;
    int s1 = 10;
    int v1 = 10;

    int h2 = 20;
    int s2 = 250;
    int v2 = 250;

    ui->verticalSlider->setValue(h1);
    ui->verticalSlider_2->setValue(s1);
    ui->verticalSlider_3->setValue(v1);

    ui->verticalSlider_4->setValue(v2);
    ui->verticalSlider_5->setValue(h2);
    ui->verticalSlider_6->setValue(s2);

    ui->spinBox->setMaximum(3);

    //połączenie sygnał slot
    connect(camImg, SIGNAL(NewCamImg(QImage)), this, SLOT(onNewCamImg(QImage)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewCamImg(QImage qimg)
{
    ui->img->setPixmap(QPixmap::fromImage(qimg).scaled(ui->img->width(),ui->img->height()));
}

void MainWindow::on_pushButton_clicked()
{
    camImg->videoInit();
}

void MainWindow::on_verticalSlider_valueChanged(int value)
{
    camImg->hmin = value;
}

void MainWindow::on_verticalSlider_2_valueChanged(int value)
{
    camImg->smin = value;
}

void MainWindow::on_verticalSlider_3_valueChanged(int value)
{
    camImg->vmin = value;
}

void MainWindow::on_verticalSlider_5_valueChanged(int value)
{
    camImg->hmax = value;
}

void MainWindow::on_verticalSlider_6_valueChanged(int value)
{
    camImg->smax = value;
}

void MainWindow::on_verticalSlider_4_valueChanged(int value)
{
    camImg->vmax = value;
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    camImg->imgNo = arg1;
}
