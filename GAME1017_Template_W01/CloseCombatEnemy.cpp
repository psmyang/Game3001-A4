#include "CloseCombatEnemy.h"
#include "CollisionManager.h"

#define ATTACKRADIUS 50

CloseCombatEnemy::CloseCombatEnemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf, double baseAngle, int dirX, int dirY, SDL_FRect* playerDest, Player* player)
	:BaseEnemy(s, d, r, t, sstart, smin, smax, nf, baseAngle, dirX, dirY, playerDest) {
	m_isInAttackRange = false;
	m_player = player;
	m_IsReloading = 700;
}

bool CloseCombatEnemy::Attack()
{
	SDL_Rect src = { 96, 343, 96,96 };
	SDL_FRect dest = { GetDstP()->x - 32, GetDstP()->y - 32, GetDstP()->w + 64, GetDstP()->h + 64 };
	SDL_RenderCopyExF(m_pRend, m_pText, &src, &dest, 0, 0, static_cast<SDL_RendererFlip>(1));
	SOMA::PlaySound("Boom", 0, 5);
	return true;
}

void CloseCombatEnemy::Update() {
	if (!m_isInAttackRange || m_isFleeing) {
		BaseEnemy::Update();
	}
	int enemyX = GetDstP()->x + GetDstP()->w / 2;
	int enemyY = GetDstP()->y + GetDstP()->h / 2;

	int playerX = m_playerDest->x + m_playerDest->w / 2;
	int playerY = m_playerDest->y + m_playerDest->h / 2;

	m_isInAttackRange = (MAMA::Distance(enemyX, playerX, enemyY, playerY) < (double)ATTACKRADIUS) ? true : false;
}

void CloseCombatEnemy::Render()
{
	BaseEnemy::Render();
	if (m_isInAttackRange && m_IsReloading == 700) {
		Attack();
		m_player->GetAttacked();
		m_IsReloading = 0;
	}
	if (m_IsReloading < 700) {
		m_IsReloading += 10;
	}
}
