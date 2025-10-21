// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#pragma once
#include "stdafx.h"
#include "SFMLRenderer.h"
#include "Box2DHelper.h"


class Pendulum
{
private:

    sf::RenderWindow* wnd;
    b2World* world;
    b2Body* anchorBody;
    b2Body* ballBody;
    sf::Sprite sprite;
    sf::Texture texture;
    float radius;

public:

    Pendulum(b2World* world, sf::RenderWindow* wnd, float anchorX, float anchorY, float ballX, float ballY, float radius);
    void Update();
    void Draw();
    void Destroy();
};
