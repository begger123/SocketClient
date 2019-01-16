#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QImage>
#include <QtMath>
#include <QHostAddress>

#define BAND_IP_ADDRESS    "192.168.2.1"
#define SERVER_IP_ADDRESS  "192.168.1.100"
#define PORT 5001

#if defined(Q_OS_WIN32)
#define SOCKET_FILE "D:/socket.bmp"
#elif defined(Q_OS_LINUX)
#define SOCKET_FILE "/home/top/Pictures/socket.bmp"
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);

    socket->bind(QHostAddress(BAND_IP_ADDRESS),PORT);
    QObject::connect(socket,&QTcpSocket::readyRead,this,&MainWindow::socketReadData);
    ui->IPText->setText(SERVER_IP_ADDRESS);
    ui->portText->setText(QString::number(PORT));
    m_buff_size = 0;
    sence = new QGraphicsScene();
    pm = new QPixmap();
    item = new QGraphicsPixmapItem();
    pkgCount = 0;
}

MainWindow::~MainWindow()
{
    delete this->socket;
    delete sence;
    delete pm;
    delete item;
    delete ui;
}


void MainWindow::on_connectButton_clicked()
{
    //socket->abort();

    QString IP(ui->IPText->text());
    quint16 port = ui->portText->text().toUInt();

    qDebug()<<"ip:"<<IP<<"port:"<<port;
    QString s1,s2;
    s1 = connectState(socket->state());
    socket->connectToHost(IP,port);
    bool socketState = socket->isValid();
    int error = socket->error();
    QString s3 = connectState(socket->state());
    if(!socket->waitForConnected(30000))
    {
        s2 = connectState(socket->state());
        QString s = "Connect server failed";
        //s += "  :preState:" + s1 + "  connectState:" + s3 + "  currentState:" + s2;
        s += " :" + connectError(error);
        QMessageBox::warning(this,"warning",s);
    }
    else
    {
        QMessageBox::warning(this,"Tip",QString("Connect to server success."));
    }
}

void MainWindow::on_sendButton_clicked()
{
    qint64 wr = socket->write(ui->sendMsgText->toPlainText().toLatin1());
    bool flush = socket->flush();
    if(wr != -1 && flush == 1)
    {
        if( wr == 0)
            QMessageBox::warning(this,"warning",QString("socket write return 0"));

        QMessageBox::warning(this,"warning",QString("Send success%1").arg(wr));
    }
}

void MainWindow::socketReadData()
{
    //QByteArray buffer;
   // buffer = socket->readAll();
    //ui->receiveMsgText->setText(buffer);
//    QGraphicsScene sence;
//    QGraphicsView view(&sence);
//    QPixmap pm;
//    pm.loadFromData(buffer,"png");
//    QGraphicsPixmapItem item(pm);
//    sence.addItem(&item);
//    view.show();
    if(!elaptimer.isValid())
        elaptimer.start();

    if(m_buff_size == 0){
       m_buffer = socket->readAll();
       if(m_buffer.isEmpty())
       {
           qDebug("Socket buffer is empty");
           return;
       }
       int digit = m_buffer.mid(0,2).toInt(); //前两位表示数据大小一共占多少位
       m_buff_size = m_buffer.mid(2,digit).toInt();
       m_buffer.remove(0,digit + 2);
       pkgCount = 1;
       qDebug("read buff size %d-dgit:%d",m_buff_size,digit);
    }
    else
    {

        m_buffer.append(socket->readAll());
        //qDebug("receive buff size is %d",m_buffer.size());
        pkgCount += 1;
        if(m_buff_size == m_buffer.size())
        {
            qDebug("buff size is %d package count is %d",m_buff_size,pkgCount);
            qDebug() << "Receive image finished:" << (m_buff_size/qPow(1024,2))/(elaptimer.elapsed()/1000.0) << "MB/s";
            elaptimer.invalidate();

            QImage receiveImg;
            receiveImg.loadFromData(m_buffer);
            if (receiveImg.isNull())
            {
                qDebug("The image is null. Something failed.%d",m_buffer.size());
            }
            else
            {
                    /*pm->loadFromData(m_buffer,"bmp");
                    item->setPixmap(*pm);
                    sence->addItem(item);
                    ui->graphicsView->setScene(sence);
                    ui->graphicsView->show();*/
                receiveImg.save(SOCKET_FILE);
            }
            m_buffer.clear();
            m_buff_size = 0;
        }

    }
}

