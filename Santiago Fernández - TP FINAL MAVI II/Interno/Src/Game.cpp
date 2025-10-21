// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#include "Game.h"


// =======================================================
// CONSTRUCTOR
// =======================================================

Game::Game(int ancho, int alto, std::string titulo)
{
    wnd = new RenderWindow(VideoMode(ancho, alto), titulo);
    wnd->setVisible(true);
    fps = 200;
    wnd->setFramerateLimit(fps);
    frameTime = 1.0f / fps;
    SetZoom();


    // --- INICIALIZAR PUNTEROS ---
    phyWorld = nullptr;
    meta = nullptr;
    controlBody = nullptr;
    draggedBody = nullptr;
    debugRender = nullptr;
    background = nullptr;
    crosshair = nullptr;


    // --- ESTADOS INICIALES ---
    currentLevel = 1;
    gameOver = false;
    esperandoFin = false;
    levelTransition = false;
    gameCompleted = false;
    inMainMenu = true;


    // --- TEXTURA Y SPRITE DEL MENU ---
    if (!menuTexture.loadFromFile("menu.png")) {
        std::cout << "No se pudo cargar menu.png\n";
    }
    else {
        menuSprite.setTexture(menuTexture);
        sf::Vector2u winSize = wnd->getSize();
        sf::Vector2u texSize = menuTexture.getSize();

        // Escala la imagen del menú a la ventana.
        float scaleX = static_cast<float>(winSize.x) / texSize.x;
        float scaleY = static_cast<float>(winSize.y) / texSize.y;
        menuSprite.setScale(scaleX, scaleY);
        menuSprite.setPosition(0.f, 0.f);
    }


    // --- MOSTRAR MENÚ PRINCIPAL ---
    while (inMainMenu && wnd->isOpen()) {
        sf::Event e;
        while (wnd->pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                wnd->close();

            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Escape) {
                    std::cout << "Saliendo del juego desde el menú.\n";
                    wnd->close();
                }
                else if (e.key.code == sf::Keyboard::Enter || e.key.code == sf::Keyboard::Space) {
                    inMainMenu = false;
                    std::cout << "Iniciando Nivel 1...\n";
                }
            }
        }

        wnd->setView(wnd->getDefaultView());
        wnd->clear(sf::Color::Black);
        wnd->draw(menuSprite);
        wnd->display();
    }


    if (!wnd->isOpen()) return;

    // Restaura la vista con zoom después del menú.
    SetZoom();
    sf::View worldView = wnd->getView();

    // Prepara el crosshair.
    wnd->setMouseCursorVisible(false);
    crosshair = new Crosshair();

    // Crea mundo base y nivel 1.
    InitBaseWorld();
    InitLevelBase();
    CreateLevel1();


    // --- BACKGROUND ---
    background = new Background(wnd, currentLevel);

    // --- BOCA DEL CAÑÓN ---
    bocaCanon.setSize(sf::Vector2f(15.0f, 10.0f));
    bocaCanon.setFillColor(sf::Color::Magenta);
    bocaCanon.setPosition(10.0f, 80.0f);
    bocaCanon.setOrigin(0.0f, bocaCanon.getSize().y / 2.0f);

    // --- FUENTE Y CONTADOR ---
    ragdollsRestantes = 3;
    if (!fuente.loadFromFile("PIXELADE.woff"))
        std::cout << "No se pudo cargar PIXELADE.woff\n";

    contadorTexto.setFont(fuente);
    contadorTexto.setCharacterSize(12);
    contadorTexto.setFillColor(sf::Color::White);
    contadorTexto.setPosition(6, 2);
    contadorTexto.setString("Ragdolls: " + std::to_string(ragdollsRestantes));

    // --- GAME OVER ---
    if (!gameOverTexture.loadFromFile("gameOver.png"))
        std::cout << "No se pudo cargar gameOver.png\n";
    else {
        gameOverSprite.setTexture(gameOverTexture);
        sf::Vector2u winSize = wnd->getSize();
        sf::Vector2u texSize = gameOverTexture.getSize();
        gameOverSprite.setPosition(
            (winSize.x - texSize.x) / 2.0f,
            (winSize.y - texSize.y) / 2.0f
        );
    }

    // --- VICTORIA ---
    if (!victoriaTexture.loadFromFile("victoria.png"))
        std::cout << "No se pudo cargar victoria.png\n";
    else {
        victoriaSprite.setTexture(victoriaTexture);
        sf::Vector2u winSize = wnd->getSize();
        sf::Vector2u texSize = victoriaTexture.getSize();
        victoriaSprite.setPosition(
            (winSize.x - texSize.x) / 2.0f,
            (winSize.y - texSize.y) / 2.0f
        );
    }
}




