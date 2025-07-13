#pragma once

#include "Common.h"
#include <string>
#include <optional>

namespace IndustrialMusic {

class VocalSynthesizer {
public:
    VocalSynthesizer();
    ~VocalSynthesizer() = default;
    
    // Set vocal type
    void setVocalType(AudioParams::VocalType type) { m_vocalType = type; }
    [[nodiscard]] AudioParams::VocalType getVocalType() const { return m_vocalType; }
    
    // Generate vocal for a specific beat and section
    [[nodiscard]] std::optional<std::string> generateVocal(
        const Section& section,
        int beat,
        const std::vector<std::string>& lyrics
    );
    
    // Process vocal audio (apply effects)
    void processAudio(std::span<float> buffer, float sampleRate);
    
private:
    AudioParams::VocalType m_vocalType = AudioParams::VocalType::Whisper;
    std::string m_currentVocal;
    int m_lastVocalBeat = -1;
    
    // Effect parameters
    float m_roboticModulation = 0.0f;
    float m_whisperBreathiness = 0.8f;
    float m_distortionAmount = 0.0f;
};

} // namespace IndustrialMusic