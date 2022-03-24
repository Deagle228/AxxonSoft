#include "Result.h"
  
int main(int argc, char* argv[])
{
    const std::filesystem::path directory(argv[1]);
    Result resultReference;
    if (argc == 2)
    {
        if (std::filesystem::is_directory(argv[1]))
        {
            std::cout << "Number of strings " << resultReference.firstTaskResult(directory);
        }
        else
        {
            std::cout << "Directory isn`t exist!";
            return 0;
        }
    }
    else if (argc == 3)
    {
        if (std::filesystem::is_directory(argv[1]))
        {
            std::cout << "Number of occurrences "<< resultReference.secondTaskResult(directory, argv[2]);
        }
        else
        {
            std::cout << "Directory isn`t exist!";
            return 0;
        }
    }
    else
    {
        std::cout << "Error! Wrong count of members.";
        return 0;
    }

    return 0;
}