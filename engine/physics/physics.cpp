#include "physics.hpp"
#include <omp.h>

void PhysicsEngine::update(Particles& particles, EnginePerformanceData& perf, float dt) {
    solveVerlet(particles, dt);
    checkCollisions(particles, perf);
}

void PhysicsEngine::setPhysicsBoundary(const vec2 minBoundary, const vec2 maxBoundary) {
    boundMin = minBoundary;
    boundMax = maxBoundary;
}

void PhysicsEngine::solveVerlet(Particles& particles, float dt) {
    const vec2 gravity{0.f, 400.f};
    auto verlstart = std::chrono::high_resolution_clock::now();

    float* curr_pos_x = particles.curr_pos_x.data();
    float* curr_pos_y = particles.curr_pos_y.data();
    float* prev_pos_x = particles.prev_pos_x.data();
    float* prev_pos_y = particles.prev_pos_y.data();

    for(size_t i = 1; i < particles.size(); i++) { 
        float vel_x = curr_pos_x[i] - prev_pos_x[i];
        float vel_y = curr_pos_y[i] - prev_pos_y[i];

        float temp_x = curr_pos_x[i];
        float temp_y = curr_pos_y[i];

        curr_pos_x[i] = curr_pos_x[i] + vel_x + (gravity.x - vel_x * 40.f) * (dt * dt);
        curr_pos_y[i] = curr_pos_y[i] + vel_y + (gravity.y - vel_y * 40.f) * (dt * dt);

        prev_pos_x[i] = temp_x;
        prev_pos_y[i] = temp_y;

        clampToBounds(particles, i);
    }

    auto verlend = std::chrono::high_resolution_clock::now();
    auto velduration = std::chrono::duration_cast<std::chrono::microseconds>(verlend - verlstart);
}

void PhysicsEngine::checkCollisions(Particles& particles, EnginePerformanceData& perf) {
    grid.clear();

    for(size_t i = 0; i < particles.size(); i++) {
        grid.insert(particles, i);
    }

    for(size_t i = 0; i < particles.size(); i++) {
        grid.getNeighbors(particles, i, neighbors);
        for(size_t j : neighbors) {
            if(j > i) {
                checkCollision(particles, i, j);
            }
        }
    }
}

void PhysicsEngine::checkCollision(Particles& particles, size_t idx1, size_t idx2) {
    const float responseCoef = 0.75f;

    vec2 pos1{particles.curr_pos_x[idx1], particles.curr_pos_y[idx1]};
    vec2 pos2{particles.curr_pos_x[idx2], particles.curr_pos_y[idx2]};
    
    vec2 posDiff = pos1 - pos2;
    float distSquared = posDiff.x * posDiff.x + posDiff.y * posDiff.y;
    float minDist = particles.radius[idx1] + particles.radius[idx2];

    if (distSquared < minDist * minDist) {
        float dist = sqrt(distSquared);
        vec2 normal = posDiff / dist;

        float massRatio1 = particles.radius[idx1] / (particles.radius[idx1] + particles.radius[idx2]);
        float massRatio2 = particles.radius[idx2] / (particles.radius[idx1] + particles.radius[idx2]);

        float delta = 0.5f * responseCoef * (dist - minDist);

        particles.curr_pos_x[idx1] -= normal.x * (massRatio2 * delta);
        particles.curr_pos_y[idx1] -= normal.y * (massRatio2 * delta);
        particles.curr_pos_x[idx2] += normal.x * (massRatio1 * delta);
        particles.curr_pos_y[idx2] += normal.y * (massRatio1 * delta);
    }
}

void PhysicsEngine::clampToBounds(Particles& particles, size_t idx) {
    const float radius = particles.radius[idx];
    const float collisionOffset = radius * 0.1f;
    const float hardClampOffset = radius * 0.5f;
    
    vec2 curr_pos{particles.curr_pos_x[idx], particles.curr_pos_y[idx]};
    
    if (curr_pos.x - radius < boundMin.x + collisionOffset) {
        vec2 boundary_pos{boundMin.x - radius + collisionOffset, curr_pos.y};
        float delta = boundMin.x + collisionOffset - (curr_pos.x - radius);
        particles.curr_pos_x[idx] += delta;
    }
    
    if (curr_pos.x + radius > boundMax.x - collisionOffset) {
        vec2 boundary_pos{boundMax.x + radius - collisionOffset, curr_pos.y};
        float delta = (curr_pos.x + radius) - (boundMax.x - collisionOffset);
        particles.curr_pos_x[idx] -= delta;
    }
    
    if (curr_pos.y - radius < boundMin.y + collisionOffset) {
        vec2 boundary_pos{curr_pos.x, boundMin.y - radius + collisionOffset};
        float delta = boundMin.y + collisionOffset - (curr_pos.y - radius);
        particles.curr_pos_y[idx] += delta;
    }
    
    if (curr_pos.y + radius > boundMax.y - collisionOffset) {
        vec2 boundary_pos{curr_pos.x, boundMax.y + radius - collisionOffset};
        float delta = (curr_pos.y + radius) - (boundMax.y - collisionOffset);
        particles.curr_pos_y[idx] -= delta;
    }
    
    curr_pos = {particles.curr_pos_x[idx], particles.curr_pos_y[idx]};
    
    if (curr_pos.x - radius < boundMin.x - hardClampOffset) {
        particles.curr_pos_x[idx] = boundMin.x - hardClampOffset + radius;
    }
    if (curr_pos.x + radius > boundMax.x + hardClampOffset) {
        particles.curr_pos_x[idx] = boundMax.x + hardClampOffset - radius;
    }
    if (curr_pos.y - radius < boundMin.y - hardClampOffset) {
        particles.curr_pos_y[idx] = boundMin.y - hardClampOffset + radius;
    }
    if (curr_pos.y + radius > boundMax.y + hardClampOffset) {
        particles.curr_pos_y[idx] = boundMax.y + hardClampOffset - radius;
    }
}
