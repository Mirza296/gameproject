#include "GameProject.hpp"

// Player setup  
void setupPlayer(Player& player, b2WorldId world)
{
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_dynamicBody;
    bd.fixedRotation = true;
    bd.position = { 0.f, WORLD_FLOOR + 2.f };
    player.id = b2CreateBody(world, &bd);

    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = 1.f;
    b2Circle c{ {0.f, 0.f}, player.radius };
    b2CreateCircleShape(player.id, &sd, &c);

    player.gfx = sf::CircleShape(player.radius * PX);
    player.gfx.setOrigin(player.radius * PX, player.radius * PX);
    player.gfx.setFillColor(sf::Color::Cyan);
    player.gfx.setOutlineColor(sf::Color::White);
    player.gfx.setOutlineThickness(4);

    // Gun symbol
    player.gun.setSize({ player.radius * PX * 1.2f, player.radius * PX * 0.4f });
    player.gun.setOrigin(
        player.gun.getSize().x * 0.2f,
        player.gun.getSize().y / 2.f
    );
    player.gun.setFillColor(sf::Color(40, 40, 40));

    // Muzzle flash
    player.muzzle = sf::CircleShape(player.radius * PX * 0.3f);
    player.muzzle.setOrigin(player.muzzle.getRadius(), player.muzzle.getRadius());
    player.muzzle.setFillColor(sf::Color::Yellow);
}

// Enemy spawn 
void spawnEnemy(std::vector<Enemy>& enemies,
    b2WorldId world,
    std::mt19937& rng,
    std::uniform_int_distribution<int>& col,
    std::uniform_int_distribution<int>& scoreDist,
    std::uniform_real_distribution<float>& xSpawn,
    std::uniform_real_distribution<float>& pathTimeDist,
    std::uniform_real_distribution<float>& jumpCDDist)
{
    if ((int)enemies.size() >= MAX_ENEMIES) return;

    Enemy e;
    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_dynamicBody;
    bd.position = { xSpawn(rng), WORLD_CEIL - 0.5f };
    e.id = b2CreateBody(world, &bd);

    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = 1.f;
    b2Circle c{ {0.f, 0.f}, e.radius };
    b2CreateCircleShape(e.id, &sd, &c);

    e.gfx = sf::CircleShape(e.radius * PX);
    e.gfx.setOrigin(e.radius * PX, e.radius * PX);
    e.gfx.setFillColor(sf::Color(col(rng), col(rng) / 2, col(rng)));
    e.scoreValue = scoreDist(rng);
    e.alive = true;
    e.sideBias = (rng() % 2 == 0) ? -1.f : 1.f;
    e.pathTimer = pathTimeDist(rng);
    e.jumpCooldown = jumpCDDist(rng);

    enemies.push_back(e);
}

// Bullet shoot 
void shoot(std::deque<Bullet>& bullets,
    Player& player,
    float dir,
    b2WorldId world)
{
    Bullet b;

    b2BodyDef bd = b2DefaultBodyDef();
    bd.type = b2_dynamicBody;

    b2Vec2 p = b2Body_GetPosition(player.id);
    float offset = player.radius + 0.4f;
    // world units (meters) 
    bd.position = { p.x + dir * offset, p.y + 0.1f };
    b.id = b2CreateBody(world, &bd);

    b2ShapeDef sd = b2DefaultShapeDef();
    sd.density = 0.2f;
    b2Circle c{ {0.f, 0.f}, 0.15f };
    b2CreateCircleShape(b.id, &sd, &c);

    b2Body_SetLinearVelocity(b.id, { dir * 15.f, 2.f });

    b.gfx = sf::CircleShape(0.15f * PX);
    b.gfx.setOrigin(0.15f * PX, 0.15f * PX);
    b.gfx.setFillColor(sf::Color::Yellow);

    bullets.push_back(b);

    // show muzzle flash briefly  
    player.muzzleTimer = 0.08f;
}
