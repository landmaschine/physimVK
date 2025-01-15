#version 460

layout (local_size_x = 32, local_size_y = 32) in;

layout(rgba16f,set = 0, binding = 0) uniform image2D image;

void main() {
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(image);

    if (texelCoord.x < size.x && texelCoord.y < size.y) {
        vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
        imageStore(image, texelCoord, color);
    }

    imageStore(image, texelCoord, color);
}