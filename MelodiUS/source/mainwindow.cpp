/*************************************************************************************************/
/* Includes ------------------------------------------------------------------------------------ */
#include "mainwindow.h"
#include "mainwindow_ui.h"

#include <iostream>
#include <thread>

#include "detection_rythme.h"
#include "generator.h"
#include "playback.h"
#include "readwrite_wav.h"
#include "recording.h"
#include "song_player.h"
#include "widgets/widget_note.h"


/*************************************************************************************************/
/* Public methods ------------------------------------------------------------------------------ */
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(this)
{
    ui.setupUi(this);
    SetupMenus();

    QObject::connect(
      &ui.pushButtonA, &QPushButton::clicked, this, &MainWindow::on_pushButtonA_clicked);
    QObject::connect(&ui.bargraphUpdater, &QTimer::timeout, this, &MainWindow::updateBargraph);
    QObject::connect(&ui.buttonStopRecord, &QPushButton::clicked, this, &MainWindow::stopRecord);
    ConnectControlSignals();

    ui.bargraphUpdater.start(100);
    ui.buttonPlay.setEnabled(false);
    ui.buttonProcess.setEnabled(false);
}

void MainWindow::startRecord()
{
    DisconnectControlSignals();
    ui.buttonPlay.setEnabled(false);
    ui.buttonProcess.setEnabled(false);
    ui.buttonSaveLoad.setEnabled(false);

    // Swap buttons record & hide
    ui.buttonRecord.hide();
    ui.buttonStopRecord.show();

    Recording_SetStopPolicy(std::function<bool()>{});
    // Activer les bouton a la fin de la minute??
    if(recordingThread.joinable())
    {
        recordingThread.join();
    }
    recordingThread = std::thread{[&]() {
        try
        {
            rec = Record(15);
        }
        catch(const recorderException&)
        {
            // Do Nothing
        }
    }};
    ConnectControlSignals();
}

void MainWindow::stopRecord()
{
    DisconnectControlSignals();

    ui.buttonStopRecord.hide();
    ui.buttonRecord.show();

    Recording_SetStopPolicy([]() {
        return true;
    });

    if(recordingThread.joinable())
    {
        recordingThread.join();
    }

    if(rec.isValid())
    {
        ui.buttonPlay.setEnabled(true);
        ui.buttonProcess.setEnabled(true);
    }
    ui.buttonSaveLoad.setEnabled(true);

    ConnectControlSignals();
}

void MainWindow::play()
{
    DisconnectControlSignals();
    DisableAllButtons();

    if(rec.isValid())
    {
        if(recordingThread.joinable())
        {
            recordingThread.join();
        }
        recordingThread = std::thread{[&]() {
            try
            {
                Playback(rec);    // Faudrait avoir un son par defaut ou avoir une
                // autre boite contextuel qui donne acces au fichier
            }
            catch(const recorderException&)
            {
                // Do Nothing
            }
        }};
    }

    EnableAllButtons();
    ConnectControlSignals();
}

void MainWindow::processing()
{
    DisconnectControlSignals();
    DisableAllButtons();

    if(rec.isValid())
    {
        if(recordingThread.joinable())
        {
            recordingThread.join();
        }
        recordingThread = std::thread{[&]() {
            analyse_rythme(rec);
        }};
    }

    EnableAllButtons();
    ConnectControlSignals();
}

void MainWindow::saveOrLoad()
{
    DisableAllButtons();

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
    if(rec.isValid())
    {
        ui.buttonPlay.setEnabled(true);
        ui.buttonProcess.setEnabled(true);
    }
    ui.buttonSaveLoad.setEnabled(true);
}

void MainWindow::saving()
{
    if(rec.isValid())
    {
        QString savename = QFileDialog::getSaveFileName(
          this, tr("Save Address Book"), "", tr("Address Book (*.wav);;All Files (*)"));
        std::string filename = savename.toStdString();
        SaveToWav(filename, rec);
    }
    else
    {
        ui.msgBoxSave.exec();
    }
}

