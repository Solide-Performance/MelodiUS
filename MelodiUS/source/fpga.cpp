/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "fpga.h"
#include <iostream>


/*****************************************************************************/
/* Static member definitions ----------------------------------------------- */
#ifndef LINUX_
CommunicationFPGA* FPGA::m_fpga = nullptr;
#endif


/*****************************************************************************/
/* Public methods definitions ---------------------------------------------- */

void FPGA::Init()
{
    if(m_fpga == nullptr)
    {
#ifndef LINUX_
        m_fpga = new CommunicationFPGA{};
#endif
    }
}
void FPGA::DeInit()
{
    WriteLED(0x00);
    // delete m_fpga;
}

/* --------------------------------- */
/* Accessors                         */
bool FPGA::isOk()
{
    if(!m_fpga)
    {
        return false;
    }

#ifdef LINUX_
    return false;
#else
    return m_fpga->estOk();
#endif
}

std::string FPGA::errorMsg()
{
    if(!m_fpga)
    {
        return "";
    }

#ifdef LINUX_
    return "No FPGA support on Linux";
#else
    return m_fpga->messageErreur();
#endif
}

uint8_t FPGA::readPort(Port port)
{
    if(!m_fpga)
    {
        return 0x00;
    }

#ifdef LINUX_
    return 0x00;
#else
    int  retval  = 0;
    bool success = true;

    switch(port)
    {
        case Port::A:
            success = m_fpga->lireRegistre(Registers::READ_A, retval);
            break;
        case Port::B:
            success = m_fpga->lireRegistre(Registers::READ_B, retval);
            break;
        case Port::C:
            success = m_fpga->lireRegistre(Registers::READ_C, retval);
            break;
        case Port::D:
            success = m_fpga->lireRegistre(Registers::READ_D, retval);
            break;

        default:
            std::cerr << "Wrong port to read from" << std::endl;
            return std::numeric_limits<uint8_t>::max();
    }

    if(!success)
    {
        std::cerr << "Error while reading from port" << std::endl;
        return std::numeric_limits<uint8_t>::max();
    }

    return static_cast<uint8_t>(retval);
#endif
}

bool FPGA::readPin(Port port, uint8_t pin)
{
    if(!m_fpga)
    {
        return false;
    }

#ifdef LINUX_
    return false;
#else
    if(pin > 0x04)
    {
        std::cerr << "Wrong pin to read from" << std::endl;
        return false;
    }

    bool    val     = false;
    uint8_t portVal = readPort(port);

    if(portVal != std::numeric_limits<uint8_t>::max())
    {
        val = static_cast<bool>(portVal & (0x01 << pin));
    }

    return val;
#endif
}


void FPGA::WriteLED(uint8_t val)
{
    if(!m_fpga)
    {
        return;
    }

#ifndef LINUX_
    m_fpga->ecrireRegistre(Registers::LED, val);
#endif
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
