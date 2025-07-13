#pragma once

#include "../Common.h"

namespace IndustrialMusic {

class Visualizer;

class Visualizer3D {
public:
    explicit Visualizer3D(Visualizer& visualizer);
    ~Visualizer3D() = default;
    
    void render();
    
private:
    Visualizer& m_visualizer;
    
    // Visualization mode
    enum class Mode {
        Bars,
        Waveform,
        Circle
    };
    Mode m_currentMode = Mode::Bars;
    
    // Helper methods
    void renderBars();
    void renderWaveform();
    void renderCircle();
    void renderControls();
    void renderSectionInfo();
};

} // namespace IndustrialMusic