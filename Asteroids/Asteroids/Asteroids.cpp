#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/glut.h>
#include <vector>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <conio.h>
#include <chrono>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>

using namespace std;
using namespace sf;

int Height = 600;
int Weight = 600;

int N = 5;
int x = 0;
int y = 0;
float ax = 0;
float ay = 0;
bool down = true;//click mouse
bool shield = false;//player shield

const float DT = 0.01;
struct SpaceObject
{
	float x;
	float y;
	float vx;
	float vy;
	float m;
	int size;
	float angle;
};



vector<SpaceObject> asteroids;
vector<SpaceObject> bullet;
SpaceObject player;

vector<SpaceObject> newAsteroids;


//draw Asteroids
void drawAsteroid()
{

	for (vector<SpaceObject>::iterator i = asteroids.begin(); i != asteroids.end(); ++i)
	{
		glPushMatrix();
		glTranslatef(i->x, i->y, 0);
		glScalef(i->size, i->size, 1);
		glRotatef(i->angle, 0, 0, 1);
		glColor3f(0.5, 0.5, 0.5);
		glBegin(GL_POLYGON);
		for (int i = 0; i < 5; ++i)
			glVertex2f(sin(2 * 3.14 / 5 * i), cos(2 * 3.14 / 5 * i));
		glEnd();

		glPopMatrix();
	}
}

//draw Bullet
void drawBullet()
{
	for (auto &b : bullet)
	{
		glPushMatrix();
		glTranslatef(b.x, b.y, 0);
		glPointSize(3);
		glColor3f(1, 0, 1);
		glBegin(GL_POINTS);
		glVertex2f(0, 0);
		glEnd();
		glPointSize(1);
		glPopMatrix();
	}
}

//draw Ship
void drawShip()
{
	glPushMatrix();
	glTranslatef(player.x, player.y, 0);
	glRotatef(player.angle, 0, 0, 1);
	glColor3f(0, 1, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, -20);
	glVertex2f(-10, 10);
	glVertex2f(0, 0);
	glVertex2f(10, 10);

	//glBegin(GL_TRIANGLES);
	//glVertex2f(0, -5);
	//glVertex2f(-2.5, +2.5);
	//glVertex2f(+2.5, +2.5);
	//{ 0.0f, -5.0f},
	//{ -2.5f, +2.5f },
	//{ +2.5f, +2.5f }
	glEnd();
	glColor3f(1, 0, 0);
	glPopMatrix();


}

void WrapCoordinates(float ix, float iy, float &ox, float &oy)
{
	ox = ix;
	oy = iy;
	if (ix < 0.0f)	ox = ix + (float)Weight;
	if (ix >= (float)Weight)	ox = ix - (float)Weight;
	if (iy < 0.0f)	oy = iy + (float)Height;
	if (iy >= (float)Height) oy = iy - (float)Height;
}

bool IspointInsideCircle(float cx, float cy, float radius, float x, float y)
{
	return sqrt((x - cx)*(x - cx) + (y - cy)*(y - cy)) <= radius;
}

