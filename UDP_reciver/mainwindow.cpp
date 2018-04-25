#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(42001);

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(ReadAndProcessData()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::ReadAndProcessData()
{
//TU WPISUJEMY POLECENIE ODBIERANIA DANYCH
    QByteArray Buffer;
    do{
        Buffer.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpSocket->readDatagram(Buffer.data(), Buffer.size(), &sender, &senderPort);
    }
    while(udpSocket->hasPendingDatagrams());

    ui->textEdit->append(Buffer.data());

//TU WPISUJEMY POLECENIA PRZETWARZANIA DANYCH

}
