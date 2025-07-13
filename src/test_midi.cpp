#include "MidiGenerator.h"
#include "SongStructure.h"
#include <iostream>
#include <chrono>

int main() {
    using namespace IndustrialMusic;
    
    std::cout << "Generating Industrial MIDI file...\n";
    
    // Create a song structure
    SongStructure songStructure;
    songStructure.loadPreset("industrial");
    
    // Set audio parameters
    AudioParams params;
    params.tempo = 140;        // Industrial tempo
    params.intensity = 8;      // High intensity
    params.distortion = 70;    // Heavy distortion
    params.songLength = 1.0f;
    
    // Generate MIDI
    MidiGenerator midiGen;
    auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
    
    auto result = midiGen.generate(songStructure.getSections(), params, seed);
    
    if (result) {
        // Save to file
        auto saveResult = midiGen.saveToFile(result.value(), "industrial_test.mid");
        if (saveResult) {
            std::cout << "MIDI file saved as 'industrial_test.mid'\n";
            std::cout << "File size: " << result.value().size() << " bytes\n";
            return 0;
        } else {
            std::cerr << "Failed to save MIDI file\n";
            return 1;
        }
    } else {
        std::cerr << "Failed to generate MIDI\n";
        return 1;
    }
}