// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#include "Pendulum.h"


Pendulum::Pendulum(b2World* world, sf::RenderWindow* wnd, float anchorX, float anchorY, float ballX, float ballY, float radius)
{
    this->wnd = wnd;
    this->world = world;
    this->radius = radius;

    // Crea los cuerpos físicos.
    anchorBody = Box2DHelper::CreateRectangularStaticBody(world, 1, 1);
    anchorBody->SetTransform(b2Vec2(anchorX, anchorY), 0.0f);

    ballBody = Box2DHelper::CreateCircularDynamicBody(world, radius, 1.0f, 0.5f, 0.1f);
    ballBody->SetTransform(b2Vec2(ballX, ballY), 0.0f);

    // Crea el joint tipo resorte/distancia.
    Box2DHelper::CreateDistanceJoint(world,
        ballBody, ballBody->GetWorldCenter(),
        anchorBody, anchorBody->GetWorldCenter(),
        0.0f, 0.1f, 20.0f);

    // Cargar sprite:
    if (!texture.loadFromFile("pendulum.png"))
        std::cout << "No se pudo cargar pendulum.png\n";

    sprite.setTexture(texture);
    sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
    sprite.setScale((radius * 2.0f) / texture.getSize().x, (radius * 2.0f) / texture.getSize().y);

    // Categoriza sus colisiones con el mundo y los objetos.
    b2Fixture* fixture = ballBody->GetFixtureList();
    while (fixture) {
        b2Filter filter = fixture->GetFilterData();
        filter.categoryBits = CATEGORY_OBSTACLE;
        filter.maskBits = 0xFFFF;
        fixture->SetFilterData(filter);
        fixture = fixture->GetNext();
    }

}

void Pendulum::Update()
{
    b2Vec2 pos = ballBody->GetPosition();
    float angle = ballBody->GetAngle() * 180.0f / b2_pi;

    sprite.setPosition(pos.x, pos.y);
    sprite.setRotation(angle);
}

void Pendulum::Draw()
{
    Update();
    wnd->draw(sprite);
}

void Pendulum::Destroy()
{
    if (world && ballBody)
        world->DestroyBody(ballBody);
    if (world && anchorBody)
        world->DestroyBody(anchorBody);
}