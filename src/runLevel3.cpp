#include <cstdlib>
#include <iostream>

void runLevel3()
{
    std::cout << "Loading level 3..." << std::endl;
#ifdef _WIN32
    int result = std::system("PixelPilgrimage_level3.exe");
#else
    int result = std::system("./PixelPilgrimage_level3");
#endif
    if (result != 0) {
        std::cerr << "Level 3 exited with code " << result << std::endl;
    }
}
