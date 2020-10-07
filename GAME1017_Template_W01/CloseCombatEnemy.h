#pragma once
#include "BaseEnemy.h"
#include "SoundManager.h"
#include "Player.h"

class CloseCombatEnemy : public BaseEnemy
{
public:
	CloseCombatEnemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf, double baseAngle, int dirX, int dirY, SDL_FRect* playerDest, Player* player);
	bool Attack();
	void Update();
	void Render();
private:
	bool m_isInAttackRange;
	Player* m_player;
	int m_IsReloading;
};

