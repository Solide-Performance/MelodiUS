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

    /* clang-format off */
    QObject::connect(&ui.buttonCalibrateA, &QPushButton::clicked,  [this]{Calibrate(Phoneme::a);});
    QObject::connect(&ui.buttonCalibrateI, &QPushButton::clicked,  [this]{Calibrate(Phoneme::i);});
    QObject::connect(&ui.buttonCalibrateEY, &QPushButton::clicked, [this]{Calibrate(Phoneme::ey);});
    QObject::connect(&ui.buttonCalibrateAE, &QPushButton::clicked, [this]{Calibrate(Phoneme::ae);});
    /* clang-format on */

    ConnectControlSignals();

    ui.bargraphUpdater.start(50);
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
    std::vector<Note> test;
    Note              a(NoteType::Pause, NoteValue::UNKNOWN);
    Note              b(NoteType::DemiPause, NoteValue::UNKNOWN);
    Note              c(NoteType::Silence, NoteValue::UNKNOWN);
    Note              d(NoteType::DemiSilence, NoteValue::UNKNOWN);
    test.push_back(a);
    test.push_back(b);
    test.push_back(c);
    test.push_back(d);
   /*
    Note a(NoteType::Noire, NoteValue::E2);
    Note b(NoteType::Noire, NoteValue::F2);
    Note c(NoteType::Noire, NoteValue::Fs2);
    Note d(NoteType::Noire, NoteValue::G2);
    Note e(NoteType::Noire, NoteValue::Gs2);
    Note f(NoteType::Noire, NoteValue::A2);
    Note g(NoteType::Noire, NoteValue::As2);
    Note h(NoteType::Noire, NoteValue::B2);
    Note i(NoteType::Noire, NoteValue::C3);
    Note j(NoteType::Noire, NoteValue::Cs3);
    Note k(NoteType::Noire, NoteValue::D3);
    Note l(NoteType::Noire, NoteValue::Ds3);
    Note m(NoteType::Noire, NoteValue::E3);
    Note n(NoteType::Noire, NoteValue::F3);
    Note o(NoteType::Noire, NoteValue::Fs3);
    Note p(NoteType::Noire, NoteValue::G3);
    Note q(NoteType::Noire, NoteValue::Gs3);
    Note r(NoteType::Noire, NoteValue::A3);
    Note s(NoteType::Noire, NoteValue::As3);
    Note t(NoteType::Noire, NoteValue::B3);
    Note u(NoteType::Noire, NoteValue::C4);
    Note v(NoteType::Noire, NoteValue::Cs4);
    Note w(NoteType::Noire, NoteValue::D4);
    Note x(NoteType::Noire, NoteValue::Ds4);
    Note y(NoteType::Noire, NoteValue::E4);
    Note z(NoteType::Noire, NoteValue::F4);
    Note ca(NoteType::Noire, NoteValue::Fs4);
    Note cb(NoteType::Noire, NoteValue::G4);
    Note cc(NoteType::Noire, NoteValue::Gs4);
    Note cd(NoteType::Noire, NoteValue::A4);
    Note ce(NoteType::Noire, NoteValue::As4);
    Note cf(NoteType::Noire, NoteValue::B4);
    Note cg(NoteType::Noire, NoteValue::C5);
    Note ch(NoteType::Noire, NoteValue::Cs5);
    Note ci(NoteType::Noire, NoteValue::D5);
    Note cj(NoteType::Noire, NoteValue::Ds5);
    Note ck(NoteType::Noire, NoteValue::E5);
    Note cl(NoteType::Noire, NoteValue::F5);
    Note cm(NoteType::Noire, NoteValue::Fs5);
    Note cn(NoteType::Noire, NoteValue::G5);
    Note co(NoteType::Noire, NoteValue::Gs5);
    Note cp(NoteType::Noire, NoteValue::A5);
    Note cq(NoteType::Noire, NoteValue::As5);
    Note cr(NoteType::Noire, NoteValue::B5);
    Note cs(NoteType::Noire, NoteValue::C6);
    test.push_back(a);
    test.push_back(b);
    test.push_back(c);
    test.push_back(d);
    test.push_back(e);
    test.push_back(f);
    test.push_back(g);
    test.push_back(h);
    test.push_back(i);
    test.push_back(j);
    test.push_back(k);
    test.push_back(l);
    test.push_back(m);
    test.push_back(n);
    test.push_back(o);
    test.push_back(p);
    test.push_back(q);
    test.push_back(r);
    test.push_back(s);
    test.push_back(t);
    test.push_back(u);
    test.push_back(v);
    test.push_back(w);
    test.push_back(x);
    test.push_back(y);
    test.push_back(z);
    test.push_back(ca);
    test.push_back(cb);
    test.push_back(cc);
    test.push_back(cd);
    test.push_back(ce);
    test.push_back(cf);
    test.push_back(cg);
    test.push_back(ch);
    test.push_back(ci);
    test.push_back(cj);
    test.push_back(ck);
    test.push_back(cl);
    test.push_back(cm);
    test.push_back(cn);
    test.push_back(co);
    test.push_back(cp);
    test.push_back(cq);
    test.push_back(cr);
    test.push_back(cs);*/
    
    

    ui.P.ecrireMusique(test);
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


/*************************************************************************************************/
/* Private methods ----------------------------------------------------------------------------- */
void MainWindow::Calibrate(Phoneme phoneme)
{
    using namespace std::chrono;
    constexpr milliseconds calibrationTime{5000};

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
        numberOfMeasures++;

        calibrationProgress.setValue(numberOfMeasures);

        std::this_thread::sleep_for(ADC_MEASURE_DELAY);
    }
    for(int& measure : measures)
    {
        measure /= numberOfMeasures;
    }

    FPGA::UpdatePhonemeThreshold(phoneme, measures);
}

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
    toolMenu->addAction("Calibration", [this]{ui.msgBoxCalibrate.show();});
    toolMenu->setTitle("Outils");
    /* clang-format off */

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(recordingMenu);
    menuBar()->addMenu(toolMenu);
}


/*************************************************************************************************/
/* END OF FILE --------------------------------------------------------------------------------- */

