#pragma once

#include "../Common.h"

namespace IndustrialMusic {

class AudioEngine;

class ControlPanel {
public:
    explicit ControlPanel(AudioEngine& audioEngine);
    ~ControlPanel() = default;
    
    void render();
    
    [[nodiscard]] const AudioParams& getParams() const { return m_params; }
    
private:
    AudioEngine& m_audioEngine;
    AudioParams m_params;
    
    // UI state
    bool m_vocalDropdownOpen = false;
    
    // Helper methods
    void renderSlider(const char* label, int* value, int min, int max, const char* format);
    void renderVocalDropdown();
    
    [[nodiscard]] const char* getVocalTypeName(AudioParams::VocalType type) const;
};

} // namespace IndustrialMusic