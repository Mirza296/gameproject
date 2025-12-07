#include "GameProject.hpp"
#include <fstream>
#include <SFML/System.hpp>   // for sf::sleep

// Runs the game
int runGame()
{
    sf::RenderWindow window(
        sf::VideoMode::getDesktopMode(),
        "Battle Box Shooter",
        sf::Style::Fullscreen
    );
    window.setFramerateLimit(60);

    // SAVE / HIGH SCORE STATE 
    int  highScore = 0;
    // score when resuming 
    int  resumeScore = 0;                 
    // how many enemies should exist
    int  resumeEnemies = MAX_ENEMIES;       
    bool hasSave = false;

    auto loadSave = [&]()
        {
            std::ifstream in("savegame.txt");
            if (!in)
                return;

            // Try to read 3 values: resumeScore, resumeEnemies, highScore
            in >> resumeScore >> resumeEnemies >> highScore;
            if (!in)
            {
                // Fallback for old 2-value save files: resumeScore + highScore
                in.clear();
                in.seekg(0);
                in >> resumeScore >> highScore;
                resumeEnemies = MAX_ENEMIES;
            }
            hasSave = true;
        };

    auto saveAll = [&]()
        {
            std::ofstream out("savegame.txt");
            if (!out)
            {
                std::cout << "Failed to write savegame.txt\n";
                return;
            }
            // Save resume state + high score
            out << resumeScore << " " << resumeEnemies << " " << highScore;
            hasSave = true;
        };

    // Try to load an existing save at startup
    loadSave();

    // Background
    sf::Texture gradient;
    sf::RectangleShape bg;
    setupWindowAndBackground(window, bg, gradient);

    // World + arena + platforms
    b2WorldId world;
    sf::RectangleShape border;
    std::vector<Bar> bars;
    std::vector<sf::RectangleShape> barGfx;
    setupWorldAndArena(world, border, bars, barGfx, window);

    // Player
    Player player;
    setupPlayer(player, world);

    // ENEMIES  
    std::vector<Enemy> enemies;
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int>   col(100, 255);
    std::uniform_int_distribution<int>   scoreDist(5, 20);
    std::uniform_real_distribution<float> xSpawn(WORLD_LEFT + 1.f, WORLD_RIGHT - 1.f);
    std::uniform_real_distribution<float> pathTimeDist(1.f, 3.f);
    std::uniform_real_distribution<float> jumpCDDist(0.8f, 1.8f);

    for (int i = 0; i < MAX_ENEMIES; ++i)
        spawnEnemy(enemies, world, rng, col, scoreDist, xSpawn, pathTimeDist, jumpCDDist);

    // BULLETS
    std::deque<Bullet> bullets;

    // TEXT
    sf::Font font;
    sf::Text scoreText, title, controls, msgText;
    setupText(font, scoreText, title, controls, msgText, window);

    // Controls text with save / load / high-score info 
    controls.setString(
        "LEFT Arrow - Move Left    SPACE - Jump\n"
        "RIGHT Arrow - Move Right   S - Shoot\n"
        "R - Restart                ENTER - Start\n"
        "CTRL+S - Save   CTRL+D - High Score\n"
        "O - Open Save (if available)"
    );

    // High score text (shown with CTRL+D)
    sf::Text highScoreText;
    highScoreText.setFont(font);
    highScoreText.setCharacterSize(32);
    highScoreText.setFillColor(sf::Color::Yellow);
    highScoreText.setOutlineColor(sf::Color::Black);
    highScoreText.setOutlineThickness(2);
    highScoreText.setPosition(30.f, 70.f);

    // AUDIO
    sf::SoundBuffer fireBuffer, enemyDeadBuffer, gameStartBuffer, lossBuffer, jumpBuffer;
    sf::Sound fireSound, enemyDeadSound, gameStartSound, lossSound, jumpSound;
    setupAudio(fireBuffer, enemyDeadBuffer, gameStartBuffer, lossBuffer, jumpBuffer,
        fireSound, enemyDeadSound, gameStartSound, lossSound, jumpSound);

    bool prevSpace = false;
    bool gameOver = false;
    bool playerWon = false;
    bool started = false;
    bool resultProcessed = false; // for automatic high-score update at end of round
    bool showHighScore = false; // toggled by CTRL+D
    bool prevSavePressed = false;
    bool prevDisplayPressed = false;

    float hueShift = 0.f;
    float glowTime = 0.f;

    // MAIN LOOP  
    while (window.isOpen())
    {
        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
                window.close();
        }

        // ESC to quit during game
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            window.close();
            continue;
        }

        // --- HOTKEYS: SAVE (CTRL+S) & SHOW HIGH SCORE (CTRL+D) -----------
        bool saveDown = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        if (saveDown && !prevSavePressed && started && !gameOver && !playerWon)
        {
            // Update resume state from current gameplay
            int aliveEnemies = 0;
            for (auto& e : enemies)
                if (e.alive) ++aliveEnemies;

            resumeScore = player.score;
            resumeEnemies = aliveEnemies;

            // Update high score if needed
            if (player.score > highScore)
                highScore = player.score;

            saveAll();

            // --- SHOW "SAVED" MESSAGE AND EXIT 
            msgText.setString("Game saved. You can load it with O next time.");
            msgText.setFillColor(sf::Color::White);
            msgText.setPosition(
                window.getSize().x / 2.f - 450.f,
                window.getSize().y / 2.f
            );

            window.clear();
            window.draw(bg);
            window.draw(msgText);
            window.display();

            // give the player time to read 
            sf::sleep(sf::seconds(1.5f));   

            // exit game after saving
            window.close();                 
            break;
        }
        prevSavePressed = saveDown;

        bool displayDown = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::D);
        if (displayDown && !prevDisplayPressed)
        {
            showHighScore = !showHighScore;
        }
        prevDisplayPressed = displayDown; 

        hueShift += DT * 10.f;
        border.setOutlineColor(
            sf::Color(
                0,
                255,
                220,
                160 + static_cast<sf::Uint8>(std::sin(hueShift) * 80)
            )
        );

        // TITLE SCREEN
        if (!started)
        {
            glowTime += DT;
            sf::Color glow(
                0,
                255,
                180,
                180 + static_cast<sf::Uint8>(std::sin(glowTime * 2.f) * 60)
            );
            title.setFillColor(glow);

            highScoreText.setString("High Score: " + std::to_string(highScore));

            window.clear();
            window.draw(bg);
            window.draw(title);
            window.draw(controls);

            if (showHighScore)
                window.draw(highScoreText);

            window.display();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
            {
                // starts looping background music 
                gameStartSound.play();
                started = true;
            }
            else if (hasSave && sf::Keyboard::isKeyPressed(sf::Keyboard::O))
            {
                // --- Restore saved progress --------------------------------
                // Abdullah: I added the code here
                // Remove existing enemies from the world
                for (auto& e : enemies)
                {
                    if (e.alive)
                        b2DestroyBody(e.id);
                }
                enemies.clear();

                // Spawn only the saved number of enemies
                int toSpawn = std::max(0, std::min(resumeEnemies, MAX_ENEMIES));
                for (int i = 0; i < toSpawn; ++i)
                    spawnEnemy(enemies, world, rng, col, scoreDist,
                        xSpawn, pathTimeDist, jumpCDDist);

                // Restore score
                player.score = resumeScore;
                // -----------------------------------------------------------

                gameStartSound.play();
                started = true;
            }

            continue;
        }

        if (!gameOver && !playerWon)
        {
            // PLAYER MOVEMENT
            b2Vec2 vel = b2Body_GetLinearVelocity(player.id);

            bool movingLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
            bool movingRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);

            if (movingLeft)
            {
                vel.x = -8.f;
                player.dir = -1.f;
            }
            else if (movingRight)
            {
                vel.x = 8.f;
                player.dir = 1.f;
            }
            else
            {
                // slow down when no key pressed
                vel.x *= 0.9f;
            }

            b2Vec2 pPos = b2Body_GetPosition(player.id);
            bool grounded = isGrounded(pPos, player.radius, bars);
            if (grounded)
                player.jumps = 2;

            // jump (double jump via re-press)
            bool space = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
            if (space && !prevSpace && player.jumps > 0)
            {
                vel.y = 12.f;
                player.jumps--;
                // play jump sound 
                jumpSound.play();
            }
            prevSpace = space;

            b2Body_SetLinearVelocity(player.id, vel);

            // SHOOTING
            player.shootCD -= DT;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && player.shootCD <= 0.f)
            {
                // Call function from GameEntities.cpp
                shoot(bullets, player, player.dir, world);
                player.shootCD = 0.25f;
                fireSound.play();
            }

            // ENEMY AI: all enemies track player but each chooses left/right path
            for (auto& en : enemies)
            {
                if (!en.alive) continue;

                b2Vec2 ePos = b2Body_GetPosition(en.id);
                b2Vec2 eVel = b2Body_GetLinearVelocity(en.id);

                // update timers
                en.pathTimer -= DT;
                en.jumpCooldown -= DT;
                if (en.jumpCooldown < 0.f) en.jumpCooldown = 0.f;

                // occasionally change which side they prefer (random path)
                if (en.pathTimer <= 0.f)
                {
                    en.sideBias = (rng() % 2 == 0) ? -1.f : 1.f;
                    en.pathTimer = pathTimeDist(rng);
                }

                // horizontal target: a little left or right of the player
                float targetX = pPos.x + en.sideBias * 2.5f;
                float dx = targetX - ePos.x;

                if (dx > 0.15f)
                    eVel.x = en.speed;
                else if (dx < -0.15f)
                    eVel.x = -en.speed;
                else
                    // small damping when roughly at target side 
                    eVel.x *= 0.8f;

                // jumping: if player is above and enemy is near horizontally
                float horizToPlayer = std::fabs(pPos.x - ePos.x);
                bool enemyGrounded = isGrounded(ePos, en.radius, bars);

                if (enemyGrounded &&
                    en.jumpCooldown == 0.f &&
                    pPos.y > ePos.y + 1.f &&
                    horizToPlayer < 4.f)
                {
                    eVel.y = 10.f;
                    // jump up towards player stage 
                    en.jumpCooldown = jumpCDDist(rng);
                }

                b2Body_SetLinearVelocity(en.id, eVel);
            }

            // PHYSICS STEP
            b2World_Step(world, DT, SUB_STEPS);

            // MUZZLE TIMER
            if (player.muzzleTimer > 0.f)
                player.muzzleTimer -= DT;

            // BULLETS
            for (auto it = bullets.begin(); it != bullets.end();)
            {
                it->life -= DT;
                if (it->life <= 0.f)
                {
                    b2DestroyBody(it->id);
                    it = bullets.erase(it);
                    continue;
                }

                sf::Vector2f bp = toSFML(b2Body_GetPosition(it->id), window);

                for (auto& en : enemies)
                {
                    if (!en.alive) continue;

                    sf::Vector2f ep = toSFML(b2Body_GetPosition(en.id), window);
                    if (vlen(ep - bp) < (en.radius * PX + 8.f))
                    {
                        en.alive = false;
                        player.score += en.scoreValue;
                        b2DestroyBody(en.id);
                        enemyDeadSound.play();
                    }
                }

                ++it;
            }

            // PLAYER HIT sensitive DETECTION 
            sf::Vector2f pp = toSFML(b2Body_GetPosition(player.id), window);
            for (auto& en : enemies)
            {
                if (!en.alive) continue;

                sf::Vector2f ep = toSFML(b2Body_GetPosition(en.id), window);
                // slightly larger than sum of radii so "little collision" also kills
                if (vlen(ep - pp) < (player.radius + en.radius) * PX + 6.f)
                {
                    lossSound.play();
                    gameOver = true;
                    break;
                }
            }

            // WIN CONDITION
            if (std::all_of(
                enemies.begin(), enemies.end(),
                [](const Enemy& e) { return !e.alive; }))
            {
                playerWon = true;
            }
        }

        // Automatic high-score update at end of round (win OR loss)
        if ((gameOver || playerWon) && !resultProcessed)
        {
            if (player.score > highScore)
                highScore = player.score;

            // we just update the high score and re-save everything
            saveAll();
            resultProcessed = true;
        }

        // RENDER
        window.clear();
        window.draw(bg);

        for (auto& r : barGfx)
            window.draw(r);

        for (auto& e : enemies)
        {
            if (!e.alive) continue;
            e.gfx.setPosition(toSFML(b2Body_GetPosition(e.id), window));
            window.draw(e.gfx);
        }

        for (auto& b : bullets)
        {
            b.gfx.setPosition(toSFML(b2Body_GetPosition(b.id), window));
            window.draw(b.gfx);
        }

        // Player + gun + muzzle
        sf::Vector2f playerPix = toSFML(b2Body_GetPosition(player.id), window);
        player.gfx.setPosition(playerPix);

        float gunOffset = player.radius * PX + 10.f;
        sf::Vector2f gunPos = playerPix;
        gunPos.x += player.dir * gunOffset;
        player.gun.setPosition(gunPos);
        player.gun.setRotation(player.dir > 0.f ? 0.f : 180.f);

        window.draw(player.gfx);
        window.draw(player.gun);

        if (player.muzzleTimer > 0.f)
        {
            float muzzleOffset = gunOffset + player.gun.getSize().x * 0.6f;
            sf::Vector2f muzzlePos = playerPix;
            muzzlePos.x += player.dir * muzzleOffset;
            player.muzzle.setPosition(muzzlePos);
            window.draw(player.muzzle);
        }

        window.draw(border);

        scoreText.setString("Score: " + std::to_string(player.score));
        window.draw(scoreText);

        // High-score overlay (CTRL+D)
        if (showHighScore)
        {
            highScoreText.setString("High Score: " + std::to_string(highScore));
            window.draw(highScoreText);
        }

        if (gameOver)
        {
            msgText.setString("GAME OVER — Press R to Restart");
            msgText.setFillColor(sf::Color::Red);
            msgText.setPosition(
                window.getSize().x / 2.f - 350.f,
                window.getSize().y / 2.f
            );
            window.draw(msgText);
        }
        else if (playerWon)
        {
            msgText.setString("YOU WIN! Press R to Play Again");
            msgText.setFillColor(sf::Color::Green);
            msgText.setPosition(
                window.getSize().x / 2.f - 400.f,
                window.getSize().y / 2.f
            );
            window.draw(msgText);
        }

        window.display();

        // RESTART
        if ((gameOver || playerWon) && sf::Keyboard::isKeyPressed(sf::Keyboard::R))
        {
            // Destroy old enemies
            for (auto& e : enemies)
            {
                if (e.alive)
                    b2DestroyBody(e.id);
            }
            enemies.clear();

            // Destroy bullets
            for (auto& b : bullets)
                b2DestroyBody(b.id);
            bullets.clear();

            // Reset player
            player.score = 0;
            player.jumps = 2;
            player.dir = 1.f;
            player.shootCD = 0.f;
            player.muzzleTimer = 0.f;
            gameOver = false;
            playerWon = false;
            resultProcessed = false;

            b2Vec2 resetPos = { 0.f, WORLD_FLOOR + 2.f };
            b2Body_SetTransform(player.id, resetPos, { 1.f, 0.f });
            b2Body_SetLinearVelocity(player.id, { 0.f, 0.f });

            // Respawn enemies  
            for (int i = 0; i < MAX_ENEMIES; ++i)
                spawnEnemy(enemies, world, rng, col, scoreDist,
                    xSpawn, pathTimeDist, jumpCDDist);
        }
    }

    b2DestroyWorld(world);
    return 0;
}

int main()
{
    return runGame();
}