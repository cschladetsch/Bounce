#pragma once

#include "Common.h"
#include <memory>

struct GLFWwindow;

namespace IndustrialMusic {

class AudioEngine;
class MidiGenerator;
class SongStructure;
class Visualizer;
class VocalSynthesizer;
class LyricsGenerator;
class MainWindow;

class Application {
public:
    Application();
    ~Application();
    
    // Prevent copying
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    
    // Allow moving
    Application(Application&&) noexcept = default;
    Application& operator=(Application&&) noexcept = default;
    
    [[nodiscard]] Result<void> initialize();
    void run();
    void shutdown();
    
    // Getters for components
    [[nodiscard]] AudioEngine& getAudioEngine() { return *m_audioEngine; }
    [[nodiscard]] MidiGenerator& getMidiGenerator() { return *m_midiGenerator; }
    [[nodiscard]] SongStructure& getSongStructure() { return *m_songStructure; }
    [[nodiscard]] Visualizer& getVisualizer() { return *m_visualizer; }
    [[nodiscard]] VocalSynthesizer& getVocalSynthesizer() { return *m_vocalSynth; }
    [[nodiscard]] LyricsGenerator& getLyricsGenerator() { return *m_lyricsGen; }
    
private:
    // Core components
    std::unique_ptr<AudioEngine> m_audioEngine;
    std::unique_ptr<MidiGenerator> m_midiGenerator;
    std::unique_ptr<SongStructure> m_songStructure;
    std::unique_ptr<Visualizer> m_visualizer;
    std::unique_ptr<VocalSynthesizer> m_vocalSynth;
    std::unique_ptr<LyricsGenerator> m_lyricsGen;
    
    // UI
    GLFWwindow* m_window = nullptr;
    std::unique_ptr<MainWindow> m_mainWindow;
    
    // State
    bool m_isRunning = false;
    std::chrono::steady_clock::time_point m_lastFrameTime;
    
    // Helper methods
    [[nodiscard]] Result<void> initializeWindow();
    [[nodiscard]] Result<void> initializeImGui();
    [[nodiscard]] Result<void> initializeComponents();
    void processInput();
    void update(float deltaTime);
    void render();
    void cleanup();
};

} // namespace IndustrialMusic