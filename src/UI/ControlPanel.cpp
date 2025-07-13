#include "UI/ControlPanel.h"
#include "AudioEngine.h"
#include <imgui.h>

namespace IndustrialMusic {

ControlPanel::ControlPanel(AudioEngine& audioEngine) 
    : m_audioEngine(audioEngine) {
    // Initialize with current audio engine values
    m_params.tempo = audioEngine.getTempo();
    m_params.intensity = audioEngine.getIntensity();
    m_params.distortion = audioEngine.getDistortion();
}

void ControlPanel::render() {
    ImGui::Text("Controls");
    
    // Tempo slider
    renderSlider("Tempo (BPM)", &m_params.tempo, 16, 240, "%d");
    m_audioEngine.updateTempo(m_params.tempo);
    
    // Intensity slider
    renderSlider("Intensity", &m_params.intensity, 1, 10, "%d");
    m_audioEngine.updateIntensity(m_params.intensity);
    
    // Distortion slider
    renderSlider("Distortion", &m_params.distortion, 0, 100, "%d");
    m_audioEngine.updateDistortion(m_params.distortion);
    
    // Song length slider
    float songLength = m_params.songLength;
    ImGui::SliderFloat("Song Length", &songLength, 0.5f, 2.0f, "%.1fx");
    m_params.songLength = songLength;
    ImGui::SameLine();
    int minutes = static_cast<int>(5 * songLength);
    ImGui::Text("(~%d min)", minutes);
    
    // Vocal type dropdown
    renderVocalDropdown();
}

void ControlPanel::renderSlider(const char* label, int* value, int min, int max, const char* format) {
    ImGui::SliderInt(label, value, min, max, format);
    ImGui::SameLine();
    ImGui::Text("%d", *value);
}

void ControlPanel::renderVocalDropdown() {
    ImGui::Text("Vocals:");
    ImGui::SameLine();
    
    const char* currentVocalName = getVocalTypeName(m_params.vocalType);
    
    if (ImGui::BeginCombo("##Vocals", currentVocalName)) {
        const AudioParams::VocalType types[] = {
            AudioParams::VocalType::Off,
            AudioParams::VocalType::Robotic,
            AudioParams::VocalType::Whisper,
            AudioParams::VocalType::Distorted
        };
        
        for (auto type : types) {
            bool isSelected = (m_params.vocalType == type);
            const char* name = getVocalTypeName(type);
            
            if (ImGui::Selectable(name, isSelected)) {
                m_params.vocalType = type;
            }
            
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        
        ImGui::EndCombo();
    }
}

const char* ControlPanel::getVocalTypeName(AudioParams::VocalType type) const {
    switch (type) {
        case AudioParams::VocalType::Off: return "Off";
        case AudioParams::VocalType::Robotic: return "Robotic";
        case AudioParams::VocalType::Whisper: return "Whisper";
        case AudioParams::VocalType::Distorted: return "Distorted";
        default: return "Unknown";
    }
}

} // namespace IndustrialMusic