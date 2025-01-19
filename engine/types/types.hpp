#pragma once
#include "math/simMath.hpp"

#include <string>
#include <fstream>
#include <vector>
#include <cstddef>
#include <memory>
#include <immintrin.h>
#include "utils/AllocatorAligned.hpp"
#include <cstring>


struct Particle {
    vec2 curr_pos;
    vec2 prev_pos;
    float radius;
};

struct Particles {
    float* curr_pos_x;
    float* curr_pos_y;
    float* prev_pos_x;
    float* prev_pos_y;
    float* radius;
    size_t capacity;
    size_t count;

    explicit Particles(size_t inital_capacity = 0)
        : curr_pos_x(nullptr)
        , curr_pos_y(nullptr)
        , prev_pos_x(nullptr)
        , prev_pos_y(nullptr)
        , radius(nullptr)
        , capacity(0)
        , count(0) {
            if(inital_capacity > 0) {
                reserve(inital_capacity);
            }
        }

        ~Particles() {
            deallocate_arrays();
        }

        Particles(const Particles&) = delete;
        Particles& operator=(const Particles&) = delete;

        Particles(Particles&& other) noexcept {
            *this = std::move(other);
        }

        Particles& operator=(Particles&& other) noexcept {
            if(this != &other) {
                deallocate_arrays();

                curr_pos_x = other.curr_pos_x;
                curr_pos_y = other.curr_pos_y;
                prev_pos_x = other.prev_pos_x;
                prev_pos_y = other.prev_pos_y;
                radius = other.radius;
                capacity = other.capacity;
                count = other.count;

                other.curr_pos_x = nullptr;
                other.curr_pos_y = nullptr;
                other.prev_pos_x = nullptr;
                other.prev_pos_y = nullptr;
                other.radius = nullptr;
                other.capacity = 0;
                other.count = 0;
            }
            return *this;
        }

        void reserve(size_t new_capacity) {
            if(new_capacity <= capacity) return;

            float* new_curr_pos_x = AlignedAllocator::allocate(new_capacity);
            float* new_curr_pos_y = AlignedAllocator::allocate(new_capacity);
            float* new_prev_pos_x = AlignedAllocator::allocate(new_capacity);
            float* new_prev_pos_y = AlignedAllocator::allocate(new_capacity);
            float* new_radius = AlignedAllocator::allocate(new_capacity);

            if(!new_curr_pos_x || !new_curr_pos_y || !new_prev_pos_x || !new_prev_pos_y || !new_radius) {
                AlignedAllocator::deallocate(new_curr_pos_x);
                AlignedAllocator::deallocate(new_curr_pos_y);
                AlignedAllocator::deallocate(new_prev_pos_x);
                AlignedAllocator::deallocate(new_prev_pos_y);
                AlignedAllocator::deallocate(new_radius);
                throw std::bad_alloc();
            }

            if (count > 0) {
                std::memcpy(new_curr_pos_x, curr_pos_x, count * sizeof(float));
                std::memcpy(new_curr_pos_y, curr_pos_y, count * sizeof(float));
                std::memcpy(new_prev_pos_x, prev_pos_x, count * sizeof(float));
                std::memcpy(new_prev_pos_y, prev_pos_y, count * sizeof(float));
                std::memcpy(new_radius, radius, count * sizeof(float));
            }

            deallocate_arrays();

            curr_pos_x = new_curr_pos_x;
            curr_pos_y = new_curr_pos_y;
            prev_pos_x = new_prev_pos_x;
            prev_pos_y = new_prev_pos_y;
            radius = new_radius;
            capacity = new_capacity;
        }

        void add_particle(const vec2& curr_pos, const vec2& prev_pos, float r) {
            if(count >= capacity) {
                size_t new_capacity = capacity == 0 ? 1 : capacity * 2;
                reserve(new_capacity);
            }

            curr_pos_x[count] = curr_pos.x;
            curr_pos_y[count] = curr_pos.y;
            prev_pos_x[count] = prev_pos.x;
            prev_pos_y[count] = prev_pos.y;
            radius[count] = r;
            ++count;
        }

        size_t size() const {
            return count;
        }

        void clear() {
            count = 0;
        }

private:    
    void deallocate_arrays() {
        AlignedAllocator::deallocate(curr_pos_x);
        AlignedAllocator::deallocate(curr_pos_y);
        AlignedAllocator::deallocate(prev_pos_x);
        AlignedAllocator::deallocate(prev_pos_y);
        AlignedAllocator::deallocate(radius);
    }
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
