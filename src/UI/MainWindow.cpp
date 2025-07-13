#include "UI/MainWindow.h"
#include "UI/SongStructureEditor.h"
#include "UI/ControlPanel.h"
#include "UI/Visualizer3D.h"
#include "Application.h"
#include "AudioEngine.h"
#include "MidiGenerator.h"
#include "SongStructure.h"
#include "LyricsGenerator.h"
#include "VocalSynthesizer.h"

#include <imgui.h>
#include <format>
#include <fstream>

namespace IndustrialMusic {

MainWindow::MainWindow(Application& app) : m_app(app) {
    m_structureEditor = std::make_unique<SongStructureEditor>(app.getSongStructure());
    m_controlPanel = std::make_unique<ControlPanel>(app.getAudioEngine());
    m_visualizer3D = std::make_unique<Visualizer3D>(app.getVisualizer());
}

MainWindow::~MainWindow() = default;

void MainWindow::render() {
    renderMenuBar();
    
    // Main window content
    ImGui::Begin("INDUSTRIELL KLANGMASCHINE", nullptr, ImGuiWindowFlags_NoCollapse);
    
    // Song structure editor
    if (m_structureEditor) {
        m_structureEditor->render();
    }
    
    ImGui::Separator();
    
    // Control panel
    if (m_controlPanel) {
        m_controlPanel->render();
        m_currentParams = m_controlPanel->getParams();
    }
    
    ImGui::Separator();
    
    // Control buttons
    ImGui::BeginGroup();
    
    if (ImGui::Button("Generate Song", ImVec2(120, 30))) {
        onGenerateSong();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Play Full Song", ImVec2(120, 30))) {
        onPlaySong();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Continuous Music", ImVec2(120, 30))) {
        onLoopToggle();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Stop", ImVec2(120, 30))) {
        onStopSong();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Download MIDI", ImVec2(120, 30))) {
        onDownloadMidi();
    }
    
    ImGui::EndGroup();
    
    // Status
    ImGui::Text("Status: %s", m_statusText.c_str());
    
    // Progress bar
    if (m_app.getAudioEngine().isPlaying()) {
        renderProgressBar();
    }
    
    ImGui::End();
    
    // Additional windows
    if (m_showVisualizer && m_visualizer3D) {
        ImGui::Begin("Frequency Analyzer", &m_showVisualizer);
        m_visualizer3D->render();
        ImGui::End();
    }
    
    if (m_showLyrics) {
        renderLyricsWindow();
    }
    
    if (m_showVocalOutput) {
        renderVocalOutputWindow();
    }
    
    renderStatusBar();
}

void MainWindow::renderMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Export MIDI...")) {
                onDownloadMidi();
            }
            if (ImGui::MenuItem("Export Lyrics...")) {
                onExportLyrics();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                // TODO: Handle exit
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Show Visualizer", nullptr, &m_showVisualizer);
            ImGui::MenuItem("Show Lyrics", nullptr, &m_showLyrics);
            ImGui::MenuItem("Show Vocal Output", nullptr, &m_showVocalOutput);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Presets")) {
            auto presets = m_app.getSongStructure().getAvailablePresets();
            for (const auto& preset : presets) {
                if (ImGui::MenuItem(preset.c_str())) {
                    m_app.getSongStructure().loadPreset(preset);
                }
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void MainWindow::renderStatusBar() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y - 25));
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, 25));
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                            ImGuiWindowFlags_NoSavedSettings;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::Begin("StatusBar", nullptr, flags);
    ImGui::PopStyleVar();
    
    auto& audio = m_app.getAudioEngine();
    if (audio.isPlaying()) {
        ImGui::Text("Playing: Beat %.1f | Section: %d | BPM: %d",
                    audio.getCurrentBeat(),
                    audio.getCurrentSectionIndex() + 1,
                    audio.getTempo());
    } else {
        ImGui::Text("Ready");
    }
    
    ImGui::End();
}

