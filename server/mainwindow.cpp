#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) 
{
    ui->setupUi(this);
    setWindowTitle("Server");
    setFixedSize(880, 670);
    move(300, 300);
    setMouseTracking(true);
    ui->ipline->setText("127.0.0.1");
    ui->portline->setText("9978");
    init();
    connectServer();
}

void MainWindow::init() {
    turn = 1;
    isjoined = false;
    for(int i = 0; i < board_grid_size; i++) {
        std::vector<int> col_vec;
        for(int j = 0; j < board_grid_size; j++) {
            col_vec.push_back(0);
        }
        gobangmap_vec.push_back(col_vec);
    }
}

void MainWindow::connectServer() {
    server* p_cnctserv = new server();        
    p_cnctserv->p_server->listen(QHostAddress::Any, ui->portline->text().toInt());

    connect(p_cnctserv->p_server, &QTcpServer::newConnection, this, [=]() {
        p_cnctserv->p_socket = p_cnctserv->p_server->nextPendingConnection();
        p_cnctserv->p_socket->write("Connection established.\n");
        if(p_cnctserv->isconnected == false) { ui->chat_text->append("Connection established.\n"); }
        p_cnctserv->isconnected = true;
        isjoined = true;

        connect(p_cnctserv->p_socket, &QTcpSocket::readyRead, this, [=](){
            QByteArray qbarr    = p_cnctserv->p_socket->readAll();
            QString strn        = qbarr.data();
            QString symbolstrn  = strn.mid(0, 1);
            QString contentstrn = strn.mid(1, -1);
            if(symbolstrn == "#") { putCounterChess(contentstrn); }
            else { ui->chat_text->append(qbarr); }
        });
    });
    connect(ui->send_pushButton, &QPushButton::clicked, this, [=]() {
        if(p_cnctserv->isconnected != true) {
            QMessageBox::warning(nullptr, "forbidden", "Connection hasn't been established yet.");
            return;
        }
        if(ui->input_text->toPlainText().isEmpty()) {
            QMessageBox::warning(nullptr, "forbidden", "Can't send empty message!");
            return;
        }

        QString strn = p_cnctserv->hostname + ": " + ui->input_text->toPlainText();
        p_cnctserv->p_socket->write(strn.toStdString().c_str());
        ui->chat_text->append("Me: " + ui->input_text->toPlainText());
        ui->input_text->clear();
    });

    connect(this, &MainWindow::sig_sendPosMsg, this, [=](QString coord) {
        QByteArray QBarr;
        QBarr.clear();
        QBarr.append(coord);
        p_cnctserv->p_socket->write(QBarr);
    });

}

void MainWindow::putCounterChess(QString qstrn) {
     int counterX, counterY;
     counterX = qstrn.mid(0, 2).toUInt();
     counterY = qstrn.mid(2, 2).toUInt();
     if(gobangmap_vec[counterX][counterY] == 0) {
         gobangmap_vec[counterX][counterY] = -1;
         ruleClient(counterX, counterY);
     }
     turn = 1;
}

