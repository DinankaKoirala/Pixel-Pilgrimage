#include <cstdlib>
#include <iostream>

void runLevel5()
{
    std::cout << "Loading level 5..." << std::endl;
#ifdef _WIN32
    int result = std::system("PixelPilgrimage_level5.exe");
#else
    int result = std::system("./PixelPilgrimage_level5");
#endif
    if (result != 0) {
        std::cerr << "Level 5 exited with code " << result << std::endl;
    }
}