//Destroy asteroids on click
void FindAsteroids()
{
	for (auto &a : asteroids)
	{

		if (IspointInsideCircle(a.x, a.y, a.size, x, y))
		{
			if (a.size > 15)
			{
				//create two asteroiids
				float angle1 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
				float angle2 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
				newAsteroids.push_back({ a.x, a.y, 20.0f * sinf(angle1), 20.0f * cosf(angle1), 0, a.size / 2, 0.0f });
				newAsteroids.push_back({ a.x, a.y, 20.0f * sinf(angle2), 20.0f * cosf(angle2), 0, a.size / 2, 0.0f });
			}
			a.x = 0 - 1;
		}

	}
	for (auto a : newAsteroids)
		asteroids.push_back(a);

	if (newAsteroids.size() > 0)
	{
		newAsteroids.clear();
	}

	// Remove asteroids 
	if (asteroids.size() > 0)
	{
		auto i = remove_if(asteroids.begin(), asteroids.end(), [&](SpaceObject o) { return (o.x < 0); });
		if (i != asteroids.end())
			asteroids.erase(i);
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	//asteroids
	drawAsteroid();
	//plane
	drawShip();
	//bullets
	drawBullet();

	if (shield)
	{
		glPushMatrix();
		glBegin(GL_LINES);
		for (int a = 0; a < 36; ++a)
		{
			float x = player.m * cos(a * 3.14 / 18);
			float y = player.m * sin(a * 3.14 / 18);
			glVertex2f(player.x + x, player.y + y);
			x = player.m * cos((a + 1) * 3.14 / 18);
			y = player.m * sin((a + 1) * 3.14 / 18);
			glVertex2f(player.x + x, player.y + y);
		}
		glEnd();
		glPopMatrix();
	}
	if (down == false) {
		FindAsteroids();
		down = true;
	}
	glutSwapBuffers();
}

//create and destroy asteroids
void destroy()
{
	//asteroid destruction from an asteroid
	for (vector<SpaceObject>::iterator i = asteroids.begin(); i != asteroids.end(); ++i)
	{
		for (vector<SpaceObject>::iterator j = asteroids.begin(); j != asteroids.end(); ++j)
		{
			if (i != j)
			{
				if (IspointInsideCircle(i->x, i->y, i->size, j->x, j->y))
				{
					if (i->size > 15)
					{
						//create two asteroiids
						float angle1 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
						float angle2 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
						newAsteroids.push_back({ i->x, i->y, 20.0f * sinf(angle1), 20.0f * cosf(angle1), 0, i->size / 2, 0.0f });
						newAsteroids.push_back({ i->x, i->y, 20.0f * sinf(angle2), 20.0f * cosf(angle2), 0, i->size / 2, 0.0f });


					}
					//i->x = -1;
				}
			}

		}
	}
	// Append new asteroids to existing vector
	for (auto a : newAsteroids)
		asteroids.push_back(a);

	if (newAsteroids.size() > 0)
	{
		newAsteroids.clear();
	}

	// Remove asteroids 
	if (asteroids.size() > 0)
	{
		auto i = remove_if(asteroids.begin(), asteroids.end(), [&](SpaceObject o) { return (o.x < 0); });
		if (i != asteroids.end())
			asteroids.erase(i);
	}
}

//move asteroids
void moveAsteroids()
{
	for (vector<SpaceObject>::iterator i = asteroids.begin(); i != asteroids.end(); ++i)
	{
		for (vector<SpaceObject>::iterator j = asteroids.begin(); j != asteroids.end(); ++j)
		{
			if (i != j)
			{
				float d = sqrt((i->x - j->x) * (i->x - j->x) + (i->y - j->y) * (i->y - j->y));
				if (d < i->m + i->m)
				{
					float f = 10 * (i->m + j->m + d);
					i->vx += f * (i->x - j->x) / d / i->m * DT;
					i->vy += f * (i->y - j->y) / d / i->m * DT;
					j->vx -= f * (i->x - j->x) / d / j->m * DT;
					j->vy -= f * (i->y - j->y) / d / j->m * DT;
				}
			}
		}
	}
	//going beyond the screen
	for (vector<SpaceObject>::iterator i = asteroids.begin(); i != asteroids.end(); ++i)
	{
		i->x += i->vx * DT;
		i->y += i->vy * DT;
		WrapCoordinates(i->x, i->y, i->x, i->y);
	}
	WrapCoordinates(player.x, player.y, player.x, player.y);
}

void ResetGame(int n)
{

	// Initialise Player Position
	player.x = Weight / 2;
	player.y = Height / 2;
	player.m = 40;
	player.vx = 0.0f;
	player.vy = 0.0f;
	player.angle = 0.0f;

	bullet.clear();
	asteroids.clear();

	for (int i = 0; i < N; i++)
	{
		SpaceObject p0 = { rand() % Weight, rand() % Height , rand() % 100000 / 500.0 - 100, rand() % 100000 / 500.0 - 100 , 1, 25,rand() % 90 };
		asteroids.push_back(p0);
	};

	// Reset game
	x = 0;
	y = 0;
	down = true;//click mouse
	shield = false;//player shield

}
//Press KEy
void KeyState()
{

	player.vx = 0;
	player.vy = 0;
	//KEY_STATE
	if (GetAsyncKeyState(VK_UP))
	{
		player.vx += sinf(player.angle * 3.14 / 180) * 20.0f;
		player.vy += -cosf(player.angle * 3.14 / 180) * 20.0f;
		player.x += player.vx * 0.1;
		player.y += player.vy * 0.1;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		player.vx += -sin(player.angle * 3.14 / 180) * 20.0f;
		player.vy += cos(player.angle * 3.14 / 180) * 20.0f;
		player.x += player.vx * 0.1;
		player.y += player.vy * 0.1;

	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		player.angle += 3.0f;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		player.angle -= 3.0f;
	}

	//Fire
	if (GetAsyncKeyState(VK_SPACE))
	{
		if (bullet.size() < 1)
			bullet.push_back({ player.x, player.y, 5 * sinf(player.angle * 3.14 / 180), -5 * cosf(player.angle * 3.14 / 180), 0, 0, 0 });
	}
}

//special skills
void KeySkill()
{

	if (GetAsyncKeyState(VK_F1))
	{
		for (auto &a : asteroids)
		{
			if (IspointInsideCircle(player.x, player.y, 100, a.x, a.y))
			{
				a.x = 0;

			}
		}

	}
	if (GetAsyncKeyState(VK_F2))
	{
		if (shield)
			shield = false;
		else
			shield = true;

	}
}


void timer(int = 0)
{
	//move asteroids
	moveAsteroids();

	if (asteroids.size() <= 0)
		ResetGame(N++);
	for (auto &a : asteroids)
	{
		if (IspointInsideCircle(a.x, a.y, a.size, player.x, player.y) && shield == false)
		{
			ResetGame(N);
		}

	}


	//move key
	KeyState();

	//skill key
	KeySkill();

	//bullet
	for (auto &b : bullet)
	{
		b.x += b.vx;
		b.y += b.vy;

		//remove bullet
		if (bullet.size() > 0)
		{
			auto i = remove_if(bullet.begin(), bullet.end(), [&](SpaceObject o)
			{
				return(o.x < 1 || o.y < 1 || o.x >= Weight - 1 || o.y >= Height - 1);
			});
			if (i != bullet.end())
				bullet.erase(i);
		}

		//two asteroids 
		for (auto &a : asteroids)
		{
			if (IspointInsideCircle(a.x, a.y, a.size, b.x, b.y))
			{
				b.x = 0 - 1;
				bullet.clear();
				if (a.size > 15)
				{
					//create two asteroiids
					float angle1 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
					float angle2 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
					newAsteroids.push_back({ a.x, a.y, 20.0f * sinf(angle1), 20.0f * cosf(angle1), 0, a.size / 2, 0.0f });
					newAsteroids.push_back({ a.x, a.y, 20.0f * sinf(angle2), 20.0f * cosf(angle2), 0, a.size / 2, 0.0f });
				}
				a.x = -1;
			}
		}

		// Append new asteroids to existing vector
		for (auto a : newAsteroids)
			asteroids.push_back(a);

		if (newAsteroids.size() > 0)
		{
			newAsteroids.clear();
		}

		// Remove asteroids 
		if (asteroids.size() > 0)
		{
			auto i = remove_if(asteroids.begin(), asteroids.end(), [&](SpaceObject o) { return (o.x < 0); });
			if (i != asteroids.end())
				asteroids.erase(i);
		}

		WrapCoordinates(b.x, b.y, b.x, b.y);
	}

	//destroy();


	display();
	glutTimerFunc(10, timer, 0);
}


void mouse(int button, int state, int ax, int ay)
{
	x = ax;
	y = ay;
	down = state = GLUT_DOWN;
}


int main(int argc, char** argv)
{

	for (int i = 0; i < argc; i++)
	{
		cout << argv[i] << endl;

	}

	N = 5;


	player.x = Weight / 2;
	player.y = Height / 2;
	player.m = 40;

	for (int i = 0; i < N; i++)
	{
		SpaceObject p0 = { rand() % Weight, rand() % Height , rand() % 100000 / 500.0 - 100, rand() % 100000 / 500.0 - 100 , 1, 25,rand() % 90 };
		asteroids.push_back(p0);
	};


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(Height, Weight);
	glutInitWindowPosition(20, 10);
	glutCreateWindow("Asteroids");
	glClearColor(0, 0, 0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, Height, Weight, 0, -1, 1);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	timer();


	glutMainLoop();
}