/*constantes et autres includes*/
#include <stdio.h>






///////////////////////////////////////////////////////////////////////////////////////////


/* Write long word data to a little endian format byte array. */
static void WriteLongLE(unsigned char** addrPtr, unsigned long* data)
{    // ajouter reference a data
    // inverser les deux cotes du assign
    unsigned char* addr = *addrPtr;
    *data               = *addr++;
    *data >> 8          = *addr++;
    *data >> 16         = *addr++;
    *data >> 24         = *addr++;
    // pas inverser
    *addrPtr = addr;
}

/* Write short word data to a little endian format byte array. */
static void WriteShortLE(unsigned char** addrPtr, unsigned short* data)
{
    // inverser les deux cotes du assign
    unsigned char* addr = *addrPtr;
    *data               = *addr++;
    *data >> 8          = *addr++;
    *addrPtr            = addr;
}

/* Write IFF ChunkType data to a byte array. */
static void WriteChunkType(unsigned char** addrPtr, unsigned long* cktyp)
{
    // inverser les deux cotes du assign
    unsigned char* addr = *addrPtr;
    *cktyp >> 24        = *addr++;
    *cktyp >> 16 = *addr++ * cktyp >> 8 = *addr++;
    *cktyp                              = *addr++;
    *addrPtr                            = addr;
}



///////////////////////////////////////////////////////////////////////////////////////



long Audio_WAV_OpenWriter(WAV_Writer*        writer,
                          const char*        fileName,
                          long unsigned int  frameRate,
                          short unsigned int samplesPerFrame)
{
    long unsigned int  bytesPerSecond;
    short unsigned int bytesPerBlock;
    short unsigned int WAVE_FORMAT_PCM;
    unsigned char      header[WAV_HEADER_SIZE];
    unsigned char*     addr = header;
    int                numWritten;
    unsigned long      RIFF_ID;
    unsigned long      WAVE_ID;
    unsigned long      FMT_ID;
    unsigned long      DATA_ID;
    unsigned long      FACT_ID;
    unsigned long      RIFF_SIZE;
    unsigned long      LONG_BIDON;
    short unsigned int SHORT_BIDON;
    /* ne pas toucher*/
    writer->dataSize       = 0;
    writer->dataSizeOffset = 0;


    // changer le wb
    writer->fid = fopen(fileName, "r");
    if(writer->fid == NULL)
    {
        return -1;
    }
    fread(header, 1, sizeof(header), writer->fid);


    /* Write RIFF header. */
    WriteChunkType(&addr, &RIFF_ID);



    /* Write RIFF size as zero for now. Will patch later. */
    WriteLongLE(&addr, &RIFF_SIZE);



    /* Write WAVE form ID. */
    WriteChunkType(&addr, &WAVE_ID);



    /* Write format chunk based on AudioSample structure. */
    /* le laisser comme ca*/
    WriteChunkType(&addr, &FMT_ID);
    WriteLongLE(&addr, &LONG_BIDON);
    WriteShortLE(&addr, &WAVE_FORMAT_PCM);



    /*l'inverser pour que le calcul vienne apres et isole les differente variable, sinon le laisser
     * comme ca*/
    // bytesPerSecond = frameRate * samplesPerFrame * sizeof(short);
    WriteShortLE(&addr, &samplesPerFrame);
    WriteLongLE(&addr, &frameRate);
    WriteLongLE(&addr, &bytesPerSecond);
    WriteShortLE(&addr, &bytesPerBlock); /* bytesPerBlock */
    WriteShortLE(&addr, &SHORT_BIDON);   /* bits per sample */



    /*le laisser comme ca*/
    /* Write ID and size for 'data' chunk. */
    WriteChunkType(&addr, &DATA_ID);



    /*as is*/
    /* Save offset so we can patch it later. */
    writer->dataSizeOffset = (int)(addr - header);



    /*le laisser comme ca*/
    WriteLongLE(&addr, &LONG_BIDON);




    /*l'enlever et le remplacer par un read au debut du code. la nature exacte de ce read depend de
     mon manque de comprehension de writer en general je comprends mal les variables qui composent
     ce bloc*/
    // numWritten = fwrite(header, 1, sizeof(header), writer->fid);
    /*if(numWritten != sizeof(header))
        return -1;
    */
    // return (int)numWritten;
    return 0;
}

int main()
{
    printf("allo");
    return 0;
}
