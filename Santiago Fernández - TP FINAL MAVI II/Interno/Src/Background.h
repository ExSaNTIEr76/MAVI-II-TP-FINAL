// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>


class Background {
public:

    Background(sf::RenderWindow* window, int level = 1);
    void SetLevel(int level);
    void Draw();

    const sf::Texture& GetTexture() const { return texture; }
    sf::Sprite& GetSprite() { return sprite; }

private:

    void LoadTextureForLevel(int level);

    sf::RenderWindow* wnd;
    sf::Texture texture;
    sf::Sprite sprite;
    int currentLevel;
};

