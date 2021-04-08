/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "fpga_phoneme.h"
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
/* Constants --------------------------------------------------------------- */
constexpr uint8_t ADC_PHONEME_THRESHOLD         = 0x80; /* 50% */
constexpr size_t  COUNTER_MAX_PHONEME_THRESHOLD = 5;


/*****************************************************************************/
/* Static member definitions ----------------------------------------------- */
#ifndef LINUX_
CommunicationFPGA* FPGA::m_fpga = nullptr;
#else
void* FPGA::m_fpga = nullptr;
#endif
bool*                                 FPGA::m_run              = nullptr;
std::thread*                          FPGA::m_listener         = nullptr;
std::array<int, 4>*                   FPGA::m_adc              = nullptr;
std::array<std::function<void()>, 5>* FPGA::m_phonemeCallbacks = nullptr;
Phoneme*                              FPGA::m_currentPhoneme   = nullptr;
Phoneme*                              FPGA::m_oldPhoneme       = nullptr;
size_t*                               FPGA::m_phonemeCounter   = nullptr;


/*****************************************************************************/
/* Public methods definitions ---------------------------------------------- */
void FPGA::Init()
{
    if(m_fpga == nullptr)
    {
#ifndef LINUX_
        m_fpga = new CommunicationFPGA{};

        m_run              = new bool(true);
        m_adc              = new std::array<int, 4>{0, 0, 0, 0};
        m_phonemeCallbacks = new std::array<std::function<void()>, 5>{EMPTY_FUNCTION};
        m_currentPhoneme   = new Phoneme{Phoneme::UNKNOWN};
        m_oldPhoneme       = new Phoneme{Phoneme::UNKNOWN};
        m_phonemeCounter   = new size_t{0};
#endif
    }
}
void FPGA::DeInit()
{
    *m_run = false;
    WriteLED(0x00);

    try
    {
        m_listener->join();

        // delete m_fpga;
        delete m_run;
        delete m_listener;
        delete m_adc;
        delete m_phonemeCallbacks;
        delete m_currentPhoneme;
        delete m_oldPhoneme;
        delete m_phonemeCounter;
    }
    catch(...)
    {
    }
}

void FPGA::StartListener()
{
    m_listener = new std::thread{listenerThread};
}

void FPGA::listenerThread()
{
    CHECK_ENABLED();

    while(*m_run == true)
    {
        bool success = true;

        /* Read ADC channels */
        READ_CHANNEL(0);
        READ_CHANNEL(1);
        READ_CHANNEL(2);
        READ_CHANNEL(3);

        uint8_t adc1 = static_cast<uint8_t>((*m_adc)[0]);
        uint8_t adc2 = static_cast<uint8_t>((*m_adc)[1]);
        uint8_t adc3 = static_cast<uint8_t>((*m_adc)[2]);
        uint8_t adc4 = static_cast<uint8_t>((*m_adc)[3]);
        printf("\r 0x%2X | 0x%2X | 0x%2X | 0x%2X", adc1, adc2, adc3, adc4);

        /* Check for phonemes in the buttons */
        checkButtonPhonemes();

        /* Check for phonemes in the ADC channel */
        checkADCPhonemes();

        /* Sleep for 100 ms */
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }
}

void FPGA::checkADCPhonemes()
{
    /* This function uses a mask that is enabled at each detected phoneme level */
    /* The mask values are :
     *  - 0b0001 (0x01) for the 1st channel
     *  - 0b0010 (0x02) for the 2nd channel
     *  - 0b0100 (0x04) for the 3rd channel
     *  - 0b1000 (0x08) for the 4th channel
     * The 4 MSB are ignored
     *
     * Phonemes correspond to certain mask values:
     * - 'A'  is 0b0011
     * - 'EY' is 0b1110
     * - 'AE' is 0b0010
     * - 'I'  is 0b1000
     */
    uint8_t phonemeMask = 0x00;
    if((*m_adc)[0] >= ADC_PHONEME_THRESHOLD)
    {
        phonemeMask |= 0b0001;
    }
    if((*m_adc)[1] >= ADC_PHONEME_THRESHOLD)
    {
        phonemeMask |= 0b0010;
    }
    if((*m_adc)[2] >= ADC_PHONEME_THRESHOLD)
    {
        phonemeMask |= 0b0100;
    }
    if((*m_adc)[3] >= ADC_PHONEME_THRESHOLD)
    {
        phonemeMask |= 0b1000;
    }

    phonemeMask &= 0x0F;
    if(phonemeMask == 0b0011)
    {
        *m_currentPhoneme = Phoneme::a;
    }
    else if(phonemeMask == 0b1110)
    {
        *m_currentPhoneme = Phoneme::ey;
    }
    else if(phonemeMask == 0b0010)
    {
        *m_currentPhoneme = Phoneme::ae;
    }
    else if(phonemeMask == 0b1000)
    {
        *m_currentPhoneme = Phoneme::i;
    }
    else
    {
        *m_currentPhoneme = Phoneme::UNKNOWN;
    }

    if(*m_currentPhoneme != *m_oldPhoneme)
    {
        /* clang-format off */
        *m_phonemeCounter = *m_phonemeCounter == 0 ||
                            *m_phonemeCounter == std::numeric_limits<size_t>::max()
                            ? 0 : *m_phonemeCounter - 1;
        /* clang-format on */

        *m_oldPhoneme = *m_currentPhoneme;
    }
    else
    {
        if(*m_phonemeCounter == std::numeric_limits<size_t>::max())
        {
            /* Do nothing */
        }
        else if(++*m_phonemeCounter >= COUNTER_MAX_PHONEME_THRESHOLD)
        {
            *m_phonemeCounter = std::numeric_limits<size_t>::max();

            callCallback(*m_currentPhoneme);
        }
    }
}

void FPGA::checkButtonPhonemes()
{
    bool success = true;

    /* Read button register */
    int buttons = 0;
    success     = m_fpga->lireRegistre(Registers::BUTTON, buttons);
    if(!success)
    {
        std::cerr << "Error while reading FPGA buttons" << std::endl;
        return;
    }

    /* Check for BTN0 press */
    if((buttons & 0x01) != 0)
    {
        callCallback(Phoneme::a);
    }
    /* Check for BTN1 press */
    if((buttons & 0x02) != 0)
    {
        callCallback(Phoneme::ey);
    }
    /* Check for BTN2 press */
    if((buttons & 0x04) != 0)
    {
        callCallback(Phoneme::ae);
    }
    /* Check for BTN3 press */
    if((buttons & 0x08) != 0)
    {
        callCallback(Phoneme::i);
    }
}

void FPGA::callCallback(Phoneme channel)
{
    /* Call callback function */
    auto callback = (*m_phonemeCallbacks)[static_cast<uint8_t>(channel)];
    if(callback)
    {
        callback();
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

Phoneme FPGA::getCurrentPhoneme()
{
    return m_currentPhoneme ? *m_currentPhoneme : Phoneme::UNKNOWN;
}

void FPGA::WriteLED(uint8_t val)
{
    CHECK_ENABLED();

#ifndef LINUX_
    m_fpga->ecrireRegistre(Registers::LED, val);
#endif
}

void FPGA::setPhonemeCallback(Phoneme number, std::function<void()> callback)
{
    /* Set new callback function is the callback parameter is callable
     * If not callable, clear the callback function */
    if(callback)
    {
        (*m_phonemeCallbacks)[static_cast<size_t>(number)] = callback;
    }
    else
    {
        (*m_phonemeCallbacks)[static_cast<size_t>(number)] = EMPTY_FUNCTION;
    }
}

/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
