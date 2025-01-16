#pragma once

template<typename U, typename T>
U to(const T& v) {
    return static_cast<U>(v);
}