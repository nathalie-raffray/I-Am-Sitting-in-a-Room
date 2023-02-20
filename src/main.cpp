#include <fstream>
#include <filesystem>
#include <mutex>

#include "rtaudio/RtAudio.h"

#include "program_options.hpp"
#include "audio_buffer.hpp"
#include "mp3_decoder.hpp"
#include "wav_encoder.hpp"

// Decide sample type.
#define WAV_FORMAT DR_WAVE_FORMAT_PCM   // DR_WAVE_FORMAT_IEEE_FLOAT
#define SAMPLE_TYPE RTAUDIO_SINT16      // RTAUDIO_FLOAT32
using sample_type = int16_t;            // float

#define MAX_NUMBER_OF_LOOPS 50

using decoder       = mp3_decoder;
using encoder       = wav_encoder;

std::mutex m;
std::condition_variable cv;
bool ready;


//--------------------------------------------------------------------------------------------------
enum class device_type
{
    input,
    output
};

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
int play_and_record(void *pOutputBuffer, void *pInputBuffer, unsigned int nBufferFrames,
    double streamTime, RtAudioStreamStatus status, void *pUserData)
{
    if (status)
    {
        if (status == RTAUDIO_INPUT_OVERFLOW)
        {
            std::cout << "Stream underflow detected: Input data was discarded because of an overflow "
                "condition at the driver." << std::endl;
        }
        if (status == RTAUDIO_OUTPUT_UNDERFLOW)
        {
            std::cout << "Stream underflow detected: The output buffer ran low, likely causing a gap  "
                "in the output sound." << std::endl;
        }
    }

    std::cout << "number buffer frames: " << nBufferFrames << std::endl;

    auto pAudioBuffer = reinterpret_cast<audio_buffer<sample_type> *>(pUserData);
    if (pAudioBuffer->reachedEndOfBuffer())
    {
        // Notify main thread we are done.
        std::lock_guard lk(m);
        ready = true;
        cv.notify_one();

        // To stop the stream and drain the output buffer (dac.stopStream()), return 1.
        // Abort the stream immediately (dac.abortStream()), return 2.
        // This callback function is only called between dac.openStream() and dac.stopStream()
        // so after returning 1, it won't be called again.
        return 1;
    }

    // Play audio.
    pAudioBuffer->read(pOutputBuffer, nBufferFrames);

    // Record audio.
    static auto firstIteration = true;
    if (firstIteration)
    {
        // First time are in play_and_record() nothing has played yet, so we wait for
        // next iteration to record.
        firstIteration = false;
    }
    else
    {
        // Recording...
        pAudioBuffer->write(pInputBuffer, nBufferFrames);
    }

    return 0;
}

