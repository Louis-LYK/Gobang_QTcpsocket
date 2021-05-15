#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) 
{
    ui->setupUi(this);
    setWindowTitle("Server");
    setFixedSize(850, 670);
    move(300, 300);
    setMouseTracking(true);
    ui->ipline->setText("192.168.0.10");
    ui->portline->setText("9978");
    init();
    connectServer();
}

void MainWindow::init() {
    turn = -1;
    isjoined = false;
    for(int i = 0; i < board_grid_size; i++) {
        std::vector<int> col_vec;
        for(int j = 0; j < board_grid_size; j++) {
            col_vec.push_back(0);
        }
        gobangmap_vec.push_back(col_vec);
    }
}

void MainWindow::putCounterChess(QString qstrn) {
    int counterX, counterY;
    counterX = qstrn.mid(0, 2).toInt();
    counterY = qstrn.mid(2, 2).toInt();
    if(gobangmap_vec[counterX][counterY] == 0) {
        gobangmap_vec[counterX][counterY] = 1;
        ruleServer(counterX, counterY);
    }
    turn = 1;
}

void MainWindow::connectServer() {
    client* p_clnt = new client();
    p_clnt->p_connect->connectToHost(QHostAddress(ui->ipline->text()), ui->portline->text().toInt());

    connect(p_clnt->p_connect, &QTcpSocket::readyRead, this, [=]() {
        isjoined = true;
        QByteArray qbarr    = p_clnt->p_connect->readAll();
        QString strn        = qbarr.data();
        QString symbolstrn  = strn.mid(0, 1);
        QString contentstrn = strn.mid(1, -1);
        if(symbolstrn == "#") { putCounterChess(contentstrn); }
        else { ui->chat_text->append(strn); }
    });

    connect(ui->send_pushButton, &QPushButton::clicked, this, [=]() {
        if( isjoined!= true) {
            QMessageBox::warning(nullptr, "forbidden", "Connection hasn't been established yet.");
            return;
        }

        QString strn = p_clnt->clnt_name + ": " + ui->input_text->toPlainText();
        p_clnt->p_connect->write(strn.toStdString().c_str());
        ui->chat_text->append("Me: " + ui->input_text->toPlainText());
        ui->input_text->clear();
    });

    connect(this, &MainWindow::sig_sendPosMsg, this, [=](QString coord) {
       QByteArray QBarr;
       QBarr.clear();
       QBarr.append(coord);
       p_clnt->p_connect->write(QBarr);
    });
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

    if( isjoined!= true) {
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
        gobangmap_vec[lastX][lastY] = -1;
        ruleClient(lastX, lastY);
    }
    QString row = QString::number(lastX, 10);
    QString column = QString::number(lastY, 10);
    if(row.length() < 2) { row = '0' + row; }
    if(column.length() < 2) { column = '0' + column; }
    emit sig_sendPosMsg(QString(coords) + row + column);
    turn = -1;
}

void MainWindow::ruleServer(int X, int Y) {
    int count = 0;
    //1.
    for(int i = 0; i < 5; i++) {
        if((Y - i < 0) || (Y - i >= board_grid_size)) { break; }
        if(gobangmap_vec[X][Y - i] != 1) { break; }
        if(gobangmap_vec[X][Y - i] == 1) { count++; }
    }
    for(int i = 1; i < 5; i++) {
        if((Y + i < 0) || (Y + i >= board_grid_size)) { break; }
        if(gobangmap_vec[X][Y + i] != 1) { break; }
        if(gobangmap_vec[X][Y + i] == 1) { count++; }
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
        if(gobangmap_vec[X - i][Y] != 1) { break; }
        if(gobangmap_vec[X - i][Y] == 1) { count++; }
    }
    for(int i = 1; i < 5; i++) {
        if((X + i < 0) || (X + i >= board_grid_size)) { break; }
        if(gobangmap_vec[X + i][Y] != 1) { break; }
        if(gobangmap_vec[X + i][Y] == 1) { count++; }
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
        if(gobangmap_vec[X - i][Y - i] != 1) { break; }
        if(gobangmap_vec[X - i][Y - i] == 1) { count++; }
    }
    for(int i = 1; i < 5; i++) {
        if((lastX + i < 0) || (X + i >= board_grid_size) || (Y + i < 0) || (Y + i >= board_grid_size)) { break; }
        if(gobangmap_vec[X + i][Y + i] != 1) { break; }
        if(gobangmap_vec[X + i][Y + i] == 1) { count++; }
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
        if(gobangmap_vec[X - i][Y + i] != 1) { break; }
        if(gobangmap_vec[X - i][Y + i] == 1) { count++; }
    }
    for(int i = 1; i < 5; i++) {
        if((X + i < 0) || (X + i >= board_grid_size) || (Y - i < 0) || (Y - i >= board_grid_size)) { break; }
        if(gobangmap_vec[X + i][Y - i] != 1) { break; }
        if(gobangmap_vec[X + i][Y - i] == 1) { count++; }
    }
    if(count >= 5) {
        QMessageBox::warning(nullptr, "gameover", "you lose!");
        clearGobangmap();
        return;
    }
    count = 0;
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
        QMessageBox::warning(nullptr, "gameover", "you win!");
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
        QMessageBox::warning(nullptr, "gameover", "you win!");
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
        QMessageBox::warning(nullptr, "gameover", "you win!");
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