// =======================================================
// BUCLE PRINCIPAL
// =======================================================

void Game::Loop()
{
    // Guarda la vista del mundo con zoom.
    sf::View worldView = wnd->getView();

    while (wnd->isOpen())
    {
        wnd->clear(clearColor);
        DoEvents();


        // --- MENÚ PRINCIPAL ---
        if (inMainMenu)
        {
            // Dibuja el menú en coordenadas de pantalla.
            wnd->setView(wnd->getDefaultView());
            wnd->draw(menuSprite);

            // Dibuja el crosshair en píxeles de pantalla.
            sf::Vector2i mousePixel = sf::Mouse::getPosition(*wnd);
            crosshair->Posicionar(static_cast<float>(mousePixel.x), static_cast<float>(mousePixel.y));
            crosshair->Dibujar(wnd);

            wnd->display();

            // Vuelve a la vista del mundo para mantener consistencia.
            wnd->setView(worldView);
            continue;
        }

        // --- JUEGO PRINCIPAL ---
        wnd->setView(worldView);
        if (background) background->Draw();

        if (!gameCompleted)
            UpdatePhysics();

        DrawGame();

        // --- OVERLAYS Y UI ---
        sf::View oldView = wnd->getView();
        wnd->setView(wnd->getDefaultView());

        // Dibuja el contador de ragdolls.
        wnd->draw(contadorTexto);

        // --- GAME OVER Y VICTORIA ---
        if (gameOver)
            wnd->draw(gameOverSprite);
        if (gameCompleted)
            wnd->draw(victoriaSprite);

        // --- CROSSHAIR ---
        sf::Vector2i mousePixel = sf::Mouse::getPosition(*wnd);
        crosshair->Posicionar(static_cast<float>(mousePixel.x), static_cast<float>(mousePixel.y));
        crosshair->Dibujar(wnd);

        wnd->setView(oldView);
        wnd->display();
    }
}




// =======================================================
// ACTUALIZACIÓN DE FÍSICA
// =======================================================

void Game::UpdatePhysics()
{
    if (gameCompleted) return;
    if (!phyWorld || !meta) return;

    phyWorld->Step(frameTime, 8, 8);
    phyWorld->ClearForces();
    phyWorld->DebugDraw();

    meta->CheckCollision();

    // Si el jugador  llegó a ganar:
    if (meta->HasWon() && !levelTransition)
    {
        levelTransition = true;
        levelClock.restart();
    }

    // Transición de nivel:
    if (levelTransition && levelClock.getElapsedTime().asSeconds() >= tiempoTransicionNivel)
    {
        levelTransition = false;

        if (currentLevel == 1) {
            currentLevel = 2;
            RestartGame();
            CreateLevel2();
            background->SetLevel(2);
        }
        else if (currentLevel == 2) {
            currentLevel = 3;
            RestartGame();
            CreateLevel3();
            background->SetLevel(3);
        }
        else if (currentLevel == 3 && !gameCompleted) {
            std::cout << "GANASTE TODOS LOS NIVELES!" << std::endl;
            gameCompleted = true;
            levelTransition = false;
        }
    }

    // Game Over tras quedarse sin ragdolls:
    if (esperandoFin && !meta->HasWon())
    {
        contadorTexto.setString("Esperando resultado...");
        if (relojFin.getElapsedTime().asSeconds() >= tiempoEsperaGameOver)
        {
            gameOver = true;
            esperandoFin = false;
            std::cout << "Game Over tras esperar 5 segundos sin victoria.\n";
        }
    }
}


