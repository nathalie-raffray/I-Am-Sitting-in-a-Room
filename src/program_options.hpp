#pragma once


//----------------------------------------------------------------------------------------------
struct program_options
{
    //------------------------------------------------------------------------------------------
    static void print_usage()
    {
        std::cout <<
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
    program_options(int argc, char *argv[])
    {
        for (auto i = 0; i < argc; ++i)
        {
            if (argv[i] == std::string("--input") || argv[i] == std::string("-i"))
            {
                inputFile = argv[++i];
            }
            else if (argv[i] == std::string("--output") || argv[i] == std::string("-o"))
            {
                outputFolder = argv[++i];
            }
        }

        areValid = !inputFile.empty() && !outputFolder.empty();
    }

    //------------------------------------------------------------------------------------------
    bool        areValid = false;
    //------------------------------------------------------------------------------------------
    std::string inputFile;
    std::string outputFolder;
};