void MainWindow::ruleClient(int X, int Y) {
    int count = 0;
       //1.
       for(int i = 0; i < 5; i++) {
           if((Y - i < 0) || (Y - i >= board_grid_size)) { break; }
           if(gobangmap_vec[X][Y - i] != -1) { break; }
           if(gobangmap_vec[X][Y - i] == -1) { count++; }
       }
       for(int i = 1; i < 5; i++) {
           if((Y + i < 0) || (Y + i >= board_grid_size)) { break; }
           if(gobangmap_vec[X][Y + i] != -1) { break; }
           if(gobangmap_vec[X][Y + i] == -1) { count++; }
       }
       if(count >= 5) {
           QMessageBox::warning(nullptr, "gameover", "you lose!");
           clearGobangmap();
           return;
       }
       count = 0;

       //2.
       for(int i = 0; i < 5; i++) {
           if((X - i < 0) || (X - i >= board_grid_size)) { break; }
           if(gobangmap_vec[X - i][Y] != -1) { break; }
           if(gobangmap_vec[X - i][Y] == -1) { count++; }
       }
       for(int i = 1; i < 5; i++) {
           if((X + i < 0) || (X + i >= board_grid_size)) { break; }
           if(gobangmap_vec[X + i][Y] != -1) { break; }
           if(gobangmap_vec[X + i][Y] == -1) { count++; }
       }
       if(count >= 5) {
           QMessageBox::warning(nullptr, "gameover", "you lose!");
           clearGobangmap();
           return;
       }
       count = 0;

       //3.
       for(int i = 0; i < 5; i++) {
           if((X - i < 0) || (X - i >= board_grid_size) || (Y - i < 0) || (Y - i >= board_grid_size) ) { break; }
           if(gobangmap_vec[X - i][Y - i] != -1) { break; }
           if(gobangmap_vec[X - i][Y - i] == -1) { count++; }
       }
       for(int i = 1; i < 5; i++) {
           if((lastX + i < 0) || (X + i >= board_grid_size) || (Y + i < 0) || (Y + i >= board_grid_size)) { break; }
           if(gobangmap_vec[X + i][Y + i] != -1) { break; }
           if(gobangmap_vec[X + i][Y + i] == -1) { count++; }
       }
       if(count >= 5) {
           QMessageBox::warning(nullptr, "gameover", "you lose!");
           clearGobangmap();
           return;
       }
       count = 0;

       //4.
       for(int i = 0; i < 5; i++) {
           if((X - i < 0) || (X - i >= board_grid_size) || (Y + i < 0) || (Y + i >= board_grid_size)) { break; }
           if(gobangmap_vec[X - i][Y + i] != -1) { break; }
           if(gobangmap_vec[X - i][Y + i] == -1) { count++; }
       }
       for(int i = 1; i < 5; i++) {
           if((X + i < 0) || (X + i >= board_grid_size) || (Y - i < 0) || (Y - i >= board_grid_size)) { break; }
           if(gobangmap_vec[X + i][Y - i] != -1) { break; }
           if(gobangmap_vec[X + i][Y - i] == -1) { count++; }
       }
       if(count >= 5) {
           QMessageBox::warning(nullptr, "gameover", "you lose!");
           clearGobangmap();
           return;
       }
       count = 0;
}

