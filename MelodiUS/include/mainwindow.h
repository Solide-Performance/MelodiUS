#pragma once
/*************************************************************************************************/
/* Includes ------------------------------------------------------------------------------------ */
#include "mainwindow_ui.h"
#include "note.h"
#include "recording.h"

#include <thread>


/*************************************************************************************************/
/* Class definition ---------------------------------------------------------------------------- */
class MainWindow : public QMainWindow
{
    Q_OBJECT;

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    void on_pushButtonA_clicked();
    void startRecord();
    void stopRecord();
    void updateBargraph();
    void updateFpgaConnection();
    void resizeEvent(QResizeEvent* event);

    void play();
    void processing();
    void saveOrLoad();
    void saving();
    void loading();
    void interpret();

    void writePartitionSlot();

signals:
    void writePartitionSignal();

private:
    void Calibrate(Phoneme phoneme);
    void DisableAllButtons();
    void EnableAllButtons();
    void ConnectControlSignals();
    void DisconnectControlSignals();
    void SetupMenus();
    void creditBox();
   // void keyPressEvent(QKeyEvent* event);
  //  void keyReleaseEvent(QKeyEvent* event);

public:
   // MainWindow(QWidget* parent = nullptr);
    //~MainWindow();
    bool voiceKey = false;

private:
    Ui_MainWindow     ui;
    Recording         rec{};
    std::thread       recordingThread;
    std::vector<Note> playedNotes;
};


/*************************************************************************************************/
/* END OF FILE --------------------------------------------------------------------------------- */
