#include "UI/Visualizer3D.h"
#include "Visualizer.h"
#include <imgui.h>
#include <cmath>

namespace IndustrialMusic {

Visualizer3D::Visualizer3D(Visualizer& visualizer) 
    : m_visualizer(visualizer) {
}

void Visualizer3D::render() {
    renderControls();
    
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    canvasSize.y = std::max(200.0f, canvasSize.y - 100.0f);
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    
    // Background
    drawList->AddRectFilled(canvasPos, 
                           ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
                           IM_COL32(20, 20, 20, 255));
    
    // Render based on current mode
    switch (m_currentMode) {
        case Mode::Bars:
            renderBars();
            break;
        case Mode::Waveform:
            renderWaveform();
            break;
        case Mode::Circle:
            renderCircle();
            break;
    }
    
    // Move cursor past the canvas
    ImGui::Dummy(canvasSize);
    
    renderSectionInfo();
}

void Visualizer3D::renderBars() {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    canvasSize.y = std::max(200.0f, canvasSize.y - 100.0f);
    
    const auto& vizData = m_visualizer.getCurrentData();
    const size_t numBars = 64;
    const float barWidth = canvasSize.x / numBars;
    const float maxHeight = canvasSize.y * 0.8f;
    
    for (size_t i = 0; i < numBars && i < vizData.frequencies.size(); ++i) {
        float freq = vizData.frequencies[i * (vizData.frequencies.size() / numBars)];
        float height = freq * maxHeight;
        
        ImVec2 barPos(canvasPos.x + i * barWidth, canvasPos.y + canvasSize.y);
        ImVec2 barTop(barPos.x + barWidth * 0.8f, barPos.y - height);
        
        // Color based on frequency range
        ImU32 color;
        if (i < numBars / 4) {
            color = IM_COL32(255, 100, 100, 255); // Low freq - red
        } else if (i < numBars / 2) {
            color = IM_COL32(100, 255, 100, 255); // Mid freq - green
        } else {
            color = IM_COL32(100, 100, 255, 255); // High freq - blue
        }
        
        drawList->AddRectFilled(barPos, barTop, color);
    }
}

void Visualizer3D::renderWaveform() {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    canvasSize.y = std::max(200.0f, canvasSize.y - 100.0f);
    
    const auto& vizData = m_visualizer.getCurrentData();
    const float centerY = canvasPos.y + canvasSize.y / 2;
    
    std::vector<ImVec2> points;
    for (size_t i = 0; i < vizData.frequencies.size(); i += 4) {
        float x = canvasPos.x + (i / float(vizData.frequencies.size())) * canvasSize.x;
        float y = centerY + vizData.frequencies[i] * canvasSize.y * 0.4f * 
                  std::sin(i * 0.1f + vizData.currentBeat);
        points.push_back(ImVec2(x, y));
    }
    
    if (points.size() > 1) {
        drawList->AddPolyline(points.data(), static_cast<int>(points.size()), 
                             IM_COL32(0, 255, 0, 255), false, 2.0f);
    }
}

void Visualizer3D::renderCircle() {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    canvasSize.y = std::max(200.0f, canvasSize.y - 100.0f);
    
    const auto& vizData = m_visualizer.getCurrentData();
    ImVec2 center(canvasPos.x + canvasSize.x / 2, canvasPos.y + canvasSize.y / 2);
    float baseRadius = std::min(canvasSize.x, canvasSize.y) * 0.3f;
    
    const int numPoints = 128;
    std::vector<ImVec2> points;
    
    for (int i = 0; i < numPoints; ++i) {
        float angle = (i / float(numPoints)) * 2 * M_PI;
        size_t freqIndex = (i * vizData.frequencies.size()) / numPoints;
        float radius = baseRadius + vizData.frequencies[freqIndex] * 50.0f;
        
        float x = center.x + radius * std::cos(angle);
        float y = center.y + radius * std::sin(angle);
        points.push_back(ImVec2(x, y));
    }
    
    drawList->AddPolyline(points.data(), numPoints, IM_COL32(255, 0, 255, 255), true, 2.0f);
}

void Visualizer3D::renderControls() {
    if (ImGui::Button("Bars")) {
        m_currentMode = Mode::Bars;
    }
    ImGui::SameLine();
    if (ImGui::Button("Waveform")) {
        m_currentMode = Mode::Waveform;
    }
    ImGui::SameLine();
    if (ImGui::Button("Circle")) {
        m_currentMode = Mode::Circle;
    }
}

void Visualizer3D::renderSectionInfo() {
    const auto& vizData = m_visualizer.getCurrentData();
    
    ImGui::Separator();
    
    std::string sectionName = sectionTypeToString(vizData.currentSection);
    ImGui::Text("Section: %s", sectionName.c_str());
    
    ImGui::ProgressBar(vizData.sectionProgress, ImVec2(-1, 20));
    
    ImGui::Text("Beat: %.1f", vizData.currentBeat);
}

} // namespace IndustrialMusic