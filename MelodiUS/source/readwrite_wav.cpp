/*
 * PortAudio Portable Real-Time Audio Library
 * Latest Version at: http://www.portaudio.com
 *
 * Copyright (c) 1999-2010 Phil Burk and Ross Bencina
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however,
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also
 * requested that these non-binding requests be included along with the
 * license above.
 */
/*
 * WAV file writer.
 *
 * Author: Phil Burk
 */
/**
 * Very simple WAV file writer for saving captured audio.
 */

/*************************************************************************************************/
/* Includes ------------------------------------------------------------------------------------ */
#include "readwrite_wav.h"
#include "globaldef.h"
#include <array>
#include <cstdio>
#include <iostream>
#include <string>


/*************************************************************************************************/
/* Constants ----------------------------------------------------------------------------------- */
constexpr size_t WAV_HEADER_SIZE = (4 + 4 + 4 +  /* RIFF+size+WAVE */
                                    4 + 4 + 16 + /* fmt chunk      */
                                    4 + 4);      /* data chunk     */

/* Define WAV Chunk and FORM types as 4 byte integers. */
/* clang-format off */
constexpr uint32_t RIFF_ID = static_cast<uint32_t>('R' << 24U)
                             | static_cast<uint32_t>('I' << 16U)
                             | static_cast<uint32_t>('F' << 8U) | 'F';
constexpr uint32_t WAVE_ID = static_cast<uint32_t>('W' << 24U)
                             | static_cast<uint32_t>('A' << 16U)
                             | static_cast<uint32_t>('V' << 8U) | 'E';
constexpr uint32_t FMT_ID = static_cast<uint32_t>('f' << 24U)
                            | static_cast<uint32_t>('m' << 16U)
                            | static_cast<uint32_t>('t' << 8U) | ' ';
constexpr uint32_t DATA_ID = static_cast<uint32_t>('d' << 24U)
                             | static_cast<uint32_t>('a' << 16U)
                             | static_cast<uint32_t>('t' << 8U) | 'a';
/* clang-format on */

/* WAV PCM data format ID */
constexpr int16_t WAVE_FORMAT_PCM = 1;
// constexpr uint32_t WAVE_FORMAT_IMA_ADPCM = 0x0011;


/*************************************************************************************************/
/* Static functions declarations --------------------------------------------------------------- */
[[nodiscard]] std::vector<int16_t> Samples_FloatToShort(const std::vector<float>& inVec);
[[nodiscard]] std::vector<float>   Samples_ShortToFloat(const std::vector<int16_t>& inVec);


/*************************************************************************************************/
/* Function definitions ------------------------------------------------------------------------ */
void SaveToWav(const std::string& filename, const Recording& recording)
{
    std::vector<int16_t> shortData = Samples_FloatToShort(recording.getSamples());

    WAV_Writer writer{filename, static_cast<uint32_t>(recording.getSampleRate()), 1};

    writer.Write(shortData.data(), shortData.size());
}

Recording LoadFromWav(const std::string& filename)
{
    WAV_Reader reader{filename};
    reader.Read();

    std::vector<float> floatData = Samples_ShortToFloat(reader.get_Data());

    // @todo
    // HARDCODED '2' & '1' !!!!!!! TO REMOVE
    return {&floatData.front(),
            &floatData.back(),
            reader.get_FrameRate(),    // This used to be divided by 2 for some unknown reason
            1,
            reader.get_NumChannels()};
}


/*************************************************************************************************/
/* Static functions definitions ---------------------------------------------------------------- */
std::vector<int16_t> Samples_FloatToShort(const std::vector<float>& inVec)
{
    std::vector<int16_t> shortData = std::vector<int16_t>(inVec.size());

    // https://stackoverflow.com/a/56213245/10827197
    for(size_t i = 0; i < inVec.size(); i++)
    {
        float floatData = inVec[i] * 32767;
        shortData[i]    = (int16_t)floatData;
    }

    return shortData;
}

