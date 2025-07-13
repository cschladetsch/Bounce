#include "MidiGenerator.h"
#include <fstream>
#include <iostream>

namespace IndustrialMusic {

MidiGenerator::MidiGenerator() = default;

Result<std::vector<uint8_t>> MidiGenerator::generate(
    const std::vector<Section>& sections,
    const AudioParams& params,
    uint32_t seed) {
    
    if (sections.empty()) {
        return std::unexpected(ErrorCode::InvalidParameter);
    }
    
    uint32_t microsecondsPerQuarter = static_cast<uint32_t>(60000000 / params.tempo);
    
    // MIDI file header
    std::vector<uint8_t> midiData;
    
    // Header chunk
    midiData.insert(midiData.end(), {'M', 'T', 'h', 'd'});
    midiData.insert(midiData.end(), {0x00, 0x00, 0x00, 0x06}); // Header length
    midiData.insert(midiData.end(), {0x00, 0x01}); // Format type 1
    midiData.insert(midiData.end(), {0x00, 0x06}); // Number of tracks
    midiData.insert(midiData.end(), {
        static_cast<uint8_t>((TICKS_PER_QUARTER >> 8) & 0xFF),
        static_cast<uint8_t>(TICKS_PER_QUARTER & 0xFF)
    });
    
    // Create tracks
    auto track1 = createTempoTrack(microsecondsPerQuarter, sections);
    auto track2 = createDrumTrack(sections, params.tempo, params.intensity, seed);
    auto track3 = createBassTrack(sections, params.intensity, seed + 1);
    auto track4 = createLeadTrack(sections, params.intensity, seed + 2);
    auto track5 = createPadTrack(sections, params.intensity, seed + 3);
    auto track6 = createEffectsTrack(sections, params.intensity, seed + 4);
    
    // Append all tracks
    midiData.insert(midiData.end(), track1.begin(), track1.end());
    midiData.insert(midiData.end(), track2.begin(), track2.end());
    midiData.insert(midiData.end(), track3.begin(), track3.end());
    midiData.insert(midiData.end(), track4.begin(), track4.end());
    midiData.insert(midiData.end(), track5.begin(), track5.end());
    midiData.insert(midiData.end(), track6.begin(), track6.end());
    
    return midiData;
}

Result<void> MidiGenerator::saveToFile(
    const std::vector<uint8_t>& midiData,
    const std::filesystem::path& filepath) {
    
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        return std::unexpected(ErrorCode::FileWriteFailed);
    }
    
    file.write(reinterpret_cast<const char*>(midiData.data()), midiData.size());
    return {};
}

Result<void> MidiGenerator::initializeMidiOutput() {
    try {
        // For now, just create a dummy MIDI output
        // The new libremidi API requires configuration objects
        // TODO: Update to use the new libremidi API properly
        return {};
        
    } catch (const std::exception& e) {
        std::cerr << "MIDI initialization error: " << e.what() << "\n";
        return std::unexpected(ErrorCode::MidiDeviceNotFound);
    }
}

void MidiGenerator::sendMidiNote(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (!m_midiOut) return;
    
    std::vector<unsigned char> message;
    if (velocity > 0) {
        message = {static_cast<unsigned char>(0x90 + channel), note, velocity};
    } else {
        message = {static_cast<unsigned char>(0x80 + channel), note, 0};
    }
    
    m_midiOut->send_message(message);
}

