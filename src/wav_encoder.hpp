#pragma once

#define DR_WAV_IMPLEMENTATION
#include "dr_libs/dr_wav.h"

#define WAV_FORMAT DR_WAVE_FORMAT_IEEE_FLOAT


//----------------------------------------------------------------------------------------------
class wav_encoder
{
public:
    //------------------------------------------------------------------------------------------
    wav_encoder(const char *filename, uint32_t numChannels, uint32_t sampleFrequency, uint32_t bytesPerSample)
    {
        drwav_data_format format;
        format.container        = drwav_container_riff; // <-- drwav_container_riff = normal WAV files, drwav_container_w64 = Sony Wave64.
        format.format           = WAV_FORMAT;           // <-- Any of the DR_WAVE_FORMAT_* codes.
        format.channels         = numChannels;
        format.sampleRate       = sampleFrequency;
        format.bitsPerSample    = bytesPerSample * 8;
        isValid_                = drwav_init_file_write(&encoder_, filename, &format, NULL);
    }

    //------------------------------------------------------------------------------------------
    ~wav_encoder()
    {
        drwav_uninit(&encoder_);
    }

    //------------------------------------------------------------------------------------------
    bool isValid() const
    {
        return isValid_;
    }

    //------------------------------------------------------------------------------------------
    bool writePcmFrames(uint64_t framesToWrite, void *pBufferIn)
    {
        drwav_uint64 framesWritten = drwav_write_pcm_frames(&encoder_, framesToWrite, pBufferIn);
        return framesWritten == framesToWrite;
    }

private:
    //------------------------------------------------------------------------------------------
    drwav       encoder_;
    //------------------------------------------------------------------------------------------
    bool        isValid_;
};
