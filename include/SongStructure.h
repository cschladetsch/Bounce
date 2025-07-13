#pragma once

#include "Common.h"
#include <functional>

namespace IndustrialMusic {

class SongStructure {
public:
    SongStructure();
    ~SongStructure() = default;
    
    // Section management
    void addSection(const Section& section);
    void removeSection(size_t index);
    void moveSection(size_t from, size_t to);
    void clearSections();
    
    // Get sections
    [[nodiscard]] const std::vector<Section>& getSections() const { return m_sections; }
    [[nodiscard]] std::vector<Section>& getSections() { return m_sections; }
    [[nodiscard]] size_t getSectionCount() const { return m_sections.size(); }
    [[nodiscard]] const Section* getSection(size_t index) const;
    
    // Presets
    void loadPreset(const std::string& presetName);
    [[nodiscard]] std::vector<std::string> getAvailablePresets() const;
    
    // Calculate timing
    [[nodiscard]] int getTotalBeats() const;
    [[nodiscard]] float getTotalDuration(int bpm) const;
    [[nodiscard]] int getBeatsUntilSection(size_t sectionIndex) const;
    
    // Validation
    [[nodiscard]] bool isValid() const;
    [[nodiscard]] std::string getValidationError() const;
    
    // Callbacks
    using SectionChangeCallback = std::function<void(size_t index, const Section& section)>;
    void setSectionChangeCallback(SectionChangeCallback callback) { m_onSectionChange = callback; }
    
private:
    std::vector<Section> m_sections;
    SectionChangeCallback m_onSectionChange;
    
    // Preset definitions
    void createStandardPreset();
    void createSimplePreset();
    void createExtendedPreset();
    void createIndustrialPreset();
    
    // Helper to notify changes
    void notifyChange(size_t index);
};

// Preset templates
namespace Presets {
    inline std::vector<Section> getStandardStructure() {
        return {
            {SectionType::Intro, "INTRO", 2, 4},
            {SectionType::Verse, "VERSE", 4, 4},
            {SectionType::PreChorus, "PRE-CHORUS", 2, 4},
            {SectionType::Chorus, "CHORUS", 4, 4},
            {SectionType::Verse, "VERSE", 4, 4},
            {SectionType::PreChorus, "PRE-CHORUS", 2, 4},
            {SectionType::Chorus, "CHORUS", 4, 4},
            {SectionType::Bridge, "BRIDGE", 4, 4},
            {SectionType::Chorus, "CHORUS", 4, 4},
            {SectionType::Outro, "OUTRO", 2, 4}
        };
    }
    
    inline std::vector<Section> getSimpleStructure() {
        return {
            {SectionType::Intro, "INTRO", 2, 4},
            {SectionType::Verse, "VERSE", 4, 4},
            {SectionType::Chorus, "CHORUS", 4, 4},
            {SectionType::Verse, "VERSE", 4, 4},
            {SectionType::Chorus, "CHORUS", 4, 4},
            {SectionType::Outro, "OUTRO", 2, 4}
        };
    }
    
    inline std::vector<Section> getExtendedStructure() {
        return {
            {SectionType::Intro, "INTRO", 4, 4},
            {SectionType::Verse, "VERSE", 4, 4},
            {SectionType::PreChorus, "PRE-CHORUS", 2, 4},
            {SectionType::Chorus, "CHORUS", 4, 4},
            {SectionType::Instrumental, "INSTRUMENTAL", 4, 4},
            {SectionType::Verse, "VERSE", 4, 4},
            {SectionType::PreChorus, "PRE-CHORUS", 2, 4},
            {SectionType::Chorus, "CHORUS", 4, 4},
            {SectionType::Bridge, "BRIDGE", 4, 4},
            {SectionType::Breakdown, "BREAKDOWN", 2, 4},
            {SectionType::Chorus, "CHORUS", 8, 4},
            {SectionType::Outro, "OUTRO", 4, 4}
        };
    }
    
    inline std::vector<Section> getIndustrialStructure() {
        return {
            {SectionType::Intro, "INTRO", 4, 4},
            {SectionType::Breakdown, "BREAKDOWN", 2, 4},
            {SectionType::Verse, "VERSE", 4, 4},
            {SectionType::Instrumental, "INSTRUMENTAL", 4, 4},
            {SectionType::Chorus, "CHORUS", 4, 4},
            {SectionType::Breakdown, "BREAKDOWN", 4, 4},
            {SectionType::Verse, "VERSE", 4, 4},
            {SectionType::Bridge, "BRIDGE", 4, 4},
            {SectionType::Chorus, "CHORUS", 8, 4},
            {SectionType::Breakdown, "BREAKDOWN", 2, 4},
            {SectionType::Outro, "OUTRO", 4, 4}
        };
    }
}

} // namespace IndustrialMusic