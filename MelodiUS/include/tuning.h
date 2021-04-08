#pragma once

#include "globaldef.h"
#include "note.h"

#include <algorithm>
#include <array>
#include <string>
#include <utility>


[[nodiscard]] inline static std::pair<std::string, NoteValue> FindNoteFromFreq(double freq)
{
    using noteLookup = std::tuple<double, std::string, NoteValue>;

    static const std::array<noteLookup, static_cast<int64_t>(NoteValue::UNKNOWN)> lookup{
      std::tuple{82.41, "E2", NoteValue::E2}, {87.31, "F2", NoteValue::F2},
      {92.50, "F#2", NoteValue::Fs2},         {98.00, "G2", NoteValue::G2},
      {103.83, "G#2", NoteValue::Gs2},        {110.00, "A2", NoteValue::A2},
      {116.54, "A#2", NoteValue::As2},        {123.47, "B2", NoteValue::B2},
      {130.81, "C3", NoteValue::C3},          {138.59, "C#3", NoteValue::Cs3},
      {146.83, "D3", NoteValue::D3},          {155.56, "D#3", NoteValue::Ds3},
      {164.81, "E3", NoteValue::E3},          {174.61, "F3", NoteValue::F3},
      {185.00, "F#3", NoteValue::Fs3},        {196.00, "G3", NoteValue::G3},
      {207.65, "G#3", NoteValue::Gs3},        {220.00, "A3", NoteValue::A3},
      {233.08, "A#3", NoteValue::As3},        {246.94, "B3", NoteValue::B3},
      {261.63, "C4", NoteValue::C4},          {277.18, "C#4", NoteValue::Cs4},
      {293.66, "D4", NoteValue::D4},          {311.13, "D#4", NoteValue::Ds4},
      {329.63, "E4", NoteValue::E4},          {349.23, "F4", NoteValue::F4},
      {369.99, "F#4", NoteValue::Fs4},        {392.00, "G4", NoteValue::G4},
      {415.30, "G#4", NoteValue::Gs4},        {440.00, "A4", NoteValue::A4},
      {466.16, "A#4", NoteValue::As4},        {493.88, "B4", NoteValue::B4},
      {523.25, "C5", NoteValue::C5},          {554.37, "C#5", NoteValue::Cs5},
      {587.33, "D5", NoteValue::D5},          {622.25, "D#5", NoteValue::Ds5},
      {659.25, "E5", NoteValue::E5},          {698.46, "F5", NoteValue::F5},
      {739.99, "F#5", NoteValue::Fs5},        {783.99, "G5", NoteValue::G5},
      {830.61, "G#5", NoteValue::Gs5},        {880.00, "A5", NoteValue::A5},
      {932.33, "A#5", NoteValue::As5},        {987.77, "B5", NoteValue::B5},
      {1046.50, "C6", NoteValue::C6}};

    /* clang-format off */
    auto it = std::min_element(lookup.begin(), lookup.end(),
                     [freq](const noteLookup& a, const noteLookup& b)
                     {
                         return std::abs(std::get<0>(a) - freq) < std::abs(std::get<0>(b) - freq);
                     });
    /* clang-format on */

    return std::pair{std::get<1>(*it), std::get<2>(*it)};
}