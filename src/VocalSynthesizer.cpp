#include "VocalSynthesizer.h"
#include <algorithm>
#include <cmath>

namespace IndustrialMusic {

VocalSynthesizer::VocalSynthesizer() = default;

std::optional<std::string> VocalSynthesizer::generateVocal(
    const Section& section,
    int beat,
    const std::vector<std::string>& lyrics) {
    
    if (m_vocalType == AudioParams::VocalType::Off) {
        return std::nullopt;
    }
    
    // Generate vocals at specific intervals based on section type
    bool shouldVocalize = false;
    
    switch (section.type) {
        case SectionType::Verse:
            shouldVocalize = (beat % 8) == 0;
            break;
        case SectionType::Chorus:
            shouldVocalize = (beat % 4) == 0;
            break;
        case SectionType::Bridge:
            shouldVocalize = (beat % 16) == 0;
            break;
        case SectionType::Breakdown:
            shouldVocalize = (beat % 32) == 0;
            break;
        default:
            shouldVocalize = false;
            break;
    }
    
    if (!shouldVocalize || lyrics.empty()) {
        return std::nullopt;
    }
    
    // Select a line from lyrics based on section and beat
    size_t lineIndex = (beat / 8) % lyrics.size();
    std::string vocal = lyrics[lineIndex];
    
    // Apply vocal style modifications
    switch (m_vocalType) {
        case AudioParams::VocalType::Robotic:
            std::transform(vocal.begin(), vocal.end(), vocal.begin(), ::toupper);
            vocal = "[" + vocal + "]";
            break;
            
        case AudioParams::VocalType::Whisper:
            std::transform(vocal.begin(), vocal.end(), vocal.begin(), ::tolower);
            vocal = "..." + vocal + "...";
            break;
            
        case AudioParams::VocalType::Distorted:
            // Add some "distortion" characters
            for (size_t i = 0; i < vocal.length(); i += 3) {
                if (i < vocal.length()) {
                    vocal[i] = std::toupper(vocal[i]);
                }
            }
            vocal = "!" + vocal + "!";
            break;
            
        default:
            break;
    }
    
    m_currentVocal = vocal;
    m_lastVocalBeat = beat;
    
    return vocal;
}

void VocalSynthesizer::processAudio(std::span<float> buffer, float sampleRate) {
    // Apply vocal effects to audio buffer
    if (m_vocalType == AudioParams::VocalType::Off) {
        return;
    }
    
    switch (m_vocalType) {
        case AudioParams::VocalType::Robotic:
            // Ring modulation for robotic effect
            m_roboticModulation += 0.1f;
            for (size_t i = 0; i < buffer.size(); ++i) {
                float modulator = std::sin(m_roboticModulation + i * 0.01f);
                buffer[i] *= modulator;
            }
            break;
            
        case AudioParams::VocalType::Whisper:
            // Add breathiness
            for (auto& sample : buffer) {
                sample *= m_whisperBreathiness;
            }
            break;
            
        case AudioParams::VocalType::Distorted:
            // Simple clipping distortion
            for (auto& sample : buffer) {
                sample = std::clamp(sample * 2.0f, -1.0f, 1.0f);
            }
            break;
            
        default:
            break;
    }
}

} // namespace IndustrialMusic