#pragma once

#include "Common.h"
#include <libremidi/libremidi.hpp>
#include <filesystem>

namespace IndustrialMusic {

class MidiGenerator {
public:
    MidiGenerator();
    ~MidiGenerator() = default;
    
    // Generate MIDI file from song structure
    [[nodiscard]] Result<std::vector<uint8_t>> generate(
        const std::vector<Section>& sections,
        const AudioParams& params,
        uint32_t seed = 0
    );
    
    // Save MIDI to file
    [[nodiscard]] Result<void> saveToFile(
        const std::vector<uint8_t>& midiData,
        const std::filesystem::path& filepath
    );
    
    // Real-time MIDI output
    [[nodiscard]] Result<void> initializeMidiOutput();
    void sendMidiNote(uint8_t channel, uint8_t note, uint8_t velocity);
    void sendMidiCC(uint8_t channel, uint8_t controller, uint8_t value);
    void sendMidiProgramChange(uint8_t channel, uint8_t program);
    
    // Get available MIDI outputs
    [[nodiscard]] std::vector<std::string> getAvailableMidiOutputs() const;
    [[nodiscard]] Result<void> selectMidiOutput(size_t index);
    
private:
    static constexpr uint16_t TICKS_PER_QUARTER = 480;
    
    // MIDI output
    std::unique_ptr<libremidi::midi_out> m_midiOut;
    size_t m_selectedOutput = 0;
    
    // Track creation methods
    [[nodiscard]] std::vector<uint8_t> createTempoTrack(uint32_t microsecondsPerQuarter, const std::vector<Section>& sections);
    [[nodiscard]] std::vector<uint8_t> createDrumTrack(const std::vector<Section>& sections, int tempo, int intensity, uint32_t seed);
    [[nodiscard]] std::vector<uint8_t> createBassTrack(const std::vector<Section>& sections, int intensity, uint32_t seed);
    [[nodiscard]] std::vector<uint8_t> createLeadTrack(const std::vector<Section>& sections, int intensity, uint32_t seed);
    [[nodiscard]] std::vector<uint8_t> createPadTrack(const std::vector<Section>& sections, int intensity, uint32_t seed);
    [[nodiscard]] std::vector<uint8_t> createEffectsTrack(const std::vector<Section>& sections, int intensity, uint32_t seed);
    
    // Helper methods
    [[nodiscard]] std::vector<uint8_t> encodeVariableLength(uint32_t value) const;
    void addNoteOn(std::vector<uint8_t>& track, uint32_t deltaTicks, uint8_t channel, uint8_t note, uint8_t velocity);
    void addNoteOff(std::vector<uint8_t>& track, uint32_t deltaTicks, uint8_t channel, uint8_t note);
    void addControlChange(std::vector<uint8_t>& track, uint32_t deltaTicks, uint8_t channel, uint8_t controller, uint8_t value);
    void addProgramChange(std::vector<uint8_t>& track, uint32_t deltaTicks, uint8_t channel, uint8_t program);
    void updateTrackLength(std::vector<uint8_t>& track);
    
    // Pattern generation (similar to AudioEngine but for MIDI)
    [[nodiscard]] std::vector<MidiNote> generateDrumPattern(const Section& section, int intensity, uint32_t seed) const;
    [[nodiscard]] std::vector<MidiNote> generateBassPattern(const Section& section, int intensity, uint32_t seed) const;
    [[nodiscard]] std::vector<MidiNote> generateLeadPattern(const Section& section, int intensity, uint32_t seed) const;
    
    // Note mapping
    static constexpr uint8_t DRUM_CHANNEL = 9; // MIDI channel 10 (0-indexed)
    static constexpr uint8_t KICK_NOTE = 36;   // C1
    static constexpr uint8_t SNARE_NOTE = 38;  // D1
    static constexpr uint8_t HIHAT_CLOSED = 42; // F#1
    static constexpr uint8_t HIHAT_OPEN = 46;   // A#1
    static constexpr uint8_t CRASH_NOTE = 49;   // C#2
    static constexpr uint8_t RIDE_NOTE = 51;    // D#2
    
    // Bass notes (industrial style - low frequencies)
    static constexpr std::array<uint8_t, 12> BASS_NOTES = {
        24, 26, 27, 29, 31, 32, 34, 36, 38, 39, 41, 43 // C1 to G2
    };
    
    // Lead notes (industrial style - mid to high frequencies)
    static constexpr std::array<uint8_t, 24> LEAD_NOTES = {
        48, 50, 51, 53, 55, 56, 58, 60, // C3 to C4
        62, 63, 65, 67, 68, 70, 72, 74, // D4 to D5
        75, 77, 79, 80, 82, 84, 86, 87  // D#5 to D#6
    };
};

} // namespace IndustrialMusic