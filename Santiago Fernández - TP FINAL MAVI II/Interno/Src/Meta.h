// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#pragma once
#include "stdafx.h"
#include "SFMLRenderer.h"
#include "Box2DHelper.h"


class Meta
{
public:

    Meta(b2World* world, sf::RenderWindow* window, float x, float y, float radius);
    ~Meta() = default;

    void Draw();
    void CheckCollision();
    void youWin();

    bool HasWon() const { return hasWon; }
    void Reset();

    b2Body* GetBody() { return body; }

    sf::Sprite& GetWinSprite() { return winSprite; }

private:

    sf::RenderWindow* wnd;
    sf::CircleShape shape;
    b2Body* body;
    sf::Texture winTexture;
    sf::Sprite winSprite;
    bool hasWon;

};
