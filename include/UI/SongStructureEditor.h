#pragma once

#include "../Common.h"
#include <imgui.h>
#include <string>
#include <vector>

namespace IndustrialMusic {

class SongStructure;

class SongStructureEditor {
public:
    explicit SongStructureEditor(SongStructure& songStructure);
    ~SongStructureEditor() = default;
    
    void render();
    
private:
    SongStructure& m_songStructure;
    
    // Drag and drop state
    int m_draggedSection = -1;
    int m_dropTarget = -1;
    bool m_isDragging = false;
    
    // UI state
    bool m_showPalette = true;
    std::string m_infoText;
    
    // Helper methods
    void renderPresetButtons();
    void renderSectionPalette();
    void renderStructureEditor();
    void renderSectionBlock(size_t index, const Section& section);
    
    [[nodiscard]] ImU32 getSectionColor(SectionType type) const;
    [[nodiscard]] std::string getSectionLabel(const Section& section) const;
};

} // namespace IndustrialMusic