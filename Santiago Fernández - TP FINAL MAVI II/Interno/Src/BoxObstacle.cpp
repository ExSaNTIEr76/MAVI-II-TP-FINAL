// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#include "BoxObstacle.h"


BoxObstacle::BoxObstacle(b2World* world, sf::RenderWindow* window, float x, float y, float width, float height, bool dynamic)
{
    wnd = window;
    this->width = width;
    this->height = height;

    // Crea el cuerpo físico.
    if (dynamic)
        body = Box2DHelper::CreateRectangularDynamicBody(world, width, height, 1.0f, 0.5f, 0.2f);
    else
        body = Box2DHelper::CreateRectangularStaticBody(world, width, height);

    body->SetTransform(b2Vec2(x, y), 0.0f);

    // Cargar textura y sprite:
    if (!texture.loadFromFile("box.png")) {
        std::cerr << "No se pudo cargar box.png" << std::endl;
    }

    sprite.setTexture(texture);

    // Ajusta la escala para que el sprite mida "width x height" en el mundo.
    sf::Vector2u texSize = texture.getSize();
    sprite.setScale(width / static_cast<float>(texSize.x),
        height / static_cast<float>(texSize.y));

    // Fija el origen al centro en pixeles de la textura (antes de escalar).
    sprite.setOrigin(texSize.x / 2.f, texSize.y / 2.f);

    // Categoriza sus colisiones con el mundo y los objetos.
    b2Fixture* fixture = body->GetFixtureList();
    while (fixture) {
        b2Filter filter = fixture->GetFilterData();
        filter.categoryBits = CATEGORY_OBSTACLE;
        filter.maskBits = 0xFFFF;
        fixture->SetFilterData(filter);
        fixture = fixture->GetNext();
    }

}


void BoxObstacle::Update()
{
    // Sincroniza la posición y rotación con el cuerpo físico.
    b2Vec2 pos = body->GetPosition();
    float angle = body->GetAngle() * 180.0f / b2_pi;

    sprite.setPosition(pos.x, pos.y);
    sprite.setRotation(angle);
}


void BoxObstacle::Draw()
{
    Update();
    wnd->draw(sprite);
}
