#pragma once
#include "core/common.hpp"
#include "SDL/SDL3/SDL_vulkan.h"
#include "SDL/SDL3/SDL_stdinc.h"

#include "vk_types.h"
#include "vk_initializers.h"
#include "vk_images.h"
#include "vk_util.hpp"
#include "vk_descriptors.h"
#include "vk_pipelines.h"

#include "vkbootstrap/VkBootstrap.h"

struct FrameData {
	VkSemaphore _swapchainSemaphore, _renderSemaphore;
	VkFence _renderFence;

	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	DeletionQueue _deletionQueue;
};

constexpr unsigned int FRAME_OVERLAP = 2;

struct ComputePushConstants {
	glm::vec4 data1;
	glm::vec4 data2;
	glm::vec4 data3;
	glm::vec4 data4;
};

struct ComputeEffect {
	const char* name;

	VkPipeline pipeline;
	VkPipelineLayout layout;

	ComputePushConstants data;
};

struct CircleMesh {
	GPUMeshBuffers buffers;
	uint32_t indexCount;
};

struct {
    AllocatedBuffer instanceBuffer;
    uint32_t maxParticles{100000};
} _particleBuffers;

struct ParticleComputeSystem {
	VkPipeline pipeline;
	VkPipelineLayout layout;
	VkDescriptorSetLayout descriptorLayout;
	VkDescriptorSet descriptorSet;

	AllocatedBuffer particleBufferIn;
	AllocatedBuffer particleBufferOut;
};

class RendererVK {
public:
	bool _isInitialized{ false };
	int _frameNumber {0};
	bool stop_rendering{false};
	VkExtent2D _windowExtent{};

	struct SDL_Window* _window{ nullptr };

	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debug_messenger;
	VkPhysicalDevice _chosenGPU;
	VkDevice _device;

	FrameData _frames[FRAME_OVERLAP];

	FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; };

	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;	

	VkSurfaceKHR _surface;
	VkSwapchainKHR _swapchain;
	VkFormat _swapchainImageFormat;
	VkExtent2D _swapchainExtent;
	VkExtent2D _drawExtent;

	DescriptorAllocator globalDescriptorAllocator;

	AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
	void destroy_buffer(const AllocatedBuffer& buffer);
	GPUMeshBuffers uploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);

	VkPipelineLayout _meshPipelineLayout;
	VkPipeline _meshPipeline;

	CircleMesh _circleMesh;

	GPUMeshBuffers rectangle;

	void init_mesh_pipeline();

	VkPipelineLayout _trianglePipelineLayout;
	VkPipeline _trianglePipeline;

	VkPipelineLayout _gradientPipelineLayout;

	std::vector<VkFramebuffer> _framebuffers;
	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;

	VkDescriptorSet _drawImageDescriptors;
	VkDescriptorSetLayout _drawImageDescriptorLayout;

	DeletionQueue _mainDeletionQueue;
	DeletionQueue _swapchainDeletionQueue;

	VmaAllocator _allocator;
	VkFence _immFence;
	VkCommandBuffer _immCommandBuffer;
	VkCommandPool _immCommandPool;

	AllocatedImage _drawImage;

	std::vector<ComputeEffect> backgroundEffects;
	int currentBackgroundEffect{ 0 };

	void init(int width, int height);
	void cleanup();
	void draw(const std::vector<Particle>& particles);
	void resize(uint32_t width, uint32_t height);

	void draw_background(VkCommandBuffer cmd);
	void draw_geometry(VkCommandBuffer cmd, const std::vector<Particle>& particles);
	void draw_imgui(VkCommandBuffer cmd,  VkImageView targetImageView);

	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);
	void init_particle_buffers();
private:
    void init_vulkan();
    void init_swapchain();
    void init_commands();
    void init_sync_structures();
    void init_descriptors();
    void init_pipelines();
    void init_background_pipelines();
    void init_imgui();
	void init_circle_mesh(int segments = 32);

	glm::mat4 projection;
    
	void create_swapchain(uint32_t width, uint32_t height);
    void destroy_swapchain();
};