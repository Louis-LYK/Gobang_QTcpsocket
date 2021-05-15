#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "stdafx.h"
#include "server.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    void init();
    void paintEvent(QPaintEvent* evnt) override;
    void mouseMoveEvent(QMouseEvent* evnt) override;
    void mousePressEvent(QMouseEvent* evnt) override;
    void connectServer();
    void rule();
    void ruleClient(int, int);
    void putCounterChess(QString);
    void clearGobangmap();
    ~MainWindow();

signals:
    void sig_sendPosMsg(QString);

private:
    int turn;
    bool isjoined;
    double mapcol, maprow;
    int lastX, lastY;
    std::vector<std::vector<int> >gobangmap_vec;

private:
    Ui::MainWindow *ui;
    server* p_ui;
};
#endif // MAINWINDOW_H
