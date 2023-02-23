#pragma once

#include <cstdint>

#define DR_MP3_IMPLEMENTATION
#include "dr_libs/dr_mp3.h"


//----------------------------------------------------------------------------------------------
class mp3_decoder
{
public:
    //------------------------------------------------------------------------------------------
    mp3_decoder(const void *pData, size_t dataSize)
    {
        isValid_ = drmp3_init_memory(&decoder_, pData, dataSize, NULL);
        sampleCount_ = isValid_ ? drmp3_get_pcm_frame_count(&decoder_) : 0;
    }

    //------------------------------------------------------------------------------------------
    ~mp3_decoder()
    {
        drmp3_uninit(&decoder_);
    }

    //------------------------------------------------------------------------------------------
    bool isValid() const
    {
        return isValid_;
    }

    //------------------------------------------------------------------------------------------
    uint64_t getSampleCount() const
    {
        return sampleCount_;
    }

    //------------------------------------------------------------------------------------------
    uint8_t getChannelCount() const
    {
        return uint8_t(decoder_.channels);
    }

    //------------------------------------------------------------------------------------------
    uint32_t getSampleFrequency() const
    {
        return decoder_.sampleRate;
    }

    //------------------------------------------------------------------------------------------
    void readPcmFrames(uint64_t framesToRead, float *pBufferOut)
    {
        drmp3_read_pcm_frames_f32(&decoder_, framesToRead, pBufferOut);
    }

    //------------------------------------------------------------------------------------------
    void readPcmFrames(uint64_t framesToRead, short *pBufferOut)
    {
        drmp3_read_pcm_frames_s16(&decoder_, framesToRead, pBufferOut);
    }

private:
    //------------------------------------------------------------------------------------------
    drmp3       decoder_;
    //------------------------------------------------------------------------------------------
    bool        isValid_;
    //------------------------------------------------------------------------------------------
    uint64_t    sampleCount_;
};
