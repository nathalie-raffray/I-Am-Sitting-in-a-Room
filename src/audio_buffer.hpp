#pragma once

//--------------------------------------------------------------------------------------------------
template<typename _SampleType>
class audio_buffer
{
public:
    //----------------------------------------------------------------------------------------------
    audio_buffer(uint64_t sampleCount, uint8_t numChannels)
        : buffer_(sampleCount * numChannels * sizeof(_SampleType))
        , readTell_(0)
        , writeTell_(0)
        , numChannels_(numChannels)
    {
    }

    //----------------------------------------------------------------------------------------------
    void write(void *pSrc, uint32_t numFrames)
    {
        const auto sizeInBytes          = numFrames * numChannels_ * sizeof(_SampleType);
        const auto sizeToWriteInBytes   = std::min(sizeInBytes, buffer_.size() - writeTell_);

        memcpy(buffer_.data() + writeTell_, pSrc, sizeToWriteInBytes);
        writeTell_ += sizeToWriteInBytes;
    }

    //----------------------------------------------------------------------------------------------
    void read(void *pDst, uint32_t numFrames)
    {
        const auto sizeInBytes          = numFrames * numChannels_ * sizeof(_SampleType);
        const auto sizeToReadInBytes    = std::min(sizeInBytes, buffer_.size() - readTell_);

        memcpy(pDst, buffer_.data() + readTell_, sizeToReadInBytes);
        readTell_ += sizeToReadInBytes;
    }

    //----------------------------------------------------------------------------------------------
    bool reachedEndOfBuffer() const 
    {
        return readTell_ == buffer_.size() && writeTell_ == buffer_.size();
    }

private:
    //----------------------------------------------------------------------------------------------
    std::vector<uint8_t>    buffer_;
    //----------------------------------------------------------------------------------------------
    uint64_t                readTell_;
    //----------------------------------------------------------------------------------------------
    uint64_t                writeTell_;
    //----------------------------------------------------------------------------------------------
    uint8_t                 numChannels_;
};

