#include "LyricsGenerator.h"
#include <fstream>
#include <sstream>

namespace IndustrialMusic {

// Industrial-themed word banks
const std::vector<std::string> LyricsGenerator::INDUSTRIAL_NOUNS = {
    "machine", "steel", "factory", "gear", "piston", "wire", "circuit", "motor",
    "concrete", "iron", "chrome", "rust", "smoke", "steam", "oil", "metal",
    "engine", "turbine", "hammer", "anvil", "chain", "bolt", "rivet", "forge"
};

const std::vector<std::string> LyricsGenerator::INDUSTRIAL_VERBS = {
    "grind", "crush", "forge", "burn", "spark", "weld", "break", "shatter",
    "pound", "drill", "cut", "slice", "tear", "rip", "smash", "crash",
    "pulse", "throb", "vibrate", "resonate", "echo", "scream", "roar", "hiss"
};

const std::vector<std::string> LyricsGenerator::INDUSTRIAL_ADJECTIVES = {
    "cold", "hard", "dark", "heavy", "sharp", "raw", "brutal", "relentless",
    "mechanical", "synthetic", "electric", "metallic", "industrial", "savage",
    "primal", "violent", "harsh", "bitter", "toxic", "corrosive", "explosive"
};

const std::vector<std::string> LyricsGenerator::THEMES = {
    "dehumanization", "mechanization", "rebellion", "dystopia", "transformation",
    "destruction", "reconstruction", "isolation", "connection", "evolution"
};

LyricsGenerator::LyricsGenerator() : m_rng(std::random_device{}()) {}

std::vector<std::string> LyricsGenerator::generate(
    const std::vector<Section>& sections,
    uint32_t seed) {
    
    m_lastSeed = seed;
    m_rng.seed(seed);
    
    std::vector<std::string> lyrics;
    int verseCount = 0;
    
    for (const auto& section : sections) {
        switch (section.type) {
            case SectionType::Intro:
                lyrics.push_back("[INTRO]");
                lyrics.push_back(generateLine(section.type, 0));
                break;
                
            case SectionType::Verse:
                lyrics.push_back("[VERSE " + std::to_string(++verseCount) + "]");
                lyrics.push_back(generateVerse(verseCount));
                break;
                
            case SectionType::PreChorus:
                lyrics.push_back("[PRE-CHORUS]");
                lyrics.push_back(generateLine(section.type, 0));
                lyrics.push_back(generateLine(section.type, 1));
                break;
                
            case SectionType::Chorus:
                lyrics.push_back("[CHORUS]");
                lyrics.push_back(generateChorus());
                break;
                
            case SectionType::Bridge:
                lyrics.push_back("[BRIDGE]");
                lyrics.push_back(generateBridge());
                break;
                
            case SectionType::Breakdown:
                lyrics.push_back("[BREAKDOWN]");
                lyrics.push_back(pickRandom(INDUSTRIAL_VERBS) + "! " + 
                               pickRandom(INDUSTRIAL_VERBS) + "! " +
                               pickRandom(INDUSTRIAL_VERBS) + "!");
                break;
                
            case SectionType::Outro:
                lyrics.push_back("[OUTRO]");
                lyrics.push_back(generateLine(section.type, 0));
                break;
                
            default:
                break;
        }
        
        lyrics.push_back(""); // Empty line between sections
    }
    
    return lyrics;
}

std::vector<std::string> LyricsGenerator::regenerate() {
    return generate({}, m_lastSeed + 1);
}

Result<void> LyricsGenerator::exportToFile(
    const std::vector<std::string>& lyrics,
    const std::filesystem::path& filepath) {
    
    std::ofstream file(filepath);
    if (!file) {
        return std::unexpected(ErrorCode::FileWriteFailed);
    }
    
    for (const auto& line : lyrics) {
        file << line << "\n";
    }
    
    return {};
}

std::string LyricsGenerator::generateLine(SectionType type, int lineIndex) {
    std::string pattern;
    
    switch (type) {
        case SectionType::Intro:
            pattern = "The {adj} {noun} {verb}s in silence";
            break;
        case SectionType::PreChorus:
            pattern = lineIndex == 0 ? 
                "We are the {adj} {noun}s" :
                "{verb}ing through the {adj} night";
            break;
        case SectionType::Outro:
            pattern = "Until the {noun} {verb}s no more";
            break;
        default:
            pattern = "{adj} {noun} {verb}s";
            break;
    }
    
    return generatePhrase(pattern);
}

std::string LyricsGenerator::generateChorus() {
    std::vector<std::string> chorusPatterns = {
        "{verb}! {verb}! The {adj} {noun}!",
        "We are {adj}, we are {noun}",
        "{verb} the {noun}, {verb} the system",
        "No more {noun}s, only {adj} {noun}s"
    };
    
    return generatePhrase(pickRandom(chorusPatterns));
}

std::string LyricsGenerator::generateVerse(int verseNumber) {
    std::ostringstream verse;
    
    // First line
    verse << generatePhrase("In the {adj} {noun} of {noun}s") << "\n";
    
    // Second line
    verse << generatePhrase("Where {noun}s {verb} and {verb}") << "\n";
    
    // Third line
    verse << generatePhrase("The {adj} {noun} {verb}s forever") << "\n";
    
    // Fourth line
    verse << generatePhrase("And we become {adj} {noun}s");
    
    return verse.str();
}

std::string LyricsGenerator::generateBridge() {
    std::ostringstream bridge;
    
    std::string theme = pickRandom(THEMES);
    bridge << "This is our " << theme << "\n";
    bridge << generatePhrase("Where {noun}s and {noun}s collide") << "\n";
    bridge << generatePhrase("We {verb} against the {adj} machine");
    
    return bridge.str();
}

std::string LyricsGenerator::pickRandom(const std::vector<std::string>& words) {
    if (words.empty()) return "";
    
    std::uniform_int_distribution<size_t> dist(0, words.size() - 1);
    return words[dist(m_rng)];
}

std::string LyricsGenerator::generatePhrase(const std::string& pattern) {
    std::string result = pattern;
    
    // Replace placeholders
    size_t pos = 0;
    while ((pos = result.find("{adj}", pos)) != std::string::npos) {
        result.replace(pos, 5, pickRandom(INDUSTRIAL_ADJECTIVES));
    }
    
    pos = 0;
    while ((pos = result.find("{noun}", pos)) != std::string::npos) {
        result.replace(pos, 6, pickRandom(INDUSTRIAL_NOUNS));
    }
    
    pos = 0;
    while ((pos = result.find("{verb}", pos)) != std::string::npos) {
        result.replace(pos, 6, pickRandom(INDUSTRIAL_VERBS));
    }
    
    return result;
}

} // namespace IndustrialMusic