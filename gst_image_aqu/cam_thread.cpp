#include "cam_thread.h"

cam_thread::cam_thread(QObject *parent): QThread(parent)
{

}

void cam_thread::run()
{
    while (true) {
        QMutexLocker lock(&mutex);


        if( !gst_app_sink_is_eos(appsink) )
        {
            gint fwidth, fheight, fdepth;
            GstBuffer *buffer = gst_app_sink_pull_buffer(appsink);
            uint8_t* data = (uint8_t*)GST_BUFFER_DATA(buffer);

            GstCaps *capss = gst_buffer_get_caps(buffer);
            GstStructure *caps_struct = gst_caps_get_structure(capss,0);
            //GstVideoFormat *vformat;
            gst_structure_get_int(caps_struct, "width", &(fwidth));
            gst_structure_get_int(caps_struct, "height", &(fheight));
            gst_structure_get_int(caps_struct, "depth", &fdepth);
            //gst_video_format_parse_caps(capss, vformat, NULL, NULL);
            int xwidth = 0, xheight = 0;

            GstVideoFormat xformat;
            if (!gst_video_format_parse_caps(capss, &xformat, &xwidth, &xheight)) {
                gst_caps_unref(capss);
                //qDebug()<<gst_video_format_parse_caps(capss, &xformat, &xwidth, &xheight);
            }
            gst_caps_unref(capss);
            //qDebug()<<"Jest "<<gst_video_format_parse_caps(capss, &xformat, &xwidth, &xheight);

            //konwersja
            cv::Size ImageSize;
            ImageSize.height = fheight;
            ImageSize.width = fwidth;

            cv::Mat yuv = yuv2rgb(ImageSize, (uchar*)data);
            img = cv::Mat(ImageSize, CV_8UC3);
            cv::cvtColor(yuv, img, CV_YUV2RGB);

            //zwolnienie bufora
            gst_buffer_unref(buffer);
        }

        bool imgToProcess = true;

        if(img.empty() == true)
        {
            qDebug()<<"no image";
            return;
        }

        else
        {
            //imgToProcess = true;
        }
        if(imgToProcess)
        {
            //send image to main thread
            emit matChange(img);
        }
    }

}

void cam_thread::videoInit()
{

    //QString pipe_name = getPipeNameFromFile("pipeName");

    //Create a pipe for gst
    QString pipe_name = "v4l2src device="+dev_name+" ! video/x-raw-yuv, width="+QString::number(res_w)+", height="+QString::number(res_h)+", framerate="+QString::number(rfps)+"/1"+" ! appsink name=asink sync=true";
    initiateGst(pipe_name);

    GstStateChangeReturn sret = gst_element_set_state (input_pipe, GST_STATE_PLAYING);
    if (sret == GST_STATE_CHANGE_FAILURE) {
        gst_element_set_state (input_pipe, GST_STATE_NULL);
        gst_object_unref (input_pipe);
        // Exit application
        //QTimer::singleShot(0, QApplication::activeWindow(), SLOT(quit()));
        qDebug()<<"cos sie dzieje";
    }

    this->start();
}

void cam_thread::initiateGst(QString pipe_name)
{
    qDebug()<<"OK1";
    //The pipelines
    //sprintf(input_str,"v4l2src device=/dev/video0 ! video/x-raw-yuv, framerate=10/1, width=320, height=240 ! ffmpegcolorspace ! xvimagesink");
    //sprintf(entrada_str,"qtkitvideosrc ! video/x-raw-yuv,width=640,height=480 ! ffmpegcolorspace "
    //                      "! video/x-raw-rgb,format=RGB3,width=640,height=480 ! appsink name=sink");

    GError *error = NULL;
    //input_pipe = gst_parse_launch("v4l2src device=/dev/video0 ! video/x-raw-yuv, width=640, height=480 ! appsink name=asink sync=true",&error);
    input_pipe = gst_parse_launch(pipe_name.toLocal8Bit().constData(),&error);
    //input_pipe = gst_parse_launch("v4l2src device=/dev/video1 always-copy=false ! video/x-raw-yuv, width=320, height=240, framerate=20/1, bpp=24 ! ffmpegcolorspace ! appsink name=asink sync=true",&error);
    //input_pipe = gst_parse_launch("videotestsrc ! video/x-raw-yuv, format=(fourcc)YUY2;video/x-raw-yuv,format=(fourcc)YV12 ! appsink name=asink sync=true",&error);

    sink = gst_bin_get_by_name (GST_BIN (input_pipe), "asink");

    appsink = (GstAppSink *)sink;
    gst_app_sink_set_max_buffers ( appsink, 2); // limit number of buffers queued
    gst_app_sink_set_drop( appsink, true ); // drop old buffers in queue when ful

    gst_app_sink_set_emit_signals(appsink, true);
    //gst_app_sink_set_max_buffers(appsink, 1);

    qDebug()<<"OK2";

    if (error != NULL) {
        g_print ("could not construct pipeline: %s\n", error->message);
        g_error_free (error);
        exit (-1);
      }

qDebug()<<"OK3";

    // prepare the pipeline
    pipeline = gst_pipeline_new ("xvoverlay");

    src = gst_element_factory_make ("v4l2src", NULL);
    g_object_set(G_OBJECT(src),"device",dev_name.toLocal8Bit().constData(),NULL);
qDebug()<<"OK4";
    //capsfilter_1 = gst_element_factory_make("capsfilter","cfilter");
    //g_object_set(G_OBJECT(capsfilter_1),"caps",gst_caps_from_string("video/x-raw-yuv, width=(int)320, height=(int)240, framerate=(fraction)20/1"),NULL);
    qDebug()<<"OK5";
}

QImage cam_thread::cvMatToQimg(const cv::Mat &mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

cv::Mat cam_thread::yuv2rgb(CvSize ImgSize, unsigned char *ImgBuffer)
{
    cv::Mat img2 = cv::Mat(ImgSize.height, ImgSize.width, CV_8UC3);

    for(int i = 0, j=0; i < ImgSize.width * ImgSize.height * 3; i+=6, j+=4)
    {
        img2.data[i] = ImgBuffer[j];            //Y
        img2.data[i+1] = ImgBuffer[j+1];        //U
        img2.data[i+2] = ImgBuffer[j+3];        //V
        img2.data[i+3] = ImgBuffer[j+2];        //Y'
        img2.data[i+4] = ImgBuffer[j+1];        //U
        img2.data[i+5] = ImgBuffer[j+3];        //V
    }

    return img2;
}
