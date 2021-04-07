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
/* Class ------------------------------------------------------------------- */

/* The FPGA class is a `singleton` */
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

public:
    static void Init();
    static void DeInit();

    /* --------------------------------- */
    /* Accessors                         */
    [[nodiscard]] static bool                   isOk();
    [[nodiscard]] static std::string            errorMsg();
    [[nodiscard]] static std::array<uint8_t, 4> getADC();
    [[nodiscard]] static uint8_t                getADC(size_t channel);

    static void WriteLED(uint8_t val);

private:
    /* --------------------------------- */
    /* Methods                           */
    FPGA() = default; /* The constructor is private to avoid instanciation */

    static void listenerThread();

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
