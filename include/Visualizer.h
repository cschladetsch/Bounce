#pragma once

#include "Common.h"

namespace IndustrialMusic {

class Visualizer {
public:
    Visualizer();
    ~Visualizer() = default;
    
    void update(const VisualizationData& data, float deltaTime);
    void reset();
    
    [[nodiscard]] const VisualizationData& getCurrentData() const { return m_currentData; }
    
    enum class Mode {
        Bars,
        Waveform,
        Circle
    };
    
    void setMode(Mode mode) { m_mode = mode; }
    [[nodiscard]] Mode getMode() const { return m_mode; }
    
private:
    VisualizationData m_currentData;
    Mode m_mode = Mode::Bars;
    float m_animationTime = 0.0f;
};

} // namespace IndustrialMusic