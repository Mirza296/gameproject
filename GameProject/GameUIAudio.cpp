#include "GameProject.hpp"

// Text and font
void setupText(sf::Font& font,
    sf::Text& scoreText,
    sf::Text& title,
    sf::Text& controls,
    sf::Text& msgText,
    const sf::RenderWindow& window)
{
    font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");

    scoreText.setFont(font);
    scoreText.setString("Score: 0");
    scoreText.setCharacterSize(32);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineColor(sf::Color::Black);
    scoreText.setOutlineThickness(2);
    scoreText.setPosition(30.f, 20.f);

    title.setFont(font);
    title.setString("BATTLE BOX SHOOTER");
    title.setCharacterSize(110);
    title.setFillColor(sf::Color(0, 255, 180));
    title.setOutlineColor(sf::Color::Black);
    title.setOutlineThickness(6);
    title.setPosition(
        window.getSize().x / 2.f - 580.f,
        window.getSize().y / 3.f - 150.f
    );

    controls.setFont(font);
    controls.setString(
        "LEFT Arrow - Move Left    SPACE - Jump\n"
        "RIGHT Arrow - Move Right   S - Shoot\n"
        "R - Restart                ENTER - Start"
    );
    controls.setCharacterSize(45);
    controls.setFillColor(sf::Color::White);
    controls.setOutlineColor(sf::Color::Black);
    controls.setOutlineThickness(3);
    controls.setPosition(
        window.getSize().x / 2.f - 550.f,
        window.getSize().y / 2.f
    );

    msgText.setFont(font);
    msgText.setString("");
    msgText.setCharacterSize(40);
    msgText.setOutlineColor(sf::Color::Black);
    msgText.setOutlineThickness(3);
}

// Audio – identical to your last version (with jump + looping game-start)
void setupAudio(sf::SoundBuffer& fireBuffer,
    sf::SoundBuffer& enemyDeadBuffer,
    sf::SoundBuffer& gameStartBuffer,
    sf::SoundBuffer& lossBuffer,
    sf::SoundBuffer& jumpBuffer,
    sf::Sound& fireSound,
    sf::Sound& enemyDeadSound,
    sf::Sound& gameStartSound,
    sf::Sound& lossSound,
    sf::Sound& jumpSound)
{
    if (!fireBuffer.loadFromFile("fire.wav"))
        std::cout << "Failed to load fire.wav\n";
    if (!enemyDeadBuffer.loadFromFile("enemy-dead.wav"))
        std::cout << "Failed to load enemy-dead.wav\n";
    if (!gameStartBuffer.loadFromFile("game-start.wav"))
        std::cout << "Failed to load game-start.wav\n";
    if (!lossBuffer.loadFromFile("loss.wav"))
        std::cout << "Failed to load loss.wav\n";
    if (!jumpBuffer.loadFromFile("jump.wav"))
        std::cout << "Failed to load jump.wav\n";

    fireSound.setBuffer(fireBuffer);
    enemyDeadSound.setBuffer(enemyDeadBuffer);
    gameStartSound.setBuffer(gameStartBuffer);
    lossSound.setBuffer(lossBuffer);
    jumpSound.setBuffer(jumpBuffer);

    // Game start should always loop
    gameStartSound.setLoop(true);
}
