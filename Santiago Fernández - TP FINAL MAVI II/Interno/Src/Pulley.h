// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#pragma once
#include "stdafx.h"
#include "Box2DHelper.h"


class Pulley
{
private:

    b2World* world;
    sf::RenderWindow* window;

    b2Body* leftPlatform;
    b2Body* rightPlatform;
    b2PulleyJoint* pulleyJoint;

    sf::Sprite leftSprite;
    sf::Sprite rightSprite;

public:

    Pulley(b2World* w, sf::RenderWindow* wnd, sf::Texture& texture,
        b2Vec2 leftPos, b2Vec2 rightPos, float width, float height);

    void Draw();
    void Destroy();

    b2Body* GetLeft() { return leftPlatform; }
    b2Body* GetRight() { return rightPlatform; }
};