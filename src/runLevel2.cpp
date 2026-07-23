#include <cstdlib>
#include <iostream>

void runLevel2()
{
    std::cout << "Loading level 2..." << std::endl;
#ifdef _WIN32
    int result = std::system("Debug\\PixelPilgrimage_level2.exe");
#else
    int result = std::system("./PixelPilgrimage_level2");
#endif
    if (result != 0) {
        std::cerr << "Level 2 exited with code " << result << std::endl;
    }
}