void MainWindow::loading()
{
    try
    {
        QString loadname = QFileDialog::getOpenFileName(
          this, tr("Open File"), "/MelodiUS/more_sounds", tr("Sound Files (*.wav)"));
        std::string filename = loadname.toStdString();
        rec                  = LoadFromWav(filename);
    }
    catch(...)
    {
        ui.msgBoxLoad.exec();
    }
}

void MainWindow::interpret()
{
    Play_Song(playedNotes);
}

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


/*************************************************************************************************/
/* Private methods ----------------------------------------------------------------------------- */
void MainWindow::ConnectControlSignals()
{
    QObject::connect(&ui.buttonRecord, &QPushButton::clicked, this, &MainWindow::startRecord);
    // QObject::connect(&ui.buttonStopRecord, &QPushButton::clicked, this, &MainWindow::stopRecord);
    QObject::connect(&ui.buttonPlay, &QPushButton::clicked, this, &MainWindow::play);
    QObject::connect(&ui.buttonProcess, &QPushButton::clicked, this, &MainWindow::processing);
    QObject::connect(&ui.buttonSaveLoad, &QPushButton::clicked, this, &MainWindow::saveOrLoad);
}
void MainWindow::DisconnectControlSignals()
{
    QObject::disconnect(&ui.buttonRecord, &QPushButton::clicked, this, &MainWindow::startRecord);
    // QObject::disconnect(&ui.buttonStopRecord, &QPushButton::clicked, this,
    // &MainWindow::stopRecord);
    QObject::disconnect(&ui.buttonPlay, &QPushButton::clicked, this, &MainWindow::play);
    QObject::disconnect(&ui.buttonProcess, &QPushButton::clicked, this, &MainWindow::processing);
    QObject::disconnect(&ui.buttonSaveLoad, &QPushButton::clicked, this, &MainWindow::saveOrLoad);
}

void MainWindow::DisableAllButtons()
{
    ui.buttonRecord.setEnabled(false);
    ui.buttonStopRecord.setEnabled(false);
    ui.buttonPlay.setEnabled(false);
    ui.buttonProcess.setEnabled(false);
    ui.buttonSaveLoad.setEnabled(false);
}

void MainWindow::EnableAllButtons()
{
    ui.buttonRecord.setEnabled(true);
    ui.buttonStopRecord.setEnabled(true);
    ui.buttonPlay.setEnabled(true);
    ui.buttonProcess.setEnabled(true);
    ui.buttonSaveLoad.setEnabled(true);
}

void MainWindow::SetupMenus()
{
    /* File */
    QMenu* fileMenu = new QMenu{this};
    fileMenu->addAction("Sauvegarder / Charger", this, &MainWindow::saveOrLoad);
    fileMenu->addAction("Fermer", this, &MainWindow::close);
    fileMenu->setTitle("Fichier");

    /* Recording */
    /* clang-format off */
    QMenu* recordingMenu = new QMenu{this};
    recordingMenu->addAction(QString::fromStdWString(L"Démarrer enregistrement"),
                             this, &MainWindow::startRecord);
    recordingMenu->addAction(QString::fromStdWString(L"Écouter enregistrement"),
                             this, &MainWindow::play);
    recordingMenu->setTitle("Enregistrement");

    /* Tools */
    QMenu* toolMenu = new QMenu{this};
    toolMenu->addAction(QString::fromStdWString(L"Écrire partition"),
                        this, &MainWindow::processing);
    toolMenu->addAction(QString::fromStdWString(L"Interpréter pièce"),
                        this, &MainWindow::interpret);
    toolMenu->setTitle("Outils");
    /* clang-format off */

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(recordingMenu);
    menuBar()->addMenu(toolMenu);
}


/*************************************************************************************************/
/* END OF FILE --------------------------------------------------------------------------------- */
