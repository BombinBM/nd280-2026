#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

void PrintUsage(const char* program)
{
    std::cerr << "Usage:\n"
              << "  " << program << " [--keep-inputs|--delete-inputs] <output.root> <input1.root> [input2.root ...]\n\n"
              << "Options:\n"
              << "  --keep-inputs     Keep original files after merge (default)\n"
              << "  --delete-inputs   Delete original files after successful merge\n"
              << "  -h, --help        Show this help\n\n"
              << "Examples:\n"
              << "  " << program << " --keep-inputs merged.root data/37/mup/*.root\n"
              << "  " << program << " --delete-inputs merged.root file1.root file2.root file3.root\n";
}

int main(int argc, char** argv)
{
    bool deleteInputs = false;
    std::string output;
    std::vector<std::string> inputs;

    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];

        if (arg == "-h" || arg == "--help")
        {
            PrintUsage(argv[0]);
            return 0;
        }

        if (arg == "--delete-inputs" || arg == "--remove-inputs")
        {
            deleteInputs = true;
            continue;
        }

        if (arg == "--keep-inputs" || arg == "--preserve-inputs")
        {
            deleteInputs = false;
            continue;
        }

        if (output.empty())
        {
            output = arg;
            continue;
        }

        inputs.push_back(arg);
    }

    if (output.empty() || inputs.empty())
    {
        PrintUsage(argv[0]);
        return 1;
    }

    std::string command = "hadd \"" + output + "\"";
    for (const auto& input : inputs)
    {
        if (input == output)
        {
            std::cerr << "ERROR: Output file cannot be one of the input files: " << input << std::endl;
            return 1;
        }
        
        command += " \"" + input + "\"";
    }

    std::cout << "  ◀ Running: " << command << std::endl;

    const int status = std::system(command.c_str());
    if (status != 0)
    {
        std::cerr << "ERROR: hadd exited with status " << status << std::endl;
        return status;
    }

    std::cout << "  ◀ Merged ROOT files written to: " << output << std::endl;

    if (deleteInputs)
    {
        for (const auto& input : inputs)
        {
            if (std::remove(input.c_str()) == 0)
            {
                std::cout << "  ◀ Deleted original file: " << input << std::endl;
            }
            else
            {
                std::cerr << "WARNING: failed to delete original file: " << input << std::endl;
            }
        }
    }
    else
    {
        std::cout << "  ◀ Original files were kept." << std::endl;
    }

    return 0;
}
