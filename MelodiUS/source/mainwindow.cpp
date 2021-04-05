#include "mainwindow.h"
#include "mainwindow_ui.h"

#include <iostream>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}
void MainWindow::on_pushButton_clicked()    // Label existe deja line 74 mainwindow_ui.h
{
    static QLabel* label_A = new QLabel(ui->centralwidget);

    QPixmap pixmap = (QString::fromUtf8("../../MelodiUS/meloiusImage/noir.png"));

    int y_do  = 180;
    int y_re  = 170;
    int y_mi  = 160;
    int y_fa  = 150;
    int y_sol = 140;
    int y_la  = (ui->line_4->y() + ui->line_3->y()) / 2 - pixmap.height() + 5;
    int y_si  = 120;

    std::cout << "ceci est le bouton A" << std::endl;
    label_A->setObjectName(QString::fromUtf8("label"));
    label_A->setGeometry(QRect(300, y_la, 20, 80));
    label_A->setPixmap(pixmap);
    label_A->setMask(pixmap.mask());

    label_A->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
