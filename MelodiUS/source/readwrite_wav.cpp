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
#include <cstdio>
#include <iostream>
#include <string>


/*************************************************************************************************/
/* Defines ------------------------------------------------------------------------------------- */
#define WAV_HEADER_SIZE                                                                            \
    (4 + 4 + 4 +  /* RIFF+size+WAVE */                                                             \
     4 + 4 + 16 + /* fmt chunk      */                                                             \
     4 + 4)       /* data chunk     */

/* Define WAV Chunk and FORM types as 4 byte integers. */
#define RIFF_ID (('R' << 24) | ('I' << 16) | ('F' << 8) | 'F')
#define WAVE_ID (('W' << 24) | ('A' << 16) | ('V' << 8) | 'E')
#define FMT_ID  (('f' << 24) | ('m' << 16) | ('t' << 8) | ' ')
#define DATA_ID (('d' << 24) | ('a' << 16) | ('t' << 8) | 'a')
#define FACT_ID (('f' << 24) | ('a' << 16) | ('c' << 8) | 't')

/* Errors returned by Audio_ParseSampleImage_WAV */
#define WAV_ERR_CHUNK_SIZE    (-1) /* Chunk size is illegal or past file size. */
#define WAV_ERR_FILE_TYPE     (-2) /* Not a WAV file. */
#define WAV_ERR_ILLEGAL_VALUE (-3) /* Illegal or unsupported value. Eg. 927 bits/sample */
#define WAV_ERR_FORMAT_TYPE   (-4) /* Unsupported format, eg. compressed. */
#define WAV_ERR_TRUNCATED     (-5) /* End of file missing. */

/* WAV PCM data format ID */
#define WAVE_FORMAT_PCM       (1)
#define WAVE_FORMAT_IMA_ADPCM (0x0011)


/*************************************************************************************************/
/* Private methods definitions ----------------------------------------------------------------- */

/* Write long word data to a little endian format byte array. */
void WAV_Writer::WriteLongLE(unsigned char** addrPtr, unsigned long data)
{
    unsigned char* addr = *addrPtr;
    *addr++             = (unsigned char)data;
    *addr++             = (unsigned char)(data >> 8);
    *addr++             = (unsigned char)(data >> 16);
    *addr++             = (unsigned char)(data >> 24);
    *addrPtr            = addr;
}

/* Write short word data to a little endian format byte array. */
void WAV_Writer::WriteShortLE(unsigned char** addrPtr, unsigned short data)
{
    unsigned char* addr = *addrPtr;
    *addr++             = (unsigned char)data;
    *addr++             = (unsigned char)(data >> 8);
    *addrPtr            = addr;
}

/* Write IFF ChunkType data to a byte array. */
void WAV_Writer::WriteChunkType(unsigned char** addrPtr, unsigned long cktyp)
{
    unsigned char* addr = *addrPtr;
    *addr++             = (unsigned char)(cktyp >> 24);
    *addr++             = (unsigned char)(cktyp >> 16);
    *addr++             = (unsigned char)(cktyp >> 8);
    *addr++             = (unsigned char)cktyp;
    *addrPtr            = addr;
}

/* --------------------------------- */

void WAV_Reader::ReadLongLE(unsigned char** addrPtr, unsigned long* data)
{
    unsigned char* addr = *addrPtr;

    *data = 0;
    *data |= ((unsigned long)(*addr++) << 0) & 0x000000FF;
    *data |= ((unsigned long)(*addr++) << 8) & 0x0000FF00;
    *data |= ((unsigned long)(*addr++) << 16) & 0x00FF0000;
    *data |= ((unsigned long)(*addr++) << 24) & 0xFF000000;

    *addrPtr = addr;
}

/* Write short word data to a little endian format byte array. */
void WAV_Reader::ReadShortLE(unsigned char** addrPtr, unsigned short* data)
{
    unsigned char* addr = *addrPtr;

    *data = 0;
    *data |= ((unsigned short)(*addr++) << 0) & 0x00FF;
    *data |= ((unsigned short)(*addr++) << 8) & 0xFF00;

    *addrPtr = addr;
}

