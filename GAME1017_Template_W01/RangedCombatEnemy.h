#pragma once
#include "BaseEnemy.h"
#include "SoundManager.h"
#include "CollisionManager.h"
#include "Player.h"
#include "Engine.h"
#include "Bullet.h"
#include <chrono>
#include <thread>

class RangedCombatEnemy : public BaseEnemy
{
public:
	RangedCombatEnemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf, double baseAngle, int dirX, int dirY, SDL_FRect* playerDest, Player* player);
	bool Attack();
	void Update();
	void Render();
private:
	void MoveBehindCover();
	void WaitBehindCover();
	bool m_isInAttackRange;
	bool m_isNearToPlayer;
	bool m_isFarFromPlayer;
	Player* m_player;
	SDL_Texture* m_pBulletText;
	Bullet* m_pBullet;
	int m_IsReloading;
};

