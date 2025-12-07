#include "GameProject.hpp"

void addStaticBox(b2WorldId world, float cx, float cy, float hx, float hy)
{
    b2BodyDef bd = b2DefaultBodyDef();
    bd.position = { cx, cy };
    b2BodyId body = b2CreateBody(world, &bd);

    b2Polygon box = b2MakeBox(hx, hy);
    b2ShapeDef sd = b2DefaultShapeDef();
    b2CreatePolygonShape(body, &sd, &box);
}

// floor as well as platforms  
bool isGrounded(const b2Vec2& pos, float radius, const std::vector<Bar>& bars)
{
    // Platforms
    for (const auto& b : bars)
    {
        float top = b.cy + b.hy;
        if (std::fabs((pos.y - radius) - top) < 0.05f)
        {
            if (pos.x > b.cx - b.hx && pos.x < b.cx + b.hx)
                return true;
        }
    }

    // Main floor (its top is at WORLD_FLOOR)
    float floorTop = WORLD_FLOOR;
    if (std::fabs((pos.y - radius) - floorTop) < 0.05f)
    {
        if (pos.x > WORLD_LEFT && pos.x < WORLD_RIGHT)
            return true;
    }

    return false;
}

// gradient background  
void setupWindowAndBackground(
    sf::RenderWindow& window,
    sf::RectangleShape& bg,
    sf::Texture& gradient)
{
    sf::Image img;
    img.create(window.getSize().x, window.getSize().y);
    for (unsigned y = 0; y < window.getSize().y; ++y)
    {
        for (unsigned x = 0; x < window.getSize().x; ++x)
        {
            img.setPixel(
                x, y,
                sf::Color(
                    30 + y / 12,
                    15 + y / 18,
                    90 + y / 22
                )
            );
        }
    }

    gradient.loadFromImage(img);
    bg.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
    bg.setTexture(&gradient);
}

// world, walls, border and platforms  e
void setupWorldAndArena(
    b2WorldId& world,
    sf::RectangleShape& border,
    std::vector<Bar>& bars,
    std::vector<sf::RectangleShape>& barGfx,
    const sf::RenderWindow& window)
{
    // Box2D world
    b2WorldDef wd = b2DefaultWorldDef();
    wd.gravity = { 0.f, -9.8f };
    world = b2CreateWorld(&wd);

    // Arena bounds
    addStaticBox(world, 0.f, WORLD_FLOOR - 0.5f, 22.f, 0.5f); // floor
    addStaticBox(world, 0.f, WORLD_CEIL + 0.5f, 22.f, 0.5f);  // ceiling
    addStaticBox(world, WORLD_LEFT - 0.5f, 0.f, 0.5f, 12.f);  // left wall
    addStaticBox(world, WORLD_RIGHT + 0.5f, 0.f, 0.5f, 12.f); // right wall

    // Border
    border.setSize({
        (WORLD_RIGHT - WORLD_LEFT + 1.f) * PX,
        (WORLD_CEIL - WORLD_FLOOR + 1.f) * PX
        });
    border.setOrigin(border.getSize() / 2.f);
    border.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(10.f);

    // Platforms (bars)
    bars.clear();
    auto addBar = [&](float cx, float cy, float hx, float hy)
        {
            addStaticBox(world, cx, cy, hx, hy);
            bars.push_back({ cx, cy, hx, hy });
        };

    addBar(0.f, -5.f, 12.f, 0.4f);
    addBar(8.f, 0.f, 6.f, 0.4f);
    addBar(-10.f, 4.f, 8.f, 0.4f);
    addBar(0.f, 7.f, 10.f, 0.4f);
    addBar(-15.f, -2.f, 5.f, 0.4f);
    addBar(14.f, 3.f, 5.f, 0.4f);

    barGfx.clear();
    for (const auto& b : bars)
    {
        sf::RectangleShape r;
        r.setSize({ 2 * b.hx * PX, 2 * b.hy * PX });
        r.setOrigin(b.hx * PX, b.hy * PX);
        r.setFillColor(sf::Color(130, 90, 255, 220));
        r.setOutlineThickness(2);
        r.setOutlineColor(sf::Color(190, 140, 255, 180));
        r.setPosition(toSFML({ b.cx, b.cy }, window));
        barGfx.push_back(r);
    }
}
