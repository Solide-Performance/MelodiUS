/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "fpga.h"
#include <iostream>


/*****************************************************************************/
/* Macros ------------------------------------------------------------------ */
#ifndef LINUX_
#define CHECK_ENABLED(x)                                                                           \
    do                                                                                             \
    {                                                                                              \
        if(!m_fpga)                                                                                \
        {                                                                                          \
            return x;                                                                              \
        }                                                                                          \
    } while(false)
#else
#define CHECK_ENABLED(x) return x
#endif

/* clang-format off */
#define EMPTY_ADC_ARRAY std::array<uint8_t, 4>{0xFF, 0xFF, 0xFF, 0xFF}
/* clang-format on */

#define READ_CHANNEL(channelNumber)                                                                \
    do                                                                                             \
    {                                                                                              \
        static_assert(channelNumber >= 0 && channelNumber < 4);                                    \
        if(success)                                                                                \
        {                                                                                          \
            success =                                                                              \
              m_fpga->lireRegistre(FPGA::Registers::ADC##channelNumber, (*m_adc)[channelNumber]);  \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            std::cerr << "Error while reading channel #" #channelNumber << '\n'                    \
                      << errorMsg() << std::endl;                                                  \
        }                                                                                          \
    } while(false)


/*****************************************************************************/
/* Static member definitions ----------------------------------------------- */
#ifndef LINUX_
CommunicationFPGA*  FPGA::m_fpga     = nullptr;
bool*               FPGA::m_run      = nullptr;
std::thread*        FPGA::m_listener = nullptr;
std::array<int, 4>* FPGA::m_adc      = nullptr;
#else
void* FPGA::m_fpga = nullptr;
#endif


/*****************************************************************************/
/* Public methods definitions ---------------------------------------------- */
void FPGA::Init()
{
    if(m_fpga == nullptr)
    {
#ifndef LINUX_
        m_fpga = new CommunicationFPGA{};

        m_run      = new bool(true);
        m_listener = new std::thread{listenerThread};
        m_adc      = new std::array<int, 4>{0, 0, 0, 0};
#endif
    }
}
void FPGA::DeInit()
{
    *m_run = false;
    WriteLED(0x00);
    // delete m_fpga;

    m_listener->join();
}

void FPGA::listenerThread()
{
    CHECK_ENABLED();

    while(*m_run == true)
    {
        bool success = true;
        READ_CHANNEL(0);
        READ_CHANNEL(1);
        READ_CHANNEL(2);
        READ_CHANNEL(3);

        /* Sleep for 100 ms */
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }
}

/* --------------------------------- */
/* Accessors                         */
bool FPGA::isOk()
{
    CHECK_ENABLED(false);

#ifndef LINUX_
    return m_fpga->estOk();
#endif
}

std::string FPGA::errorMsg()
{
    CHECK_ENABLED("");

#ifndef LINUX_
    return m_fpga->messageErreur();
#endif
}

std::array<uint8_t, 4> FPGA::getADC()
{
    CHECK_ENABLED(EMPTY_ADC_ARRAY);

    return std::array<uint8_t, 4>{static_cast<uint8_t>((*m_adc)[0]),
                                  static_cast<uint8_t>((*m_adc)[1]),
                                  static_cast<uint8_t>((*m_adc)[2]),
                                  static_cast<uint8_t>((*m_adc)[3])};
}
uint8_t FPGA::getADC(size_t channel)
{
    CHECK_ENABLED(0xFF);

    if(channel < 4)
    {
        return static_cast<uint8_t>((*m_adc)[channel]);
    }
    else
    {
        return 0xFF;
    }
}

void FPGA::WriteLED(uint8_t val)
{
    CHECK_ENABLED();

#ifndef LINUX_
    m_fpga->ecrireRegistre(Registers::LED, val);
#endif
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
