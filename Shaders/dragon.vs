#version 330

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texture;
layout (location = 3) in ivec4 BoneIDs;           // Dimensional Integer Vector
layout (location = 4) in vec4 Weights;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

const int MAX_BONES = 100;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

uniform mat4 gBones[MAX_BONES];

void main(){

    mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    BoneTransform     += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform     += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform     += gBones[BoneIDs[3]] * Weights[3];

    vec4 PosL = BoneTransform * vec4(vertex_position,1.0);

    gl_Position = proj * view * model * PosL;

    TexCoords = vertex_texture;

    Normal = mat3(transpose(inverse(model))) * (BoneTransform * vec4(vertex_normal, 0.0)).xyz;
    FragPos = vec3(model * PosL);
}

