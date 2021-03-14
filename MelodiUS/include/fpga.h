#pragma once
/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#ifndef LINUX_
#include "CommunicationFPGA.h"
#endif

#include "globaldef.h"
#include <string>
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

    FPGA(); /* The constructor is private to avoid instanciation */

public:
    /* --------------------------------- */
    /* Types                             */
    enum class Port
    {
        A,
        B,
        C,
        D,
    };

    static void Init();
    static void DeInit();

    /* --------------------------------- */
    /* Accessors                         */
    static bool        isOk();
    static std::string errorMsg();

    /* Pin is a value from 0 to 3 */
    static bool    readPin(Port port, uint8_t pin);
    static uint8_t readPort(Port port);

    static void WriteLED(uint8_t val);

private:
    /* --------------------------------- */
    /* Types                             */
    enum Registers : int
    {
        /* Ports, these values might change */
        READ_A  = 0,
        WRITE_A = 1,
        READ_B  = 2,
        WRITE_B = 3,
        READ_C  = 4,
        WRITE_C = 5,
        READ_D  = 6,
        WRITE_D = 7,
        SWITCH  = 8,
        BUTTON  = 9,
        LED     = 10,
        AN0     = 11,
        AN1     = 12,
        AN2     = 13,
        AN3     = 14
    };
};


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