std::vector<float> Samples_ShortToFloat(const std::vector<int16_t>& inVec)
{
    std::vector<float> floatData(inVec.size());

    for(size_t i = 0; i < inVec.size(); i++)
    {
        float shortData = (float)inVec[i] / 32767;
        floatData[i]    = shortData;
    }

    return floatData;
}


/*************************************************************************************************/
/* Private methods definitions ----------------------------------------------------------------- */

/* Write long word data to a little endian format byte array. */
void WAV_Writer::WriteLongLE(uint8_t** addrPtr, uint32_t data)
{
    uint8_t* addr = *addrPtr;

    *addr++ = (uint8_t)data;            // NOLINT
    *addr++ = (uint8_t)(data >> 8);     // NOLINT
    *addr++ = (uint8_t)(data >> 16);    // NOLINT
    *addr++ = (uint8_t)(data >> 24);    // NOLINT

    *addrPtr = addr;
}

/* Write int16_t word data to a little endian format byte array. */
void WAV_Writer::WriteShortLE(uint8_t** addrPtr, uint16_t data)
{
    uint8_t* addr = *addrPtr;

    *addr++ = (uint8_t)data;           // NOLINT
    *addr++ = (uint8_t)(data >> 8);    // NOLINT

    *addrPtr = addr;
}

/* Write IFF ChunkType data to a byte array. */
void WAV_Writer::WriteChunkType(uint8_t** addrPtr, uint32_t cktyp)
{
    uint8_t* addr = *addrPtr;

    *addr++ = (uint8_t)(cktyp >> 24);    // NOLINT
    *addr++ = (uint8_t)(cktyp >> 16);    // NOLINT
    *addr++ = (uint8_t)(cktyp >> 8);     // NOLINT
    *addr++ = (uint8_t)cktyp;            // NOLINT

    *addrPtr = addr;
}

/* --------------------------------- */

void WAV_Reader::ReadLongLE(uint8_t** addrPtr, uint32_t* data)
{
    uint8_t* addr = *addrPtr;

    *data = 0;
    *data |= ((uint32_t)(*addr++) << 0) & 0x000000FF;     // NOLINT
    *data |= ((uint32_t)(*addr++) << 8) & 0x0000FF00;     // NOLINT
    *data |= ((uint32_t)(*addr++) << 16) & 0x00FF0000;    // NOLINT
    *data |= ((uint32_t)(*addr++) << 24) & 0xFF000000;    // NOLINT

    *addrPtr = addr;
}

/* Write int16_t word data to a little endian format byte array. */
void WAV_Reader::ReadShortLE(uint8_t** addrPtr, uint16_t* data)
{
    uint8_t* addr = *addrPtr;

    *data = 0;
    *data |= ((uint16_t)(*addr++) << 0) & 0x00FF;    // NOLINT
    *data |= ((uint16_t)(*addr++) << 8) & 0xFF00;    // NOLINT

    *addrPtr = addr;
}

/* Write IFF ChunkType data to a byte array. */
void WAV_Reader::ReadChunkType(uint8_t** addrPtr, uint32_t* cktyp)
{
    uint8_t* addr = *addrPtr;

    *cktyp = 0;
    *cktyp |= ((uint32_t)(*addr++) << 24) & 0xFF000000;    // NOLINT
    *cktyp |= ((uint32_t)(*addr++) << 16) & 0x00FF0000;    // NOLINT
    *cktyp |= ((uint32_t)(*addr++) << 8) & 0x0000FF00;     // NOLINT
    *cktyp |= ((uint32_t)(*addr++) << 0) & 0x000000FF;     // NOLINT

    *addrPtr = addr;
}


/*********************************************************************************
 * Open named file and write WAV header to the file.
 * The header includes the DATA chunk type and size.
 * Returns number of bytes written to file or negative error code.
 */
