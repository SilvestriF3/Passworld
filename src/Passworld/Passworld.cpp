#include <iostream>
#include "rule.h"
#include "generator.h"
#include "wordlist.h"
#include "ImGui/applicationGui.h"
#include <thread>
#include <chrono>

int main()
{
#ifdef __linux__
    //detach application from shell
    if (daemon(1, 0))
        std::cout << "bye bye\n";
#endif
    std::cout << "Welcome to Passworld!\n";
    GeneratorConfig config{};
    Generator g(&config);
    std::thread t{gui_main, &g};
    t.join();
}
