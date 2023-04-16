#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->myImage->setStyleSheet("image:url(test.png); ");


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    //https://stackoverflow.com/questions/44385978/qt-how-to-show-a-qlabel-in-fullscreen-on-my-laptop
    ui->myImage->setParent(nullptr);
    //ui->myImage->setText("going full screen");
    ui->myImage->showMaximized();
    ui->myImage->showFullScreen();
}

