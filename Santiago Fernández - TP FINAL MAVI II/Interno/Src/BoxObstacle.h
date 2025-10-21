// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#pragma once
#include "stdafx.h"
#include "SFMLRenderer.h"
#include "Box2DHelper.h"

class BoxObstacle {
public:

    BoxObstacle(b2World* world, sf::RenderWindow* window, float x, float y, float width, float height, bool dynamic = false);
    void Draw();
    void Update();

private:

    b2Body* body;
    sf::RenderWindow* wnd;
    sf::Texture texture;
    sf::Sprite sprite;
    float width, height;
};