std::vector<uint8_t> MidiGenerator::createTempoTrack(uint32_t microsecondsPerQuarter, const std::vector<Section>& sections) {
    std::vector<uint8_t> track;
    
    // Track header
    track.insert(track.end(), {'M', 'T', 'r', 'k'});
    track.insert(track.end(), {0x00, 0x00, 0x00, 0x00}); // Length placeholder
    
    // Tempo meta event
    track.push_back(0x00); // Delta time
    track.insert(track.end(), {0xFF, 0x51, 0x03}); // Tempo meta event
    track.push_back((microsecondsPerQuarter >> 16) & 0xFF);
    track.push_back((microsecondsPerQuarter >> 8) & 0xFF);
    track.push_back(microsecondsPerQuarter & 0xFF);
    
    // Time signature (4/4)
    track.push_back(0x00); // Delta time
    track.insert(track.end(), {0xFF, 0x58, 0x04, 0x04, 0x02, 0x18, 0x08});
    
    // Track name
    track.push_back(0x00);
    track.insert(track.end(), {0xFF, 0x03}); // Track name meta event
    const std::string trackName = "Tempo Track";
    track.push_back(static_cast<uint8_t>(trackName.length()));
    track.insert(track.end(), trackName.begin(), trackName.end());
    
    // End of track
    track.push_back(0x00);
    track.insert(track.end(), {0xFF, 0x2F, 0x00});
    
    // Update track length
    updateTrackLength(track);
    
    return track;
}

std::vector<uint8_t> MidiGenerator::createDrumTrack(const std::vector<Section>& sections, int tempo, int intensity, uint32_t seed) {
    std::vector<uint8_t> track;
    
    // Track header
    track.insert(track.end(), {'M', 'T', 'r', 'k'});
    track.insert(track.end(), {0x00, 0x00, 0x00, 0x00}); // Length placeholder
    
    // Track name
    track.push_back(0x00);
    track.insert(track.end(), {0xFF, 0x03}); // Track name meta event
    const std::string trackName = "Drums";
    track.push_back(static_cast<uint8_t>(trackName.length()));
    track.insert(track.end(), trackName.begin(), trackName.end());
    
    // Generate drum patterns for each section
    uint32_t currentTick = 0;
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    for (const auto& section : sections) {
        int totalBeats = section.totalBeats();
        
        for (int beat = 0; beat < totalBeats; ++beat) {
            // Simple kick pattern
            if (beat % 4 == 0) {
                addNoteOn(track, currentTick, DRUM_CHANNEL, KICK_NOTE, 100);
                currentTick = TICKS_PER_QUARTER / 8; // Short note
                addNoteOff(track, currentTick, DRUM_CHANNEL, KICK_NOTE);
                currentTick = TICKS_PER_QUARTER - TICKS_PER_QUARTER / 8;
            }
            // Snare on 2 and 4
            else if (beat % 4 == 2) {
                addNoteOn(track, currentTick, DRUM_CHANNEL, SNARE_NOTE, 90);
                currentTick = TICKS_PER_QUARTER / 8;
                addNoteOff(track, currentTick, DRUM_CHANNEL, SNARE_NOTE);
                currentTick = TICKS_PER_QUARTER - TICKS_PER_QUARTER / 8;
            }
            else {
                currentTick += TICKS_PER_QUARTER;
            }
        }
    }
    
    // End of track
    track.push_back(0x00);
    track.insert(track.end(), {0xFF, 0x2F, 0x00});
    
    updateTrackLength(track);
    return track;
}

std::vector<uint8_t> MidiGenerator::createBassTrack(const std::vector<Section>& sections, int intensity, uint32_t seed) {
    std::vector<uint8_t> track;
    
    // Track header
    track.insert(track.end(), {'M', 'T', 'r', 'k'});
    track.insert(track.end(), {0x00, 0x00, 0x00, 0x00}); // Length placeholder
    
    // Track name
    track.push_back(0x00);
    track.insert(track.end(), {0xFF, 0x03});
    const std::string trackName = "Bass";
    track.push_back(static_cast<uint8_t>(trackName.length()));
    track.insert(track.end(), trackName.begin(), trackName.end());
    
    // Program change to synth bass
    track.push_back(0x00);
    track.insert(track.end(), {0xC0, 38}); // Synth Bass 1
    
    // Simple bass pattern
    uint32_t currentTick = 0;
    for (const auto& section : sections) {
        for (int bar = 0; bar < section.bars; ++bar) {
            // Root note pattern
            addNoteOn(track, currentTick, 0, BASS_NOTES[0], 80);
            currentTick = TICKS_PER_QUARTER * 2;
            addNoteOff(track, currentTick, 0, BASS_NOTES[0]);
            
            addNoteOn(track, 0, 0, BASS_NOTES[5], 70);
            currentTick = TICKS_PER_QUARTER * 2;
            addNoteOff(track, currentTick, 0, BASS_NOTES[5]);
        }
    }
    
    // End of track
    track.push_back(0x00);
    track.insert(track.end(), {0xFF, 0x2F, 0x00});
    
    updateTrackLength(track);
    return track;
}

