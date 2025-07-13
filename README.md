# Industrial Music Machine 🎵🏭

A C++23 industrial music generator with real-time visualization, MIDI export, and AI-generated lyrics. Converted from the JavaScript Industrial Producer project to native C++ for enhanced performance and system integration.

![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.20%2B-green.svg)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)

## 🎸 Features

### Core Functionality
- **🎼 Song Structure Editor**: Intuitive drag-and-drop interface to create custom song structures
- **🎹 MIDI Export**: Generate standard MIDI files with 6 tracks (drums, bass, lead, pads, effects)
- **📊 Real-time Visualizer**: Multiple visualization modes (bars, waveform, circle)
- **📝 AI Lyrics Generator**: Generate industrial-themed lyrics with customizable themes
- **🎤 Vocal Synthesizer**: Multiple vocal styles (robotic, whisper, distorted)
- **🎛️ Parameter Control**: Tempo (16-240 BPM), Intensity (1-10), Distortion (0-100%)

### Technical Features
- **Modern C++23**: Utilizes latest features including ranges, expected, format, and concepts
- **ImGui Interface**: Fast, immediate-mode GUI with industrial dark theme
- **libremidi Integration**: Professional MIDI file generation and manipulation
- **Modular Architecture**: Clean separation of concerns with reusable components

## 🔧 Building

### Prerequisites

- CMake 3.20 or higher
- C++23 compatible compiler (GCC 13+, Clang 16+, MSVC 2022+)
- Git (for submodules)
- OpenGL 3.3+

#### Platform-Specific Requirements

**Linux:**
```bash
sudo apt-get install build-essential cmake git
sudo apt-get install libasound2-dev libgl1-mesa-dev libx11-dev
```

**macOS:**
```bash
# Install Xcode Command Line Tools
xcode-select --install
# Install CMake via Homebrew
brew install cmake
```

**Windows:**
- Visual Studio 2022 with C++ development tools
- CMake (included with VS2022 or standalone)

### Build Instructions

```bash
# Clone the repository with submodules
git clone --recursive https://github.com/yourusername/CppMusicMachine.git
cd CppMusicMachine

# Create build directory
mkdir build && cd build

# Configure (Release build recommended)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release -j$(nproc)

# Run
./IndustrialMusicMachine
```

### Quick Test: Generate MIDI

```bash
# Build and run the MIDI test generator
cmake --build . --target test_midi
./test_midi
# This creates 'industrial_test.mid' in the build directory
```

## 🎮 Usage

### Main Application

1. **Launch**: Run `./IndustrialMusicMachine` from the build directory

2. **Create Song Structure**:
   - Choose from presets: Standard, Simple, Extended, or Industrial
   - Drag sections from the palette to build custom arrangements
   - Available sections: Intro, Verse, Pre-Chorus, Chorus, Bridge, Instrumental, Breakdown, Outro
   - Right-click sections to remove or duplicate

3. **Adjust Parameters**:
   - **Tempo**: 16-240 BPM (default: 70 BPM for dark industrial sound)
   - **Intensity**: 1-10 (affects dynamics and complexity)
   - **Distortion**: 0-100% (industrial grit level)
   - **Song Length**: 0.5x-2.0x multiplier
   - **Vocals**: Off, Robotic, Whisper, or Distorted

4. **Generate & Export**:
   - Click "Generate Song" to create the composition
   - "Download MIDI" exports a multi-track MIDI file
   - View generated lyrics in the lyrics window
   - Export lyrics as .txt file

### Playing Generated MIDI Files

The application generates standard MIDI files that can be played with:

**Software Synthesizers:**
```bash
# TiMidity++
timidity industrial_test.mid

# FluidSynth (recommended)
fluidsynth -a alsa -m alsa_seq /usr/share/sounds/sf2/FluidR3_GM.sf2 industrial_test.mid
```

**DAWs (Digital Audio Workstations):**
- Import into Reaper, FL Studio, Ableton Live, Logic Pro, etc.
- Apply industrial-style VST instruments and effects

