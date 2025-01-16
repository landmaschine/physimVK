#pragma once
#include "math/simMath.hpp"

struct Particle {
    vec2 curr_pos;
    vec2 prev_pos;
    vec2 accel;
    float radius;
};

struct Player : public Particle {
    vec2 targetPos;
};

typedef struct EnginePerformanceData_t {
    double frameTime = 0;
    double inputTime = 0;
    double updateTime = 0;
    double collisionTime = 0;
    double verletTime = 0;
    double renderTime = 0;
    double fps = 0;
} EnginePerformanceData;