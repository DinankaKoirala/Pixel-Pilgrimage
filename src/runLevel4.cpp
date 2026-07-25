#include <cstdlib>
#include <iostream>

void runLevel4()
{
    std::cout << "Loading level 4..." << std::endl;
#ifdef _WIN32
    int result = std::system("Debug\\PixelPilgrimage_level4.exe");
#else
    int result = std::system("./PixelPilgrimage_level4");
#endif
    if (result != 0) {
        std::cerr << "Level 4 exited with code " << result << std::endl;
    }
}
