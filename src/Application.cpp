#include "Application.h"
#include "AudioEngine.h"
#include "MidiGenerator.h"
#include "SongStructure.h"
#include "Visualizer.h"
#include "VocalSynthesizer.h"
#include "LyricsGenerator.h"
#include "UI/MainWindow.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

namespace IndustrialMusic {

Application::Application() = default;

Application::~Application() {
    cleanup();
}

Result<void> Application::initialize() {
    // Initialize GLFW and create window
    if (auto result = initializeWindow(); !result) {
        return result;
    }
    
    // Initialize ImGui
    if (auto result = initializeImGui(); !result) {
        return result;
    }
    
    // Initialize core components
    if (auto result = initializeComponents(); !result) {
        return result;
    }
    
    m_isRunning = true;
    m_lastFrameTime = std::chrono::steady_clock::now();
    
    return {};
}

Result<void> Application::initializeWindow() {
    if (!glfwInit()) {
        return std::unexpected(ErrorCode::AudioInitFailed);
    }
    
    // OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create window
    m_window = glfwCreateWindow(1280, 800, "Industrial Music Machine", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        return std::unexpected(ErrorCode::AudioInitFailed);
    }
    
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // Enable vsync
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return std::unexpected(ErrorCode::AudioInitFailed);
    }
    
    return {};
}

Result<void> Application::initializeImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Industrial dark theme
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Customize colors for industrial look
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.00f, 0.50f, 0.00f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.90f, 0.00f, 1.00f);
    
    // Rounded corners for industrial aesthetic
    style.WindowRounding = 0.0f;
    style.FrameRounding = 2.0f;
    style.GrabRounding = 2.0f;
    
    // Initialize ImGui for GLFW and OpenGL3
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    return {};
}

Result<void> Application::initializeComponents() {
    // Create core components
    m_audioEngine = std::make_unique<AudioEngine>();
    m_midiGenerator = std::make_unique<MidiGenerator>();
    m_songStructure = std::make_unique<SongStructure>();
    m_visualizer = std::make_unique<Visualizer>();
    m_vocalSynth = std::make_unique<VocalSynthesizer>();
    m_lyricsGen = std::make_unique<LyricsGenerator>();
    
    // Initialize audio engine
    if (auto result = m_audioEngine->initialize(); !result) {
        return result;
    }
    
    // Initialize MIDI output
    if (auto result = m_midiGenerator->initializeMidiOutput(); !result) {
        std::cerr << "Warning: MIDI output initialization failed\n";
        // Not critical, continue
    }
    
    // Create UI
    m_mainWindow = std::make_unique<MainWindow>(*this);
    
    // Load default song structure
    m_songStructure->loadPreset("standard");
    
    return {};
}

void Application::run() {
    while (m_isRunning && !glfwWindowShouldClose(m_window)) {
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - m_lastFrameTime).count();
        m_lastFrameTime = currentTime;
        
        processInput();
        update(deltaTime);
        render();
    }
}

void Application::processInput() {
    glfwPollEvents();
    
    // Handle escape key
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, true);
    }
}

void Application::update(float deltaTime) {
    // Update visualizer with audio data
    if (m_audioEngine->isPlaying()) {
        VisualizationData vizData;
        auto freqData = m_audioEngine->getFrequencyData();
        std::copy(freqData.begin(), freqData.end(), vizData.frequencies.begin());
        vizData.currentBeat = m_audioEngine->getCurrentBeat();
        vizData.currentSection = static_cast<SectionType>(m_audioEngine->getCurrentSectionIndex());
        vizData.sectionProgress = m_audioEngine->getSectionProgress();
        
        m_visualizer->update(vizData, deltaTime);
    }
}

void Application::render() {
    // Clear
    int displayW, displayH;
    glfwGetFramebufferSize(m_window, &displayW, &displayH);
    glViewport(0, 0, displayW, displayH);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Render main window
    m_mainWindow->render();
    
    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glfwSwapBuffers(m_window);
}

void Application::shutdown() {
    cleanup();
}

void Application::cleanup() {
    if (m_audioEngine) {
        m_audioEngine->shutdown();
    }
    
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    // Cleanup GLFW
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

} // namespace IndustrialMusic