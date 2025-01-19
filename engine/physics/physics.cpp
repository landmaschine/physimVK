#include "physics.hpp"
#include <x86intrin.h>

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

    float* curr_pos_x = particles.curr_pos_x;
    float* curr_pos_y = particles.curr_pos_y;
    float* prev_pos_x = particles.prev_pos_x;
    float* prev_pos_y = particles.prev_pos_y;

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
    size_t num_particles = particles.size();

    for(size_t i = 0; i < num_particles; i++) {
        size_t j = i + 1;

        _mm_prefetch(&particles.curr_pos_x[i + 64], _MM_HINT_T0);
        _mm_prefetch(&particles.curr_pos_y[i + 64], _MM_HINT_T0);
        _mm_prefetch(&particles.radius[i + 64], _MM_HINT_T0);

        for(; j + 31 < num_particles; j+=32) {
            checkCollisionSIMD(particles, i, j);
        }

        for(; j + 7 < num_particles; j+=8) {
            checkCollisionSIMDsmall(particles, i, j);
        }

        for(; j < num_particles; j++) {
            checkCollision(particles, i, j);
        }
    }
}

void PhysicsEngine::checkCollision(Particles& particles, size_t idx1, size_t idx2) {
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

void PhysicsEngine::checkCollisionSIMD(Particles& particles, size_t idx1, size_t j_start) {
    __m256 pos1_x = _mm256_set1_ps(particles.curr_pos_x[idx1]);
    __m256 pos1_y = _mm256_set1_ps(particles.curr_pos_y[idx1]);
    __m256 radius1 = _mm256_set1_ps(particles.radius[idx1]);

    size_t remaining = particles.size() - j_start;
    if(remaining >= 32) {
        __m256 sum_adj1_x = _mm256_setzero_ps();
        __m256 sum_adj1_y = _mm256_setzero_ps();
        
        for(size_t offset = 0; offset < 32; offset += 8) {
            __m256 pos2_x = _mm256_loadu_ps(&particles.curr_pos_x[j_start + offset]);
            __m256 pos2_y = _mm256_loadu_ps(&particles.curr_pos_y[j_start + offset]);
            __m256 radius2 = _mm256_loadu_ps(&particles.radius[j_start + offset]);

            __m256 diff_x = _mm256_sub_ps(pos1_x, pos2_x);
            __m256 diff_y = _mm256_sub_ps(pos1_y, pos2_y);
            
            __m256 dist_squared = _mm256_add_ps(
                _mm256_mul_ps(diff_x, diff_x),
                _mm256_mul_ps(diff_y, diff_y)
            );

            const float EPSILON = 1e-6f;
            __m256 min_dist = _mm256_add_ps(radius1, radius2);
            __m256 min_dist_squared = _mm256_mul_ps(min_dist, min_dist);
            __m256 collision_mask = _mm256_cmp_ps(dist_squared, min_dist_squared, _CMP_LT_OQ);

            int mask = _mm256_movemask_ps(collision_mask);
            if (mask == 0) continue;

            __m256 dist = _mm256_sqrt_ps(_mm256_add_ps(dist_squared, _mm256_set1_ps(EPSILON)));
            __m256 inv_dist = _mm256_div_ps(_mm256_set1_ps(1.0f), dist);
            __m256 normal_x = _mm256_mul_ps(diff_x, inv_dist);
            __m256 normal_y = _mm256_mul_ps(diff_y, inv_dist);

            __m256 total_radius = _mm256_add_ps(radius1, radius2);
            __m256 inv_total_radius = _mm256_div_ps(_mm256_set1_ps(1.0f), total_radius);
            __m256 mass_ratio1 = _mm256_mul_ps(radius1, inv_total_radius);
            __m256 mass_ratio2 = _mm256_mul_ps(radius2, inv_total_radius);

            __m256 response = _mm256_set1_ps(0.5f * responseCoef);
            __m256 delta = _mm256_mul_ps(response, _mm256_sub_ps(dist, min_dist));

            __m256 masked_mass_ratio1 = _mm256_and_ps(mass_ratio1, collision_mask);
            __m256 masked_mass_ratio2 = _mm256_and_ps(mass_ratio2, collision_mask);
            __m256 masked_delta = _mm256_and_ps(delta, collision_mask);

            __m256 adj2_x = _mm256_mul_ps(_mm256_mul_ps(normal_x, masked_mass_ratio1), masked_delta);
            __m256 adj2_y = _mm256_mul_ps(_mm256_mul_ps(normal_y, masked_mass_ratio1), masked_delta);
            __m256 adj1_x = _mm256_mul_ps(_mm256_mul_ps(normal_x, masked_mass_ratio2), masked_delta);
            __m256 adj1_y = _mm256_mul_ps(_mm256_mul_ps(normal_y, masked_mass_ratio2), masked_delta);

            sum_adj1_x = _mm256_add_ps(sum_adj1_x, adj1_x);
            sum_adj1_y = _mm256_add_ps(sum_adj1_y, adj1_y);

            _mm256_maskstore_ps(&particles.curr_pos_x[j_start + offset], 
                              _mm256_castps_si256(collision_mask), 
                              _mm256_add_ps(_mm256_loadu_ps(&particles.curr_pos_x[j_start + offset]), adj2_x));
            _mm256_maskstore_ps(&particles.curr_pos_y[j_start + offset], 
                              _mm256_castps_si256(collision_mask), 
                              _mm256_add_ps(_mm256_loadu_ps(&particles.curr_pos_y[j_start + offset]), adj2_y));
        }

        __m128 high_x = _mm256_extractf128_ps(sum_adj1_x, 1);
        __m128 low_x = _mm256_castps256_ps128(sum_adj1_x);
        __m128 sum_x = _mm_add_ps(high_x, low_x);
        __m128 total_x = _mm_hadd_ps(_mm_hadd_ps(sum_x, sum_x), sum_x);

        __m128 high_y = _mm256_extractf128_ps(sum_adj1_y, 1);
        __m128 low_y = _mm256_castps256_ps128(sum_adj1_y);
        __m128 sum_y = _mm_add_ps(high_y, low_y);
        __m128 total_y = _mm_hadd_ps(_mm_hadd_ps(sum_y, sum_y), sum_y);

        float adj_x, adj_y;
        _mm_store_ss(&adj_x, total_x);
        _mm_store_ss(&adj_y, total_y);

        particles.curr_pos_x[idx1] -= adj_x;
        particles.curr_pos_y[idx1] -= adj_y;
    }
}

void PhysicsEngine::checkCollisionSIMDsmall(Particles& particles, size_t idx1, size_t j_start) {
    __m256 pos1_x = _mm256_set1_ps(particles.curr_pos_x[idx1]);
    __m256 pos1_y = _mm256_set1_ps(particles.curr_pos_y[idx1]);
    __m256 radius1 = _mm256_set1_ps(particles.radius[idx1]);

    __m256 pos2_x = _mm256_loadu_ps(&particles.curr_pos_x[j_start]);
    __m256 pos2_y = _mm256_loadu_ps(&particles.curr_pos_y[j_start]);
    __m256 radius2 = _mm256_loadu_ps(&particles.radius[j_start]);

    __m256 diff_x = _mm256_sub_ps(pos1_x, pos2_x);
    __m256 diff_y = _mm256_sub_ps(pos1_y, pos2_y);

    __m256 dist_squared = _mm256_add_ps(
        _mm256_mul_ps(diff_x, diff_x),
        _mm256_mul_ps(diff_y, diff_y)
    );

    __m256 min_dist = _mm256_add_ps(radius1, radius2);
    __m256 min_dist_squared = _mm256_mul_ps(min_dist, min_dist);

    __m256 collision_mask = _mm256_cmp_ps(dist_squared, min_dist_squared, _CMP_LT_OQ);

    int mask = _mm256_movemask_ps(collision_mask);
    if (mask == 0) return;
    const float EPSILON = 1e-6f;
    
    __m256 dist = _mm256_sqrt_ps(_mm256_add_ps(dist_squared, _mm256_set1_ps(EPSILON)));
    __m256 inv_dist = _mm256_div_ps(_mm256_set1_ps(1.0f), dist);
    __m256 normal_x = _mm256_mul_ps(diff_x, inv_dist);
    __m256 normal_y = _mm256_mul_ps(diff_y, inv_dist);

    __m256 total_radius = _mm256_add_ps(radius1, radius2);
    __m256 inv_total_radius = _mm256_div_ps(_mm256_set1_ps(1.0f), total_radius);
    __m256 mass_ratio1 = _mm256_mul_ps(radius1, inv_total_radius);
    __m256 mass_ratio2 = _mm256_mul_ps(radius2, inv_total_radius);

    __m256 response = _mm256_set1_ps(0.5f * responseCoef);
    __m256 delta = _mm256_mul_ps(response, _mm256_sub_ps(dist, min_dist));

    __m256 adj2_x = _mm256_mul_ps(_mm256_mul_ps(normal_x, mass_ratio1), delta);
    __m256 adj2_y = _mm256_mul_ps(_mm256_mul_ps(normal_y, mass_ratio1), delta);
    __m256 adj1_x = _mm256_mul_ps(_mm256_mul_ps(normal_x, mass_ratio2), delta);
    __m256 adj1_y = _mm256_mul_ps(_mm256_mul_ps(normal_y, mass_ratio2), delta);

    float* adj1_x_ptr = (float*)&adj1_x;
    float* adj1_y_ptr = (float*)&adj1_y;
    float* adj2_x_ptr = (float*)&adj2_x;
    float* adj2_y_ptr = (float*)&adj2_y;

    for(int k = 0; k < 8; k++) {
        if(mask & (1 << k)) {
            particles.curr_pos_x[idx1] -= adj1_x_ptr[k];
            particles.curr_pos_y[idx1] -= adj1_y_ptr[k];
            particles.curr_pos_x[j_start + k] += adj2_x_ptr[k];
            particles.curr_pos_y[j_start + k] += adj2_y_ptr[k];
        }
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
