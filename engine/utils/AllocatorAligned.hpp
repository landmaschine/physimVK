#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <cstddef>
#include <memory>
#include <immintrin.h>

constexpr size_t AVX2_ALIGNMENT = 32;

class AlignedAllocator {
public:
    static float* allocate(size_t count) {
        size_t size = sizeof(void*) + AVX2_ALIGNMENT + (count * sizeof(float));
        void* original_ptr = std::malloc(size);
        if (!original_ptr) return nullptr;

        void* user_ptr = static_cast<char*>(original_ptr) + sizeof(void*);
        size_t remaining_size = size - sizeof(void*);
        void* aligned_ptr = std::align(AVX2_ALIGNMENT, count * sizeof(float), user_ptr, remaining_size);
        
        if (!aligned_ptr) {
            std::free(original_ptr);
            return nullptr;
        }

        *(static_cast<void**>(aligned_ptr) - 1) = original_ptr;
        
        return static_cast<float*>(aligned_ptr);
    }

    static void deallocate(float* ptr) {
        if (ptr) {
            void* original_ptr = *(reinterpret_cast<void**>(ptr) - 1);
            std::free(original_ptr);
        }
    }
};