#ifndef STDAFX_H
#define STDAFX_H

#include <QApplication>
#include <QWidget>
#include <QObject>
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QKeyEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDebug>
#include <vector>
#include <cmath>

#define coords "#"

const int board_grid_size = 16;
const int margin = 30;
const int chess_radius = 15;
const int mark_size = 6;
const int block_size = 40;
const int mouse_click_offset = block_size * 0.4;

#endif // STDAFX_H
