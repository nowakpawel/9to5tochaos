#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include "Camera.h"

struct Vertex3D { float x, y, z, r, g, b; };
struct Vertex2D { float x, y, r, g, b, a; };

class Renderer {
public:
    Renderer();
    ~Renderer();
    void init(int screenW, int screenH);
    void beginFrame();
    void endFrame();
    void setCamera(const Camera& cam);

    void drawQuad3D(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d, const glm::vec3& color);
    void drawBox3D(const glm::vec3& mn, const glm::vec3& mx, const glm::vec3& color);
    void flushGeometry();

    void drawRect2D(float x, float y, float w, float h, float r, float g, float b, float a = 1.0f);
    void drawText2D(float x, float y, float scale, float r, float g, float b, const char* text);
    void flushHUD();

    int screenW{1280}, screenH{720};

private:
    Shader* m_basic3D{nullptr};
    Shader* m_hud2D{nullptr};

    GLuint m_vao3D{0}, m_vbo3D{0};
    GLuint m_vaoHUD{0}, m_vboHUD{0};

    std::vector<Vertex3D> m_verts3D;
    std::vector<Vertex2D> m_vertsHUD;

    glm::mat4 m_view{1.0f};
    glm::mat4 m_proj{1.0f};
    glm::mat4 m_hudProj{1.0f};

    void buildFontTexture();
    void drawChar2D(float x, float y, float scale, float r, float g, float b, char c);

    static const uint8_t s_font[128][8];
};
