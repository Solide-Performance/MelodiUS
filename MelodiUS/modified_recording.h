#pragma once
#include "recording.h"
struct gain
{
    size_t gain_droit;
    size_t gain_gauche;
};
class modified_recording : public Recording
{
private:
    struct gain;
    size_t              m_numChannels=0;
    std::vector<SAMPLE> m_samplesModified{};
public:
    modified_recording(const Recording&);
    ~modified_recording();

    void normalize(int max);
    void panning(int position);
};
/* --------------------------------- */
modified_recording::modified_recording(const Recording& rec)
{
    
}
modified_recording::~modified_recording()
{
}
inline void modified_recording::normalize(int max)
{
    if(m_numChannels == 2)
    {

    }
    else
    {
        int pek = m_samplesModified[0];
        for(int i = 0; i < m_samplesModified.size(); i++)
            {
                if(m_samplesModified[i] > pek)
                {
                    pek = m_samplesModified[i];
                }
            }
    }
}
