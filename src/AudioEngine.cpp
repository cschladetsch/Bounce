#include "AudioEngine.h"
#include <iostream>
#include <cmath>

namespace IndustrialMusic {

AudioEngine::AudioEngine() : m_rng(std::random_device{}()) {
}

AudioEngine::~AudioEngine() {
    shutdown();
}

Result<void> AudioEngine::initialize() {
    // TODO: Initialize actual audio backend (e.g., PortAudio, RtAudio, or platform-specific)
    std::cout << "AudioEngine: Initializing audio system...\n";
    
    // Start audio thread
    m_shouldStop = false;
    m_audioThread = std::thread(&AudioEngine::audioThreadFunc, this);
    
    return {};
}

void AudioEngine::shutdown() {
    stop();
    
    m_shouldStop = true;
    m_cv.notify_all();
    
    if (m_audioThread.joinable()) {
        m_audioThread.join();
    }
}

void AudioEngine::play() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isPlaying = true;
    m_isPaused = false;
    m_startTime = std::chrono::steady_clock::now();
    m_cv.notify_all();
}

void AudioEngine::pause() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isPaused = true;
    m_cv.notify_all();
}

void AudioEngine::stop() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isPlaying = false;
    m_isPaused = false;
    m_currentBeat = 0.0f;
    m_currentSection = 0;
    m_cv.notify_all();
}

void AudioEngine::updateTempo(int bpm) {
    m_currentTempo = bpm;
}

void AudioEngine::updateIntensity(int intensity) {
    m_currentIntensity = intensity;
}

void AudioEngine::updateDistortion(int distortion) {
    m_currentDistortion = distortion;
}

float AudioEngine::getSectionProgress() const {
    std::lock_guard<std::mutex> lock(m_sectionMutex);
    if (m_sections.empty() || m_currentSection >= m_sections.size()) {
        return 0.0f;
    }
    
    const auto& section = m_sections[m_currentSection];
    float sectionBeats = static_cast<float>(section.totalBeats());
    float beatInSection = std::fmod(m_currentBeat.load(), sectionBeats);
    return beatInSection / sectionBeats;
}

float AudioEngine::getTotalProgress() const {
    if (m_sections.empty()) return 0.0f;
    
    int totalBeats = 0;
    for (const auto& section : m_sections) {
        totalBeats += section.totalBeats();
    }
    
    if (totalBeats == 0) return 0.0f;
    return m_currentBeat.load() / static_cast<float>(totalBeats);
}

std::span<const float> AudioEngine::getFrequencyData() const {
    std::lock_guard<std::mutex> lock(m_audioDataMutex);
    return m_frequencyData;
}

float AudioEngine::getAverageVolume() const {
    return m_averageVolume.load();
}

void AudioEngine::setSongSections(const std::vector<Section>& sections) {
    std::lock_guard<std::mutex> lock(m_sectionMutex);
    m_sections = sections;
}

void AudioEngine::audioThreadFunc() {
    while (!m_shouldStop) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this] { return m_shouldStop || (m_isPlaying && !m_isPaused); });
        
        if (m_shouldStop) break;
        
        lock.unlock();
        processAudioFrame();
        
        // Simulate audio callback rate (approximately 60 fps)
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void AudioEngine::processAudioFrame() {
    if (!m_isPlaying || m_isPaused) return;
    
    // Update beat position
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<float>(now - m_startTime).count();
    float beatsPerSecond = m_currentTempo.load() / 60.0f;
    m_currentBeat = elapsed * beatsPerSecond;
    
    // Update section
    std::lock_guard<std::mutex> lock(m_sectionMutex);
    if (!m_sections.empty()) {
        int beatCount = 0;
        for (size_t i = 0; i < m_sections.size(); ++i) {
            int sectionBeats = m_sections[i].totalBeats();
            if (m_currentBeat < beatCount + sectionBeats) {
                m_currentSection = static_cast<int>(i);
                break;
            }
            beatCount += sectionBeats;
        }
        
        // Loop if needed
        if (m_isLooping && m_currentBeat >= beatCount) {
            m_currentBeat = std::fmod(m_currentBeat.load(), static_cast<float>(beatCount));
            m_startTime = std::chrono::steady_clock::now();
        }
    }
    
    // Generate some fake frequency data for visualization
    std::lock_guard<std::mutex> audioLock(m_audioDataMutex);
    for (size_t i = 0; i < m_frequencyData.size(); ++i) {
        float freq = i / static_cast<float>(m_frequencyData.size());
        float value = std::sin(m_currentBeat * freq * 10.0f) * 0.5f + 0.5f;
        value *= (1.0f - freq * 0.8f); // Decrease amplitude for higher frequencies
        value *= m_currentIntensity.load() / 10.0f;
        m_frequencyData[i] = value;
    }
    
    // Calculate average volume
    float sum = 0.0f;
    for (float val : m_frequencyData) {
        sum += val;
    }
    m_averageVolume = sum / m_frequencyData.size();
}

AudioEngine::DrumPattern AudioEngine::getDrumPattern(const Section& section, int beat, int intensity, float random) const {
    DrumPattern pattern;
    
    // Basic 4/4 pattern with variations based on section
    bool isBeat = (beat % 4) == 0;
    bool isOffBeat = (beat % 2) == 1;
    
    switch (section.type) {
        case SectionType::Intro:
            pattern.kick = isBeat && beat > 8;
            pattern.hihat = true;
            pattern.hihatVelocity = 0.5f;
            break;
            
        case SectionType::Verse:
            pattern.kick = isBeat;
            pattern.snare = (beat % 8) == 4;
            pattern.hihat = true;
            break;
            
        case SectionType::Chorus:
            pattern.kick = isBeat || (intensity > 7 && isOffBeat);
            pattern.snare = (beat % 4) == 2;
            pattern.hihat = true;
            pattern.kickVelocity = 1.0f;
            break;
            
        case SectionType::Breakdown:
            pattern.kick = (beat % 8) == 0;
            pattern.snare = random > 0.7f;
            pattern.hihat = random > 0.5f;
            pattern.snareVelocity = random;
            break;
            
        default:
            pattern.kick = isBeat;
            pattern.snare = (beat % 4) == 2;
            pattern.hihat = true;
            break;
    }
    
    // Intensity affects velocity
    float intensityFactor = intensity / 10.0f;
    pattern.kickVelocity *= intensityFactor;
    pattern.snareVelocity *= intensityFactor;
    pattern.hihatVelocity *= intensityFactor * 0.7f;
    
    return pattern;
}

float AudioEngine::seededRandom(uint32_t seed) const {
    m_rng.seed(seed);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(m_rng);
}

} // namespace IndustrialMusic