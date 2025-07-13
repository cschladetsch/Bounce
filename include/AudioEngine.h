#pragma once

#include "Common.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace IndustrialMusic {

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();
    
    // Initialize audio system
    [[nodiscard]] Result<void> initialize();
    void shutdown();
    
    // Playback control
    void play();
    void pause();
    void stop();
    [[nodiscard]] bool isPlaying() const { return m_isPlaying.load(); }
    [[nodiscard]] bool isPaused() const { return m_isPaused.load(); }
    
    // Parameter updates
    void updateTempo(int bpm);
    void updateIntensity(int intensity);
    void updateDistortion(int distortion);
    
    // Get current parameters
    [[nodiscard]] int getTempo() const { return m_currentTempo.load(); }
    [[nodiscard]] int getIntensity() const { return m_currentIntensity.load(); }
    [[nodiscard]] int getDistortion() const { return m_currentDistortion.load(); }
    
    // Playback info
    [[nodiscard]] float getCurrentBeat() const { return m_currentBeat.load(); }
    [[nodiscard]] int getCurrentSectionIndex() const { return m_currentSection.load(); }
    [[nodiscard]] float getSectionProgress() const;
    [[nodiscard]] float getTotalProgress() const;
    
    // Audio analysis
    [[nodiscard]] std::span<const float> getFrequencyData() const;
    [[nodiscard]] float getAverageVolume() const;
    
    // Song structure
    void setSongSections(const std::vector<Section>& sections);
    
    // Continuous mode
    void setLooping(bool loop) { m_isLooping.store(loop); }
    [[nodiscard]] bool isLooping() const { return m_isLooping.load(); }
    
private:
    // Audio processing thread
    std::thread m_audioThread;
    std::atomic<bool> m_shouldStop{false};
    std::mutex m_mutex;
    std::condition_variable m_cv;
    
    // Playback state
    std::atomic<bool> m_isPlaying{false};
    std::atomic<bool> m_isPaused{false};
    std::atomic<bool> m_isLooping{false};
    
    // Parameters
    std::atomic<int> m_currentTempo{70};
    std::atomic<int> m_currentIntensity{7};
    std::atomic<int> m_currentDistortion{60};
    
    // Playback position
    std::atomic<float> m_currentBeat{0.0f};
    std::atomic<int> m_currentSection{0};
    std::chrono::steady_clock::time_point m_startTime;
    
    // Song structure
    std::vector<Section> m_sections;
    mutable std::mutex m_sectionMutex;
    
    // Audio data
    mutable std::array<float, 1024> m_frequencyData{};
    mutable std::mutex m_audioDataMutex;
    std::atomic<float> m_averageVolume{0.0f};
    
    // Audio buffer
    std::vector<float> m_audioBuffer;
    size_t m_bufferSize = 512;
    
    // Internal methods
    void audioThreadFunc();
    void processAudioFrame();
    void generateAudio(std::span<float> buffer);
    
    // Synthesis methods
    void playKick(float time, float velocity);
    void playSnare(float time, float velocity);
    void playHihat(float time, float velocity);
    void playSynth(float time, float frequency, float velocity, float duration);
    void playBass(float time, float frequency, float velocity, float duration);
    
    // Pattern generation
    struct DrumPattern {
        bool kick = false;
        bool snare = false;
        bool hihat = false;
        float kickVelocity = 1.0f;
        float snareVelocity = 1.0f;
        float hihatVelocity = 1.0f;
    };
    
    [[nodiscard]] DrumPattern getDrumPattern(const Section& section, int beat, int intensity, float random) const;
    [[nodiscard]] std::vector<std::pair<float, float>> getBassPattern(const Section& section, int intensity, uint32_t seed) const;
    [[nodiscard]] std::vector<std::pair<float, float>> getSynthPattern(const Section& section, int intensity, uint32_t seed) const;
    
    // Random number generation
    mutable std::mt19937 m_rng;
    [[nodiscard]] float seededRandom(uint32_t seed) const;
};

} // namespace IndustrialMusic