WAV_Writer::WAV_Writer(const std::string& fileName, uint32_t frameRate, uint16_t samplesPerFrame)
{
    std::array<uint8_t, WAV_HEADER_SIZE> header{0};
    uint8_t*                             addr = header.data();

    dataSize       = 0;
    dataSizeOffset = 0;

#ifdef LINUX_
    fid = fopen(file.c_str(), "wb");
    if(fid == nullptr)
#else
    errno_t err = fopen_s(&fid, fileName.c_str(), "wb");
    if(fid == nullptr || err != 0)
#endif
    {
        std::cerr << "Could not open file to write" << std::endl;
    }

    /* Write RIFF header. */
    WriteChunkType(&addr, RIFF_ID);

    /* Write RIFF size as zero for now. Will patch later. */
    WriteLongLE(&addr, 0);

    /* Write WAVE form ID. */
    WriteChunkType(&addr, WAVE_ID);

    /* Write format chunk based on AudioSample structure. */
    // https://www.isip.piconepress.com/projects/speech/software/tutorials/production/fundamentals/v1.0/section_02/s02_01_p05.html#:~:text=The%20WAV%20format%20is%20by,up%20to%202%20for%20stereo).
    WriteChunkType(&addr, FMT_ID);
    WriteLongLE(&addr, 2 + 2 + 4 + 4 + 2 + 2);
    WriteShortLE(&addr, WAVE_FORMAT_PCM);

    uint32_t bytesPerSecond = frameRate * samplesPerFrame * sizeof(int16_t);
    WriteShortLE(&addr, samplesPerFrame);
    WriteLongLE(&addr, frameRate);
    WriteLongLE(&addr, bytesPerSecond);
    WriteShortLE(&addr, (samplesPerFrame * sizeof(int16_t))); /* alignement */
    WriteShortLE(&addr, CHAR_BIT * sizeof(int16_t));          /* bits per sample */

    /* Write ID and size for 'data' chunk. */
    WriteChunkType(&addr, DATA_ID);
    /* Save offset so we can patch it later. */
    dataSizeOffset = addr - header.data();
    WriteLongLE(&addr, 0);

    size_t numWritten = fwrite(header.data(), 1, header.size(), fid);
    if(numWritten != sizeof(header))
    {
        throw std::logic_error("Number of bytes written to file does not match");
    }
}



/*********************************************************************************
 * Close WAV file.
 * Update chunk sizes so it can be read by audio applications.
 */
WAV_Writer::~WAV_Writer()
{
    std::array<uint8_t, 4> buffer{0};
    uint8_t*               bufferPtr = buffer.data();

    /* Go back to beginning of file and update DATA size */
    fseek(fid, static_cast<long>(dataSizeOffset), SEEK_SET);

    WriteLongLE(&bufferPtr, static_cast<uint32_t>(dataSize));

    fwrite(buffer.data(), 1, buffer.size(), fid);

    /* Update RIFF size */
    fseek(fid, 4, SEEK_SET);

    size_t riffSize = dataSize + (WAV_HEADER_SIZE - 8);    // NOLINT
    bufferPtr       = buffer.data();

    WriteLongLE(&bufferPtr, static_cast<uint32_t>(riffSize));

    fwrite(buffer.data(), 1, buffer.size(), fid);

    fclose(fid);
}


/*********************************************************************************
 * Write to the data chunk portion of a WAV file.
 * Returns bytes written or negative error code.
 */
void WAV_Writer::Write(int16_t* samples, size_t numSamples)
{
    for(size_t i = 0; i < numSamples; i++)
    {
        std::array<uint8_t, 2> buffer{0};
        uint8_t*               bufferPtr = buffer.data();
        WriteShortLE(&bufferPtr, *samples++);    // NOLINT

        size_t numWritten = fwrite(buffer.data(), 1, buffer.size(), fid);
        if(numWritten != buffer.size())
        {
            throw std::logic_error("Number of bytes written to file does not match");
        }
    }

    dataSize += numSamples * sizeof(int16_t);
}


