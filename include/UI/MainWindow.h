#pragma once

#include "../Common.h"
#include <memory>

namespace IndustrialMusic {

class Application;
class SongStructureEditor;
class ControlPanel;
class Visualizer3D;

class MainWindow {
public:
    explicit MainWindow(Application& app);
    ~MainWindow();
    
    void render();
    
private:
    Application& m_app;
    
    // UI components
    std::unique_ptr<SongStructureEditor> m_structureEditor;
    std::unique_ptr<ControlPanel> m_controlPanel;
    std::unique_ptr<Visualizer3D> m_visualizer3D;
    
    // State
    bool m_showVisualizer = false;
    bool m_showLyrics = false;
    bool m_showVocalOutput = false;
    
    // Current values
    AudioParams m_currentParams;
    std::string m_statusText;
    float m_progress = 0.0f;
    
    // Lyrics
    std::vector<std::string> m_currentLyrics;
    std::string m_currentVocalText;
    
    // Helper methods
    void renderMenuBar();
    void renderStatusBar();
    void renderProgressBar();
    void renderLyricsWindow();
    void renderVocalOutputWindow();
    
    // Actions
    void onGenerateSong();
    void onPlaySong();
    void onStopSong();
    void onLoopToggle();
    void onDownloadMidi();
    void onRegenerateLyrics();
    void onExportLyrics();
};

} // namespace IndustrialMusic