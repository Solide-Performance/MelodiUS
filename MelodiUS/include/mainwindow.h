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
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() = default;

public slots:
    void on_pushButtonA_clicked();
    void startRecord();
    void stopRecord();
    void updateBargraph();
    void resizeEvent(QResizeEvent* event);

    void play();
    void processing();
    void saveOrLoad();
    void saving();
    void loading();
    void interpret();


private:
    void Calibrate(Phoneme phoneme);
    void DisableAllButtons();
    void EnableAllButtons();
    void ConnectControlSignals();
    void DisconnectControlSignals();
    void SetupMenus();


private:
    Ui_MainWindow     ui;
    Recording         rec{};
    std::thread       recordingThread;
    std::vector<Note> playedNotes;
};


/*************************************************************************************************/
/* END OF FILE --------------------------------------------------------------------------------- */
