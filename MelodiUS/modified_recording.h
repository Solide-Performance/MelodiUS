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
    std::vector<SAMPLE> m_samplesModified{};
public:
    modified_recording(const Recording&);
    ~modified_recording();
};
/* --------------------------------- */
modified_recording::modified_recording(const Recording& rec)
{
    //m_samplesModified = std::move();
}
modified_recording::~modified_recording()
{
}