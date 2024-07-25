#include <iostream>

extern "C" __declspec(dllexport) void myFunction()
{
    std::cout << "Hello from the DLL!" << std::endl;
}