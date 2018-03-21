#include "videothread.h"

videothread::videothread(QObject *parent) :
    QThread(parent)
{
    imgNo = 1;

}
//INICJALIZACJA WIDEO AVI
void videothread::videoInit()
{
    //capture.open(fname.toStdString());

    capture.open(0);
    if(capture.isOpened() == false)
    {
        qDebug()<<"No camera2";
        return;
    }

    this->start();
}

//Star naszego watku
void videothread::run()
{
    while(1)
    {
        //QMutex mutex;
        //mutex.lock();

        //mutex.unlock();
        capture.read(input_img);

        if(input_img.empty() == true) return;

        //TU WPISUJEMY WSZYSTKO CO DZIEJE SIE Z NASZYM OBRAZEM Z KAMERY

        //progujemy obraz kolorowy, zostawiamy tylko wybrany kolor
        cv::Mat skinMask;
        adaptivSkinDet(input_img, skinMask);

        //cv::GaussianBlur(skinMask, skinMask, cv::Size(7,7), 5, 5);

        //detekcja i zamkniecie konturow
        //detekcja konturow
        std::vector<std::vector<cv::Point> > contours;
        cv::Mat m = skinMask.clone();
        cv::Mat drawing = input_img.clone();
        cv::findContours(m, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

        //Zamknięcie konturow
        //cv::drawContours(skinMask, contours, -1, CV_RGB(255,255,255), -1);



        double maxS = 0;
        int ci = 1;
        cv::Rect rect;
        for (int i = 0; i < contours.size(); i++)
        {
            double area = cv::contourArea(contours[i]);    // Blob area


            if(area>maxS)
            {
                maxS = area;
                ci = i;
            }
        }

        //estymacja powierzchni konturu prostokątem
        if(maxS>0)
        {
            rect = cv::boundingRect(contours[ci]); // Bounding box
            cv::rectangle(skinMask, rect, CV_RGB(255,255,255), 3, 8, 0);
            cv::rectangle(drawing, rect, CV_RGB(255,255,255), 3, 8, 0);
        }

        //detekcja wypukłości i wklęsłości konturu
        std::vector<std::vector<cv::Point> >hull( contours.size() );
        std::vector<std::vector<int> >hullI( contours.size() );
        std::vector<cv::Vec4i> defects;

        std::vector<cv::Point> hull_points;
        std::vector<cv::Point> def_points;

        if(!contours.empty())
        {

            cv::convexHull(cv::Mat(contours[ci]), hull[ci], false, true);
            cv::convexHull(cv::Mat(contours[ci]), hullI[ci], false);
            cv::convexityDefects(cv::Mat(contours[ci]), hullI[ci], defects);

            for(int i=0; i<hull[ci].size();i++)
            {
                //int ind = hull[largest_c_id][i];
                hull_points.push_back(hull[ci][i]);
                //cv::circle(drawing, hull_points[i],15,cv::Scalar( 255, 0, 0 ), 3);
                //cv::circle(skinMask, hull_points[i],15,cv::Scalar( 255, 255, 255 ), 3);

            }
            int nm1 = 10;
            for(int i=0;i<hull_points.size()-1;i++)
            {
                //cv::circle(drawing,def_points[i],15,cv::Scalar(0,0,255),-1);
                double d = dist(hull_points[i], hull_points[i+1]);

                if(d>=nm1)
                {
                    cv::circle(skinMask, hull_points[i],15,cv::Scalar( 255, 255, 255 ), 3);
                    cv::circle(drawing, hull_points[i],15,cv::Scalar( 0, 255, 0 ), 3);

                    //nm1 = d;
                    //nm1_i = i;
                }
            }

            drawContours( drawing, hull, ci, CV_RGB(0,0,255), 3);

            int depth_t = 0;
            for(int i=0;i<defects.size();i++)
            {
                int ind = defects[i][2];
                float depth = defects[i][3];

                if(depth>3)
                {
                    depth_t+=1000;
                }
                else
                {
                    depth_t-=100;
                }

                    def_points.push_back(contours[ci][ind]);
//                    cv::circle(drawing,contours[largest_c_id][ind],15,cv::Scalar(0,0,255),-1);
                    cv::circle(skinMask,def_points[i],15,cv::Scalar(255,255,255),-1);
                    cv::circle(drawing,def_points[i],15,cv::Scalar(255,0,0),-1);

            }

        }

        //prosta detekcja gestów
        std::vector<double> dist_hull;
        std::vector<double> dist_deff;

        double avg_dist_hull = 0, avg_dist_deff = 0;

        cv::Point hand_c;
        hand_c.x = rect.x + rect.width/2;
        hand_c.y = rect.y + rect.height/2;
        cv::circle(drawing, hand_c, 25, CV_RGB(255,0,0), 8, -1);


        for(int i=0;i<hull_points.size();i++)
        {
            double d = dist(hand_c, hull_points[i]);
            dist_hull.push_back(d);
            avg_dist_hull +=d;
        }

        for(int i=0;i<def_points.size();i++)
        {
            double d = dist(hand_c, def_points[i]);
            dist_deff.push_back(d);
            avg_dist_deff +=d;
        }

        avg_dist_hull = avg_dist_hull/dist_hull.size();
        avg_dist_deff = avg_dist_deff/dist_deff.size();

        if(1.1*avg_dist_deff<avg_dist_hull)
        {
            //OPEN
            putText(drawing, "OPEN", cv::Point(100, 100), 2, 4, CV_RGB(255, 255, 0), 4, 8, false);
            putText(input_img, "OPEN", cv::Point(100, 100), 2, 4, CV_RGB(255, 255, 0), 4, 8, false);
        }
        else
        {
            //CLOSE
            putText(drawing, "CLOSED", cv::Point(100, 100), 2, 4, CV_RGB(0, 0, 0), 4, 8, false);
            putText(input_img, "CLOSED", cv::Point(100, 100), 2, 4, CV_RGB(0, 0, 0), 4, 8, false);
        }


        //...

        //Konwersja do QImage
        QImage qimginput;
        switch (imgNo) {
        case 1:
            qimginput = qinputimage(input_img);
            break;
        case 2:
            qimginput = qinputimage(drawing);
            break;
        case 3:
            qimginput = qinputimage(skinMask);
        default:
            break;
        }



        //Emisja sygnału do watku głównego
        emit NewCamImg(qimginput);

    }
}
void videothread::adaptivSkinDet(cv::Mat img, cv::Mat &mask)
{
    cv::Mat hsvimg;
    cv::cvtColor(img,hsvimg, CV_BGR2HSV);

    //rozmycie gaussowskie
    cv::GaussianBlur(hsvimg, hsvimg, cv::Size(17,17), 5, 5);

    //progowanie koloru skory
    cv::inRange(hsvimg, cv::Scalar(hmin, smin, vmin), cv::Scalar(hmax,smax,vmax), mask);

    //erozja
    int erode_elem = 0;
    int erode_type;
    int erode_size = 3;
    if( erode_elem == 0 )
    {
        erode_type = cv::MORPH_RECT;
    }
    else if( erode_elem == 1 )
    {
        erode_type = cv::MORPH_CROSS;
    }
    else if( erode_elem == 2)
    {
        erode_type = cv::MORPH_ELLIPSE;
    }

    //dylacja
    int dilation_elem = 0;
    int dilation_type;
    int dilation_size = 7;
    if( dilation_elem == 0 )
    {
        dilation_type = cv::MORPH_RECT;
    }
    else if( dilation_elem == 1 )
    {
        dilation_type = cv::MORPH_CROSS;
    }
    else if( dilation_elem == 2)
    {
        dilation_type = cv::MORPH_ELLIPSE;
    }

    cv::Mat element1 = cv::getStructuringElement( erode_type,
                                           cv::Size( 2*erode_size + 1, 2*erode_size+1 ),
                                           cv::Point( erode_size, erode_size ) );
    cv::Mat element = cv::getStructuringElement( dilation_type,
                                           cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                           cv::Point( dilation_size, dilation_size ) );
    /// Apply the dilation operation
    cv::erode(mask, mask,element1);
    cv::dilate( mask, mask, element );

    //cv::inRange(hsvimg, cv::Scalar(hmin, 15, 15), cv::Scalar(hmax,250,250), mask);
}


QImage videothread::qinputimage(const cv::Mat &mat)
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
double videothread::dist(cv::Point p1, cv::Point p2)
{
    double dist = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
    return dist;
}

