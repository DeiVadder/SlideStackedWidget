#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("QMainWindow{background-color:white;}\
                        cStackedWidget{background-color:red;}\
                        QLabel{background-color:blue;}");
//    this->setStyleSheet("background-color:red;");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->slideinNext();
    if(ui->stackedWidget->currentIndex() == ui->stackedWidget->count()-1)
        ui->stackedWidget->slideinIndex(0);
}
