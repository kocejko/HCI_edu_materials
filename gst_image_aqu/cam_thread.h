#ifndef CAM_THREAD_H
#define CAM_THREAD_H

#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QtCore>
#include <QImage>
#include <QDialog>
#include <QMessageBox>
//Step 2: add gst libs to thread header
//gstreamer
#include <glib.h>
#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/interfaces/xoverlay.h>
#include <gst/app/gstappsink.h>

//Step 5: add opencv libs
#include <QtCore>
//#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/highgui/highgui_c.h"
#include <opencv2/opencv.hpp>
#include <opencv2/photo/photo.hpp>

class cam_thread: public QThread
{
    Q_OBJECT
public:
    explicit cam_thread(QObject *parent = 0);

    //Step 3: create main function of a thread
    void run();
    void stop();
    void videoInit();

    //Step 6: define opencv variables
    cv::Mat img;
    //Step 7: function to convert cv::Mat to QImage
    QImage cvMatToQimg(const cv::Mat &mat);

    int cam_no;
    int res_w;
    int res_h;
    int rfps;
    QString dev_name;


//Step 4: ... add all the gst variables
private:
    gchar *descr;
    GstAppSink *appsink;
    // prepare the pipeline
    GstElement *pipeline;
    GstElement *input_pipe;
    //GError *error = NULL;
    gint fwidth, fheight, fdepth, fsize;
    GstElement *src;
    GstElement *capsfilter_1;
    GstElement *sink;

    //zwiazane z Gstreamerem
    void initiateGst(QString pipe_name);
    QString getPipeNameFromFile(QString fname);

    //...and mutex for thread managing
    QMutex mutex;

    //decoding gst stream
    cv::Mat yuv2rgb(CvSize ImgSize, unsigned char *ImgBuffer);
signals:
    //Step 8: Create a signal to transfer mage from thread to main GUI
    void matChange(cv::Mat);
};

#endif // CAM_THREAD_H
