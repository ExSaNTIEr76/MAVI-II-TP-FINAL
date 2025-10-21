// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#include "Background.h"


Background::Background(sf::RenderWindow* window, int level)
{
    wnd = window;
    currentLevel = level;
    LoadTextureForLevel(level);
}


void Background::SetLevel(int level)
{
    if (level != currentLevel)
    {
        currentLevel = level;
        LoadTextureForLevel(level);
    }
}


void Background::LoadTextureForLevel(int level)
{
    std::string filename = "background_" + std::to_string(level) + ".png";
    if (!texture.loadFromFile(filename)) {
        std::cerr << "No se pudo cargar " << filename << ", usando background_1.png\n";
        texture.loadFromFile("background_1.png");
    }

    sprite.setTexture(texture);
    sprite.setPosition(0, 0);

    sf::Vector2u texSize = texture.getSize();
    sprite.setScale(
        100.0f / texSize.x,
        100.0f / texSize.y
    );
}


void Background::Draw()
{
    wnd->draw(sprite);
}