// =======================================================
// EVENTOS
// =======================================================
void Game::DoEvents()
{
    Event evt;
    while (wnd->pollEvent(evt))
    {
        switch (evt.type)
        {
        case Event::Closed:
            wnd->close();
            break;

        case Event::KeyPressed:
            if (evt.key.code == Keyboard::Escape) {
                wnd->close();
            }

            // --- MENÚ PRINCIPAL ---
            if (inMainMenu && evt.key.code == Keyboard::Space)
            {
                inMainMenu = false;
                std::cout << "Iniciando juego...\n";

                InitBaseWorld();
                InitLevelBase();
                CreateLevel1();
                background = new Background(wnd, 1);
                ragdollsRestantes = 3;
                contadorTexto.setString("Ragdolls: " + std::to_string(ragdollsRestantes));
            }

            // --- REINICIO DE NIVEL ---
            if (!inMainMenu && evt.key.code == Keyboard::R)
            {
                // Reinicio completo tras "Victoria".
                if (gameCompleted && meta->HasWon())
                {
                    gameCompleted = false;
                    currentLevel = 1;
                    RestartGame();
                    CreateLevel1();
                    background->SetLevel(1);
                    break;
                }

                // Evita reiniciar si el nivel actual se encuentra en "You Win".
                if (meta && meta->HasWon()) {
                    break;
                }

                // Reinicio normal de nivel.
                std::cout << "Reiniciando nivel actual...\n";
                RestartGame();
                if (currentLevel == 1) CreateLevel1();
                else if (currentLevel == 2) CreateLevel2();
                else if (currentLevel == 3) CreateLevel3();
            }
            break;

        case Event::MouseButtonPressed:
            if (inMainMenu) break;

            // Evita disparar ragdolls si ya se ganó.
            if (gameOver || (meta && meta->HasWon()) || gameCompleted)
                break;

            if (evt.mouseButton.button == Mouse::Left)
                CreateRagdollFromCannon();
            break;
        }
    }
}




// =======================================================
// ZOOM Y CÁMARA
// =======================================================

void Game::SetZoom()
{
    View camara;
    camara.setSize(100.0f, 100.0f);
    camara.setCenter(50.0f, 50.0f);
    wnd->setView(camara);
}


// =======================================================
// DIBUJO DE ELEMENTOS
// =======================================================

void Game::DrawGame()
{
    // Dibujar obstáculos:
    for (auto& box : boxes)
        box->Draw();

    // Control body sólo si existe:
    if (controlBody) {
        sf::CircleShape controlShape(5);
        controlShape.setFillColor(sf::Color::Magenta);
        controlShape.setPosition(controlBody->GetPosition().x - 5, controlBody->GetPosition().y - 5);
        wnd->draw(controlShape);
    }

    // --- BOCA DEL CAÑÓN ---
    if (!gameOver && !meta->HasWon() && !gameCompleted)
    {
        sf::Vector2i mousePixel = sf::Mouse::getPosition(*wnd);
        sf::Vector2f mouseWorld = wnd->mapPixelToCoords(mousePixel);

        sf::Vector2f cannonOrigin = bocaCanon.getPosition();
        sf::Vector2f dir = mouseWorld - cannonOrigin;
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0.0001f) dir /= len; else dir = sf::Vector2f(1.0f, 0.0f);
        float angleDeg = std::atan2(dir.y, dir.x) * 180.0f / b2_pi;
        bocaCanon.setRotation(angleDeg);
    }

    for (auto& pend : pendulums)
        pend->Draw();

    sf::RectangleShape pieCanon(sf::Vector2f(8, 10));
    pieCanon.setFillColor(sf::Color::Cyan);
    pieCanon.setPosition(10, 85);
    wnd->draw(pieCanon);
    wnd->draw(bocaCanon);


    // --- UI ---
    wnd->draw(contadorTexto);

    // Meta
    if (meta && !gameCompleted)
        meta->Draw();

    // --- GAME OVER ---
    if (gameOver) {
        sf::View oldView = wnd->getView();
        wnd->setView(wnd->getDefaultView());
        wnd->draw(gameOverSprite);
        wnd->setView(oldView);
    }

    // --- VICTORIA ---
    if (gameCompleted) {
        sf::View oldView = wnd->getView();
        wnd->setView(wnd->getDefaultView());
        wnd->draw(victoriaSprite);
        wnd->setView(oldView);
    }

    // --- CROSSHAIR ---
    sf::Vector2i mousePixel = sf::Mouse::getPosition(*wnd);
    sf::Vector2f mouseWorld = wnd->mapPixelToCoords(mousePixel);
    crosshair->Posicionar(mouseWorld.x, mouseWorld.y);
    crosshair->Dibujar(wnd);

}


// =======================================================
// CREAR RAGDOLL
// =======================================================

// Invoca al ragdoll en la boca del cañón y le aplica un
// impulso para dispararlo por los aires.

