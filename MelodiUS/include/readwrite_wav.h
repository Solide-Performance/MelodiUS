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
#pragma once

/*************************************************************************************************/
/* Includes ------------------------------------------------------------------------------------ */
#include "globaldef.h"
#include "recording.h"
#include <string_view>
#include <vector>


/*************************************************************************************************/
/* Function declarations ----------------------------------------------------------------------- */
void SaveToWav(const std::string& filename, const Recording& recording);

[[nodiscard]] Recording LoadFromWav(const std::string& filename);


/*************************************************************************************************/
/* Classes definitions ------------------------------------------------------------------------- */
class WAV_Writer
{
    FILE* fid = nullptr;

    /* Offset in file for data size. */
    size_t dataSizeOffset = 0;
    size_t dataSize       = 0;

public:
    WAV_Writer()                  = delete;
    WAV_Writer(const WAV_Writer&) = delete;
    WAV_Writer(WAV_Writer&&)      = delete;

    WAV_Writer& operator=(const WAV_Writer&) = delete;
    WAV_Writer& operator=(WAV_Writer&&) = delete;


    /*********************************************************************************
     * Open named file and write WAV header to the file.
     * The header includes the DATA chunk type and size.
     * Returns number of bytes written to file or negative error code.
     */
    WAV_Writer(const std::string& fileName, unsigned long frameRate, unsigned short samplesPerFrame);
    /*********************************************************************************
     * Close WAV file.
     * Update chunk sizes so it can be read by audio applications.
     */
    ~WAV_Writer();

    /*********************************************************************************
     * Write to the data chunk portion of a WAV file.
     * Returns bytes written or negative error code.
     */
    void Write(short* samples, size_t numSamples);


private:
    static void WriteLongLE(unsigned char** addrPtr, unsigned long data);
    static void WriteShortLE(unsigned char** addrPtr, unsigned short data);
    static void WriteChunkType(unsigned char** addrPtr, unsigned long cktyp);
};

class WAV_Reader
{
    FILE*              fid = nullptr;
    std::vector<short> data;

    /* Offset in file for data size. */
    size_t        dataSizeOffset = 0;
    unsigned long dataSize       = 0;

    unsigned long      frameRate       = 0;
    unsigned short     samplesPerFrame = 0;
    long unsigned int  bytesPerSecond  = 0;
    short unsigned int bytesPerBlock   = 0;

public:
    WAV_Reader()                  = delete;
    WAV_Reader(const WAV_Reader&) = delete;
    WAV_Reader(WAV_Reader&&)      = delete;

    WAV_Reader& operator=(const WAV_Reader&) = delete;
    WAV_Reader& operator=(WAV_Reader&&) = delete;


    /*********************************************************************************
     * Open named file and write WAV header to the file.
     * The header includes the DATA chunk type and size.
     * Returns number of bytes written to file or negative error code.
     */
    explicit WAV_Reader(const std::string& fileName);


    /*********************************************************************************
     * Close WAV file.
     * Update chunk sizes so it can be read by audio applications.
     */
    ~WAV_Reader()
    {
        fclose(fid);
    }


    /*********************************************************************************
     * Write to the data chunk portion of a WAV file.
     * Returns bytes written or negative error code.
     */
    std::vector<short>& Read();

#pragma region Accessors
    [[nodiscard]] const std::vector<short>& get_Data() const
    {
        return data;
    }

    [[nodiscard]] unsigned long get_FrameRate() const
    {
        return frameRate;
    }

    [[nodiscard]] unsigned short get_SamplesPerFrame() const
    {
        return samplesPerFrame;
    }

    [[nodiscard]] long unsigned int get_BytesPerSecond() const
    {
        return bytesPerSecond;
    }

    [[nodiscard]] short unsigned int get_BytesPerBlock() const
    {
        return bytesPerBlock;
    }
#pragma endregion

private:
    static void ReadLongLE(unsigned char** addrPtr, unsigned long* data);
    static void ReadShortLE(unsigned char** addrPtr, unsigned short* data);
    static void ReadChunkType(unsigned char** addrPtr, unsigned long* cktyp);
};
