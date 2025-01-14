#pragma once
#include <iostream>
#include <cmath>

struct vec2 {
    float x, y;

    vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

    vec2 operator-() const { return vec2(-x, -y); }

    vec2 operator+(const vec2& other) const { return vec2(x + other.x, y + other.y); }
    vec2 operator-(const vec2& other) const { return vec2(x - other.x, y - other.y); }
    vec2 operator*(const vec2& other) const { return vec2(x * other.x, y * other.y); }
    vec2 operator/(const vec2& other) const { return vec2(x / other.x, y / other.y); } 

    vec2 operator*(float scalar) const { return vec2(x * scalar, y * scalar); }
    vec2 operator/(float scalar) const { return vec2(x / scalar, y / scalar); }
    
    friend vec2 operator*(float scalar, const vec2& v) {
        return { v.x * scalar, v.y * scalar };
    }
    
    vec2& operator+=(const vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    vec2& operator-=(const vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    vec2& operator*=(const vec2& other) {
        x *= other.x;
        y *= other.y;
        return *this;
    }
    vec2& operator/=(const vec2& other) {
        x /= other.x;
        y /= other.y;
        return *this;
    }
    vec2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    vec2& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    bool operator==(const vec2& other) const { return x == other.x && y == other.y; }
    bool operator!=(const vec2& other) const { return !(*this == other); }

    // uses sqrt
    float magnitude() const { return std::sqrt(x * x + y * y); }

    vec2 normalized() const {
        float mag = magnitude();
        return mag > 0 ? *this / mag : vec2(0, 0);
    }

    float dot(const vec2& other) const {
        return x * other.x + y * other.y;
    }

    friend std::ostream& operator<<(std::ostream& os, const vec2& v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }
};

inline float dot(const vec2& a, const vec2& b) {
    return a.x * b.x + a.y * b.y;
}