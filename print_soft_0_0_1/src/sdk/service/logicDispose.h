#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_printDeviceMoudle.h"

class printDeviceMoudle : public QMainWindow
{
    Q_OBJECT

public:
    printDeviceMoudle(QWidget *parent = nullptr);
    ~printDeviceMoudle();

private:
    Ui::printDeviceMoudleClass ui;
};
