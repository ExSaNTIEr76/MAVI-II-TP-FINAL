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
    void ActualizarPosiciones();
    void Dibujar();

    // Fuerzas / impulsos
    void AplicarFuerza(const b2Vec2& fuerza, const b2Vec2& origen);
    void AplicarImpulso(b2Vec2& impulso, b2Vec2& origen = b2Vec2(0.0f, 0.0f));
    void AplicarImpulsoATodo(const b2Vec2& direction, float desiredSpeed);

    void AplicarFuerzaDerecha(float fuerza);
    void Despertar();

private:

    // crea una parte y la guarda en partes
    b2Body* CreateBodyPart(b2World* world, float x, float y, float width, float height);

    // junta revolute/simple
    void CreateSpringJoint(b2World* world, b2Body* bodyA, b2Body* bodyB, b2Vec2 localAnchorA, b2Vec2 localAnchorB, float lowerAngle, float upperAngle);

    sf::RenderWindow* wnd;
    sf::Texture texture;
    sf::Sprite sprite;

    std::vector<b2Body*> partes; // todas las partes que forman el ragdoll
    b2Body* _body; // referencia al último cuerpo creado (compatibilidad)
};
