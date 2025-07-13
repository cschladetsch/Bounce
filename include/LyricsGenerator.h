#pragma once

#include "Common.h"
#include <string>
#include <vector>
#include <random>
#include <filesystem>

namespace IndustrialMusic {

class LyricsGenerator {
public:
    LyricsGenerator();
    ~LyricsGenerator() = default;
    
    // Generate lyrics for a song structure
    [[nodiscard]] std::vector<std::string> generate(
        const std::vector<Section>& sections,
        uint32_t seed = 0
    );
    
    // Regenerate with new seed
    [[nodiscard]] std::vector<std::string> regenerate();
    
    // Export lyrics to text file
    [[nodiscard]] Result<void> exportToFile(
        const std::vector<std::string>& lyrics,
        const std::filesystem::path& filepath
    );
    
private:
    uint32_t m_lastSeed = 0;
    std::mt19937 m_rng;
    
    // Industrial-themed word banks
    static const std::vector<std::string> INDUSTRIAL_NOUNS;
    static const std::vector<std::string> INDUSTRIAL_VERBS;
    static const std::vector<std::string> INDUSTRIAL_ADJECTIVES;
    static const std::vector<std::string> THEMES;
    
    // Generation methods
    [[nodiscard]] std::string generateLine(SectionType type, int lineIndex);
    [[nodiscard]] std::string generateChorus();
    [[nodiscard]] std::string generateVerse(int verseNumber);
    [[nodiscard]] std::string generateBridge();
    
    // Helper methods
    [[nodiscard]] std::string pickRandom(const std::vector<std::string>& words);
    [[nodiscard]] std::string generatePhrase(const std::string& pattern);
};

} // namespace IndustrialMusic