#include "Application.h"
#include <iostream>
#include <format>

int main(int argc, char* argv[]) {
    using namespace IndustrialMusic;
    
    std::cout << "INDUSTRIELL KLANGMASCHINE v1.0\n";
    std::cout << "================================\n\n";
    
    try {
        Application app;
        
        auto initResult = app.initialize();
        if (!initResult) {
            std::cerr << std::format("Failed to initialize application: error code {}\n", 
                static_cast<int>(initResult.error()));
            return -1;
        }
        
        std::cout << "Application initialized successfully\n";
        std::cout << "Starting main loop...\n\n";
        
        app.run();
        app.shutdown();
        
    } catch (const std::exception& e) {
        std::cerr << std::format("Fatal error: {}\n", e.what());
        return -1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred\n";
        return -1;
    }
    
    std::cout << "Application terminated successfully\n";
    return 0;
}