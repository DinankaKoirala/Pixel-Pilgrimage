#include <cstdlib>
#include <iostream>

void runLevel2()
{
    std::cout << "Loading level 2..." << std::endl;
    int result = std::system("./PixelPilgrimage_level2");
    if (result != 0) {
        std::cerr << "Level 2 exited with code " << result << std::endl;
    }
}
