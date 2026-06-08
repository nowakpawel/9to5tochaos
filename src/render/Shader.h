#pragma once
#include <GL/glew.h>
#include <string>

class Shader {
public:
    GLuint id{0};
    Shader() = default;
    Shader(const char* vertSrc, const char* fragSrc);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    void use() const;
    void setMat4(const char* name, const float* value) const;
    void setVec3(const char* name, float x, float y, float z) const;
    void setFloat(const char* name, float value) const;
    void setVec4(const char* name, float x, float y, float z, float w) const;
private:
    static GLuint compile(GLenum type, const char* src);
};