/* Write IFF ChunkType data to a byte array. */
void WAV_Reader::ReadChunkType(unsigned char** addrPtr, unsigned long* cktyp)
{
    unsigned char* addr = *addrPtr;

    *cktyp = 0;
    *cktyp |= ((unsigned long)(*addr++) << 24) & 0xFF000000;
    *cktyp |= ((unsigned long)(*addr++) << 16) & 0x00FF0000;
    *cktyp |= ((unsigned long)(*addr++) << 8) & 0x0000FF00;
    *cktyp |= ((unsigned long)(*addr++) << 0) & 0x000000FF;

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
    unsigned char  header[WAV_HEADER_SIZE];
    unsigned char* addr = header;

    dataSize       = 0;
    dataSizeOffset = 0;

    std::string file{fileName};
    errno_t     err = fopen_s(&fid, file.c_str(), "wb");
    if(fid == nullptr || err != 0)
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
    WriteLongLE(&addr, 16);
    WriteShortLE(&addr, WAVE_FORMAT_PCM);

    uint32_t bytesPerSecond = frameRate * samplesPerFrame * sizeof(short);
    WriteShortLE(&addr, samplesPerFrame);
    WriteLongLE(&addr, frameRate);
    WriteLongLE(&addr, bytesPerSecond);
    WriteShortLE(&addr, (samplesPerFrame * sizeof(short))); /* bytesPerBlock */
    WriteShortLE(&addr, 16);                                /* bits per sample */

    /* Write ID and size for 'data' chunk. */
    WriteChunkType(&addr, DATA_ID);
    /* Save offset so we can patch it later. */
    dataSizeOffset = addr - header;
    WriteLongLE(&addr, 0);

    size_t numWritten = fwrite(header, 1, sizeof(header), fid);
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
    unsigned char buffer[4];

    /* Go back to beginning of file and update DATA size */
    fseek(fid, static_cast<long>(dataSizeOffset), SEEK_SET);

    unsigned char* bufferPtr = buffer;
    WriteLongLE(&bufferPtr, static_cast<unsigned long>(dataSize));

    fwrite(buffer, 1, sizeof(buffer), fid);

    /* Update RIFF size */
    fseek(fid, 4, SEEK_SET);

    size_t riffSize = dataSize + (WAV_HEADER_SIZE - 8);
    bufferPtr       = buffer;

    WriteLongLE(&bufferPtr, static_cast<unsigned long>(riffSize));

    fwrite(buffer, 1, sizeof(buffer), fid);

    fclose(fid);
}


/*********************************************************************************
 * Write to the data chunk portion of a WAV file.
 * Returns bytes written or negative error code.
 */
void WAV_Writer::Write(short* samples, size_t numSamples)
{
    for(int i = 0; i < numSamples; i++)
    {
        uint8_t  buffer[2];
        uint8_t* bufferPtr = buffer;
        WriteShortLE(&bufferPtr, *samples++);

        size_t numWritten = fwrite(buffer, 1, sizeof(buffer), fid);
        if(numWritten != sizeof(buffer))
        {
            throw std::logic_error("Number of bytes written to file does not match");
        }
    }

    dataSize += numSamples * sizeof(short);
}


/*****************************************************************************/


WAV_Reader::WAV_Reader(std::string_view fileName)
{
    unsigned char  header[WAV_HEADER_SIZE];
    unsigned char* addr = header;

    int short unsigned short_bidon;
    int long unsigned  long_bidon;

    /* Opening file for reading */
    std::string file{fileName};
    errno_t err = fopen_s(&fid, file.c_str(), "rb");
    if(fid == nullptr || err != 0)
    {
        std::cerr << "Could not open file to read" << std::endl;
    }
    fread(header, 1, sizeof(header), fid);

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
    if(long_bidon != 16)
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
    if(short_bidon != 16)
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
    dataSizeOffset = addr - header;

    ReadLongLE(&addr, &dataSize);
}


std::vector<short>& WAV_Reader::Read()
{
    data.reserve(dataSize / sizeof(short));

    for(int i = 0; i < dataSize / sizeof(short); i++)
    {
        uint8_t  buffer[2];
        uint8_t* bufferPtr = buffer;

        size_t bytesRead = fread(buffer, 1, sizeof(buffer), fid);
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
