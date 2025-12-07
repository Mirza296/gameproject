#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <box2d/box2d.h>
#include <vector>
#include <deque>
#include <cmath>
#include <string>
#include <algorithm>
#include <random>
#include <iostream>

// ==== CONSTANTS (same as before) ============================================
constexpr float PX = 30.f;             // pixels per meter
constexpr float DT = 1.f / 60.f;       // timestep
constexpr int   SUB_STEPS = 4;         // Box2D 3.x sub-steps

constexpr float WORLD_LEFT = -21.f;
constexpr float WORLD_RIGHT = 21.f;
constexpr float WORLD_FLOOR = -10.f;
constexpr float WORLD_CEIL = 10.f;

constexpr int   MAX_ENEMIES = 8;

// ==== SMALL HELPERS (same as before) =======================================

struct Bar { float cx, cy, hx, hy; };

inline sf::Vector2f toSFML(const b2Vec2& p, const sf::RenderWindow& window)
{
    return {
        window.getSize().x / 2.f + p.x * PX,
        window.getSize().y / 2.f - p.y * PX
    };
}

inline float vlen(sf::Vector2f v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

// Implemented in GameSetup.cpp
void addStaticBox(b2WorldId world, float cx, float cy, float hx, float hy);
bool isGrounded(const b2Vec2& pos, float radius, const std::vector<Bar>& bars);

// ==== GAME OBJECTS (same as before) ========================================

struct Bullet {
    b2BodyId id{};
    float life = 2.f;
    sf::CircleShape gfx;
};

struct Enemy {
    b2BodyId id{};
    float radius = 0.55f;
    float speed = 4.5f;
    bool  alive = true;
    int   scoreValue = 10;
    sf::CircleShape gfx;

    // random-path behaviour
    float sideBias = 1.f;      // -1 = prefers left side, 1 = right side
    float pathTimer = 0.f;     // when <= 0, choose a new sideBias
    float jumpCooldown = 0.f;  // time until it can jump again
};

struct Player {
    b2BodyId id{};
    int   score = 0;
    float shootCD = 0.f;
    float radius = 0.6f;
    int   jumps = 2;
    float dir = 1.f;       // 1 = right, -1 = left

    sf::CircleShape    gfx;     // body
    sf::RectangleShape gun;     // gun symbol (fire exit)
    sf::CircleShape    muzzle;  // muzzle flash
    float muzzleTimer = 0.f;
};

// ==== MODULE 1 – setup =====================================================

void setupWindowAndBackground(
    sf::RenderWindow& window,
    sf::RectangleShape& bg,
    sf::Texture& gradient);

void setupWorldAndArena(
    b2WorldId& world,
    sf::RectangleShape& border,
    std::vector<Bar>& bars,
    std::vector<sf::RectangleShape>& barGfx,
    const sf::RenderWindow& window);

// ==== MODULE 2 – entities ==================================================

void setupPlayer(Player& player, b2WorldId world);

void spawnEnemy(std::vector<Enemy>& enemies,
    b2WorldId world,
    std::mt19937& rng,
    std::uniform_int_distribution<int>& col,
    std::uniform_int_distribution<int>& scoreDist,
    std::uniform_real_distribution<float>& xSpawn,
    std::uniform_real_distribution<float>& pathTimeDist,
    std::uniform_real_distribution<float>& jumpCDDist);

void shoot(std::deque<Bullet>& bullets,
    Player& player,
    float dir,
    b2WorldId world);

// ==== MODULE 3 – UI + audio ===============================================

void setupText(sf::Font& font,
    sf::Text& scoreText,
    sf::Text& title,
    sf::Text& controls,
    sf::Text& msgText,
    const sf::RenderWindow& window);

void setupAudio(sf::SoundBuffer& fireBuffer,
    sf::SoundBuffer& enemyDeadBuffer,
    sf::SoundBuffer& gameStartBuffer,
    sf::SoundBuffer& lossBuffer,
    sf::SoundBuffer& jumpBuffer,
    sf::Sound& fireSound,
    sf::Sound& enemyDeadSound,
    sf::Sound& gameStartSound,
    sf::Sound& lossSound,
    sf::Sound& jumpSound);

// ==== MODULE 4 – whole game ===============================================

int runGame();