void MainWindow::paintEvent(QPaintEvent *evnt) {
    QPainter painter(this);
    painter.setPen(QPen(QBrush(Qt::darkGray), 2, Qt::SolidLine));

    for(int i = 0; i < board_grid_size; i++) {

        //drawing verticcal line.
        painter.drawLine(margin + block_size * i, margin, margin + block_size * i, size().height() - block_size);
        //draw horizental line.
        painter.drawLine(margin, margin + block_size * i, margin + block_size * (board_grid_size - 1), margin + block_size * i);
    }

    QBrush qbrush;
    qbrush.setStyle(Qt::SolidPattern);
    if(gobangmap_vec[mapcol][maprow] == 0) {
        qbrush.setColor(Qt::black);
        painter.setBrush(qbrush);
        painter.drawRect(margin + block_size * mapcol - mark_size / 2, margin + block_size * maprow - mark_size / 2, mark_size, mark_size);
    }

    for(int i = 0; i < board_grid_size; i++) {
        for(int j = 0; j < board_grid_size; j++) {
            if(gobangmap_vec[i][j] == 1) {
                qbrush.setColor(Qt::black);
                painter.setBrush(qbrush);
                painter.drawEllipse(margin + block_size * i - chess_radius, margin + block_size * j - chess_radius, chess_radius * 2, chess_radius * 2);
            }
            else if(gobangmap_vec[i][j] == -1) {
                qbrush.setColor(Qt::white);
                painter.setBrush(qbrush);
                painter.drawEllipse(margin + block_size * i - chess_radius, margin + block_size * j - chess_radius, chess_radius * 2, chess_radius * 2);
            }
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *evnt) {
    double x = evnt->x();
    double y = evnt->y();
    if(x < margin - chess_radius || x > margin + block_size * 15 + chess_radius ||
       y < margin - chess_radius || y > size().height() - block_size + chess_radius) { return; }

    mapcol = std::round((x - margin) / block_size);
    maprow = std::round((y - margin) / block_size);
    update();

}

void MainWindow::mousePressEvent(QMouseEvent* evnt) {
    double x = evnt->x();
    double y = evnt->y();
    if(turn == -1) { return; }

    //else if(selecturn == 1) { selecturn = 0; }

    if(isjoined != true) {
        QMessageBox::warning(nullptr, "forbidden", "Connection hasn't been established yet.");
        return;
    }
    if((x < margin - mark_size * 2) || (x > margin + block_size * 15 + mark_size * 2) ||
       (y < margin - mark_size * 2) || (y > margin + block_size * 15 + mark_size * 2)) {
        return;
    }
    lastX = std::round((x - margin) / block_size);
    lastY = std::round((y - margin) / block_size);
    if(gobangmap_vec[lastX][lastY] == 0) {
        gobangmap_vec[lastX][lastY] = 1;
        rule();
    }
    QString Row = QString::number(lastX, 10);
    if (Row.length() < 2) { Row = '0' + Row; }//使数字变成两位数，如：02 12
    QString Column = QString::number(lastY, 10);
    if (Column.length() < 2) { Column = '0' + Column; } //使数字变成两位数，如：02 12
    emit sig_sendPosMsg(QString(coords) + Row + Column);
    turn = -1;
}

void MainWindow::rule() {
    int count = 0;
    //1.
    for(int i = 0; i < 5; i++) {
        if((lastY - i < 0) || (lastY - i >= board_grid_size)) { break; }
        if(gobangmap_vec[lastX][lastY - i] != 1) { break; }
        if(gobangmap_vec[lastX][lastY - i] == 1) { count++; }
    }
    for(int i = 1; i < 5; i++) {
        if((lastY + i < 0) || (lastY + i >= board_grid_size)) { break; }
        if(gobangmap_vec[lastX][lastY + i] != 1) { break; }
        if(gobangmap_vec[lastX][lastY + i] == 1) { count++; }
    }
    if(count >= 5) {
        QMessageBox::warning(nullptr, "gameover", "you win!");
        clearGobangmap();
        return;
    }
    count = 0;

    //2.
    for(int i = 0; i < 5; i++) {
        if((lastX - i < 0) || (lastX - i >= board_grid_size)) { break; }
        if(gobangmap_vec[lastX - i][lastY] != 1) { break; }
        if(gobangmap_vec[lastX - i][lastY] == 1) { count++; }
    }
    for(int i = 1; i < 5; i++) {
        if((lastX + i < 0) || (lastX + i >= board_grid_size)) { break; }
        if(gobangmap_vec[lastX + i][lastY] != 1) { break; }
        if(gobangmap_vec[lastX + i][lastY] == 1) { count++; }
    }
    if(count >= 5) {
        QMessageBox::warning(nullptr, "gameover", "you win!");
        clearGobangmap();
        return;
    }
    count = 0;

    //3.
    for(int i = 0; i < 5; i++) {
        if((lastX - i < 0) || (lastX - i >= board_grid_size) || (lastY - i < 0) || (lastY - i >= board_grid_size) ) { break; }
        if(gobangmap_vec[lastX - i][lastY - i] != 1) { break; }
        if(gobangmap_vec[lastX - i][lastY - i] == 1) { count++; }
    }
    for(int i = 1; i < 5; i++) {
        if((lastX + i < 0) || (lastX + i >= board_grid_size) || (lastY + i < 0) || (lastY + i >= board_grid_size)) { break; }
        if(gobangmap_vec[lastX + i][lastY + i] != 1) { break; }
        if(gobangmap_vec[lastX + i][lastY + i] == 1) { count++; }
    }
    if(count >= 5) {
        QMessageBox::warning(nullptr, "gameover", "you win!");
        clearGobangmap();
        return;
    }
    count = 0;

    //4.
    for(int i = 0; i < 5; i++) {
        if((lastX - i < 0) || (lastX - i >= board_grid_size) || (lastY + i < 0) || (lastY + i >= board_grid_size)) { break; }
        if(gobangmap_vec[lastX - i][lastY + i] != 1) { break; }
        if(gobangmap_vec[lastX - i][lastY + i] == 1) { count++; }
    }
    for(int i = 1; i < 5; i++) {
        if((lastX + i < 0) || (lastX + i >= board_grid_size) || (lastY - i < 0) || (lastY - i >= board_grid_size)) { break; }
        if(gobangmap_vec[lastX + i][lastY - i] != 1) { break; }
        if(gobangmap_vec[lastX + i][lastY - i] == 1) { count++; }
    }
    if(count >= 5) {
        QMessageBox::warning(nullptr, "gameover", "you win!");
        clearGobangmap();
        return;
    }
    count = 0;
}

void MainWindow::clearGobangmap() {
    for(int i = 0; i < board_grid_size; i++) {
        for(int j = 0; j < board_grid_size; j++) {
            gobangmap_vec[i][j] = 0;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

