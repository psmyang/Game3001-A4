#pragma once
#ifndef _STATES_H_
#define _STATES_H_

#include <SDL.h>
#include "Button.h"
#include "Label.h"

#include "Tile.h"
#include "Player.h"
#include "BaseEnemy.h"
#include "CloseCombatEnemy.h"
#include "RangedCombatEnemy.h"
#include "Bullet.h"
#include <array>
#include <map>
#include <vector>

#define ROWS 24
#define COLS 32

class State // This is the abstract base class for all specific states.
{
public: // Public methods.
	virtual void Update() = 0; // Having at least one 'pure virtual' method like this, makes a class abtract.
	virtual void Render();     // Meaning we cannot create objects of the class.
	virtual void Enter() = 0;  // Virtual keyword means we can override in derived class.
	virtual void Exit() = 0;
	virtual void Resume();

protected: // Private but inherited.
	State() {}
};



// Let's play game
class GameState : public State
{
public:
	GameState();
	void Update();
	void Render();
	void Enter();
	void Exit();
	void Resume();
	void GameOver(bool);
	void PushToEnemies(BaseEnemy* enemy);
	static int AddNewEnemy(void*);

private:
	void DrawHealthBar(SDL_Rect rect, int level);
	// Example-specific properties.
	SDL_Texture* m_pTileText, * m_pPlayerText, * m_pEnemyText, * m_pCircleText, * m_pBulletText;
	std::map<char, Tile*> m_tiles;
	std::vector<BaseEnemy*> m_enemies;

	Player* m_pPlayer;
	Bullet* m_pBullet;
	Label* m_GameOverLabel;
	bool m_showCosts = false, m_hEuclid = true;
	bool m_EnemyRecovering = false;
	bool m_isGameOver = false;

	// Label for instruction
	Label* m_pInstruct[6]; // press R, press H, press F, press M, right-click, left-click
};

// Start the game
class TitleState : public State
{
public:
	TitleState();
	void Update();
	void Render();
	void Enter();
	void Exit();
private:
	Button* m_playBtn;
	Sprite* m_pGameStart;
	//Label* m_pStartLabel;
	Label* m_pNameLabel;
};
#endif