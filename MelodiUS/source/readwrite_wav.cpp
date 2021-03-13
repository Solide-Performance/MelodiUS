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
/* Defines ------------------------------------------------------------------------------------- */
constexpr size_t WAV_HEADER_SIZE = (4 + 4 + 4 +  /* RIFF+size+WAVE */
                                    4 + 4 + 16 + /* fmt chunk      */
                                    4 + 4);      /* data chunk     */

/* Define WAV Chunk and FORM types as 4 byte integers. */
constexpr uint32_t RIFF_ID = static_cast<uint32_t>(u8'R' << 24U)
                             | static_cast<uint32_t>(u8'I' << 16U)
                             | static_cast<uint32_t>(u8'F' << 8U) | u8'F';
constexpr uint32_t WAVE_ID = static_cast<uint32_t>(u8'W' << 24U)
                             | static_cast<uint32_t>(u8'A' << 16U)
                             | static_cast<uint32_t>(u8'V' << 8U) | u8'E';
constexpr uint32_t FMT_ID = static_cast<uint32_t>(u8'f' << 24U)
                            | static_cast<uint32_t>(u8'm' << 16U)
                            | static_cast<uint32_t>(u8't' << 8U) | u8' ';
constexpr uint32_t DATA_ID = static_cast<uint32_t>(u8'd' << 24U)
                             | static_cast<uint32_t>(u8'a' << 16U)
                             | static_cast<uint32_t>(u8't' << 8U) | u8'a';

/* WAV PCM data format ID */
constexpr short WAVE_FORMAT_PCM = 1;
// constexpr uint32_t WAVE_FORMAT_IMA_ADPCM = 0x0011;


/*************************************************************************************************/
/* Static functions declarations --------------------------------------------------------------- */
[[nodiscard]] std::vector<short> Samples_FloatToShort(const std::vector<float>& inVec);
[[nodiscard]] std::vector<float> Samples_ShortToFloat(const std::vector<short>& inVec);


/*************************************************************************************************/
/* Function definitions ------------------------------------------------------------------------ */
void SaveToWav(std::string_view filename, const Recording& recording)
{
    std::vector<short> shortData = Samples_FloatToShort(recording.getSamples());

    WAV_Writer writer{
      filename,
      static_cast<unsigned long>(recording.getSampleRate() * recording.getNumChannels()),
      1};

    writer.Write(shortData.data(), shortData.size());
}

Recording LoadFromWav(std::string_view filename)
{
    WAV_Reader reader{filename};
    reader.Read();

    std::vector<float> floatData = Samples_ShortToFloat(reader.get_Data());

    // @todo
    // HARDCODED '2' & '1' !!!!!!! TO REMOVE
    return {&floatData.front(), &floatData.back(), (size_t)(reader.get_FrameRate() / 2), 1, 2};
}


/*************************************************************************************************/
/* Static functions definitions ---------------------------------------------------------------- */
std::vector<short> Samples_FloatToShort(const std::vector<float>& inVec)
{
    std::vector<short> shortData = std::vector<short>(inVec.size());

    // https://stackoverflow.com/a/56213245/10827197
    for(size_t i = 0; i < inVec.size(); i++)
    {
        float floatData = inVec[i] * 32767;
        shortData[i]    = (short)floatData;
    }

    return shortData;
}

std::vector<float> Samples_ShortToFloat(const std::vector<short>& inVec)
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
void WAV_Writer::WriteLongLE(unsigned char** addrPtr, unsigned long data)
{
    unsigned char* addr = *addrPtr;

    *addr++ = (unsigned char)data;            // NOLINT
    *addr++ = (unsigned char)(data >> 8);     // NOLINT
    *addr++ = (unsigned char)(data >> 16);    // NOLINT
    *addr++ = (unsigned char)(data >> 24);    // NOLINT

    *addrPtr = addr;
}

/* Write short word data to a little endian format byte array. */
void WAV_Writer::WriteShortLE(unsigned char** addrPtr, unsigned short data)
{
    unsigned char* addr = *addrPtr;

    *addr++ = (unsigned char)data;           // NOLINT
    *addr++ = (unsigned char)(data >> 8);    // NOLINT

    *addrPtr = addr;
}

/* Write IFF ChunkType data to a byte array. */
void WAV_Writer::WriteChunkType(unsigned char** addrPtr, unsigned long cktyp)
{
    unsigned char* addr = *addrPtr;

    *addr++ = (unsigned char)(cktyp >> 24);    // NOLINT
    *addr++ = (unsigned char)(cktyp >> 16);    // NOLINT
    *addr++ = (unsigned char)(cktyp >> 8);     // NOLINT
    *addr++ = (unsigned char)cktyp;            // NOLINT

    *addrPtr = addr;
}

/* --------------------------------- */

void WAV_Reader::ReadLongLE(unsigned char** addrPtr, unsigned long* data)
{
    unsigned char* addr = *addrPtr;

    *data = 0;
    *data |= ((unsigned long)(*addr++) << 0) & 0x000000FF;     // NOLINT
    *data |= ((unsigned long)(*addr++) << 8) & 0x0000FF00;     // NOLINT
    *data |= ((unsigned long)(*addr++) << 16) & 0x00FF0000;    // NOLINT
    *data |= ((unsigned long)(*addr++) << 24) & 0xFF000000;    // NOLINT

    *addrPtr = addr;
}

