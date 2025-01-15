#include "physics.hpp"

void PhysicsEngine::update(std::vector<Particle>& particles, float dt) {
    solveVerlet(particles, dt);
    checkCollisions(particles);
}

void PhysicsEngine::setPhysicsBoundary(const vec2 minBoundary, const vec2 maxBoundary) {
    boundMin = minBoundary;
    boundMax = maxBoundary;
}

void PhysicsEngine::solveVerlet(std::vector<Particle>& particles, float dt) {
    const vec2 gravity{0.f, 500.f};

    auto verlstart = std::chrono::high_resolution_clock::now();

    for(auto& particle : particles) {
        if(!particle.isPlayer) {
            vec2 vel = particle.curr_pos - particle.prev_pos;

            vec2 temp = particle.curr_pos;
            particle.accel += gravity;
            particle.curr_pos = particle.curr_pos + vel + particle.accel * (dt * dt);
            particle.prev_pos = temp;
            particle.accel = vec2(0.0f, 0.0f);
        }
        clampToBounds(particle);
    }

    auto verlend = std::chrono::high_resolution_clock::now();
    auto velduration = std::chrono::duration_cast<std::chrono::microseconds>(verlend - verlstart);
}

void PhysicsEngine::checkCollisions(std::vector<Particle>& particles) {
    grid.clear();
    for(const auto& particle : particles) {
        grid.insert(particle);
    }

    for(auto& particle : particles) {
        auto neighbors = grid.getNeighbors(particle);
        for(const auto* neighbor : neighbors) {
            if(&particle != neighbor) {
                checkCollision(particle, *const_cast<Particle*>(neighbor));
            }
        }
    }
}

void PhysicsEngine::checkCollision(Particle& particle1, Particle& particle2) {
    const float responseCoef = 0.75f;

    vec2 posDiff = particle1.curr_pos - particle2.curr_pos;
    float distSquared = posDiff.x * posDiff.x + posDiff.y * posDiff.y;
    float minDist = particle1.radius + particle2.radius;

    if (distSquared < minDist * minDist) {
        float dist = sqrt(distSquared);
        vec2 normal = posDiff / dist;

        float massRatio1 = particle1.radius / (particle1.radius + particle2.radius);
        float massRatio2 = particle2.radius / (particle1.radius + particle2.radius);

        float delta = 0.5f * responseCoef * (dist - minDist);

        particle1.curr_pos -= normal * (massRatio2 * delta);
        particle2.curr_pos += normal * (massRatio1 * delta);
    }
}

void PhysicsEngine::clampToBounds(Particle& particle) {
    const float collisionOffset = particle.radius * 0.1f; 
    const float hardClampOffset = particle.radius * 0.5f;
    
    Particle boundaryParticle = particle;
    
    if (particle.curr_pos.x - particle.radius < boundMin.x + collisionOffset) {
        boundaryParticle.curr_pos = vec2(boundMin.x - particle.radius + collisionOffset, particle.curr_pos.y);
        checkCollision(particle, boundaryParticle);
    }
    
    if (particle.curr_pos.x + particle.radius > boundMax.x - collisionOffset) {
        boundaryParticle.curr_pos = vec2(boundMax.x + particle.radius - collisionOffset, particle.curr_pos.y);
        checkCollision(particle, boundaryParticle);
    }
    
    if (particle.curr_pos.y - particle.radius < boundMin.y + collisionOffset) {
        boundaryParticle.curr_pos = vec2(particle.curr_pos.x, boundMin.y - particle.radius + collisionOffset);
        checkCollision(particle, boundaryParticle);
    }
    
    if (particle.curr_pos.y + particle.radius > boundMax.y - collisionOffset) {
        boundaryParticle.curr_pos = vec2(particle.curr_pos.x, boundMax.y + particle.radius - collisionOffset);
        checkCollision(particle, boundaryParticle);
    }
    
    if (particle.curr_pos.x - particle.radius < boundMin.x - hardClampOffset) {
        particle.curr_pos.x = boundMin.x - hardClampOffset + particle.radius;
    }
    if (particle.curr_pos.x + particle.radius > boundMax.x + hardClampOffset) {
        particle.curr_pos.x = boundMax.x + hardClampOffset - particle.radius;
    }
    if (particle.curr_pos.y - particle.radius < boundMin.y - hardClampOffset) {
        particle.curr_pos.y = boundMin.y - hardClampOffset + particle.radius;
    }
    if (particle.curr_pos.y + particle.radius > boundMax.y + hardClampOffset) {
        particle.curr_pos.y = boundMax.y + hardClampOffset - particle.radius;
    }
}