void Game::CreateRagdollFromCannon()
{
    if (ragdollsRestantes <= 0 || !phyWorld) return;

    float spawnX = 15.0f;
    float spawnY = 60.0f;
    b2Vec2 spawnPos(spawnX, spawnY);

    sf::Vector2i mousePixel = sf::Mouse::getPosition(*wnd);
    sf::Vector2f mouseWorldSF = wnd->mapPixelToCoords(mousePixel);
    b2Vec2 mouseWorld(mouseWorldSF.x, mouseWorldSF.y);

    b2Vec2 direction = mouseWorld - spawnPos;
    float len = direction.Length();
    if (len > 0.0001f) direction *= 1.0f / len;
    else direction.Set(1.0f, 0.0f);

    float desiredSpeed = 200.0f;

    Ragdoll* ragdoll = new Ragdoll(phyWorld, wnd, spawnX, spawnY);
    ragdoll->AplicarImpulsoATodo(direction, desiredSpeed);
    ragdoll->Despertar();

    ragdollsRestantes--;
    contadorTexto.setString("Ragdolls: " + std::to_string(ragdollsRestantes));

    if (ragdollsRestantes <= 0) {
        esperandoFin = true;
        relojFin.restart();
    }
}


// =======================================================
// NIVELES
// =======================================================


void Game::CreateLevel1()
{
    std::cout << "Cargando Nivel 1...\n";
    ClearWorld();
    InitBaseWorld();
    InitLevelBase();

    // --- CAJAS ---
    boxes.push_back(new BoxObstacle(phyWorld, wnd, 58.0f, 100.0f, 12.0f, 12.0f, true));
    boxes.push_back(new BoxObstacle(phyWorld, wnd, 62.0f, 100.0f, 12.0f, 12.0f, true));

    boxes.push_back(new BoxObstacle(phyWorld, wnd, 58.0f, 65.0f, 12.0f, 12.0f, true));
    boxes.push_back(new BoxObstacle(phyWorld, wnd, 62.0f, 65.0f, 12.0f, 12.0f, true));

    boxes.push_back(new BoxObstacle(phyWorld, wnd, 58.0f, 55.0f, 12.0f, 12.0f, true));
    boxes.push_back(new BoxObstacle(phyWorld, wnd, 62.0f, 55.0f, 12.0f, 12.0f, true));

    boxes.push_back(new BoxObstacle(phyWorld, wnd, 58.0f, 45.0f, 12.0f, 12.0f, true));
    boxes.push_back(new BoxObstacle(phyWorld, wnd, 62.0f, 45.0f, 12.0f, 12.0f, true));

    // --- PLATAFORMA ---
    b2Body* obstacle = Box2DHelper::CreateRectangularStaticBody(phyWorld, 25, 10);
    obstacle->SetTransform(b2Vec2(60.0f, 70.0f), 0.0f);

    // --- META ---
    meta->GetBody()->SetTransform(b2Vec2(80.0f, 48.0f), 0.0f);

}


void Game::CreateLevel2()
{
    std::cout << "Cargando Nivel 2...\n";
    ClearWorld();
    InitBaseWorld();
    InitLevelBase();

    // --- PÉNUDLO ---
    pendulums.push_back(new Pendulum(phyWorld, wnd, 75.0f, 10.0f, 75.0f, 60.0f, 10.0f));

    // --- META ---
    meta->GetBody()->SetTransform(b2Vec2(90.0f, 10.0f), 0.0f);
}


void Game::CreateLevel3()
{
    std::cout << "Cargando Nivel 3...\n";
    ClearWorld();
    InitBaseWorld();
    InitLevelBase();

    // -- CAJAS ---
    boxes.push_back(new BoxObstacle(phyWorld, wnd, 70.0f, 30.0f, 16.0f, 16.0f, true));
    boxes.push_back(new BoxObstacle(phyWorld, wnd, 55.0f, 60.0f, 10.0f, 10.0f, true));

    // --- PLATAFORMA INCLINADA ---
    b2Body* obstacle = Box2DHelper::CreateRectangularStaticBody(phyWorld, 40, 10);
    obstacle->SetTransform(b2Vec2(57.0f, 70.0f), 10.0f);

    // --- PLATAFORMA HORIZONTAL ---
    b2Body* obstacle2 = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 10);
    obstacle2->SetTransform(b2Vec2(68.0f, 45.0f), 0.0f);

    // --- META ---
    meta->GetBody()->SetTransform(b2Vec2(90.0f, 90.0f), 0.0f);

}



