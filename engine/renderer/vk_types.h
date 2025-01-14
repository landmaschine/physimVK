#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <span>
#include <array>
#include <functional>
#include <deque>
#include <iostream>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "VMA/vk_mem_alloc.h"

#include "math/simMath.hpp"


#define VK_CHECK(x)                                                    \
    do {                                                               \
        VkResult err = x;                                              \
        if (err) {                                                     \
            std::cout << "Detected Vulkan error in " << __FILE__       \
                     << " at line " << __LINE__ << ": "                \
                     << string_VkResult(err) << std::endl;             \
            abort();                                                   \
        }                                                              \
    } while (0)

struct AllocatedImage {
    VkImage image;
    VkImageView imageView;
    VmaAllocation allocation;
    VkExtent3D imageExtent;
    VkFormat imageFormat;
};

struct AllocatedBuffer {
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo info;
};

struct Vertex {
    glm::vec3 position;
    float uv_x;
    glm::vec3 normal;
    float uv_y;
    glm::vec4 color;
};

struct GPUMeshBuffers {
    AllocatedBuffer indexBuffer;
    AllocatedBuffer vertexBuffer;
    VkDeviceAddress vertexBufferAddress;
};

struct GPUDrawPushConstants {
    glm::mat4 projection;
    glm::mat4 world_matrix;
    VkDeviceAddress vertexBuffer;
};

struct ParticleInstance {
    vec2 position;  
    float radius;
    float padding;
};