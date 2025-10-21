// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#include "Meta.h"


Meta::Meta(b2World* world, sf::RenderWindow* window, float x, float y, float radius)
    : wnd(window), body(nullptr), hasWon(false)
{
    // Crea el cuerpo físico.
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(x, y);
    body = world->CreateBody(&bodyDef);

    b2CircleShape circleShape;
    circleShape.m_radius = radius;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.isSensor = false;
    fixtureDef.density = 0.0f;
    body->CreateFixture(&fixtureDef);

    // Configura su apariencia con SFML.
    shape.setRadius(radius);
    shape.setFillColor(sf::Color::Red);
    shape.setOrigin(radius, radius);
    shape.setPosition(x, y);

    // Cargar imagen de You Win:
    if (!winTexture.loadFromFile("youWin.png")) {
        std::cerr << "No se pudo cargar youWin.png" << std::endl;
    }
    else {
        winSprite.setTexture(winTexture);
        // La centra en la pantalla.
        sf::Vector2u winSize = wnd->getSize();
        sf::Vector2u texSize = winTexture.getSize();
        winSprite.setPosition(
            (winSize.x - texSize.x) / 2.0f,
            (winSize.y - texSize.y) / 2.0f
        );
    }

    // Categoriza sus colisiones con el mundo y los objetos.
    b2Fixture* fixture = body->GetFixtureList();
    while (fixture) {
        b2Filter filter = fixture->GetFilterData();
        filter.categoryBits = CATEGORY_META;
        filter.maskBits = CATEGORY_RAGDOLL;
        fixture->SetFilterData(filter);
        fixture = fixture->GetNext();
    }


}


void Meta::Draw()
{
    // Sincroniza el sprite con el cuerpo físico.
    if (body) {
        b2Vec2 pos = body->GetPosition();
        shape.setPosition(pos.x, pos.y);
    }

    wnd->draw(shape);

    if (hasWon) {
        // Guarda vista actual.
        sf::View oldView = wnd->getView();

        // Dibuja el sprite de You Win en la vista por defecto.
        wnd->setView(wnd->getDefaultView());
        wnd->draw(winSprite);

        // Restaura la vista del juego.
        wnd->setView(oldView);
    }
}


void Meta::CheckCollision()
{
    for (b2ContactEdge* ce = body->GetContactList(); ce; ce = ce->next)
    {
        b2Fixture* fixtureA = ce->contact->GetFixtureA();
        b2Fixture* fixtureB = ce->contact->GetFixtureB();

        uint16 categoryA = fixtureA->GetFilterData().categoryBits;
        uint16 categoryB = fixtureB->GetFilterData().categoryBits;

        if ((categoryA == CATEGORY_META && categoryB == CATEGORY_RAGDOLL) ||
            (categoryB == CATEGORY_META && categoryA == CATEGORY_RAGDOLL))
        {
            hasWon = true;
            break;
        }

    }
}


void Meta::youWin()
{
    hasWon = true;
    std::cout << "Ganaste! :D" << std::endl;

    // Quita el focus del mouse.
    if (wnd)
        wnd->setMouseCursorGrabbed(false);
}


void Meta::Reset()
{
    hasWon = false;
    shape.setFillColor(sf::Color::Red);
}
