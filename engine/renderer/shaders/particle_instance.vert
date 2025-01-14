#version 450
#extension GL_EXT_buffer_reference : require

struct Vertex {

	vec3 position;
	float uv_x;
	vec3 normal;
	float uv_y;
	vec4 color;
}; 

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 2) in vec2 instancePos;
layout(location = 3) in float instanceRadius;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform constants {
    mat4 projection;
    Vertex vertexBuffer;
} PushConstants;

void main() {
    vec2 worldPos = (inPosition.xy * instanceRadius) + instancePos;
    gl_Position = PushConstants.projection * vec4(worldPos, 0.0, 1.0);

    outColor = inColor;
}