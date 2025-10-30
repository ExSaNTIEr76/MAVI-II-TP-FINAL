// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#include "Pulley.h"


Pulley::Pulley(b2World* w, sf::RenderWindow* wnd, sf::Texture& texture,
    b2Vec2 leftPos, b2Vec2 rightPos, float width, float height)
{
    world = w;
    window = wnd;

    // Crea las plataformas dinámicas.
    leftPlatform = Box2DHelper::CreateRectangularDynamicBody(world, width, height, 1.0f, 0.5f, 0.2f);
    rightPlatform = Box2DHelper::CreateRectangularDynamicBody(world, width, height, 1.0f, 0.5f, 0.2f);

    leftPlatform->SetTransform(leftPos, 0);
    rightPlatform->SetTransform(rightPos, 0);

    leftPlatform->SetLinearDamping(2.0f);
    leftPlatform->SetAngularDamping(8.0f);
    rightPlatform->SetLinearDamping(2.0f);
    rightPlatform->SetAngularDamping(8.0f);

    leftPlatform->SetFixedRotation(true);
    rightPlatform->SetFixedRotation(true);


    // --- FILTROS DE COLISIÓN ---
    auto SetCategory = [](b2Body* body, uint16 category, uint16 mask)
        {
            for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
                b2Filter filter = fixture->GetFilterData();
                filter.categoryBits = category;
                filter.maskBits = mask;
                fixture->SetFilterData(filter);
            }
        };

    SetCategory(leftPlatform, CATEGORY_PULLEY, CATEGORY_WALL | CATEGORY_OBSTACLE | CATEGORY_RAGDOLL);
    SetCategory(rightPlatform, CATEGORY_PULLEY, CATEGORY_WALL | CATEGORY_OBSTACLE | CATEGORY_RAGDOLL);

    // Crea los joint de polea.
    b2Vec2 worldAnchorLeft(leftPos.x, leftPos.y - 30.0f);
    b2Vec2 worldAnchorRight(rightPos.x, rightPos.y - 30.0f);

    b2PulleyJointDef pulleyDef;
    pulleyDef.Initialize(leftPlatform, rightPlatform,
        worldAnchorLeft, worldAnchorRight,
        leftPlatform->GetWorldCenter(), rightPlatform->GetWorldCenter(),
        1.1f);

    pulleyDef.collideConnected = false;
    pulleyJoint = (b2PulleyJoint*)world->CreateJoint(&pulleyDef);

    // Crea los sprites.
    leftSprite.setTexture(texture);
    rightSprite.setTexture(texture);

    leftSprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
    rightSprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);

    leftSprite.setScale(width / texture.getSize().x, height / texture.getSize().y);
    rightSprite.setScale(width / texture.getSize().x, height / texture.getSize().y);
}


void Pulley::Draw()
{
    b2Vec2 posL = leftPlatform->GetPosition();
    b2Vec2 posR = rightPlatform->GetPosition();

    float angleL = leftPlatform->GetAngle();
    float angleR = rightPlatform->GetAngle();

    leftSprite.setRotation(angleL * 180.0f / b2_pi);
    rightSprite.setRotation(angleR * 180.0f / b2_pi);

    leftSprite.setPosition(posL.x, posL.y);
    rightSprite.setPosition(posR.x, posR.y);

    window->draw(leftSprite);
    window->draw(rightSprite);
}


void Pulley::Destroy()
{
    if (pulleyJoint) {
        world->DestroyJoint(pulleyJoint);
        pulleyJoint = nullptr;
    }

    if (leftPlatform) {
        world->DestroyBody(leftPlatform);
        leftPlatform = nullptr;
    }

    if (rightPlatform) {
        world->DestroyBody(rightPlatform);
        rightPlatform = nullptr;
    }
}
