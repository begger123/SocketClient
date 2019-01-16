#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QElapsedTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QString connectState(int state);

    QString connectError(int error);

private slots:
    void on_connectButton_clicked();

    void on_sendButton_clicked();

    void socketReadData();

    void socketDisconnected();


private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QGraphicsScene *sence;
    QPixmap *pm;
    QGraphicsPixmapItem *item;
    QByteArray m_buffer;
    int m_buff_size;
    int pkgCount;
    QElapsedTimer elaptimer;
};

#endif // MAINWINDOW_H