void MainWindow::renderProgressBar() {
    float progress = m_app.getAudioEngine().getTotalProgress();
    ImGui::ProgressBar(progress, ImVec2(-1, 20));
}

void MainWindow::renderLyricsWindow() {
    ImGui::Begin("Generated Lyrics", &m_showLyrics);
    
    if (ImGui::Button("Copy")) {
        // TODO: Copy to clipboard
    }
    ImGui::SameLine();
    if (ImGui::Button("Regenerate")) {
        onRegenerateLyrics();
    }
    ImGui::SameLine();
    if (ImGui::Button("Export .txt")) {
        onExportLyrics();
    }
    
    ImGui::Separator();
    
    ImGui::BeginChild("LyricsContent", ImVec2(0, 0), true);
    for (const auto& line : m_currentLyrics) {
        ImGui::TextWrapped("%s", line.c_str());
    }
    ImGui::EndChild();
    
    ImGui::End();
}

void MainWindow::renderVocalOutputWindow() {
    ImGui::Begin("Vocal Output", &m_showVocalOutput);
    
    auto& audio = m_app.getAudioEngine();
    if (audio.isPlaying()) {
        int sectionIndex = audio.getCurrentSectionIndex();
        auto sections = m_app.getSongStructure().getSections();
        
        if (sectionIndex < sections.size()) {
            std::string sectionName = sectionTypeToString(sections[sectionIndex].type);
            ImGui::Text("Section: %s", sectionName.c_str());
        }
        
        ImGui::Text("Current Vocal: %s", m_currentVocalText.c_str());
        ImGui::Text("Beat: %.0f", audio.getCurrentBeat());
    }
    
    ImGui::End();
}

void MainWindow::onGenerateSong() {
    m_statusText = "Generating song...";
    
    // Generate lyrics
    m_currentLyrics = m_app.getLyricsGenerator().generate(
        m_app.getSongStructure().getSections(),
        std::chrono::steady_clock::now().time_since_epoch().count()
    );
    
    // Update audio engine with current structure
    m_app.getAudioEngine().setSongSections(m_app.getSongStructure().getSections());
    
    // Update vocal synthesizer
    m_app.getVocalSynthesizer().setVocalType(m_currentParams.vocalType);
    
    m_statusText = "Song generated!";
    m_showLyrics = true;
}

void MainWindow::onPlaySong() {
    if (!m_app.getAudioEngine().isPlaying()) {
        m_app.getAudioEngine().play();
        m_statusText = "Playing...";
        m_showVisualizer = true;
        m_showVocalOutput = (m_currentParams.vocalType != AudioParams::VocalType::Off);
    }
}

void MainWindow::onStopSong() {
    m_app.getAudioEngine().stop();
    m_statusText = "Stopped";
}

void MainWindow::onLoopToggle() {
    bool isLooping = !m_app.getAudioEngine().isLooping();
    m_app.getAudioEngine().setLooping(isLooping);
    m_statusText = isLooping ? "Looping enabled" : "Looping disabled";
}

void MainWindow::onDownloadMidi() {
    auto result = m_app.getMidiGenerator().generate(
        m_app.getSongStructure().getSections(),
        m_currentParams,
        std::chrono::steady_clock::now().time_since_epoch().count()
    );
    
    if (result) {
        auto saveResult = m_app.getMidiGenerator().saveToFile(
            result.value(),
            "industrial_song.mid"
        );
        
        if (saveResult) {
            m_statusText = "MIDI file saved!";
        } else {
            m_statusText = "Failed to save MIDI file";
        }
    } else {
        m_statusText = "Failed to generate MIDI";
    }
}

void MainWindow::onRegenerateLyrics() {
    m_currentLyrics = m_app.getLyricsGenerator().regenerate();
}

void MainWindow::onExportLyrics() {
    auto result = m_app.getLyricsGenerator().exportToFile(
        m_currentLyrics,
        "lyrics.txt"
    );
    
    if (result) {
        m_statusText = "Lyrics exported!";
    } else {
        m_statusText = "Failed to export lyrics";
    }
}

} // namespace IndustrialMusic