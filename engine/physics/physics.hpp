#include "core/common.hpp"
#include "uniformGrid.hpp"

class PhysicsEngine {
public:
    PhysicsEngine() : grid(20.f) {}
    ~PhysicsEngine() {}

    void update(Particles& particles, EnginePerformanceData& perf, float dt);
    void setPhysicsBoundary(const vec2 minBound, const vec2 maxBound);

private:
    void solveVerlet(Particles& particles, float dt);
    void checkCollisions(Particles& particle1, EnginePerformanceData& perf);
    void checkCollision(Particles& particle1, size_t idx1, size_t idx2);
    void checkCollisionSIMD(Particles& particles, size_t idx1, size_t j_start);
    void checkCollisionSIMDsmall(Particles& particles, size_t idx1, size_t j_start);
    void clampToBounds(Particles& particle, size_t idx);

    vec2 boundMin;
    vec2 boundMax;

    UniformGrid grid;
    std::vector<size_t> neighbors;

    const float responseCoef = 0.75f;
};