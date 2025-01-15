#include "core/common.hpp"
#include "uniformGrid.hpp"

class PhysicsEngine {
public:
    PhysicsEngine() : grid(100.f) {}
    ~PhysicsEngine() {}

    void update(std::vector<Particle>& particles, float dt);
    void setPhysicsBoundary(const vec2 minBound, const vec2 maxBound);

private:

    void solveVerlet(std::vector<Particle>& particles, float dt);
    void checkCollisions(std::vector<Particle>& particles);
    void checkCollision(Particle& particle1, Particle& particle2);
    void clampToBounds(Particle& particle);

    vec2 boundMin;
    vec2 boundMax;

    UniformGrid grid;
};