//--------------------------------------------------------------------------------------------------
bool try_begin_playing_and_recording_audio(RtAudio &dac, decoder &decoder, 
    audio_buffer<sample_type> &audioBuffer, int32_t inputDeviceId, int32_t outputDeviceId)
{
    RtAudio::StreamParameters iParams, oParams;
    oParams.deviceId            = outputDeviceId;
    oParams.nChannels           = decoder.getChannelCount();
    oParams.firstChannel        = 0;
    iParams.deviceId            = inputDeviceId;
    iParams.nChannels           = decoder.getChannelCount();
    iParams.firstChannel        = 0;

    const auto sampleRate       = decoder.getSampleFrequency();
    constexpr auto latencyInS   = 40u;
    auto desiredBufferFrames    = (uint32_t)std::min(
        decoder.getSampleCount(), 
        (uint64_t)(decoder.getSampleFrequency()) * latencyInS
    );

    RtAudio::StreamOptions streamOptions;
    streamOptions.flags         = RTAUDIO_SCHEDULE_REALTIME;
    streamOptions.priority      = std::numeric_limits<int>::max();

    try 
    {
        dac.openStream(&oParams, &iParams, SAMPLE_TYPE, sampleRate, &desiredBufferFrames, 
            &play_and_record, (void *)&audioBuffer/*, &streamOptions */ );
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
bool is_correct_device_type(const RtAudio::DeviceInfo &info, device_type deviceType)
{
    return deviceType == device_type::input ? info.inputChannels > 0 : info.outputChannels > 0;
}

//--------------------------------------------------------------------------------------------------
bool is_valid_device_id(int id, device_type deviceType, RtAudio &dac)
{
    const auto deviceCount = static_cast<int32_t>(dac.getDeviceCount());
    if (id < 0 || id >= deviceCount)
    {
        return false;
    }

    return is_correct_device_type(dac.getDeviceInfo(id), deviceType);
}

//--------------------------------------------------------------------------------------------------
void list_devices_of_type(device_type deviceType, RtAudio &dac)
{
    RtAudio::DeviceInfo info;
    for (auto i = 0u; i < dac.getDeviceCount(); i++)
    {
        info = dac.getDeviceInfo(i);
        if (info.probed == true && is_correct_device_type(info, deviceType))
        {
            std::cout << i << ": " << info.name << std::endl;
        }
    }
}

//--------------------------------------------------------------------------------------------------
int get_valid_device_id_from_user(device_type deviceType, RtAudio &dac)
{
    auto userInput = std::string("");
    std::cin >> userInput;

    while (!is_valid_device_id(std::stoi(userInput), deviceType, dac))
    {
        std::cout << "Invalid input. Try again." << std::endl;
        std::cin >> userInput;
    }

    // clear cin
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return std::stoi(userInput);
}

//--------------------------------------------------------------------------------------------------
int get_user_specified_device_id(RtAudio &dac, device_type deviceType)
{
    const auto deviceTypeStr = deviceType == device_type::input ? "input" : "output";
    std::cout << "Please specify from the list below your chosen " << deviceTypeStr << " device. "
        "Enter the number preceding it to choose it." << std::endl;

    list_devices_of_type(deviceType, dac);

    const auto deviceId = get_valid_device_id_from_user(deviceType, dac);

    std::cout << "Your chosen " << deviceTypeStr << " device is " 
        << dac.getDeviceInfo(deviceId).name << std::endl << std::endl;

    return deviceId;
}

//--------------------------------------------------------------------------------------------------
bool valid_loop_number(int loops)
{
    return loops > 0 && loops < MAX_NUMBER_OF_LOOPS;
}

//--------------------------------------------------------------------------------------------------
int get_user_specified_number_of_loops()
{
    std::cout << "How many loops of audio do you want?" << std::endl;

    auto userInput = std::string("");
    std::cin >> userInput;

    while (!valid_loop_number(std::stoi(userInput)))
    {
        std::cout << "Invalid loop number. Please specify number of loops between 0 and " 
            << MAX_NUMBER_OF_LOOPS << ". Try again." << std::endl;
        std::cin >> userInput;
    }

    // clear cin
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << std::endl;

    return std::stoi(userInput);
}

//--------------------------------------------------------------------------------------------------
void init_audio_buffer(audio_buffer<sample_type> &audioBuffer, decoder &decoder)
{
    auto song = std::vector<uint8_t>(decoder.getSampleCount() * decoder.getChannelCount() * sizeof(sample_type));
    decoder.readPcmFrames(decoder.getSampleCount(), (float *)song.data());
    audioBuffer.write(song.data(), static_cast<uint32_t>(decoder.getSampleCount()));
}

//--------------------------------------------------------------------------------------------------
bool export_song(const char *outputFilename, decoder &decoder, audio_buffer<sample_type> &audioBuffer, uint16_t loopNumber)
{
    auto upEncoder = std::make_unique<encoder>(outputFilename, decoder.getChannelCount(),
        decoder.getSampleFrequency(), WAV_FORMAT, static_cast<uint32_t>(sizeof(sample_type)));

    return upEncoder->isValid() && 
        upEncoder->writePcmFrames(decoder.getSampleCount() * loopNumber, audioBuffer.data());
}

//--------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    const auto &options = program_options(argc, argv);
    if (!options.areValid)
    {
        program_options::print_usage();
        return -1;
    }

    if (!std::filesystem::exists(options.inputFile))
    {
        std::cout << "Error: The file path provided (" << options.inputFile << ") doesn't exist." << std::endl;
        return -1;
    }

    auto mp3Bytes = read_file(options.inputFile);
    if (mp3Bytes.empty())
    {
        std::cout << "Error: Failed to read the contents of file at " << options.inputFile << "." << std::endl;
        return -1;
    }

    auto upDecoder = std::make_unique<decoder>(mp3Bytes.data(), mp3Bytes.size());
    if (!upDecoder || !upDecoder->isValid())
    {
        std::cout << "Error: Failed to decode mp3 file " << options.inputFile << "." << std::endl;
        return -1;
    }

    RtAudio dac;
    if (dac.getDeviceCount() < 1) 
    {
        std::cout << "Error: No audio devices found!" << std::endl;
        return -1;
    }

    const auto chosenInputDeviceId  = get_user_specified_device_id(dac, device_type::input);
    const auto chosenOutputDeviceId = get_user_specified_device_id(dac, device_type::output);
    const auto loopNumber           = get_user_specified_number_of_loops();

    const auto totalNumberSamples   = upDecoder->getSampleCount() * loopNumber;
    auto audioBuffer                = audio_buffer<sample_type>(totalNumberSamples, upDecoder->getChannelCount());

    init_audio_buffer(audioBuffer, *upDecoder);

    if (!try_begin_playing_and_recording_audio(dac, *upDecoder, audioBuffer, chosenInputDeviceId, chosenOutputDeviceId))
    {
        std::cout << "Error: Failed to play audio." << std::endl;
        return -1;
    }

    // Wait until play_and_record ends.
    std::unique_lock lk(m);
    cv.wait(lk, [] { return ready; });

    std::cout << "Stream ended..." << std::endl;

    // Export song.
    const auto outputFilepath 
        = std::filesystem::path(options.outputFolder) / 
        (std::filesystem::path(options.inputFile).stem().string() + ".wav");

    if (std::filesystem::exists(outputFilepath))
    {
        std::filesystem::remove(outputFilepath);
    }
    else
    {
        std::filesystem::create_directory(outputFilepath.parent_path());
    }
    
    if (!export_song(outputFilepath.string().c_str(), *upDecoder, audioBuffer, loopNumber))
    {
        std::cout << "Error : failed to write out all the audio samples into the exported "
            "wav file." << std::endl;
        return -1;
    }
    
    return 0;
}
