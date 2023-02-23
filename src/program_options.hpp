#pragma once


//----------------------------------------------------------------------------------------------
struct program_options
{
    //------------------------------------------------------------------------------------------
    static void print_usage()
    {
        std::wcout <<
            "Invalid usage.\n"
            "I am sitting in a room different from the one you are in now.\n"
            "\n"
            "  I-Am-Sitting-in-a-Room [-i|-o]"
            "\n"
            "Options description:"
            "\n"
            "  --input          , -i    Path to a supported sound file (mp3).\n"
            "  --output         , -o    Path to the folder where the exported sound wav file will be generated.\n"
        << std::endl;
    }

    //------------------------------------------------------------------------------------------
    program_options(int argc, wchar_t *argv[])
    {
        for (auto i = 0; i < argc; ++i)
        {
            if (argv[i] == std::wstring(L"--input") || argv[i] == std::wstring(L"-i"))
            {
                inputFile = argv[++i];
            }
            else if (argv[i] == std::wstring(L"--output") || argv[i] == std::wstring(L"-o"))
            {
                outputFolder = argv[++i];
            }
        }

        areValid = !inputFile.empty() && !outputFolder.empty();
    }

    //------------------------------------------------------------------------------------------
    bool            areValid = false;
    //------------------------------------------------------------------------------------------
    std::wstring    inputFile;
    std::wstring    outputFolder;
};
