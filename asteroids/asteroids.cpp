#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#include "olcConsoleGameEngine.h"

class OneLoneCoder_Asteroids : public olcConsoleGameEngine
{
public:
	OneLoneCoder_Asteroids()
	{
		m_sAppName = L"Asteroids";
	}

private:
	struct sSpaceObject 
	{
		float x;
		float y;
		float dx;
		float dy;
		int nSize;
		float angle;
	};

	vector<sSpaceObject> vecAsteroids;
	vector<sSpaceObject> vecBullets;
	sSpaceObject player;

	vector<pair<float, float>> vecModelShip;
	vector<pair<float, float>> vecModelAsteroid;


protected:
	//called by olcconsolegameengine
	virtual bool OnUserCreate()
	{
		vecAsteroids.push_back({ 20.0f, 20.0f, 8.0f, -6.0f, (int)16, 0.0f});

		//player pos
		player.x = ScreenWidth() / 2.0f;
		player.y = ScreenHeight() / 2.0f;
		player.dx = 0.0f;
		player.dy = 0.0f;
		player.angle = 0.0f;

		vecModelShip =
		{
			{ 0.0f, -5.0f},
			{-2.5f, +2.5f},
			{+2.5f, +2.5f}
		};
		
		int verts = 20;
		for (int i = 0; i < verts; i++)
		{
			float radius = 5.0f;
			float a = ((float)i / (float)verts) * 6.28318f; //pi*2
			vecModelAsteroid.push_back(make_pair(radius * sinf(a), radius * cosf(a)));
		}
		return true;
	}

	//called by olcconsolegameengine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		//clearing screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, 0);

		//steer
		if (m_keys[VK_LEFT].bHeld)
			player.angle -= 5.0f * fElapsedTime;
		if (m_keys[VK_RIGHT].bHeld)
			player.angle += 5.0f * fElapsedTime;

		//thrust
		if (m_keys[VK_UP].bHeld)
		{
			//ACCELERATION changes VELOCITY (with respect to time)
			player.dx += sin(player.angle) * 20.0f * fElapsedTime;
			player.dy += -cos(player.angle) * 20.0f * fElapsedTime;
		}

		//VELOCITY changes POSITION (with respect to time)
		player.x += player.dx * fElapsedTime;
		player.y += player.dy * fElapsedTime;
		
		//ship on map
		WrapCoordinates(player.x, player.y, player.x, player.y);

		//fire bullet
		if (m_keys[VK_SPACE].bReleased)
			vecBullets.push_back({player.x, player.y, 50.0f * sinf(player.angle), -50.0f * cosf(player.angle), 0, 0 });

		//update and draw bullets
		for (auto& b : vecBullets)
		{
			b.x += b.dx * fElapsedTime;
			b.y += b.dy * fElapsedTime;
			WrapCoordinates(b.x, b.y, b.x, b.y);
			Draw(b.x, b.y);
		}

		//remove bullets
		if (vecBullets.size() > 0)
		{
			auto i = remove_if(vecBullets.begin(), vecBullets.end(), [&](sSpaceObject o) {return (o.x < 1 || o.y < 1 || o.x >= ScreenWidth() || o.y >= ScreenHeight());});
			if (i != vecBullets.end())
				vecBullets.erase(i);
		}

		//update and draw asteroids
		for (auto& a : vecAsteroids)
		{
			// VELOCITY changes POSITION (with respect to time)
			a.x += a.dx * fElapsedTime;
			a.y += a.dy * fElapsedTime;
			a.angle += 0.5f * fElapsedTime; // Add swanky rotation :)

			WrapCoordinates(a.x, a.y, a.x, a.y);

			DrawWireFrameModel(vecModelAsteroid, a.x, a.y, a.angle);
		}

		
		//draw ship
		DrawWireFrameModel(vecModelShip, player.x, player.y, player.angle);

		return true;
	}

	void DrawWireFrameModel(const vector<pair<float, float>>& vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short col = FG_WHITE)
	{
		// pair.first = x coordinate
		// pair.second = y coordinate

		// Create translated model vector of coordinate pairs
		vector<pair<float, float>> vecTransformedCoordinates;
		int verts = vecModelCoordinates.size();
		vecTransformedCoordinates.resize(verts);

		// Rotate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
			vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
		}

		// Scale
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
		}

		// Translate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}

		// Draw Closed Polygon
		for (int i = 0; i < verts + 1; i++)
		{
			int j = (i + 1);
			DrawLine(vecTransformedCoordinates[i % verts].first, vecTransformedCoordinates[i % verts].second,
				vecTransformedCoordinates[j % verts].first, vecTransformedCoordinates[j % verts].second, PIXEL_SOLID, col);
		}
	};

	void WrapCoordinates(float ix, float iy, float& ox, float& oy)
	{
		ox = ix;
		oy = iy;
		if (ix < 0.0f) ox = ix + (float)ScreenWidth();
		if (ix >= (float)ScreenWidth()) ox = ix - (float)ScreenWidth();
		if (iy < 0.0f) oy = iy + (float)ScreenHeight();
		if (iy >= (float)ScreenHeight()) oy = iy - (float)ScreenHeight();
	}

	virtual void Draw(int x, int y, short c = 0x2588, short col = 0x000F)
	{
		float fx, fy;
		WrapCoordinates(x, y, fx, fy);
		olcConsoleGameEngine::Draw(fx, fy, c, col);
	}
};
int main()
{
	// Use olcConsoleGameEngine derived app
	OneLoneCoder_Asteroids game;
	game.ConstructConsole(160, 100, 8, 8);
	game.Start();
	return 0;
}
// Uruchomienie programu: Ctrl + F5 lub menu Debugowanie > Uruchom bez debugowania
// Debugowanie programu: F5 lub menu Debugowanie > Rozpocznij debugowanie

// Porady dotyczące rozpoczynania pracy:
//   1. Użyj okna Eksploratora rozwiązań, aby dodać pliki i zarządzać nimi
//   2. Użyj okna programu Team Explorer, aby nawiązać połączenie z kontrolą źródła
//   3. Użyj okna Dane wyjściowe, aby sprawdzić dane wyjściowe kompilacji i inne komunikaty
//   4. Użyj okna Lista błędów, aby zobaczyć błędy
//   5. Wybierz pozycję Projekt > Dodaj nowy element, aby utworzyć nowe pliki kodu, lub wybierz pozycję Projekt > Dodaj istniejący element, aby dodać istniejące pliku kodu do projektu
//   6. Aby w przyszłości ponownie otworzyć ten projekt, przejdź do pozycji Plik > Otwórz > Projekt i wybierz plik sln