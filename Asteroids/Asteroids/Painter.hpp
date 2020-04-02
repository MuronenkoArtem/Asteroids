#pragma once
class Painter
{
public:
	void drawAsteroid(float x, float y, float size, float angle);
	enum Jet { FORVARD, BACK, LEFT, RIGHT };
	void drawShip(float x, float y, float angle, Jet state);
	void drawBullet(float x, float y);
	void drawStar(float x, float y);
};



