#include "Shader.h"
#include <cstdio>
#include <cstring>

Shader::Shader(const char* vertSrc, const char* fragSrc) {
    GLuint vert = compile(GL_VERTEX_SHADER, vertSrc);
    GLuint frag = compile(GL_FRAGMENT_SHADER, fragSrc);
    id = glCreateProgram();
    glAttachShader(id, vert);
    glAttachShader(id, frag);
    glLinkProgram(id);
    GLint ok = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(id, 512, nullptr, log);
        fprintf(stderr, "Shader link error: %s\n", log);
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
}

Shader::~Shader() {
    if (id) glDeleteProgram(id);
}

GLuint Shader::compile(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        fprintf(stderr, "Shader compile error: %s\n", log);
    }
    return s;
}

void Shader::use() const {
    glUseProgram(id);
}

void Shader::setMat4(const char* name, const float* value) const {
    GLint loc = glGetUniformLocation(id, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, value);
}

void Shader::setVec3(const char* name, float x, float y, float z) const {
    GLint loc = glGetUniformLocation(id, name);
    glUniform3f(loc, x, y, z);
}

void Shader::setFloat(const char* name, float value) const {
    GLint loc = glGetUniformLocation(id, name);
    glUniform1f(loc, value);
}

void Shader::setVec4(const char* name, float x, float y, float z, float w) const {
    GLint loc = glGetUniformLocation(id, name);
    glUniform4f(loc, x, y, z, w);
}