void MainWindow::socketDisconnected()
{

}


QString MainWindow::connectState(int state)
{
    QString str;
    switch (state) {
    case QTcpSocket::UnconnectedState:
        str = "UnconnectedState";
        break;

    case QTcpSocket::HostLookupState:
        str = "HostLookupState";
        break;

    case QTcpSocket::ConnectingState:
        str = "ConnectingState";
        break;

    case QTcpSocket::ConnectedState:
        str = "ConnectedState";
        break;

    case QTcpSocket::BoundState:
        str = "BoundState";
        break;

    case QTcpSocket::ListeningState:
        str = "ListeningState";
        break;

    case QTcpSocket::ClosingState:
        str = "ClosingState";
        break;

    default:
        break;
    }

    return str;
}

QString MainWindow::connectError(int error)
{
    QString str;
    switch (error) {
    case QTcpSocket::ConnectionRefusedError:
        str = "ConnectionRefusedError";
        break;

    case QTcpSocket::RemoteHostClosedError:
        str = "RemoteHostClosedError";
        break;

    case QTcpSocket::HostNotFoundError:
        str = "HostNotFoundError";
        break;

    case QTcpSocket::SocketAccessError:
        str = "SocketAccessError";
        break;

    case QTcpSocket::SocketResourceError:
        str = "SocketResourceError";
        break;

    case QTcpSocket::SocketTimeoutError:
        str = "SocketTimeoutError";
        break;

    case QTcpSocket::DatagramTooLargeError:
        str = "DatagramTooLargeError";
        break;

    case QTcpSocket::NetworkError:
        str = "NetworkError";
        break;

    case QTcpSocket::AddressInUseError:
        str = "AddressInUseError";
        break;

    case QTcpSocket::SocketAddressNotAvailableError:
        str = "SocketAddressNotAvailableError";
        break;

    case QTcpSocket::UnsupportedSocketOperationError:
        str = "UnsupportedSocketOperationError";
        break;

    case QTcpSocket::UnfinishedSocketOperationError:
        str = "UnfinishedSocketOperationError";
        break;

    case QTcpSocket::ProxyAuthenticationRequiredError:
        str = "ProxyAuthenticationRequiredError";
        break;

    case QTcpSocket::SslHandshakeFailedError:
        str = "SslHandshakeFailedError";
        break;

    case QTcpSocket::ProxyConnectionRefusedError:
        str = "ProxyConnectionRefusedError";
        break;

    case QTcpSocket::ProxyConnectionClosedError:
        str = "ProxyConnectionClosedError";
        break;

    case QTcpSocket::ProxyConnectionTimeoutError:
        str = "SocketTimeoutError";
        break;

    case QTcpSocket::ProxyNotFoundError:
        str = "ProxyNotFoundError";
        break;

    case QTcpSocket::ProxyProtocolError:
        str = "ProxyProtocolError";
        break;

    case QTcpSocket::OperationError:
        str = "OperationError";
        break;

    case QTcpSocket::SslInternalError:
        str = "SslInternalError";
        break;

    case QTcpSocket::SslInvalidUserDataError:
        str = "SslInvalidUserDataError";
        break;

    case QTcpSocket::TemporaryError:
        str = "TemporaryError";
        break;

    case QTcpSocket::UnknownSocketError:
        str = "UnknownSocketError";
        break;

    default:
        break;
    }

    return str;
}
