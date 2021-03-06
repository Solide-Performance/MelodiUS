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

/* --------------------------------- */
/* Constructor --------------------- */
#pragma region Constructor
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(this)
{
    ui.setupUi(this);
    SetupMenus();

    /* clang-format off */
    QObject::connect(&ui.pushButtonA, &QPushButton::clicked,
                     this, &MainWindow::on_pushButtonA_clicked);

    QObject::connect(&ui.buttonStopRecord, &QPushButton::clicked,
                     this, &MainWindow::stopRecord);

    QObject::connect(&ui.bargraphUpdater, &QTimer::timeout,
                     this, &MainWindow::updateBargraph);
    QObject::connect(&ui.fpgaConnectionPoller, &QTimer::timeout,
                     this, &MainWindow::updateFpgaConnection);

    QObject::connect(this, &MainWindow::writePartitionSignal,
                     this, &MainWindow::writePartitionSlot);

    QObject::connect(&ui.fpgaConnection, &QPushButton::clicked,
                     []()
                     {
                         FPGA::Init();
                         if(FPGA::isConnected())
                         {
                             FPGA::StartListener();
                         }
                     });

    QObject::connect(&ui.buttonCalibrateA, &QPushButton::clicked,  [this]{Calibrate(Phoneme::a);});
    QObject::connect(&ui.buttonCalibrateI, &QPushButton::clicked,  [this]{Calibrate(Phoneme::i);});
    QObject::connect(&ui.buttonCalibrateEY, &QPushButton::clicked, [this]{Calibrate(Phoneme::ey);});
    QObject::connect(&ui.buttonCalibrateAE, &QPushButton::clicked, [this]{Calibrate(Phoneme::ae);});

    QObject::connect(&ui.buttonClear, &QPushButton::clicked, [this]{ui.P.clear();ui.groupBoxPartition.resize(ui.scrollArea.width() - 20,300);});

    FPGA::SetPhonemeCallback(Phoneme::a,  [this]() mutable { ui.buttonRecord.click(); });
    FPGA::SetPhonemeCallback(Phoneme::i,  [this]()         { ui.buttonPlay.click(); });
    FPGA::SetPhonemeCallback(Phoneme::ey, [this]() mutable { ui.buttonSaveLoad.click(); });
    FPGA::SetPhonemeCallback(Phoneme::ae, [this]() mutable { ui.buttonProcess.click(); });
    /* clang-format on */

    ConnectControlSignals();
    ui.bargraphUpdater.start(50);
    ui.fpgaConnectionPoller.start(1000);
    ui.buttonPlay.setEnabled(false);
    ui.buttonProcess.setEnabled(false);
}

MainWindow::~MainWindow()
{
    Recording_SetStopPolicy([]() {
        return true;
    });
    if(recordingThread.joinable())
    {
        recordingThread.join();
    }
}
#pragma endregion

