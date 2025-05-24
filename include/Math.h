#pragma once

#include <glm/glm.hpp>
//#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/quaternion.hpp>

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

typedef float Vec1f;
typedef glm::vec2  Vec2f;
typedef glm::vec3  Vec3f;
typedef glm::vec4  Vec4f;
typedef int Vec1i;
typedef glm::ivec2 Vec2i;
typedef glm::ivec3 Vec3i;
typedef glm::ivec4 Vec4i;
typedef glm::mat4  Mat4;
typedef glm::mat3  Mat3;
typedef glm::mat2  Mat2;
typedef glm::quat  Quat;

