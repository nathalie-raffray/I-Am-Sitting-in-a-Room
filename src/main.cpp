#include <fstream>
#include <filesystem>

#include "rtaudio/RtAudio.h"

#include "mp3_decoder.hpp"


//--------------------------------------------------------------------------------------------------
std::vector<char> read_file(const std::string &filePath)
{
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size))
    {
        return {};
    }

    return buffer;
}

//--------------------------------------------------------------------------------------------------
int sing_song(void *pOutputBuffer, void *pInputBuffer, unsigned int nBufferFrames,
    double streamTime, RtAudioStreamStatus status, void *pUserData)
{
    if (status)
    {
        std::cout << "Stream underflow detected!" << std::endl;
    }

    auto pOutputFloatBuffer = reinterpret_cast<float *>(pOutputBuffer);
    auto pMp3Decoder        = reinterpret_cast<mp3_decoder *>(pUserData);

    pMp3Decoder->readPcmFrames(nBufferFrames, pOutputFloatBuffer);

    return 0;
}

//--------------------------------------------------------------------------------------------------
bool try_begin_playing_audio(RtAudio &dac, mp3_decoder &decoder)
{
    RtAudio::StreamParameters parameters;
    parameters.deviceId     = dac.getDefaultOutputDevice();
    parameters.nChannels    = decoder.getChannelCount();
    parameters.firstChannel = 0;
    const auto sampleRate   = decoder.getSampleFrequency();
    auto bufferFrames       = 256u; // 256 sample frames

    try 
    {
        dac.openStream(&parameters, NULL, RTAUDIO_FLOAT32,
            sampleRate, &bufferFrames, &sing_song, (void *)&decoder);
        dac.startStream();
    }
    catch (RtAudioError &e)
    {
        e.printMessage();
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool try_end_playing_audio(RtAudio &dac)
{
    auto success = true;
    try
    {
        dac.stopStream();
    }
    catch (RtAudioError &e)
    {
        e.printMessage();
        success = false;
    }

    if (dac.isStreamOpen())
    {
        dac.closeStream();
    }

    return success;
}

//--------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    const auto filePath = "../../res/input/i_am_sitting_in_a_room.mp3";
    if (!std::filesystem::exists(filePath))
    {
        std::cout << "Error: The file path provided (" << filePath << ") doesn't exist." << std::endl;
        return -1;
    }

    auto mp3Bytes = read_file(filePath);
    if (mp3Bytes.empty())
    {
        std::cout << "Error: Failed to read the contents of file at " << filePath << "." << std::endl;
        return -1;
    }

    mp3_decoder decoder(mp3Bytes.data(), mp3Bytes.size());
    if (!decoder.isValid())
    {
        std::cout << "Error: Failed to decode mp3 file " << filePath << "." << std::endl;
        return -1;
    }

    RtAudio dac;
    if (dac.getDeviceCount() < 1) 
    {
        std::cout << "Error: No audio devices found!" << std::endl;
        return -1;
    }

    if (!try_begin_playing_audio(dac, decoder))
    {
        std::cout << "Error: Failed to play audio." << std::endl;
        return -1;
    }
    
    char input;
    std::cout << "Playing ... press <enter> to quit." << std::endl;
    std::cin.get(input);

    if (!try_end_playing_audio(dac))
    {
        std::cout << "Error: Failed to stop stream." << std::endl;
        return -1;
    }

    return 0;
}
