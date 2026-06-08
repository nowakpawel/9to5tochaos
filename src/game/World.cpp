#include "World.h"
#include "render/Renderer.h"
#include <cmath>
#include <algorithm>

World::World() {
    buildGeometry();
}

void World::addRoom(float x1, float z1, float x2, float z2,
                    glm::vec3 floor, glm::vec3 wall, glm::vec3 ceil) {
    Room r;
    r.min = {x1, z1};
    r.max = {x2, z2};
    r.floorY = 0.0f;
    r.ceilY  = 3.0f;
    r.floorColor = floor;
    r.wallColor  = wall;
    r.ceilColor  = ceil;
    m_rooms.push_back(r);
}

void World::buildGeometry() {
    // Rooms
    addRoom(0,0,20,12,   {0.4f,0.4f,0.4f}, {0.7f,0.7f,0.6f}, {0.9f,0.9f,0.9f}); // Open office
    addRoom(0,12,6,18,   {0.5f,0.4f,0.4f}, {0.8f,0.6f,0.6f}, {0.9f,0.9f,0.9f}); // HR
    addRoom(7,12,13,18,  {0.3f,0.3f,0.35f},{0.6f,0.6f,0.7f}, {0.85f,0.85f,0.85f}); // Corridor
    addRoom(14,12,20,18, {0.4f,0.45f,0.4f},{0.6f,0.7f,0.6f}, {0.9f,0.9f,0.9f}); // Boss
    addRoom(7,18,13,24,  {0.3f,0.3f,0.3f}, {0.4f,0.4f,0.5f}, {0.7f,0.7f,0.7f}); // Server Room

    // Objects
    glm::vec3 deskColor{0.55f,0.4f,0.25f};
    m_objects.push_back({ObjectType::DESK,          {4,0,3},    {1.0f,0.75f,0.6f}, deskColor, "Biurko 1", false});
    m_objects.push_back({ObjectType::DESK,          {10,0,3},   {1.0f,0.75f,0.6f}, deskColor, "Biurko 2", false});
    m_objects.push_back({ObjectType::DESK,          {16,0,3},   {1.0f,0.75f,0.6f}, deskColor, "Biurko 3", false});
    m_objects.push_back({ObjectType::TERMINAL,      {10,0.75f,3},{0.3f,0.3f,0.2f}, {0.2f,0.2f,0.3f}, "Terminal", true});
    m_objects.push_back({ObjectType::PRINTER,       {18,0,8},   {0.4f,0.5f,0.4f}, {0.8f,0.8f,0.8f}, "Drukarka", true});
    m_objects.push_back({ObjectType::SERVER_RACK,   {10,0,21},  {0.6f,1.8f,0.4f}, {0.3f,0.3f,0.4f}, "Serwer", true});
    m_objects.push_back({ObjectType::COFFEE_MACHINE,{1,0,10},   {0.3f,0.7f,0.3f}, {0.2f,0.15f,0.1f}, "Ekspres", true});

    // Outer boundary wall segments (only outer walls for collision)
    m_walls.push_back({{0,0},{20,0}});
    m_walls.push_back({{20,0},{20,24}});
    m_walls.push_back({{0,0},{0,24}});
    m_walls.push_back({{0,24},{20,24}});
    // Internal partial walls (not blocking all movement between rooms)
    m_walls.push_back({{0,12},{6,12}});
    m_walls.push_back({{14,12},{20,12}});
}

void World::renderRoom(Renderer& r, const Room& rm) const {
    float x1 = rm.min.x, z1 = rm.min.y;
    float x2 = rm.max.x, z2 = rm.max.y;
    float fy = rm.floorY;
    float cy = rm.ceilY;

    // Floor
    r.drawQuad3D({x1,fy,z1},{x2,fy,z1},{x2,fy,z2},{x1,fy,z2}, rm.floorColor);
    // Ceiling
    r.drawQuad3D({x1,cy,z1},{x1,cy,z2},{x2,cy,z2},{x2,cy,z1}, rm.ceilColor);
    // Walls
    // South (z=z1)
    r.drawQuad3D({x1,fy,z1},{x2,fy,z1},{x2,cy,z1},{x1,cy,z1}, rm.wallColor);
    // North (z=z2)
    r.drawQuad3D({x2,fy,z2},{x1,fy,z2},{x1,cy,z2},{x2,cy,z2}, rm.wallColor);
    // West (x=x1)
    r.drawQuad3D({x1,fy,z2},{x1,fy,z1},{x1,cy,z1},{x1,cy,z2}, rm.wallColor);
    // East (x=x2)
    r.drawQuad3D({x2,fy,z1},{x2,fy,z2},{x2,cy,z2},{x2,cy,z1}, rm.wallColor);
}

