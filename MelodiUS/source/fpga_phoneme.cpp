/*****************************************************************************/
/* Includes ---------------------------------------------------------------- */
#include "fpga_phoneme.h"
#include <filesystem>
#include <fstream>
#include <iostream>



namespace FPGA
{

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


#ifndef LINUX_
#define READ_CHANNEL(channelNumber)                                                                \
    do                                                                                             \
    {                                                                                              \
        static_assert(channelNumber >= 0 && channelNumber < 4);                                    \
        if(success)                                                                                \
        {																						   \
            success = m_fpga->lireRegistre(Registers::ADC##channelNumber, m_adc[channelNumber]);   \
            if(!success)                                                                           \
            {                                                                                      \
                AddError();                                                                        \
            }                                                                                      \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            std::cerr << "Error while reading channel #" #channelNumber << '\n'                    \
                      << ErrorMsg() << std::endl;                                                  \
        }                                                                                          \
    } while(false)
#else
#define READ_CHANNEL(channelNumber)	;
#endif


/*****************************************************************************/
/* Constants --------------------------------------------------------------- */
constexpr float FILTER1_WEIGTH = 1.f;
constexpr float FILTER2_WEIGTH = 1.f;
constexpr float FILTER3_WEIGTH = 0.5f;
constexpr float FILTER4_WEIGTH = 2.f;

constexpr uint8_t ADC_PHONEME_DEFAULT_THRESHOLD = 0x80; /* 50% */
constexpr int     ADC_PHONEME_MARGIN            = 0x80; /* 50% */
constexpr size_t  COUNTER_MAX_PHONEME_THRESHOLD = 5;
constexpr size_t  MAX_ERROR_COUNT               = 10;

constexpr std::chrono::seconds MIN_DELAY_BETWEEN_PHONEMES{2};

/* constexpr char* is not a const char* but a char* const */
constexpr const char* CALIB_FILE_NAME = "phonemes.calib";

constexpr std::array<uint8_t, 4> EMPTY_ADC_ARRAY{0xFF, 0xFF, 0xFF, 0xFF};

/* clang-format off */
constexpr std::array<std::array<int, 4>, 4> DEFAULT_THRESHOLDS{std::array{0x80, 0x80, 0x80, 0x80},
                                                                         {0x80, 0x80, 0x80, 0x80},
                                                                         {0x80, 0x80, 0x80, 0x80},
                                                                         {0x80, 0x80, 0x80, 0x80}};
/* clang-format on */


/*****************************************************************************/
/* Static method declarations ---------------------------------------------- */
static void ListenerThread();
static bool CheckADCPhonemes();
static void CheckButtonPhonemes();
static void CallCallback(Phoneme channel);
static int  CheckADCDistance(const std::array<int, 4>& values, Phoneme phonemeToCheck);
static void DisplayADC();
static bool AddError();


/*****************************************************************************/
/* Static variables -------------------------------------------------------- */
#ifndef LINUX_
static CommunicationFPGA* m_fpga = nullptr;
#else
static void* m_fpga = nullptr;
#endif

static bool                                 m_run         = true;
static bool                                 m_isConnected = false;
static std::thread                          m_listener;
static std::array<int, 4>                   m_adc{};
static std::array<std::array<int, 4>, 4>    m_adcThreshold{DEFAULT_THRESHOLDS};
static std::array<std::function<void()>, 5> m_phonemeCallbacks{EMPTY_FUNCTION};
static Phoneme                              m_currentPhoneme         = Phoneme::UNKNOWN;
static Phoneme                              m_oldPhoneme             = Phoneme::UNKNOWN;
static size_t                               m_phonemeCounter         = 0;
static size_t                               m_errorCounter           = 0;
static bool                                 m_phonemeCallbackEnabled = true;
static bool                                 m_wait                   = false;


/*****************************************************************************/
/* Public methods definitions ---------------------------------------------- */
void Init()
{
    try
    {
        if(m_fpga == nullptr)
        {
#ifndef LINUX_
            m_fpga = new CommunicationFPGA{};
            if(m_fpga && m_fpga->estOk())
            {
                m_isConnected = true;
            }
            else
            {
                m_isConnected = false;
            }
#else
            m_isConnected = false;
#endif
        }
#ifndef LINUX_
        else
        {
            struct OnSeFaitPasChierAvecLaLibAdept
            {
                bool          _pas_acces_au_membre_pas_grave;
                char          _les_variables_privees_ne_marretent_pas[1024];
                unsigned long hif;
            };
            reinterpret_cast<OnSeFaitPasChierAvecLaLibAdept*>(m_fpga)->hif = 1;
            SAFE_DELETE(&m_fpga);
            Init();
        }
#endif
    }
    catch(...)
    {
        m_isConnected = false;
    }
}

void DeInit()
{
    if(m_run)
    {
        m_run = false;
    }
    WriteLED(0x00);

    m_isConnected = false;
    try
    {
        m_listener.join();
        delete m_fpga;
    }
    catch(...)
    {
    }
}

void StartListener()
{
    if(!m_listener.joinable())
    {
        m_listener = std::thread{ListenerThread};
    }
}

/* --------------------------------- */
/* Accessors                         */
bool isOk()
{
    CHECK_ENABLED(false);

#ifndef LINUX_
    return m_fpga->estOk();
#endif
}

[[nodiscard]] bool isConnected()
{
    CHECK_ENABLED(false);
    return m_isConnected;
}

std::string ErrorMsg()
{
    CHECK_ENABLED("");

#ifndef LINUX_
    return m_fpga->messageErreur();
#endif
}

std::array<uint8_t, 4> GetADC()
{
    CHECK_ENABLED(EMPTY_ADC_ARRAY);

    return std::array<uint8_t, 4>{static_cast<uint8_t>(m_adc[0]),
                                  static_cast<uint8_t>(m_adc[1]),
                                  static_cast<uint8_t>(m_adc[2]),
                                  static_cast<uint8_t>(m_adc[3])};
}
uint8_t GetADC(size_t channel)
{
    CHECK_ENABLED(0xFF);

    if(channel < 4)
    {
        return static_cast<uint8_t>(m_adc[channel]);
    }
    else
    {
        return 0xFF;
    }
}

Phoneme GetCurrentPhoneme()
{
    return m_currentPhoneme;
}

void WriteLED(uint8_t val)
{
    CHECK_ENABLED();

#ifndef LINUX_
    m_fpga->ecrireRegistre(Registers::LED, val);
#endif
}

void SetPhonemeCallback(Phoneme number, std::function<void()> callback)
{
    /* Set new callback function is the callback parameter is callable
     * If not callable, clear the callback function */
    if(callback)
    {
        m_phonemeCallbacks[PHONEME_INDEX(number)] = callback;
    }
    else
    {
        m_phonemeCallbacks[PHONEME_INDEX(number)] = EMPTY_FUNCTION;
    }
}

void UpdatePhonemeThreshold(std::array<std::array<int, 4>, 4> newThreshold)
{
    for(std::array<int, 4>& thres : newThreshold)
    {
        for(int& thr : thres)
        {
            /* Make sure the threshold is between 0x00 and 0xFF */
            thr = std::max(0, std::min(255, thr));
        }
    }

    m_adcThreshold = newThreshold;
}
void UpdatePhonemeThreshold(Phoneme phoneme, std::array<int, 4> newThreshold)
{
    /* Make sure the threshold is between 0x00 and 0xFF */
    for(int& thr : newThreshold)
    {
        /* Make sure the threshold is between 0x00 and 0xFF */
        thr = std::max(0, std::min(255, thr));
    }

    m_adcThreshold[PHONEME_INDEX(phoneme)] = newThreshold;
}

void SetPhonemeCallbackEnabled(bool enabled)
{
    m_phonemeCallbackEnabled = enabled;
}

[[nodiscard]] std::array<std::array<int, 4>, 4> GetPhonemeThresholds()
{
    return m_adcThreshold;
}

void SaveCalibrationFile()
{
    std::ofstream calibFile{"phonemes.calib"};

    for(const std::array<int, 4>& filter : m_adcThreshold)
    {
        for(int phoneme : filter)
        {
            calibFile << phoneme << '\n';
        }
    }

    calibFile.close();
}
bool LoadCalibrationFile()
{
    if(std::filesystem::exists("phonemes.calib"))
    {
        try
        {
            std::ifstream calibFile{"phonemes.calib"};

            for(size_t filter = 0; filter < m_adcThreshold.size(); filter++)
            {
                for(size_t phoneme = 0; phoneme < m_adcThreshold[filter].size(); phoneme++)
                {
                    calibFile >> m_adcThreshold[filter][phoneme];
                }
            }

            calibFile.close();
            return true;
        }
        catch(...)
        {
            m_adcThreshold = {DEFAULT_THRESHOLDS};
            return false;
        }
    }
    else
    {
        return false;
    }
}

void Wait()
{
    m_wait = true;
}


/*****************************************************************************/
/* Static functions definitions -------------------------------------------- */

static void ListenerThread()
{
    CHECK_ENABLED();

    while(m_run == true)
    {
        if(m_wait == true)
        {
            /* Sleep */
            m_wait = false;
            std::this_thread::sleep_for(ADC_MEASURE_DELAY * 20);
            continue;
        }
        if(m_isConnected == false)
        {
            m_wait = false;
            /* Sleep */
            std::this_thread::sleep_for(ADC_MEASURE_DELAY * 10);
            continue;
        }

        bool success = true;

        /* Read ADC channels */
        READ_CHANNEL(0);
        READ_CHANNEL(1);
        READ_CHANNEL(2);
        READ_CHANNEL(3);

        if(success)
        {
            m_errorCounter = 0;
            m_isConnected  = true;
        }

        uint8_t adc1  = static_cast<uint8_t>(m_adc[0]);
        uint8_t adc2  = static_cast<uint8_t>(m_adc[1]);
        uint8_t adc3  = static_cast<uint8_t>(m_adc[2]);
        uint8_t adc4  = static_cast<uint8_t>(m_adc[3]);
        int     adc1p = adc1 / 255.f * 100.f;
        int     adc2p = adc2 / 255.f * 100.f;
        int     adc3p = adc3 / 255.f * 100.f;
        int     adc4p = adc4 / 255.f * 100.f;

        /* clang-format off */
        std::printf(
          "\r 0x%2X (%2.d%%)\t| 0x%2X (%2.d%%)\t| 0x%2X (%2.d%%)\t| 0x%2X (%2.d%%) \t Vu:%s \t",
              adc1,  adc1p,     adc2,  adc2p,     adc3,  adc3p,     adc4,  adc4p, 
          m_currentPhoneme == Phoneme::a ? "A" :
          m_currentPhoneme == Phoneme::ey ? "EY" :
          m_currentPhoneme == Phoneme::ae ? "AE" :
          m_currentPhoneme == Phoneme::i ? "I" : "  ");
        /* clang-format on */

        /* Check for phonemes in the buttons */
        CheckButtonPhonemes();

        /* Check for phonemes in the ADC channel */
        bool foundPhoneme = CheckADCPhonemes();

        /* Display ADC value (selected with switches) on the 7-segment display */
        DisplayADC();

        /* Sleep */
        std::this_thread::sleep_for(ADC_MEASURE_DELAY);
    }
}

static int CheckADCDistance(const std::array<int, 4>& values, Phoneme phonemeToCheck)
{
    std::array<int, 4>& threshold = m_adcThreshold[PHONEME_INDEX(phonemeToCheck)];

    int distanceFilter1 = std::abs(values[0] - threshold[0]) / FILTER1_WEIGTH;
    int distanceFilter2 = std::abs(values[1] - threshold[1]) / FILTER2_WEIGTH;
    int distanceFilter3 = std::abs(values[2] - threshold[2]) / FILTER3_WEIGTH;
    int distanceFilter4 = std::abs(values[3] - threshold[3]) / FILTER4_WEIGTH;

    return distanceFilter1 + distanceFilter2 + distanceFilter3 + distanceFilter4;
}

static bool CheckADCPhonemes()
{
    int thresholdDistance1 = CheckADCDistance({}, Phoneme::a);
    int thresholdDistance2 = CheckADCDistance({}, Phoneme::ey);
    int thresholdDistance3 = CheckADCDistance({}, Phoneme::ae);
    int thresholdDistance4 = CheckADCDistance({}, Phoneme::i);
    int minThresholdDistance =
      std::min({thresholdDistance1, thresholdDistance2, thresholdDistance3, thresholdDistance4});

    std::array<int, 6> distances = {0, 0, 0, 0, ADC_PHONEME_MARGIN, minThresholdDistance / 2};

    distances[0] = CheckADCDistance(m_adc, Phoneme::a);
    distances[1] = CheckADCDistance(m_adc, Phoneme::ey);
    distances[2] = CheckADCDistance(m_adc, Phoneme::ae);
    distances[3] = CheckADCDistance(m_adc, Phoneme::i);

    auto smallest = std::min_element(distances.begin(), distances.end()) - distances.begin();

    switch(smallest)
    {
        case 0:
            m_currentPhoneme = Phoneme::a;
            break;
        case 1:
            m_currentPhoneme = Phoneme::ey;
            break;
        case 2:
            m_currentPhoneme = Phoneme::ae;
            break;
        case 3:
            m_currentPhoneme = Phoneme::i;
            break;
        case 4:
            [[fallthrough]];
        case 5:
            [[fallthrough]];
        default:
            m_currentPhoneme = Phoneme::UNKNOWN;
            break;
    }

    if(m_currentPhoneme != m_oldPhoneme)
    {
        /* clang-format off */
        m_phonemeCounter = m_phonemeCounter == 0 ||
                            m_phonemeCounter == std::numeric_limits<size_t>::max()
                            ? 0 : m_phonemeCounter - 1;
        /* clang-format on */

        m_oldPhoneme = m_currentPhoneme;
    }
    else
    {
        if(m_phonemeCounter == std::numeric_limits<size_t>::max())
        {
            /* Do nothing */
        }
        else if(++m_phonemeCounter >= COUNTER_MAX_PHONEME_THRESHOLD)
        {
            m_phonemeCounter = std::numeric_limits<size_t>::max();

            if(m_currentPhoneme != Phoneme::UNKNOWN)
            {
                CallCallback(m_currentPhoneme);
                return true;
            }
        }
    }
    return false;
}

static void CheckButtonPhonemes()
{
    bool success = true;

    /* Read button register */
    int buttons = 0;
#ifndef LINUX_
    success     = m_fpga->lireRegistre(Registers::BUTTON, buttons);
#else
	success = false;
#endif
    if(!success)
    {
        std::cerr << "Error while reading FPGA buttons\n" << ErrorMsg() << std::endl;
        return;
    }

    /* Check for BTN0 press */
    if((buttons & 0x01) != 0)
    {
        CallCallback(Phoneme::a);
    }
    /* Check for BTN1 press */
    if((buttons & 0x02) != 0)
    {
        CallCallback(Phoneme::ey);
    }
    /* Check for BTN2 press */
    if((buttons & 0x04) != 0)
    {
        CallCallback(Phoneme::ae);
    }
    /* Check for BTN3 press */
    if((buttons & 0x08) != 0)
    {
        CallCallback(Phoneme::i);
    }
}

static void CallCallback(Phoneme channel)
{
    if(m_phonemeCallbackEnabled == true)
    {
        /* Call callback function */
        auto callback = m_phonemeCallbacks[PHONEME_INDEX(channel)];
        if(callback)
        {
            callback();
        }
    }
}

static void DisplayADC()
{
    int  switches = 0;
#ifndef LINUX_
    bool success  = m_fpga->lireRegistre(Registers::SWITCH, switches);
#else
	bool success = false;
#endif
    if(!success)
    {
        std::cerr << "Error while reading FPGA switches\n" << ErrorMsg() << std::endl;
        return;
    }

    switches &= 0b11;

    /* Two-digits BCD */
    uint8_t adc     = m_adc[switches] / 255.f * 100.f;
    uint8_t adcTemp = adc % 10;
    adc             = ((adc / 10) % 10) << 4;
    adc |= adcTemp;

    /* Print percentage value */
#ifndef LINUX_
    success = m_fpga->ecrireRegistre(Registers::AN1, adc);
#else
	success = false;
#endif
    if(!success)
    {
        std::cerr << "Error while writing FPGA 7-segments\n" << ErrorMsg() << std::endl;
        return;
    }
}

static bool AddError()
{
    if(m_errorCounter++ >= MAX_ERROR_COUNT)
    {
        m_errorCounter = MAX_ERROR_COUNT;
        m_isConnected  = false;
        return true;
    }
    return false;
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
}    // namespace FPGA
