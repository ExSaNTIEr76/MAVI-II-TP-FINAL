// =======================================================
// SANTIAGO EXEQUIEL FERNÁNDEZ - MAVI II, TP FINAL
// =======================================================


#pragma once
#include "stdafx.h"
#include "SFMLRenderer.h"
#include "Box2DHelper.h"

using namespace sf;

class Crosshair {

	Texture _crossTex;
	Sprite _crossSprite;

public:

	Crosshair() {

		_crossTex.loadFromFile("crosshair.png");
		_crossSprite.setTexture(_crossTex);
		_crossSprite.setScale(1, 1);
		Vector2u size = _crossTex.getSize();
		_crossSprite.setOrigin(size.x / 2.0f, _crossTex.getSize().y / 2.0f);

	}

	void Dibujar(RenderWindow* wnd) {

		wnd->draw(_crossSprite);

	}

	void Posicionar(float x, float y) {

		_crossSprite.setPosition(x, y);

	}

	Vector2f ObtenerPosicion() {
		return _crossSprite.getPosition();
	}

};