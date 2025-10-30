// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#include "Game.h"
#include "Ragdoll.h"

using namespace sf;


Ragdoll::Ragdoll(b2World* world, sf::RenderWindow* window, float x, float y)
    : wnd(window)
{
    // Crea las partes físicas y visuales para el ragdoll.
    auto crearParte = [&](float px, float py, float w, float h, Color color) -> b2Body*
        {
            b2BodyDef bodyDef;
            bodyDef.type = b2_dynamicBody;
            bodyDef.position.Set(px, py);
            b2Body* body = world->CreateBody(&bodyDef);

            b2PolygonShape shape;
            shape.SetAsBox(w / 2.0f, h / 2.0f);

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &shape;
            fixtureDef.density = 1.0f;
            fixtureDef.friction = 0.4f;
            fixtureDef.restitution = 0.1f;
            body->CreateFixture(&fixtureDef);

            // Categoriza sus colisiones con el mundo y los objetos.
            for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
                b2Filter filter = f->GetFilterData();
                filter.categoryBits = CATEGORY_RAGDOLL;
                filter.maskBits = CATEGORY_RAGDOLL | CATEGORY_WALL | CATEGORY_OBSTACLE | CATEGORY_PULLEY | CATEGORY_META;
                f->SetFilterData(filter);
            }

            body->SetAwake(true);

            // Guardar la info visual.
            ParteVisual pv;
            pv.body = body;
            pv.color = color;
            pv.width = w;
            pv.height = h;
            partes.push_back(pv);

            return body;
        };

    // Crea cada parte de su cuerpo con su skin de color.
    b2Body* torso = crearParte(x, y, 8.0f, 12.0f, Color::Magenta);
    b2Body* cabeza = crearParte(x, y - 10.0f, 6.0f, 6.0f, Color::Yellow);
    b2Body* brazoIzq = crearParte(x - 6.0f, y + 3.0f, 4.0f, 6.0f, Color::Yellow);
    b2Body* brazoDer = crearParte(x + 6.0f, y + 3.0f, 4.0f, 6.0f, Color::Yellow);
    b2Body* piernaIzq = crearParte(x - 3.0f, y + 30.0f, 4.0f, 10.0f, Color::Blue);
    b2Body* piernaDer = crearParte(x + 3.0f, y + 30.0f, 4.0f, 10.0f, Color::Blue);

    // Las une a través de Revolute Joints.
    CreateSpringJoint(world, cabeza, torso, { 0, 1 }, { 0, -8 }, -0.2f * b2_pi, 0.2f * b2_pi);
    CreateSpringJoint(world, brazoIzq, torso, { -5, 0 }, { 0, -4 }, -0.6f * b2_pi, 0.6f * b2_pi);
    CreateSpringJoint(world, brazoDer, torso, { 5, 0 }, { 0, -4 }, -0.6f * b2_pi, 0.6f * b2_pi);
    CreateSpringJoint(world, piernaIzq, torso, { -4, 0 }, { 0, 4 }, -0.4f * b2_pi, 0.4f * b2_pi);
    CreateSpringJoint(world, piernaDer, torso, { 4, 0 }, { 0, 4 }, -0.4f * b2_pi, 0.4f * b2_pi);
}


void Ragdoll::CreateSpringJoint(b2World* world, b2Body* bodyA, b2Body* bodyB,
    b2Vec2 localAnchorA, b2Vec2 localAnchorB,
    float lowerAngle, float upperAngle)
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


void Ragdoll::Despertar()
{
    for (auto& p : partes)
        if (p.body) p.body->SetAwake(true);
}


void Ragdoll::Dibujar()
{
    for (auto& p : partes)
    {
        if (!p.body) continue;
        b2Vec2 pos = p.body->GetPosition();
        float ang = p.body->GetAngle() * 180.f / b2_pi;

        RectangleShape rect;
        rect.setSize(Vector2f(p.width, p.height));
        rect.setOrigin(p.width / 2.f, p.height / 2.f);
        rect.setPosition(pos.x, pos.y);
        rect.setRotation(ang);
        rect.setFillColor(p.color);

        wnd->draw(rect);
    }
}


void Ragdoll::AplicarFuerza(const b2Vec2& fuerza, const b2Vec2& origen)
{
    for (auto& p : partes)
        if (p.body) p.body->ApplyForce(fuerza, origen, true);
}


void Ragdoll::AplicarImpulso(b2Vec2& impulso, b2Vec2& origen)
{
    for (auto& p : partes)
        if (p.body) p.body->ApplyLinearImpulse(impulso, origen, true);
}


void Ragdoll::AplicarFuerzaDerecha(float fuerza)
{
    b2Vec2 f(fuerza, 0.0f);
    for (auto& p : partes)
        if (p.body) p.body->ApplyForceToCenter(f, true);
}


// Aplica un impulso para lograr una velocidad inicial (desiredSpeed) en la dirección dada.
// Se calcula: impulse = mass * desiredSpeed  => así todas obtienen velocidad similar independientemente de su masa
void Ragdoll::AplicarImpulsoATodo(const b2Vec2& direction, float desiredSpeed)
{
    b2Vec2 dir = direction;
    float len = dir.Length();
    if (len <= 0.0001f) return;
    dir *= 1.0f / len;

    for (auto& p : partes)
    {
        if (!p.body) continue;
        float mass = p.body->GetMass();
        b2Vec2 impulse = mass * desiredSpeed * dir;
        p.body->ApplyLinearImpulseToCenter(impulse, true);
        p.body->SetAwake(true);
    }
}
