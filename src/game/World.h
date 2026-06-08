#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

enum class ObjectType {
    DESK, PRINTER, SERVER_RACK, COFFEE_MACHINE, TERMINAL, NONE
};

struct WorldObject {
    ObjectType type;
    glm::vec3 position;
    glm::vec3 size;       // half-extents for AABB
    glm::vec3 color;
    std::string label;
    bool interactive{true};
};

struct Room {
    glm::vec2 min, max;   // XZ bounds
    float floorY{0}, ceilY{3.0f};
    glm::vec3 floorColor, wallColor, ceilColor;
};

class World {
public:
    World();
    void render(class Renderer& renderer) const;

    const std::vector<WorldObject>& getObjects() const { return m_objects; }
    const WorldObject* getInteractable(const glm::vec3& pos, const glm::vec3& dir, float maxDist) const;

    glm::vec3 resolveCollision(const glm::vec3& pos, float radius) const;
    bool isPositionVisible(const glm::vec3& from, const glm::vec3& to) const;

private:
    std::vector<Room> m_rooms;
    std::vector<WorldObject> m_objects;

    struct WallSegment { glm::vec2 a, b; };
    std::vector<WallSegment> m_walls;

    void buildGeometry();
    void addRoom(float x1, float z1, float x2, float z2,
                 glm::vec3 floor, glm::vec3 wall, glm::vec3 ceil);
    void renderRoom(class Renderer& r, const Room& room) const;
    void renderObject(class Renderer& r, const WorldObject& obj) const;
};
