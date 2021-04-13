#include "mainwindow.h"
#include "mainwindow_ui.h"

#include <iostream>
#include <thread>

#include "generator.h"
#include "playback.h"
#include "recording.h"
#include "widgets/widget_note.h"

// using namespace Ui_MainWindow;

static Recording rec{};

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(this)
{
    ui.setupUi(this);

    QObject::connect(
      &ui.pushButtonA, &QPushButton::clicked, this, &MainWindow::on_pushButtonA_clicked);

    QObject::connect(&ui.buttonRecord, &QPushButton::clicked, this, &MainWindow::startRecord);
    QObject::connect(&ui.buttonStopRecord, &QPushButton::clicked, this, &MainWindow::stopRecord);
    QObject::connect(&ui.buttonPlay, &QPushButton::clicked, this, &MainWindow::play);
    QObject::connect(&ui.buttonProcess, &QPushButton::clicked, this, &MainWindow::processing);
    QObject::connect(&ui.buttonSaveLoad, &QPushButton::clicked, this, &MainWindow::saveOrLoad);
    QObject::connect(&ui.buttonDark, &QPushButton::clicked, this, &MainWindow::darkMode);
    QObject::connect(&ui.buttonLight, &QPushButton::clicked, this, &MainWindow::lightMode);
    QObject::connect(&ui.bargraphUpdater, &QTimer::timeout, this, &MainWindow::updateBargraph);

    ui.bargraphUpdater.start(100);
}
std::thread what_if_another_thread_fixes_it;
void        MainWindow::startRecord()
{
    ui.buttonPlay.setEnabled(false);
    ui.buttonProcess.setEnabled(false);
    ui.buttonSaveLoad.setEnabled(false);
    ui.buttonRecord.hide();
    ui.buttonStopRecord.show();

    Recording_SetStopPolicy(std::function<bool()>{});
    // Activer les bouton à la fin de la minute??
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
    ui.buttonPlay.setEnabled(true);
    ui.buttonProcess.setEnabled(true);
    ui.buttonSaveLoad.setEnabled(true);
}
void MainWindow::play()
{
    ui.buttonRecord.setEnabled(false);
    ui.buttonStopRecord.setEnabled(false);
    ui.buttonProcess.setEnabled(false);
    ui.buttonSaveLoad.setEnabled(false);

    // Playback(rec);                                Faudrait avoir un son par défaut ou avoir une
    // autre boite contextuel qui donne acces au fichier


    ui.buttonRecord.setEnabled(true);
    ui.buttonStopRecord.setEnabled(true);
    ui.buttonProcess.setEnabled(true);
    ui.buttonSaveLoad.setEnabled(true);
}
void MainWindow::processing()
{
    ui.buttonRecord.setEnabled(false);
    ui.buttonStopRecord.setEnabled(false);
    ui.buttonPlay.setEnabled(false);
    ui.buttonSaveLoad.setEnabled(false);



    ui.buttonRecord.setEnabled(true);
    ui.buttonStopRecord.setEnabled(true);
    ui.buttonPlay.setEnabled(true);
    ui.buttonSaveLoad.setEnabled(true);
}
void MainWindow::saveOrLoad()
{
    ui.buttonRecord.setEnabled(false);
    ui.buttonStopRecord.setEnabled(false);
    ui.buttonPlay.setEnabled(false);
    ui.buttonProcess.setEnabled(false);

    int select = ui.SLD.exec();
    switch(select)
    {
        case QMessageBox::Save:
            saving();
            break;
        case QMessageBox::Open:
            loading();
            break;
        case QMessageBox::Cancel:
            // Cancel was clicked IDK what is going on here, but the best guest is nothing
            break;
        default:
            // should never be reached
            break;
    }

    ui.buttonRecord.setEnabled(true);
    ui.buttonStopRecord.setEnabled(true);
    ui.buttonPlay.setEnabled(true);
    ui.buttonProcess.setEnabled(true);
}
void MainWindow::saving()
{
}
void MainWindow::loading()
{
}
void MainWindow::darkMode()
{
    ui.buttonDark.hide();
    ui.buttonLight.show();
    ui.groupBoxMenu.setStyleSheet("background-color:#2c2f33");
    ui.groupBoxPartition.setStyleSheet("background-color:#2c2f33");
}
void MainWindow::lightMode()
{   
    ui.buttonLight.hide();
    ui.buttonDark.show();
    ui.groupBoxMenu.setStyleSheet("background-color:#ffffff");
    ui.groupBoxPartition.setStyleSheet("background-color:#ffffff");
}

void MainWindow::on_pushButtonA_clicked()
{
    int        nbs = ui.P.ajoutLigne();
    if(nbs >= 6)
    {
        ui.groupBoxPartition.resize(ui.groupBoxPartition.width(), 885 + ((nbs - 6) * 150));
    }
    NoteWidget A{&ui.groupBoxPartition, Note{NoteType::Noire, NoteValue::A4}, 100,0};
    A.show();
    NoteWidget B{&ui.groupBoxPartition, Note{NoteType::Blanche, NoteValue::A5}, 150,0};
    B.show();
    NoteWidget C{&ui.groupBoxPartition, Note{NoteType::Croche, NoteValue::C4}, 200,0};
    C.show();
}

void MainWindow::updateBargraph()
{
    std::array<uint8_t, 4> adcValues = FPGA::GetADC();

    ui.bargraph1.setValue(adcValues[0]);
    ui.bargraph2.setValue(adcValues[1]);
    ui.bargraph3.setValue(adcValues[2]);
    ui.bargraph4.setValue(adcValues[3]);
}

MainWindow::~MainWindow()
{
}