/* --------------------------------- */
/* Main functions ------------------ */
#pragma region Main Functions
void                MainWindow::startRecord()
{
    DisconnectControlSignals();
    ui.buttonPlay.setEnabled(false);
    ui.buttonProcess.setEnabled(false);
    ui.buttonSaveLoad.setEnabled(false);
    FPGA::SetPhonemeCallback(Phoneme::a, [this]() mutable {
        ui.buttonStopRecord.click();
    });

    // Swap buttons record & hide
    ui.buttonRecord.hide();
    ui.buttonStopRecord.show();

    Recording_SetStopPolicy(EMPTY_STOP_POLICY);
    // Activer les bouton a la fin de la minute??
    if(recordingThread.joinable())
    {
        recordingThread.join();
    }
    recordingThread = std::thread{[&]() {
        try
        {
            rec = Record(120);
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


    Recording_SetStopPolicy(TRUE_STOP_POLICY);

    if(recordingThread.joinable())
    {
        recordingThread.join();
    }

    ui.buttonStopRecord.hide();
    ui.buttonRecord.show();

    FPGA::SetPhonemeCallback(Phoneme::a, [this]() mutable {
        ui.buttonRecord.click();
    });

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
            playedNotes = analyse_rythme(rec);

            emit writePartitionSignal();
            // ui.P.ecrireMusique(playedNotes);
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
    std::vector<Note> test;


    Note a(NoteType::Croche, NoteValue::B2);
    Note b(NoteType::Blanche, NoteValue::B3);
    Note c(NoteType::Blanche, NoteValue::B4);
    test.push_back(a);
    test.push_back(b);
    test.push_back(c);
    test.push_back(a);
    test.push_back(b);
    test.push_back(c);
    test.push_back(a);
    test.push_back(b);
    test.push_back(c);
    test.push_back(a);
    test.push_back(b);
    test.push_back(c);
    test.push_back(a);
    test.push_back(b);
    test.push_back(c);
    test.push_back(a);
    test.push_back(b);
    test.push_back(c);
    test.push_back(a);
    test.push_back(b);
    test.push_back(c);
    test.push_back(a);
    test.push_back(b);
    test.push_back(c);
    test.push_back(a);
    test.push_back(b);
    test.push_back(c);
    test.push_back(a);
    test.push_back(b);
    test.push_back(c);

    playedNotes = test;
    emit writePartitionSignal();
    //    ui.P.ecrireMusique(test);
}
#pragma endregion


/* --------------------------------- */
/* Updating GUI -------------------- */
#pragma reguin Updating GUI
void                    MainWindow::updateFpgaConnection()
{
    static bool fpgaConnectionErrorAcknowledged = false;

    if(FPGA::isConnected())
    {
        QIcon connectedFPGA{"images/FPGA_connected.png"};
        ui.fpgaConnection.setIcon(QIcon{connectedFPGA});
        ui.fpgaConnection.setIconSize(QSize{25, 25});
        ui.fpgaConnection.setToolTip(QString::fromWCharArray(L"FPGA connecté"));

        ui.fpgaConnectionError.clear();
        fpgaConnectionErrorAcknowledged = false;
    }
    else
    {
        QPixmap disconnectedFPGA{"images/FPGA_disconnected.png"};
        ui.fpgaConnection.setIcon(QIcon{disconnectedFPGA});
        ui.fpgaConnection.setIconSize(QSize{25, 25});
        ui.fpgaConnection.setToolTip(
          QString::fromWCharArray(L"FPGA déconnecté\n(appuyez pour essayer une connection)"));

        if(fpgaConnectionErrorAcknowledged == false)
        {
            if(FPGA::ErrorMsg() == "Mapping failed for error code: 3084")
            {
                ui.fpgaConnectionError.setPixmap(QString("images/usb.png"));
                ui.fpgaConnectionError.setToolTip(QString::fromWCharArray(
                  L"Erreur de communication avec le FPGA.\nDébranchez et "
                  L"rebranchez la carte FPGA au port série.\nPuis redémarrez le logiciel"));
                QMessageBox* fpgaCommunicationErrorMsgBox = new QMessageBox(&ui.centralwidget);
                fpgaCommunicationErrorMsgBox->warning(
                  &ui.centralwidget,
                  "Erreur de communication avec le FPGA",
                  QString::fromWCharArray(L"Débranchez et rebranchez la carte FPGA au port "
                                          L"série.\nPuis redémarrez le logiciel."),
                  QMessageBox::StandardButton::Ok,
                  QMessageBox::StandardButton::NoButton);

                fpgaConnectionErrorAcknowledged = true;
            }
        }
    }
}

void MainWindow::updateBargraph()
{
    std::array<uint8_t, 4>            adcValues  = FPGA::GetADC();
    std::array<std::array<int, 4>, 4> thresholds = FPGA::GetPhonemeThresholds();

    /* clang-format off */
    std::array<int, 4> valsFilter1{thresholds[0][0], thresholds[1][0], 
                                   thresholds[2][0], thresholds[3][0]};

    std::array<int, 4> valsFilter2{thresholds[0][1], thresholds[1][1],
                                   thresholds[2][1], thresholds[3][1]};

    std::array<int, 4> valsFilter3{thresholds[0][2], thresholds[1][2],
                                   thresholds[2][2], thresholds[3][2]};

    std::array<int, 4> valsFilter4{thresholds[0][3], thresholds[1][3],
                                   thresholds[2][3], thresholds[3][3]};
    /* clang-format on */

    ui.bargraph1.setValue(adcValues[0]);
    ui.bargraph1.ChangeVal(valsFilter1);
    ui.bargraph2.setValue(adcValues[1]);
    ui.bargraph2.ChangeVal(valsFilter2);
    ui.bargraph3.setValue(adcValues[2]);
    ui.bargraph3.ChangeVal(valsFilter3);
    ui.bargraph4.setValue(adcValues[3]);
    ui.bargraph4.ChangeVal(valsFilter4);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    ui.scrollArea.resize(width() - 250, height());

    if(ui.groupBoxPartition.width() <= ui.scrollArea.width())
    {
        ui.groupBoxPartition.resize(ui.scrollArea.width() - 20, ui.groupBoxPartition.height());
    }
}
#pragma endregion


void MainWindow::writePartitionSlot()
{
    ui.P.clear();
    ui.P.ecrireMusique(playedNotes);
}


/*************************************************************************************************/
/* Private methods ----------------------------------------------------------------------------- */
void MainWindow::Calibrate(Phoneme phoneme)
{
    using namespace std::chrono;
    constexpr milliseconds calibrationTime{3000};

    FPGA::SetPhonemeCallbackEnabled(false);

    QProgressDialog calibrationProgress{
      "Calibration", "Annuler", 0, calibrationTime / ADC_MEASURE_DELAY, &ui.centralwidget};
    calibrationProgress.setWindowModality(Qt::WindowModal);
    calibrationProgress.setAutoClose(false);
    calibrationProgress.setMinimumDuration(1);
    calibrationProgress.show();

    time_point now = system_clock::now();
    time_point end = now + calibrationTime;

    size_t             numberOfMeasures = 0;
    std::array<int, 4> measures{0, 0, 0, 0};
    while(system_clock::now() < end)
    {
        if(calibrationProgress.wasCanceled())
        {
            FPGA::SetPhonemeCallbackEnabled(true);
            return;
        }

        std::array<uint8_t, 4> values = FPGA::GetADC();

        std::transform(measures.begin(),
                       measures.end(),
                       values.begin(),
                       measures.begin(),
                       [](int measure, uint8_t value) {
                           return measure + value;
                       });

        if(values[0] == 0 && values[1] == 0 && values[2] == 0 && values[3] == 0)
        {
            /* Do nothing */
        }
        else
        {
            numberOfMeasures++;
        }

        calibrationProgress.setValue(numberOfMeasures);

        std::this_thread::sleep_for(ADC_MEASURE_DELAY);
    }
    for(int& measure : measures)
    {
        measure /= numberOfMeasures;
    }

    FPGA::Wait();
    FPGA::UpdatePhonemeThreshold(phoneme, measures);
    FPGA::SetPhonemeCallbackEnabled(true);
}

void MainWindow::ConnectControlSignals()
{
    FPGA::SetPhonemeCallbackEnabled(true);
    QObject::connect(&ui.buttonRecord, &QPushButton::clicked, this, &MainWindow::startRecord);
    // QObject::connect(&ui.buttonStopRecord, &QPushButton::clicked, this, &MainWindow::stopRecord);
    QObject::connect(&ui.buttonPlay, &QPushButton::clicked, this, &MainWindow::play);
    QObject::connect(&ui.buttonProcess, &QPushButton::clicked, this, &MainWindow::processing);
    QObject::connect(&ui.buttonSaveLoad, &QPushButton::clicked, this, &MainWindow::saveOrLoad);
}
void MainWindow::DisconnectControlSignals()
{
  //  FPGA::SetPhonemeCallbackEnabled(false);
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
    fileMenu->setTitle("Fichier");
    fileMenu->addAction("Sauvegarder / Charger", this, &MainWindow::saveOrLoad);
    fileMenu->addAction("Fermer", this, &MainWindow::close);

    /* Recording */
    /* clang-format off */
    QMenu* recordingMenu = new QMenu{this};
    recordingMenu->setTitle("Enregistrement");
    recordingMenu->addAction(QString::fromStdWString(L"Démarrer enregistrement"),
                             this, &MainWindow::startRecord);
    recordingMenu->addAction(QString::fromStdWString(L"Écouter enregistrement"),
                             this, &MainWindow::play);

    /* Calibration */
    QMenu* calibMenu = new QMenu{this};
    calibMenu->setTitle("Calibration");
    calibMenu->addAction(QString::fromWCharArray(L"Calibrer phonèmes"), [this]{ui.msgBoxCalibrate.show();});
    calibMenu->addAction(QString::fromWCharArray(L"Sauvegarder les paramètres de calibration"), &FPGA::SaveCalibrationFile);
    calibMenu->addAction(QString::fromWCharArray(L"Charger les paramètres de calibration"), &FPGA::LoadCalibrationFile);
    
    /* Tools */
    QMenu* toolMenu = new QMenu{this};
    toolMenu->setTitle("Outils");
    toolMenu->addAction(QString::fromStdWString(L"Écrire partition"),
                        this, &MainWindow::processing);
    toolMenu->addAction(QString::fromStdWString(L"Interpréter pièce"),
                        this, &MainWindow::interpret);
    toolMenu->addAction("Calibration", [this]{ui.msgBoxCalibrate.show();});
    toolMenu->setTitle("Outils");

    /*Credit*/
    QMenu* creditMenu = new QMenu{this};
    creditMenu->addAction(QString::fromStdWString(L"Crédit"),
                        this, &MainWindow::creditBox);
    creditMenu->setTitle("Credit");
    /* clang-format off */

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(recordingMenu);
    menuBar()->addMenu(calibMenu);
    menuBar()->addMenu(toolMenu);
    menuBar()->addMenu(creditMenu);
}
//void MainWindow::keyPressEvent(QKeyEvent* event)
//{
//    if (event->key() == Qt::Key_Space) 
//    {
//        FPGA::SetFlag(true);
//    }
//
//}
//void MainWindow::keyReleaseEvent(QKeyEvent* event)
//{
//    if (event->key() == Qt::Key_Space) 
//    {
//        FPGA::SetFlag(false);
//    }
//}
void MainWindow::creditBox()
{
    ui.msgBoxCredit.exec();

}

/*************************************************************************************************/
/* END OF FILE --------------------------------------------------------------------------------- */