void World::renderObject(Renderer& r, const WorldObject& obj) const {
    glm::vec3 mn = obj.position - glm::vec3(obj.size.x, 0, obj.size.z);
    glm::vec3 mx = obj.position + glm::vec3(obj.size.x, obj.size.y * 2.0f, obj.size.z);
    // For desks: height is actual size.y, not doubled
    if (obj.type == ObjectType::DESK) {
        mn = {obj.position.x - obj.size.x, obj.position.y, obj.position.z - obj.size.z};
        mx = {obj.position.x + obj.size.x, obj.position.y + obj.size.y, obj.position.z + obj.size.z};
    } else if (obj.type == ObjectType::TERMINAL) {
        mn = {obj.position.x - obj.size.x, obj.position.y, obj.position.z - obj.size.z};
        mx = {obj.position.x + obj.size.x, obj.position.y + obj.size.y * 2.0f, obj.position.z + obj.size.z};
    } else {
        mn = {obj.position.x - obj.size.x, obj.position.y, obj.position.z - obj.size.z};
        mx = {obj.position.x + obj.size.x, obj.position.y + obj.size.y * 2.0f, obj.position.z + obj.size.z};
    }
    r.drawBox3D(mn, mx, obj.color);
}

void World::render(Renderer& renderer) const {
    for (const auto& room : m_rooms) renderRoom(renderer, room);
    for (const auto& obj : m_objects) renderObject(renderer, obj);
}

glm::vec3 World::resolveCollision(const glm::vec3& pos, float radius) const {
    glm::vec3 result = pos;
    // World boundary
    if (result.x < radius)       result.x = radius;
    if (result.x > 20.0f-radius) result.x = 20.0f-radius;
    if (result.z < radius)       result.z = radius;
    if (result.z > 24.0f-radius) result.z = 24.0f-radius;

    // Resolve against wall segments
    for (const auto& wall : m_walls) {
        glm::vec2 ab = wall.b - wall.a;
        glm::vec2 ap = glm::vec2(result.x, result.z) - wall.a;
        float len2 = glm::dot(ab, ab);
        if (len2 < 0.0001f) continue;
        float t = glm::dot(ap, ab) / len2;
        t = std::clamp(t, 0.0f, 1.0f);
        glm::vec2 closest = wall.a + t * ab;
        glm::vec2 diff = glm::vec2(result.x, result.z) - closest;
        float dist = glm::length(diff);
        if (dist < radius && dist > 0.0001f) {
            glm::vec2 push = glm::normalize(diff) * (radius - dist);
            result.x += push.x;
            result.z += push.y;
        }
    }
    return result;
}

const WorldObject* World::getInteractable(const glm::vec3& pos, const glm::vec3& dir, float maxDist) const {
    const WorldObject* best = nullptr;
    float bestT = maxDist;

    for (const auto& obj : m_objects) {
        if (!obj.interactive) continue;
        glm::vec3 mn = {obj.position.x - obj.size.x, obj.position.y, obj.position.z - obj.size.z};
        glm::vec3 mx = {obj.position.x + obj.size.x, obj.position.y + obj.size.y * 2.0f + 0.1f, obj.position.z + obj.size.z};

        float tmin = 0.001f, tmax = maxDist;
        for (int i = 0; i < 3; ++i) {
            float invD = 1.0f / (&dir.x)[i];
            float t1 = ((&mn.x)[i] - (&pos.x)[i]) * invD;
            float t2 = ((&mx.x)[i] - (&pos.x)[i]) * invD;
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            if (tmin > tmax) goto next_obj;
        }
        if (tmin < bestT) {
            bestT = tmin;
            best = &obj;
        }
        next_obj:;
    }
    return best;
}

bool World::isPositionVisible(const glm::vec3& from, const glm::vec3& to) const {
    // Simple: check if both points are in the same or adjacent rooms
    auto inRoom = [](const glm::vec2& p, const Room& r) {
        return p.x >= r.min.x && p.x <= r.max.x && p.y >= r.min.y && p.y <= r.max.y;
    };
    glm::vec2 fp{from.x, from.z};
    glm::vec2 tp{to.x, to.z};

    int fromRoom = -1, toRoom = -1;
    for (int i = 0; i < (int)m_rooms.size(); ++i) {
        if (inRoom(fp, m_rooms[i])) fromRoom = i;
        if (inRoom(tp, m_rooms[i])) toRoom = i;
    }
    if (fromRoom == toRoom) return true;
    // Adjacent rooms (empirically: open office connects to everything via corridors)
    // For MVP, allow visibility if distance is short
    float dist = glm::length(from - to);
    return dist < 10.0f;
}
