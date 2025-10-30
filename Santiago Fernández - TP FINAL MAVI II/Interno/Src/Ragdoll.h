// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#pragma once
#include "stdafx.h"
#include "SFMLRenderer.h"
#include "Box2DHelper.h"


class Ragdoll
{
public:

    Ragdoll(b2World* world, sf::RenderWindow* window, float x, float y);
    ~Ragdoll() = default;

    // Dibujo / actualización
    void Dibujar();

    // Fuerzas / impulsos
    void AplicarFuerza(const b2Vec2& fuerza, const b2Vec2& origen);
    void AplicarImpulso(b2Vec2& impulso, b2Vec2& origen = b2Vec2(0.0f, 0.0f));
    void AplicarImpulsoATodo(const b2Vec2& direction, float desiredSpeed);

    void AplicarFuerzaDerecha(float fuerza);
    void Despertar();

private:

    // Junta revolute/simple
    void CreateSpringJoint(b2World* world, b2Body* bodyA, b2Body* bodyB, b2Vec2 localAnchorA, b2Vec2 localAnchorB, float lowerAngle, float upperAngle);

    sf::RenderWindow* wnd;
    sf::Texture texture;
    sf::Sprite sprite;

    struct ParteVisual {
        b2Body* body;
        sf::Color color;
        float width;
        float height;
    };

    // Todas las partes que forman el ragdoll
    std::vector<ParteVisual> partes;

    // Referencia al último cuerpo creado (compatibilidad)
    b2Body* _body;
};