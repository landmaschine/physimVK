#pragma once
#include <iostream>
#include <cmath>
#include "vec2.hpp"

struct vec2i {
    int x, y;

    vec2i(int x = 0, int y = 0) : x(x), y(y) {}

    vec2i operator-() const { return vec2i(-x, -y); }

    vec2i operator+(const vec2i& other) const { return vec2i(x + other.x, y + other.y); }
    vec2i operator-(const vec2i& other) const { return vec2i(x - other.x, y - other.y); }
    vec2i operator*(const vec2i& other) const { return vec2i(x * other.x, y * other.y); }
    vec2i operator/(const vec2i& other) const { return vec2i(x / other.x, y / other.y); } 

    vec2i operator*(int scalar) const { return vec2i(x * scalar, y * scalar); }
    vec2i operator/(int scalar) const { return vec2i(x / scalar, y / scalar); }
    
    friend vec2i operator*(int scalar, const vec2i& v) {
        return { v.x * scalar, v.y * scalar };
    }
    
    vec2i& operator+=(const vec2i& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    vec2i& operator-=(const vec2i& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    vec2i& operator*=(const vec2i& other) {
        x *= other.x;
        y *= other.y;
        return *this;
    }
    vec2i& operator/=(const vec2i& other) {
        x /= other.x;
        y /= other.y;
        return *this;
    }
    vec2i& operator*=(int scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    vec2i& operator/=(int scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    bool operator==(const vec2i& other) const { return x == other.x && y == other.y; }
    bool operator!=(const vec2i& other) const { return !(*this == other); }

    // Returns magnitude as a float since it involves square root
    float magnitude() const { return std::sqrt(static_cast<float>(x * x + y * y)); }

    // Returns a floating point normalized vector since normalization always involves division
    vec2 normalized() const {
        float mag = magnitude();
        return mag > 0 ? vec2(x / mag, y / mag) : vec2(0, 0);
    }

    int dot(const vec2i& other) const {
        return x * other.x + y * other.y;
    }

    friend std::ostream& operator<<(std::ostream& os, const vec2i& v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }
};

inline int dot(const vec2i& a, const vec2i& b) {
    return a.x * b.x + a.y * b.y;
}