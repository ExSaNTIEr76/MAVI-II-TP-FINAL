// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#include "Game.h"
#include "Ragdoll.h"

using namespace sf;


Ragdoll::Ragdoll(b2World* world, sf::RenderWindow* window, float x, float y)
    : wnd(window), _body(nullptr)
{
    // torso
    b2Body* torso = CreateBodyPart(world, x, y, 8.0f, 12.0f);

    // cabeza
    b2Body* cabeza = CreateBodyPart(world, x, y - 10.0f, 6.0f, 6.0f);

    // brazos
    b2Body* brazoIzq = CreateBodyPart(world, x - 6.0f, y + 3.0f, 4.0f, 6.0f);
    b2Body* brazoDer = CreateBodyPart(world, x + 6.0f, y + 3.0f, 4.0f, 6.0f);

    // piernas
    b2Body* piernaIzq = CreateBodyPart(world, x - 3.0f, y + 30.0f, 4.0f, 10.0f);
    b2Body* piernaDer = CreateBodyPart(world, x + 3.0f, y + 30.0f, 4.0f, 10.0f);

    // unir con joints (ejemplo)
    CreateSpringJoint(world, cabeza, torso, b2Vec2(0.0f, 1.0f), b2Vec2(0.0f, -8.0f), -0.2f * b2_pi, 0.2f * b2_pi);
    CreateSpringJoint(world, brazoIzq, torso, b2Vec2(-5.0f, 0.0f), b2Vec2(0.0f, -4.0f), -0.6f * b2_pi, 0.6f * b2_pi);
    CreateSpringJoint(world, brazoDer, torso, b2Vec2(5.0f, 0.0f), b2Vec2(0.0f, -4.0f), -0.6f * b2_pi, 0.6f * b2_pi);
    CreateSpringJoint(world, piernaIzq, torso, b2Vec2(-4.0f, 0.0f), b2Vec2(0.0f, 4.0f), -0.4f * b2_pi, 0.4f * b2_pi);
    CreateSpringJoint(world, piernaDer, torso, b2Vec2(4.0f, 0.0f), b2Vec2(0.0f, 4.0f), -0.4f * b2_pi, 0.4f * b2_pi);

    // asegurar que estén despiertas
    for (b2Body* b : partes) if (b) b->SetAwake(true);
}


b2Body* Ragdoll::CreateBodyPart(b2World* phyworld, float x, float y, float width, float height)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody; // dinámico
    bodyDef.position.Set(x, y);
    b2Body* body = phyworld->CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(width / 2.0f, height / 2.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.4f;
    fixtureDef.restitution = 0.1f;
    body->CreateFixture(&fixtureDef);


    // Categoriza sus colisiones con el mundo y los objetos.
    b2Fixture* fixture = body->GetFixtureList();
    while (fixture) {
        b2Filter filter = fixture->GetFilterData();
        filter.categoryBits = CATEGORY_RAGDOLL;
        filter.maskBits = CATEGORY_RAGDOLL | CATEGORY_WALL | CATEGORY_OBSTACLE | CATEGORY_META;
        fixture->SetFilterData(filter);
        fixture = fixture->GetNext();
    }

    body->SetAwake(true);
    body->ResetMassData();

    partes.push_back(body);
    _body = body; // mantener compatibilidad

    return body;

}


void Ragdoll::CreateSpringJoint(b2World* world, b2Body* bodyA, b2Body* bodyB, b2Vec2 localAnchorA, b2Vec2 localAnchorB, float lowerAngle, float upperAngle)
{
    b2RevoluteJointDef jointDef;
    jointDef.bodyA = bodyA;
    jointDef.bodyB = bodyB;
    jointDef.localAnchorA = localAnchorA;
    jointDef.localAnchorB = localAnchorB;
    jointDef.lowerAngle = lowerAngle;
    jointDef.upperAngle = upperAngle;
    jointDef.enableLimit = true;
    world->CreateJoint(&jointDef);
}


void Ragdoll::ActualizarPosiciones()
{
    if (_body) {
        b2Vec2 pos = _body->GetPosition();
        sprite.setPosition(pos.x, pos.y);
    }
}


void Ragdoll::Despertar()
{
    for (b2Body* b : partes) if (b) b->SetAwake(true);
}


void Ragdoll::Dibujar()
{
    wnd->draw(sprite);
}


void Ragdoll::AplicarFuerza(const b2Vec2& fuerza, const b2Vec2& origen)
{
    for (b2Body* b : partes) if (b) b->ApplyForce(fuerza, origen, true);
}


void Ragdoll::AplicarImpulso(b2Vec2& impulso, b2Vec2& origen)
{
    for (b2Body* b : partes) if (b) b->ApplyLinearImpulse(impulso, origen, true);
}


void Ragdoll::AplicarFuerzaDerecha(float fuerza)
{
    b2Vec2 fuerzaDerecha(fuerza, 0.0f);
    for (b2Body* b : partes) if (b) b->ApplyForceToCenter(fuerzaDerecha, true);
}


// Aplica un impulso para lograr una velocidad inicial (desiredSpeed) en la dirección dada.
// Se calcula: impulse = mass * desiredSpeed  => así todas obtienen velocidad similar independientemente de su masa
void Ragdoll::AplicarImpulsoATodo(const b2Vec2& direction, float desiredSpeed)
{
    b2Vec2 dir = direction;
    float len = dir.Length();
    if (len <= 0.0001f) return;
    dir *= 1.0f / len; // normalizar

    for (b2Body* b : partes)
    {
        if (!b) continue;
        float mass = b->GetMass();
        b2Vec2 impulse = mass * desiredSpeed * dir;
        b->ApplyLinearImpulseToCenter(impulse, true);
        b->SetAwake(true);
    }
}