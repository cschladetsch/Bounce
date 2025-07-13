#include "SongStructure.h"
#include <algorithm>

namespace IndustrialMusic {

SongStructure::SongStructure() {
    // Start with a default structure
    loadPreset("standard");
}

void SongStructure::addSection(const Section& section) {
    m_sections.push_back(section);
    notifyChange(m_sections.size() - 1);
}

void SongStructure::removeSection(size_t index) {
    if (index >= m_sections.size()) return;
    
    m_sections.erase(m_sections.begin() + index);
    notifyChange(index);
}

void SongStructure::moveSection(size_t from, size_t to) {
    if (from >= m_sections.size() || to >= m_sections.size()) return;
    if (from == to) return;
    
    Section section = m_sections[from];
    m_sections.erase(m_sections.begin() + from);
    
    if (to > from) {
        m_sections.insert(m_sections.begin() + to - 1, section);
    } else {
        m_sections.insert(m_sections.begin() + to, section);
    }
    
    notifyChange(std::min(from, to));
}

void SongStructure::clearSections() {
    m_sections.clear();
    notifyChange(0);
}

const Section* SongStructure::getSection(size_t index) const {
    if (index >= m_sections.size()) return nullptr;
    return &m_sections[index];
}

void SongStructure::loadPreset(const std::string& presetName) {
    if (presetName == "standard") {
        createStandardPreset();
    } else if (presetName == "simple") {
        createSimplePreset();
    } else if (presetName == "extended") {
        createExtendedPreset();
    } else if (presetName == "industrial") {
        createIndustrialPreset();
    }
}

std::vector<std::string> SongStructure::getAvailablePresets() const {
    return {"standard", "simple", "extended", "industrial"};
}

int SongStructure::getTotalBeats() const {
    int total = 0;
    for (const auto& section : m_sections) {
        total += section.totalBeats();
    }
    return total;
}

float SongStructure::getTotalDuration(int bpm) const {
    return (getTotalBeats() * 60.0f) / bpm;
}

int SongStructure::getBeatsUntilSection(size_t sectionIndex) const {
    if (sectionIndex >= m_sections.size()) return getTotalBeats();
    
    int beats = 0;
    for (size_t i = 0; i < sectionIndex; ++i) {
        beats += m_sections[i].totalBeats();
    }
    return beats;
}

bool SongStructure::isValid() const {
    return !m_sections.empty();
}

std::string SongStructure::getValidationError() const {
    if (m_sections.empty()) {
        return "Song structure is empty";
    }
    return "";
}

void SongStructure::createStandardPreset() {
    m_sections = Presets::getStandardStructure();
    notifyChange(0);
}

void SongStructure::createSimplePreset() {
    m_sections = Presets::getSimpleStructure();
    notifyChange(0);
}

void SongStructure::createExtendedPreset() {
    m_sections = Presets::getExtendedStructure();
    notifyChange(0);
}

void SongStructure::createIndustrialPreset() {
    m_sections = Presets::getIndustrialStructure();
    notifyChange(0);
}

void SongStructure::notifyChange(size_t index) {
    if (m_onSectionChange && index < m_sections.size()) {
        m_onSectionChange(index, m_sections[index]);
    }
}

} // namespace IndustrialMusic