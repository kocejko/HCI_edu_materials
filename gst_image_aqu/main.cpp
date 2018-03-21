#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    if (!g_thread_supported ())
        g_thread_init (NULL);
    //zainicjowanie gstreamera
    gst_init(&argc, &argv);

    QApplication a(argc, argv);
    //...sprobuj usunac nastepna linie
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit ()));
    MainWindow w;
    w.show();

    return a.exec();
}