/*****************************************************************************/


WAV_Reader::WAV_Reader(const std::string& fileName)
{
    std::array<uint8_t, WAV_HEADER_SIZE> header{0};
    uint8_t*                             addr = header.data();


    uint16_t short_bidon = 0;
    uint32_t long_bidon  = 0;

    /* Opening file for reading */
    std::string file{fileName};
#ifndef LINUX_
    errno_t err = fopen_s(&fid, file.c_str(), "rb");
#else
    fid = fopen(file.c_str(), "rb");
#endif

    if(fid == nullptr
#ifdef LINUX_
    )
#else
       || err != 0)
#endif
    {
        std::cerr << "Could not open file to read" << std::endl;
        throw std::runtime_error("Could not open file to read");
    }
    fread(header.data(), 1, sizeof(header), fid);

    /* Read RIFF header. */
    ReadChunkType(&addr, &long_bidon);
    if(long_bidon != RIFF_ID)
    {
        throw std::logic_error("Wrong RIFF_ID");
    }
    /* Read file size, we don't care. */
    ReadLongLE(&addr, &long_bidon);

    /* Read WAVE form ID. */
    ReadChunkType(&addr, &long_bidon);
    uint32_t wid = WAVE_ID;
    if(long_bidon != wid)
    {
        throw std::logic_error("Wrong WAVE_ID");
    }

    /* Read format chunk based on AudioSample structure. */
    ReadChunkType(&addr, &long_bidon);
    if(long_bidon != FMT_ID)
    {
        throw std::logic_error("Wrong FMT_ID");
    }
    ReadLongLE(&addr, &long_bidon);
    if(long_bidon != 16)
    {
        throw std::logic_error("1: Wrong it's 16 (" + std::to_string(long_bidon) + ")");
    }

    ReadShortLE(&addr, &short_bidon);
    if(short_bidon != WAVE_FORMAT_PCM)
    {
        throw std::logic_error("Wrong WAVE_FORMAT_PCM");
    }

    // bytesPerSecond = frameRate * samplesPerFrame * sizeof(int16_t);
    ReadShortLE(&addr, &numChannels);
    ReadLongLE(&addr, &frameRate);
    ReadLongLE(&addr, &bytesPerSecond);
    ReadShortLE(&addr, &bytesPerBlock); /* bytesPerBlock */
    ReadShortLE(&addr, &short_bidon);   /* bits per sample */
    if(short_bidon != CHAR_BIT * sizeof(int16_t))
    {
        throw std::logic_error("Invalid bits per sample, only supports 16-bits ("
                               + std::to_string(short_bidon) + ")");
    }

    /* Read ID and size for 'data' chunk. */
    ReadChunkType(&addr, &long_bidon);
    if(long_bidon != DATA_ID)
    {
        throw std::logic_error("Wrong DATA_ID chunk");
    }

    /* Save offset so we can patch it later. */
    dataSizeOffset = addr - header.data();

    ReadLongLE(&addr, &dataSize);
}


std::vector<int16_t>& WAV_Reader::Read()
{
    data.reserve(dataSize / sizeof(int16_t));

    for(size_t i = 0; i < dataSize / sizeof(int16_t); i++)
    {
        std::array<uint8_t, 2> buffer{0};
        uint8_t*               bufferPtr = buffer.data();

        size_t bytesRead = fread(buffer.data(), 1, sizeof(buffer), fid);
        if(bytesRead != sizeof(buffer))
        {
            throw std::logic_error{"fread did not read the right number of bytes"};
        }

        uint16_t sample;
        ReadShortLE(&bufferPtr, &sample);
        data.push_back(sample);
    }

    return data;
}


/*****************************************************************************/
/* END OF FILE ------------------------------------------------------------- */
