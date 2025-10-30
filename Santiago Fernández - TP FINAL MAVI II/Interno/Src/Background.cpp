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

    sf::Vector2u winSize = wnd->getSize();
    sf::Vector2u texSize = texture.getSize();

    sprite.setTexture(texture);
    sprite.setPosition(0.f, 0.f);
    sprite.setScale(
        static_cast<float>(winSize.x) / texSize.x,
        static_cast<float>(winSize.y) / texSize.y
    );
}


void Background::Draw()
{
    sf::View oldView = wnd->getView();
    wnd->setView(wnd->getDefaultView());
    wnd->draw(sprite);
    wnd->setView(oldView);
}

