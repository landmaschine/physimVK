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
    const int maxSteps = 3;
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

inline void InitializeParticles(size_t particleCount, const float radius, std::vector<Particle>& particles) {
    particles.reserve(particles.size() + particleCount);
    
    const float width = 1920;
    const float height = 1080;
    
    const float cellSize = radius * 2.0f;  
    const int gridWidth = static_cast<int>(width / cellSize) + 1;
    const int gridHeight = static_cast<int>(height / cellSize) + 1;
    
    std::vector<std::vector<vec2>> grid(gridWidth * gridHeight);
    std::mutex gridMutex;
    std::mutex particlesMutex;
    
    const unsigned int threadCount = std::thread::hardware_concurrency();
    std::vector<std::future<void>> futures;
    std::atomic<size_t> placedCount{0};

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disX(radius, width - radius);
    std::uniform_real_distribution<float> disY(radius, height - radius);
    
    size_t placed = 0;
    size_t maxAttempts = 10;  
    
    while (placed < particleCount) {
        vec2 position(disX(gen), disY(gen));
        
        int cellX = static_cast<int>(position.x / cellSize);
        int cellY = static_cast<int>(position.y / cellSize);
        
        bool collision = false;
        const float radiusSquared = (radius * 2.0f) * (radius * 2.0f);
        
        for (int dx = -1; dx <= 1 && !collision; ++dx) {
            for (int dy = -1; dy <= 1 && !collision; ++dy) {
                int neighborX = cellX + dx;
                int neighborY = cellY + dy;
                
                if (neighborX >= 0 && neighborX < gridWidth && 
                    neighborY >= 0 && neighborY < gridHeight) {
                    
                    const auto& cell = grid[neighborY * gridWidth + neighborX];
                    for (const auto& pos : cell) {
                        float distSquared = (position.x - pos.x) * (position.x - pos.x) +
                                          (position.y - pos.y) * (position.y - pos.y);
                        if (distSquared <= radiusSquared) {
                            collision = true;
                            break;
                        }
                    }
                }
            }
        }
        
        if (!collision) {
            grid[cellY * gridWidth + cellX].push_back(position);
            Particle particle;
            particle.curr_pos = position;
            particle.prev_pos = position;
            particle.radius = radius;
            particles.push_back(particle);
            placed++;
        }
    }
}