#pragma once
#include "core/common.hpp"
#include "renderer/rendererVK.hpp"
#include "input/input.hpp"
#include "physics/physics.hpp"

#include <random>
#include <thread>
#include <mutex>
#include <future>

typedef struct EngineData_t {
    uint64_t previousTicks = 0;
    uint64_t currentTicks = 0;
    uint64_t elapsedTicks= 0;
    double accumulator = 0.f;

    const float timeStep = 1.f / 165.f;
    const double maxFrameTime = 0.25f;
    const int maxSteps = 2;
    const int maxCatchUpSteps = 3;

    const double targetFrameTime = 1 / 165.f;

    const size_t MaxParticles = 100000;
    const size_t spawnParticles = 1000 - 1;
    const float particleRadius = 1.5f;
    const float maxParticleRadius = 10.f;
} EngineData;

class Engine {
public:
    void run();
private:
    void init_engine();
    void shutdown();

    void input();
    void update();
    void render();
    void renderGui();

    bool m_stop;
    Input m_input;
    EngineData m_engineData;
    EnginePerformanceData perfData;

    SDL_Event m_event;
    RendererVK rendererVK;
    PhysicsEngine physics;
    vec2 windowSize = vec2(1920, 1080);

    Particles particles;
};