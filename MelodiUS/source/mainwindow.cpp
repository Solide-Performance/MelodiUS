#include "mainwindow.h"
#include "mainwindow_ui.h"

#include <iostream>
#include <thread>

#include "detection_rythme.h"
#include "generator.h"
#include "playback.h"
#include "readwrite_wav.h"
#include "widgets/widget_note.h"



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
    // Activer les bouton a la fin de la minute??
    what_if_another_thread_fixes_it = std::thread{[&]() {
        //   rec = Record(NUM_SECONDS, SAMPLE_RATE, FRAMES_PER_BUFFER, 1);
        rec = Record(10);
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
    ui.buttonPlay.setEnabled(false);
    ui.buttonProcess.setEnabled(false);
    ui.buttonSaveLoad.setEnabled(false);
    Playback(rec);    // Faudrait avoir un son par defaut ou avoir une
    // autre boite contextuel qui donne acces au fichier


    ui.buttonRecord.setEnabled(true);
    ui.buttonStopRecord.setEnabled(true);
    ui.buttonProcess.setEnabled(true);
    ui.buttonPlay.setEnabled(
      true);    // Peut etre qui le réactive trop vite ? but idk didnt test it tib
    ui.buttonSaveLoad.setEnabled(true);
}
void MainWindow::processing()
{
    ui.buttonRecord.setEnabled(false);
    ui.buttonStopRecord.setEnabled(false);
    ui.buttonPlay.setEnabled(false);
    ui.buttonProcess.setEnabled(false);
    ui.buttonSaveLoad.setEnabled(false);

    analyse_rythme(rec);

    ui.buttonRecord.setEnabled(true);
    ui.buttonStopRecord.setEnabled(true);
    ui.buttonPlay.setEnabled(true);
    ui.buttonProcess.setEnabled(true);
    ui.buttonSaveLoad.setEnabled(true);
}
void MainWindow::saveOrLoad()
{
    ui.buttonRecord.setEnabled(false);
    ui.buttonStopRecord.setEnabled(false);
    ui.buttonPlay.setEnabled(false);
    ui.buttonProcess.setEnabled(false);
    ui.buttonSaveLoad.setEnabled(false);

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
            // Cancel was clicked. IDK what is going on here, but the best guest is nothing
            break;
        default:
            // should never be reached
            break;
    }

    ui.buttonRecord.setEnabled(true);
    ui.buttonStopRecord.setEnabled(true);
    ui.buttonPlay.setEnabled(true);
    ui.buttonProcess.setEnabled(true);
    ui.buttonSaveLoad.setEnabled(true);
}
void MainWindow::saving()
{
    ui.SaveName = QFileDialog::getSaveFileName(
      this, tr("Save Address Book"), "", tr("Address Book (*.wav);;All Files (*)"));
    ui.saveName = ui.SaveName.toStdString();
    SaveToWav(ui.saveName, rec);
    // ui.msgBoxSave.exec();
}

void MainWindow::loading()
{
    ui.FileName = QFileDialog::getOpenFileName(
      this, tr("Open File"), "/MelodiUS/more_sounds", tr("Sound Files (*.wav)"));
    ui.fileName = ui.FileName.toStdString();
    rec         = LoadFromWav(ui.fileName);
    // ui.msgBoxLoad.exec();
}
// void MainWindow::darkMode()
//{
//    ui.buttonDark.hide();
//    ui.buttonLight.show();
//    ui.groupBoxMenu.setStyleSheet("background-color:#2c2f33");
//    ui.groupBoxPartition.setStyleSheet("background-color:#2c2f33");
//}
// void MainWindow::lightMode()
//{
//    ui.buttonLight.hide();
//    ui.buttonDark.show();
//    ui.groupBoxMenu.setStyleSheet("background-color:#ffffff");
//    ui.groupBoxPartition.setStyleSheet("background-color:#ffffff");
//}

void MainWindow::on_pushButtonA_clicked()
{
    int nbs = ui.P.ajoutLigne();
    if(nbs >= 6)
    {
        ui.groupBoxPartition.resize(ui.groupBoxPartition.width(), 885 + ((nbs - 6) * 150));
    }
    NoteWidget A{&ui.groupBoxPartition, Note{NoteType::Noire, NoteValue::A4}, 100};
    A.show();
    NoteWidget B{&ui.groupBoxPartition, Note{NoteType::Blanche, NoteValue::A5}, 150};
    B.show();
    NoteWidget C{&ui.groupBoxPartition, Note{NoteType::Croche, NoteValue::C4}, 200};
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
void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Space)    // si space fuck le chien Key_U
    {
        printf("space pressed ");
        voiceKey = true;
    }
}
void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key_Space)
    {
        printf("space release ");
        voiceKey = false;
    }
}
