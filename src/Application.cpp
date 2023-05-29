
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "Engine.h"

#include <stdio.h>

// Main code
int main(int, char**)
{
    Engine engine = Engine();

    if (!engine.CreateWindow(v2i(1280, 720), "aaaa"))
        std::printf("error: you are an idiot\n");
    engine.Mainloop(true);

    return 0;
}
