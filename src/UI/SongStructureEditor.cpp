#include "UI/SongStructureEditor.h"
#include "SongStructure.h"
#include <imgui.h>

namespace IndustrialMusic {

SongStructureEditor::SongStructureEditor(SongStructure& songStructure) 
    : m_songStructure(songStructure) {
    m_infoText = "Drag sections from above or click a preset to start";
}

void SongStructureEditor::render() {
    ImGui::Text("Song Structure");
    
    renderPresetButtons();
    
    if (m_showPalette) {
        renderSectionPalette();
    }
    
    renderStructureEditor();
    
    ImGui::Text("%s", m_infoText.c_str());
}

void SongStructureEditor::renderPresetButtons() {
    ImGui::Text("Presets:");
    ImGui::SameLine();
    
    auto presets = m_songStructure.getAvailablePresets();
    for (const auto& preset : presets) {
        if (ImGui::Button(preset.c_str())) {
            m_songStructure.loadPreset(preset);
            m_infoText = "Loaded " + preset + " preset";
        }
        ImGui::SameLine();
    }
    ImGui::NewLine();
}

void SongStructureEditor::renderSectionPalette() {
    ImGui::Text("Available Sections (drag to add):");
    
    ImGui::BeginChild("SectionPalette", ImVec2(0, 60), true);
    
    const std::vector<std::pair<SectionType, std::string>> availableSections = {
        {SectionType::Intro, "INTRO"},
        {SectionType::Verse, "VERSE"},
        {SectionType::PreChorus, "PRE-CHORUS"},
        {SectionType::Chorus, "CHORUS"},
        {SectionType::Bridge, "BRIDGE"},
        {SectionType::Instrumental, "INSTRUMENTAL"},
        {SectionType::Breakdown, "BREAKDOWN"},
        {SectionType::Outro, "OUTRO"}
    };
    
    for (const auto& [type, name] : availableSections) {
        ImGui::PushStyleColor(ImGuiCol_Button, getSectionColor(type));
        
        if (ImGui::Button(name.c_str(), ImVec2(100, 40))) {
            // Add section on click
            m_songStructure.addSection({type, name, 4, 4});
            m_infoText = "Added " + name + " section";
        }
        
        // Drag source
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("SECTION_TYPE", &type, sizeof(SectionType));
            ImGui::Text("Add %s", name.c_str());
            ImGui::EndDragDropSource();
        }
        
        ImGui::PopStyleColor();
        ImGui::SameLine();
    }
    
    ImGui::EndChild();
}

void SongStructureEditor::renderStructureEditor() {
    ImGui::Text("Song Structure:");
    
    ImGui::BeginChild("StructureEditor", ImVec2(0, 200), true);
    
    auto& sections = m_songStructure.getSections();
    
    if (sections.empty()) {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), 
                          "No sections. Add sections from above or select a preset.");
    } else {
        for (size_t i = 0; i < sections.size(); ++i) {
            renderSectionBlock(i, sections[i]);
            
            if (i < sections.size() - 1) {
                ImGui::SameLine();
                ImGui::Text("→");
                ImGui::SameLine();
            }
        }
    }
    
    // Drop target for the whole area
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SECTION_TYPE")) {
            SectionType type = *(const SectionType*)payload->Data;
            std::string name = sectionTypeToString(type);
            m_songStructure.addSection({type, name, 4, 4});
            m_infoText = "Added " + name + " section";
        }
        ImGui::EndDragDropTarget();
    }
    
    ImGui::EndChild();
    
    // Structure info
    int totalBeats = m_songStructure.getTotalBeats();
    float duration = m_songStructure.getTotalDuration(120); // Assuming 120 BPM for display
    ImGui::Text("Total: %d beats (~%.1f minutes at 120 BPM)", totalBeats, duration / 60.0f);
}

void SongStructureEditor::renderSectionBlock(size_t index, const Section& section) {
    ImGui::PushID(static_cast<int>(index));
    
    ImVec2 size(80, 60);
    ImGui::PushStyleColor(ImGuiCol_Button, getSectionColor(section.type));
    
    std::string label = getSectionLabel(section);
    if (ImGui::Button(label.c_str(), size)) {
        // TODO: Edit section properties
    }
    
    // Drag source for reordering
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("SECTION_MOVE", &index, sizeof(size_t));
        ImGui::Text("Move %s", section.name.c_str());
        ImGui::EndDragDropSource();
    }
    
    // Drop target for reordering
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SECTION_MOVE")) {
            size_t sourceIndex = *(const size_t*)payload->Data;
            if (sourceIndex != index) {
                // Move section
                // Note: This is a simplified version
                m_infoText = "Moved section";
            }
        }
        ImGui::EndDragDropTarget();
    }
    
    // Context menu for deletion
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Remove")) {
            m_songStructure.removeSection(index);
            m_infoText = "Removed section";
        }
        if (ImGui::MenuItem("Duplicate")) {
            m_songStructure.addSection(section);
            m_infoText = "Duplicated section";
        }
        ImGui::EndPopup();
    }
    
    ImGui::PopStyleColor();
    ImGui::PopID();
}

ImU32 SongStructureEditor::getSectionColor(SectionType type) const {
    switch (type) {
        case SectionType::Intro: return IM_COL32(74, 85, 104, 255);
        case SectionType::Verse: return IM_COL32(45, 55, 72, 255);
        case SectionType::PreChorus: return IM_COL32(85, 60, 154, 255);
        case SectionType::Chorus: return IM_COL32(229, 62, 62, 255);
        case SectionType::Bridge: return IM_COL32(49, 130, 206, 255);
        case SectionType::Instrumental: return IM_COL32(56, 161, 105, 255);
        case SectionType::Breakdown: return IM_COL32(214, 158, 46, 255);
        case SectionType::Outro: return IM_COL32(113, 128, 150, 255);
        default: return IM_COL32(100, 100, 100, 255);
    }
}

std::string SongStructureEditor::getSectionLabel(const Section& section) const {
    return section.name + "\n" + 
           std::to_string(section.bars) + " bars";
}

} // namespace IndustrialMusic