**Online Players:**
- Upload to online MIDI players for quick playback

## 🏗️ Architecture

```
CppMusicMachine/
├── include/              # Header files
│   ├── Application.h     # Main application controller
│   ├── AudioEngine.h     # Audio synthesis engine (stub)
│   ├── MidiGenerator.h   # MIDI file creation
│   ├── SongStructure.h   # Song arrangement manager
│   ├── Visualizer.h      # Audio visualization
│   ├── LyricsGenerator.h # AI lyrics generation
│   └── UI/              # User interface components
├── src/                 # Implementation files
├── external/            # Dependencies (ImGui submodule)
├── assets/              # Resources
└── CMakeLists.txt       # Build configuration
```

### Key Components

- **Application**: Central coordinator managing all subsystems
- **AudioEngine**: Real-time audio synthesis (currently stub, ready for backend integration)
- **MidiGenerator**: Creates multi-track MIDI files with industrial patterns
- **SongStructure**: Manages song sections and arrangements with drag-and-drop support
- **Visualizer**: Real-time frequency analysis and visualization
- **LyricsGenerator**: Procedural generation of industrial-themed lyrics
- **VocalSynthesizer**: Applies vocal effects and timing
- **UI Components**: Modular ImGui-based interface elements

## 🎼 MIDI Output Format

Generated MIDI files contain 6 tracks:
1. **Tempo Track**: Timing and tempo changes
2. **Drums** (Channel 10): Industrial drum patterns with kick, snare, hi-hats
3. **Bass** (Channel 1): Deep, driving basslines
4. **Lead** (Channel 2): Harsh lead synthesizer lines
5. **Pads** (Channel 3): Atmospheric background textures
6. **Effects** (Channel 4): Industrial sound effects and hits

## 🛠️ Development

### Adding New Features

1. **New Section Types**: Add to `SectionType` enum in `Common.h`
2. **New Vocal Styles**: Extend `AudioParams::VocalType` and implement in `VocalSynthesizer`
3. **New Presets**: Add methods to `SongStructure::Presets` namespace
4. **New Visualizations**: Add modes to `Visualizer::Mode` enum

### Code Style

- C++23 features encouraged (concepts, ranges, format)
- RAII for resource management
- `Result<T>` for error handling (std::expected)
- Clear separation between UI and logic

## 📦 Dependencies

- **[Dear ImGui](https://github.com/ocornut/imgui)**: Immediate mode GUI (git submodule)
- **[GLFW](https://www.glfw.org/)**: Window and input management (fetched by CMake)
- **[GLAD](https://glad.dav1d.de/)**: OpenGL loader (fetched by CMake)
- **[libremidi](https://github.com/jcelerier/libremidi)**: Modern C++ MIDI library (fetched by CMake)

## 🚀 Future Enhancements

- [ ] Real-time audio synthesis backend (PortAudio/RtAudio/miniaudio)
- [ ] VST3/AU plugin version
- [ ] Advanced synthesis algorithms (FM, granular, physical modeling)
- [ ] Real-time MIDI output to hardware/software instruments
- [ ] Project save/load functionality
- [ ] Text-to-speech integration for vocals
- [ ] 3D spectrum analyzer
- [ ] Pattern sequencer for detailed editing
- [ ] Export to various audio formats (WAV, MP3, OGG)
- [ ] Network collaboration features

## 🐛 Known Issues

- Audio synthesis is currently stubbed (no sound output from main app)
- MIDI output API needs updating for latest libremidi
- Docking support disabled (requires newer ImGui features)
- Some systems may not have ALSA MIDI sequencer available

## 📄 License

This project is open source, licensed under the same terms as the original JavaScript Industrial Producer project.

## 🙏 Acknowledgments

- Original JavaScript Industrial Producer project for the concept and algorithms
- Dear ImGui team for the excellent GUI library
- libremidi developers for modern MIDI support
- The industrial music community for inspiration

---

**Note**: This is a port of a JavaScript web application to native C++. While the MIDI generation is fully functional, the real-time audio synthesis requires additional audio backend implementation for actual sound output.