glslangValidator -V -S comp gradient_comp.glsl -o gradient_comp.spv
glslangValidator -V -S comp gradient_compPush.glsl -o gradient_compPush.spv
glslangValidator -V -S comp sky_comp.glsl -o sky_comp.spv
glslangValidator -V -S comp black_comp.glsl -o black_comp.spv
glslangValidator -V -S comp particle_collision_comp.glsl -o particle_collision_comp.spv

glslangValidator -V -S vert colored_triangle.vert -o colored_triangle.vert.spv
glslangValidator -V -S frag colored_triangle.frag -o colored_triangle.frag.spv

glslangValidator -V -S vert particle_mesh.vert -o particle_mesh.vert.spv
glslangValidator -V -S vert particle_instance.vert -o particle_instance.vert.spv