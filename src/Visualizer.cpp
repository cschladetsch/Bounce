#include "Visualizer.h"

namespace IndustrialMusic {

Visualizer::Visualizer() {
    reset();
}

void Visualizer::update(const VisualizationData& data, float deltaTime) {
    m_currentData = data;
    m_animationTime += deltaTime;
}

void Visualizer::reset() {
    m_currentData = {};
    m_animationTime = 0.0f;
}

} // namespace IndustrialMusic