// =======================================================
// INICIALIZACIÓN DE MUNDO BASE
// =======================================================

void Game::InitBaseWorld()
{
    ClearWorld();
    // Destruye el mundo y dependencias anteriores para evitar errores
    // y fugas de memoria.

    phyWorld = new b2World(b2Vec2(0.0f, 80.0f));

    debugRender = new SFMLRenderer(wnd);
    debugRender->SetFlags(UINT_MAX);
    phyWorld->SetDebugDraw(debugRender);

    auto SetCategory = [](b2Body* body, uint16 category, uint16 mask) {
        b2Fixture* fixture = body->GetFixtureList();
        while (fixture) {
            b2Filter filter = fixture->GetFilterData();
            filter.categoryBits = category;
            filter.maskBits = mask;
            fixture->SetFilterData(filter);
            fixture = fixture->GetNext();
        }
        };

    // --- SUELO Y PAREDES ---
    b2Body* groundBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
    groundBody->SetTransform(b2Vec2(50.0f, 100.0f), 0.0f);
    SetCategory(groundBody, CATEGORY_WALL, 0xFFFF);

    b2Body* leftWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
    leftWallBody->SetTransform(b2Vec2(0.0f, 50.0f), 0.0f);
    SetCategory(leftWallBody, CATEGORY_WALL, 0xFFFF);

    b2Body* rightWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
    rightWallBody->SetTransform(b2Vec2(100.0f, 50.0f), 0.0f);
    SetCategory(rightWallBody, CATEGORY_WALL, 0xFFFF);

    b2Body* topWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
    topWallBody->SetTransform(b2Vec2(50.0f, 0.0f), 0.0f);
    SetCategory(topWallBody, CATEGORY_WALL, 0xFFFF);


    // --- BASE DEL CAÑÓN Y PROPIEDADES ---
    b2Body* pieCanon = Box2DHelper::CreateRectangularStaticBody(phyWorld, 8, 10);
    pieCanon->SetTransform(b2Vec2(14.0f, 90.0f), 0.0f);

    b2Fixture* fixture = pieCanon->GetFixtureList();
    while (fixture) {
        b2Filter filter = fixture->GetFilterData();
        filter.categoryBits = CATEGORY_CANON;
        filter.maskBits = CATEGORY_OBSTACLE | CATEGORY_META;
        fixture->SetFilterData(filter);
        fixture = fixture->GetNext();
    }

}


// =======================================================
// INICIALIZACIÓN DE META EN NIVEL
// =======================================================

void Game::InitLevelBase()
{
    if (!phyWorld) return;

    if (meta) {
        if (meta->GetBody() && phyWorld)
            phyWorld->DestroyBody(meta->GetBody());
        delete meta;
        meta = nullptr;
    }

    meta = new Meta(phyWorld, wnd, 80.0f, 40.0f, 5.0f);


}


// =======================================================
// LIMPIEZA GENERAL DE MUNDO
// =======================================================

void Game::ClearWorld()
{
    for (auto box : boxes)
        delete box;
    boxes.clear();

    for (auto pend : pendulums)
        delete pend;
    pendulums.clear();


    if (meta) {
        delete meta;
        meta = nullptr;
    }

    if (debugRender) {
        delete debugRender;
        debugRender = nullptr;
    }

    if (phyWorld) {
        delete phyWorld;
        phyWorld = nullptr;
    }

    controlBody = nullptr;
}


// =======================================================
// REINICIO DE NIVEL
// =======================================================

// Función que permite reintentar un nivel o el juego.

void Game::RestartGame()
{
    std::cout << "Reiniciando nivel " << currentLevel << "...\n";
    gameOver = false;
    esperandoFin = false;
    if (meta) meta->Reset();

    ragdollsRestantes = 3;
    contadorTexto.setString("Ragdolls: " + std::to_string(ragdollsRestantes));

    wnd->setMouseCursorGrabbed(true);
}


// =======================================================
// MAIN
// =======================================================

int _tmain(int argc, _TCHAR* argv[])
{
    // Creación del juego y bucle principal.
    Game* Juego;
    Juego = new Game(800, 600, "Santiago Fernández - MAVI II, TP FINAL");
    Juego->Loop(); // Inicia el bucle principal del juego.

    return 0;
}