#include "mainwindow.h"
#include "mainwindow_ui.h"

#include <iostream>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}
void MainWindow::on_pushButton_clicked()    // Label existe déja line 74 mainwindow_ui.h
{
    int y_do  = 180;
    int y_re  = 170;
    int y_mi  = 160;
    int y_fa  = 150;
    int y_sol = 140;
    int y_la  = 130;
    int y_si  = 120;


    static QLabel* label_A = nullptr;
    delete label_A;
    label_A = new QLabel(ui->centralwidget);

    printf("ceci est le bouton A");
    label_A->setObjectName(QString::fromUtf8("label"));
    label_A->setGeometry(QRect(300, y_la, 20, 80));
    QPixmap pixmap = (QString::fromUtf8("../../MelodiUS/meloiusImage/noir.png"));
    label_A->setPixmap(pixmap);
    label_A->setMask(pixmap.mask());
    /*label_A->setText(QCoreApplication::translate(
      "MainWindow", "<html><head/><body><p>Bon Matin</p></body></html>", nullptr));*/

    std::cout << pixmap.width() << std::endl;


    label_A->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
