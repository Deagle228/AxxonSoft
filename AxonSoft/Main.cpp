#include "Result.h"
  
int main(int argc, char* argv[])
{
    Result resultReference(argv[1]);
    if (argc == 2)
    {
        if (std::filesystem::is_directory(argv[1]))
        {
            
            std::cout << "Number of strings " << resultReference.firstTaskResult();
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
            std::cout << "Number of occurrences "<< resultReference.secondTaskResult(argv[2]);
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