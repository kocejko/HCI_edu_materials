#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <math.h>
#include <stdio.h>
//Step 2: add gst libs to thread header
//gstreamer
////gstreamer
//#include <glib.h>
//#include <gst/gst.h>
//#include <gst/video/video.h>
//#include <gst/interfaces/xoverlay.h>
//#include <gst/app/gstappsink.h>
//#include <omp.h>

//thred
#include <cam_thread.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    cam_thread *cam1;

private:
    Ui::MainWindow *ui;

    //Step 10: Create a slot for our signal
public slots:
    void onMatChange(cv::Mat mat);
};

#endif // MAINWINDOW_H
