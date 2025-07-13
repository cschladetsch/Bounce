#pragma once

#include <array>
#include <chrono>
#include <cmath>
#include <memory>
#include <random>
#include <string>
#include <vector>
#include <span>
#include <expected>
#include <format>
#include <functional>
#include <algorithm>
#include <ranges>

namespace IndustrialMusic {

// Song structure types
enum class SectionType {
    Intro,
    Verse,
    PreChorus,
    Chorus,
    Bridge,
    Instrumental,
    Breakdown,
    Outro
};

struct Section {
    SectionType type;
    std::string name;
    int bars = 4;
    int beatsPerBar = 4;
    
    [[nodiscard]] constexpr int totalBeats() const noexcept {
        return bars * beatsPerBar;
    }
};

// Audio parameters
struct AudioParams {
    int tempo = 70;         // BPM
    int intensity = 7;      // 1-10
    int distortion = 60;    // 0-100
    float songLength = 1.0f; // Multiplier
    
    enum class VocalType {
        Off,
        Robotic,
        Whisper,
        Distorted
    };
    VocalType vocalType = VocalType::Whisper;
};

// MIDI note structure
struct MidiNote {
    uint8_t channel;
    uint8_t note;
    uint8_t velocity;
    uint32_t startTick;
    uint32_t duration;
};

// Visualization data
struct VisualizationData {
    std::array<float, 1024> frequencies{};
    float currentBeat = 0.0f;
    SectionType currentSection = SectionType::Intro;
    float sectionProgress = 0.0f;
};

// Error handling
enum class ErrorCode {
    Success,
    AudioInitFailed,
    MidiDeviceNotFound,
    FileWriteFailed,
    InvalidParameter
};

template<typename T>
using Result = std::expected<T, ErrorCode>;

// Utility functions
inline std::string sectionTypeToString(SectionType type) {
    switch (type) {
        case SectionType::Intro: return "INTRO";
        case SectionType::Verse: return "VERSE";
        case SectionType::PreChorus: return "PRE-CHORUS";
        case SectionType::Chorus: return "CHORUS";
        case SectionType::Bridge: return "BRIDGE";
        case SectionType::Instrumental: return "INSTRUMENTAL";
        case SectionType::Breakdown: return "BREAKDOWN";
        case SectionType::Outro: return "OUTRO";
    }
    return "UNKNOWN";
}

inline SectionType stringToSectionType(const std::string& str) {
    if (str == "intro") return SectionType::Intro;
    if (str == "verse") return SectionType::Verse;
    if (str == "pre-chorus") return SectionType::PreChorus;
    if (str == "chorus") return SectionType::Chorus;
    if (str == "bridge") return SectionType::Bridge;
    if (str == "instrumental") return SectionType::Instrumental;
    if (str == "breakdown") return SectionType::Breakdown;
    if (str == "outro") return SectionType::Outro;
    return SectionType::Verse; // Default
}

// Color scheme for UI
namespace Colors {
    constexpr auto Background = 0x1A1A1AFF;
    constexpr auto Primary = 0x00FF00FF;
    constexpr auto Secondary = 0xFF0000FF;
    constexpr auto Text = 0xE0E0E0FF;
    constexpr auto Border = 0x404040FF;
    
    // Section colors
    constexpr auto Intro = 0x4A5568FF;
    constexpr auto Verse = 0x2D3748FF;
    constexpr auto PreChorus = 0x553C9AFF;
    constexpr auto Chorus = 0xE53E3EFF;
    constexpr auto Bridge = 0x3182CEFF;
    constexpr auto Instrumental = 0x38A169FF;
    constexpr auto Breakdown = 0xD69E2EFF;
    constexpr auto Outro = 0x718096FF;
}

} // namespace IndustrialMusic