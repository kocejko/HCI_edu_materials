#include "mainwindow.h"
#include "ui_mainwindow.h"

//If we want to query the cv::Mat (in signal slot relation) object it must be registered
Q_DECLARE_METATYPE(cv::Mat)


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //If we want to query the cv::Mat (in signal slot relation) object it must be registered
    qRegisterMetaType< cv::Mat >("cv::Mat");

    ui->setupUi(this);

    //declaer our cma thread object
    cam1 = new cam_thread(this);

    //defain initial parameters
    //cam1->cam_no = 1;
    cam1->dev_name = "/dev/video0";
    cam1->res_w = 640;
    cam1->res_h = 480;
    cam1->rfps = 30;

    //Step 12: Connect our slot and signal
    connect(cam1, SIGNAL(matChange(cv::Mat)), this, SLOT(onMatChange(cv::Mat)));

    cam1->videoInit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onMatChange(cv::Mat mat)
{
    QImage cam_img = cam1->cvMatToQimg(mat);
    ui->label->setPixmap(QPixmap::fromImage(cam_img).scaled(ui->label->width(),ui->label->height()));;
}
