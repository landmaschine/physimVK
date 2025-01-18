#pragma once
#include "math/simMath.hpp"

#include <string>
#include <fstream>
#include <vector>

struct Particle {
    vec2 curr_pos;
    vec2 prev_pos;
    float radius;
};

struct Particles {
    std::vector<float> curr_pos_x;
    std::vector<float> curr_pos_y;
    std::vector<float> prev_pos_x;
    std::vector<float> prev_pos_y;
    std::vector<float> radius;

    explicit Particles(size_t initial_capacity = 0) {
        reserve(initial_capacity);
    }

    void reserve(size_t capacity) {
        curr_pos_x.reserve(capacity);
        curr_pos_y.reserve(capacity);
        prev_pos_x.reserve(capacity);
        prev_pos_y.reserve(capacity);
        radius.reserve(capacity);
    }

    void add_particle(const vec2& curr_pos, const vec2& prev_pos, float r) {
        curr_pos_x.push_back(curr_pos.x);
        curr_pos_y.push_back(curr_pos.y);
        prev_pos_x.push_back(prev_pos.x);
        prev_pos_y.push_back(prev_pos.y);
        radius.push_back(r);
    }

    void remove_particle(size_t index) {
        if (index >= size()) return;

        size_t last = size() - 1;
        if (index != last) {
            curr_pos_x[index] = curr_pos_x[last];
            curr_pos_y[index] = curr_pos_y[last];
            prev_pos_x[index] = prev_pos_x[last];
            prev_pos_y[index] = prev_pos_y[last];
            radius[index] = radius[last];
        }
        
        curr_pos_x.pop_back();
        curr_pos_y.pop_back();
        prev_pos_x.pop_back();
        prev_pos_y.pop_back();
        radius.pop_back();
    }

    size_t size() const {
        return curr_pos_x.size();
    }

    void clear() {
        curr_pos_x.clear();
        curr_pos_y.clear();
        prev_pos_x.clear();
        prev_pos_y.clear();
        radius.clear();
    }

    void set_curr_pos(size_t index, const vec2& pos) {
        if (index >= size()) return;
        curr_pos_x[index] = pos.x;
        curr_pos_y[index] = pos.y;
    }

    void set_prev_pos(size_t index, const vec2& pos) {
        if (index >= size()) return;
        prev_pos_x[index] = pos.x;
        prev_pos_y[index] = pos.y;
    }

    vec2 get_curr_pos(size_t index) const {
        return {curr_pos_x[index], curr_pos_y[index]};
    }

    vec2 get_prev_pos(size_t index) const {
        return {prev_pos_x[index], prev_pos_y[index]};
    }

    float* curr_pos_x_data() { return curr_pos_x.data(); }
    float* curr_pos_y_data() { return curr_pos_y.data(); }
    float* prev_pos_x_data() { return prev_pos_x.data(); }
    float* prev_pos_y_data() { return prev_pos_y.data(); }
    float* radius_data() { return radius.data(); }
};

typedef struct EnginePerformanceData_t {
    double frameTime = 0;
    double inputTime = 0;
    double updateTime = 0;
    double collisionTime = 0;
    double gridTime = 0;
    double verletTime = 0;
    double renderTime = 0;
    double fps = 0;
} EnginePerformanceData;

struct ProcessMemoryStats {
    long minor_faults;
    long major_faults;

    static ProcessMemoryStats get() {
        ProcessMemoryStats stats;
        std::ifstream stat_file("proc/self/stat");

        std::string unused;
        for(int i = 0; i < 9; i++) {
            stat_file >> unused;
        }
        stat_file >> stats.minor_faults;
        stat_file >> unused;
        stat_file >> stats.major_faults;

        return stats;
    }
};
