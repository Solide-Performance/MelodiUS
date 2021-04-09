#include "mainwindow.h"
#include "mainwindow_ui.h"

#include <iostream>
#include <thread>

#include "generator.h"
#include "playback.h"
#include "recording.h"
#include "widgets/widget_note.h"

//using namespace Ui_MainWindow;

static Recording rec{};

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(this)
{
    ui.setupUi(this);

    QObject::connect(&ui.pushButtonA, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    QObject::connect(&ui.buttonRecord, &QPushButton::clicked, this, &MainWindow::startRecord);
    QObject::connect(&ui.buttonStopRecord, &QPushButton::clicked, this, &MainWindow::stopRecord);
}
std::thread what_if_another_thread_fixes_it;
void        MainWindow::startRecord()
{
    ui.buttonRecord.hide();
    ui.buttonStopRecord.show();

    Recording_SetStopPolicy(std::function<bool()>{});

    what_if_another_thread_fixes_it = std::thread{[]() {
        rec = Record(NUM_SECONDS, SAMPLE_RATE, FRAMES_PER_BUFFER, 1);
    }};
}
void MainWindow::stopRecord()
{
    ui.buttonStopRecord.hide();
    ui.buttonRecord.show();

    Recording_SetStopPolicy([]() {
        return true;
    });

    what_if_another_thread_fixes_it.join();
    Playback(rec);
}

void MainWindow::on_pushButton_clicked()    // Label existe deja line 74 mainwindow_ui.h
{
    ui.P.ajoutLigne();
    /*
    NoteWidget A{&ui.groupBoxPartition, Note{NoteType::Noire, NoteValue::A4}, 300};
    A.show();
    */
}

MainWindow::~MainWindow()
{
}