/* Write short word data to a little endian format byte array. */
void WAV_Reader::ReadShortLE(unsigned char** addrPtr, unsigned short* data)
{
    unsigned char* addr = *addrPtr;

    *data = 0;
    *data |= ((unsigned short)(*addr++) << 0) & 0x00FF;    // NOLINT
    *data |= ((unsigned short)(*addr++) << 8) & 0xFF00;    // NOLINT

    *addrPtr = addr;
}

/* Write IFF ChunkType data to a byte array. */
void WAV_Reader::ReadChunkType(unsigned char** addrPtr, unsigned long* cktyp)
{
    unsigned char* addr = *addrPtr;

    *cktyp = 0;
    *cktyp |= ((unsigned long)(*addr++) << 24) & 0xFF000000;    // NOLINT
    *cktyp |= ((unsigned long)(*addr++) << 16) & 0x00FF0000;    // NOLINT
    *cktyp |= ((unsigned long)(*addr++) << 8) & 0x0000FF00;     // NOLINT
    *cktyp |= ((unsigned long)(*addr++) << 0) & 0x000000FF;     // NOLINT

    *addrPtr = addr;
}


/*********************************************************************************
 * Open named file and write WAV header to the file.
 * The header includes the DATA chunk type and size.
 * Returns number of bytes written to file or negative error code.
 */
WAV_Writer::WAV_Writer(std::string_view fileName,
                       unsigned long    frameRate,
                       unsigned short   samplesPerFrame)
{
    std::array<uint8_t, WAV_HEADER_SIZE> header{0};
    uint8_t*                             addr = header.data();

    dataSize       = 0;
    dataSizeOffset = 0;

    std::string file{fileName};
#ifndef LINUX_
    errno_t err = fopen_s(&fid, file.c_str(), "wb");
#else
    fid = fopen(file.c_str(), "wb");
#endif

    if(fid == nullptr
#ifdef LINUX_
    )
#else
       || err != 0)
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
    WriteChunkType(&addr, FMT_ID);
    WriteLongLE(&addr, CHAR_BIT * sizeof(short));
    WriteShortLE(&addr, WAVE_FORMAT_PCM);

    uint32_t bytesPerSecond = frameRate * samplesPerFrame * sizeof(short);
    WriteShortLE(&addr, samplesPerFrame);
    WriteLongLE(&addr, frameRate);
    WriteLongLE(&addr, bytesPerSecond);
    WriteShortLE(&addr, (samplesPerFrame * sizeof(short))); /* bytesPerBlock */
    WriteShortLE(&addr, CHAR_BIT * sizeof(short));          /* bits per sample */

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

    WriteLongLE(&bufferPtr, static_cast<unsigned long>(dataSize));

    fwrite(buffer.data(), 1, buffer.size(), fid);

    /* Update RIFF size */
    fseek(fid, 4, SEEK_SET);

    size_t riffSize = dataSize + (WAV_HEADER_SIZE - 8);    // NOLINT
    bufferPtr       = buffer.data();

    WriteLongLE(&bufferPtr, static_cast<unsigned long>(riffSize));

    fwrite(buffer.data(), 1, buffer.size(), fid);

    fclose(fid);
}


/*********************************************************************************
 * Write to the data chunk portion of a WAV file.
 * Returns bytes written or negative error code.
 */
void WAV_Writer::Write(short* samples, size_t numSamples)
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

    dataSize += numSamples * sizeof(short);
}


/*****************************************************************************/


WAV_Reader::WAV_Reader(std::string_view fileName)
{
    std::array<uint8_t, WAV_HEADER_SIZE> header{0};
    uint8_t*                             addr = header.data();


    unsigned short short_bidon = 0;
    unsigned long  long_bidon  = 0;

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

    /* Write RIFF header. */
    ReadChunkType(&addr, &long_bidon);
    if(long_bidon != RIFF_ID)
    {
        throw std::logic_error("Wrong RIFF_ID");
    }
    /* Write RIFF size as zero for now. Will patch later. */
    ReadLongLE(&addr, &long_bidon);

    /* Read WAVE form ID. */
    ReadChunkType(&addr, &long_bidon);
    unsigned long wid = WAVE_ID;
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
    if(long_bidon != CHAR_BIT * sizeof(short))
    {
        throw std::logic_error("Wrong it's 16 (" + std::to_string(long_bidon) + ")");
    }

    ReadShortLE(&addr, &short_bidon);
    if(short_bidon != WAVE_FORMAT_PCM)
    {
        throw std::logic_error("Wrong WAVE_FORMAT_PCM");
    }

    // bytesPerSecond = frameRate * samplesPerFrame * sizeof(short);
    ReadShortLE(&addr, &samplesPerFrame);
    ReadLongLE(&addr, &frameRate);
    ReadLongLE(&addr, &bytesPerSecond);
    ReadShortLE(&addr, &bytesPerBlock); /* bytesPerBlock */
    ReadShortLE(&addr, &short_bidon);   /* bits per sample */
    if(short_bidon != CHAR_BIT * sizeof(short))
    {
        throw std::logic_error("Wrong it's 16 (" + std::to_string(short_bidon) + ")");
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


std::vector<short>& WAV_Reader::Read()
{
    data.reserve(dataSize / sizeof(short));

    for(size_t i = 0; i < dataSize / sizeof(short); i++)
    {
        std::array<uint8_t, 2> buffer{0};
        uint8_t*               bufferPtr = buffer.data();
        ;

        size_t bytesRead = fread(buffer.data(), 1, sizeof(buffer), fid);
        if(bytesRead != sizeof(buffer))
        {
            throw std::logic_error{"fread did not read the right number of bytes"};
        }

        unsigned short sample;
        ReadShortLE(&bufferPtr, &sample);
        data.push_back(sample);
    }

    return data;
}
