#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#ifndef LINUX_
#include "CommunicationFPGA/CommunicationFPGA.h"
#endif

#include "globaldef.h"
#include <array>
#include <string>
#include <thread>
#include <vector>


/*****************************************************************************/
/* Enums ------------------------------------------------------------------- */
enum class Phoneme : uint8_t
{
    a = 0,
    ey,
    ae,
    i,
    UNKNOWN
};


/*****************************************************************************/
/* Class ------------------------------------------------------------------- */

/* The FPGA class is a `static singleton`
   This code could be out of a class, and within a simple namespace instead. */
class FPGA
{
#ifdef LINUX_
    static void* m_fpga;
#else
    static CommunicationFPGA* m_fpga;
#endif

    static std::thread*        m_listener;
    static bool*               m_run;
    static std::array<int, 4>* m_adc;    // Stuck with `int` because of CommunicationFPGA lib
    static std::array<std::function<void()>, 5>*        m_phonemeCallbacks;
    static std::function<void(std::array<uint8_t, 4>)>* m_updateCallback;
    static Phoneme*                                     m_currentPhoneme;
    static Phoneme*                                     m_oldPhoneme;
    static size_t*                                      m_phonemeCounter;

public:
    static void Init();
    static void DeInit();
    static void StartListener();

    /* --------------------------------- */
    /* Accessors                         */
    [[nodiscard]] static bool                   isOk();
    [[nodiscard]] static std::string            ErrorMsg();
    [[nodiscard]] static std::array<uint8_t, 4> GetADC();
    [[nodiscard]] static uint8_t                GetADC(size_t channel);
    [[nodiscard]] static Phoneme                GetCurrentPhoneme();
    static void SetPhonemeCallback(Phoneme number, std::function<void()> callback);
    static void SetUpdateCallback(std::function<void(std::array<uint8_t, 4>)> callback);

    static void WriteLED(uint8_t val);

private:
    /* --------------------------------- */
    /* Methods                           */
    FPGA() = default; /* The constructor is private to avoid instanciation */

    static void ListenerThread();
    static void CheckADCPhonemes();
    static void CheckButtonPhonemes();
    static void CallCallback(Phoneme channel);
    static void DisplayADC();

    /* --------------------------------- */
    /* Types                             */
    enum Registers : int
    {
        /* Ports, these values might change */
        BUTTON       = 0,
        SWITCH       = 1,
        TIME_COUNTER = 2,
        ADC0         = 3,
        ADC1         = 4,
        ADC2         = 5,
        ADC3         = 6,
        AN0          = 7,
        AN1          = 8,
        DOT          = 9,
        LED          = 10,
    };
};


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
