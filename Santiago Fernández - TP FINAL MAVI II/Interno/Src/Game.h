// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#pragma once
#include "stdafx.h"
#include "Ragdoll.h"
#include "Crosshair.h"
#include "Background.h"
#include "BoxObstacle.h"
#include "Pendulum.h"
#include "Meta.h"

using namespace sf;

class Game
{
private:

    // Propiedades de la ventana
    int alto;
    int ancho;
    sf::RenderWindow* wnd;
    Color clearColor;

    // Objetos de box2d
    b2Body* controlBody;
    b2Body* draggedBody;
    Background* background;
    Meta* meta;
    Crosshair* crosshair;
    b2Vec2 clickOffset;
    b2World* phyWorld;
    SFMLRenderer* debugRender;
    sf::RectangleShape bocaCanon;
    std::vector<BoxObstacle*> boxes;
    std::vector<Pendulum*> pendulums;


    // Estados del juego
    int currentLevel = 1;
    bool gameCompleted = false;
    bool levelTransition = false;
    bool esperandoFin = false;
    bool gameOver = false;

    sf::Clock levelClock;
    sf::Clock relojFin;

    // segundos entre niveles
    float tiempoTransicionNivel = 3.0f;
    float tiempoEsperaGameOver = 5.0f;

    // Recursos gráficos
    sf::Texture cannonBaseTexture;
    sf::Texture cannonMouthTexture;
    sf::Texture platformTexture;

    sf::Sprite cannonBaseSprite;
    sf::Sprite cannonMouthSprite;

    std::vector<sf::Sprite> plataformasSprites;

    sf::Texture gameOverTexture;
    sf::Sprite gameOverSprite;

    sf::Texture victoriaTexture;
    sf::Sprite victoriaSprite;

    // Tiempo de frame
    float frameTime;
    int fps;

    // Métodos internos
    void ClearWorld();
    void InitBaseWorld();
    void InitLevelBase();

public:

    // UI
    bool inMainMenu = true;
    int ragdollsRestantes;
    sf::Font fuente;
    sf::Text contadorTexto;

    // Menú principal
    sf::Texture menuTexture;
    sf::Sprite menuSprite;

    // Constructores y funciones principales
    Game(int ancho, int alto, std::string titulo);
    void Loop();
    void DrawGame();
    void UpdatePhysics();
    void DoEvents();
    void SetZoom();
    void RestartGame();
    void CreateRagdollFromCannon();

    // Creación de niveles
    void CreateLevel1();
    void CreateLevel2();
    void CreateLevel3();
};