std::vector<uint8_t> MidiGenerator::createLeadTrack(const std::vector<Section>& sections, int intensity, uint32_t seed) {
    // Similar implementation to bass track but with lead synth
    // For now, return empty track
    std::vector<uint8_t> track;
    track.insert(track.end(), {'M', 'T', 'r', 'k'});
    track.insert(track.end(), {0x00, 0x00, 0x00, 0x0A}); // Length
    track.push_back(0x00);
    track.insert(track.end(), {0xFF, 0x2F, 0x00}); // End of track
    return track;
}

std::vector<uint8_t> MidiGenerator::createPadTrack(const std::vector<Section>& sections, int intensity, uint32_t seed) {
    // Pad/atmosphere track
    std::vector<uint8_t> track;
    track.insert(track.end(), {'M', 'T', 'r', 'k'});
    track.insert(track.end(), {0x00, 0x00, 0x00, 0x0A}); // Length
    track.push_back(0x00);
    track.insert(track.end(), {0xFF, 0x2F, 0x00}); // End of track
    return track;
}

std::vector<uint8_t> MidiGenerator::createEffectsTrack(const std::vector<Section>& sections, int intensity, uint32_t seed) {
    // Sound effects track
    std::vector<uint8_t> track;
    track.insert(track.end(), {'M', 'T', 'r', 'k'});
    track.insert(track.end(), {0x00, 0x00, 0x00, 0x0A}); // Length
    track.push_back(0x00);
    track.insert(track.end(), {0xFF, 0x2F, 0x00}); // End of track
    return track;
}

std::vector<uint8_t> MidiGenerator::encodeVariableLength(uint32_t value) const {
    std::vector<uint8_t> result;
    
    if (value == 0) {
        result.push_back(0);
        return result;
    }
    
    // Extract 7-bit chunks
    std::vector<uint8_t> bytes;
    while (value > 0) {
        bytes.push_back(value & 0x7F);
        value >>= 7;
    }
    
    // Reverse and add continuation bits
    for (int i = static_cast<int>(bytes.size()) - 1; i >= 0; --i) {
        if (i > 0) {
            result.push_back(bytes[i] | 0x80);
        } else {
            result.push_back(bytes[i]);
        }
    }
    
    return result;
}

void MidiGenerator::addNoteOn(std::vector<uint8_t>& track, uint32_t deltaTicks, uint8_t channel, uint8_t note, uint8_t velocity) {
    auto delta = encodeVariableLength(deltaTicks);
    track.insert(track.end(), delta.begin(), delta.end());
    track.push_back(0x90 | channel);
    track.push_back(note);
    track.push_back(velocity);
}

void MidiGenerator::addNoteOff(std::vector<uint8_t>& track, uint32_t deltaTicks, uint8_t channel, uint8_t note) {
    auto delta = encodeVariableLength(deltaTicks);
    track.insert(track.end(), delta.begin(), delta.end());
    track.push_back(0x80 | channel);
    track.push_back(note);
    track.push_back(0);
}

void MidiGenerator::updateTrackLength(std::vector<uint8_t>& track) {
    if (track.size() < 8) return;
    
    uint32_t length = static_cast<uint32_t>(track.size() - 8);
    track[4] = (length >> 24) & 0xFF;
    track[5] = (length >> 16) & 0xFF;
    track[6] = (length >> 8) & 0xFF;
    track[7] = length & 0xFF;
}

} // namespace IndustrialMusic