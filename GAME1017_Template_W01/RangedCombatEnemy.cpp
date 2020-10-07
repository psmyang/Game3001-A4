#include "RangedCombatEnemy.h"

#define MINRADIUS 150
#define MAXRADIUS 350
#define RELOADING 2000

RangedCombatEnemy::RangedCombatEnemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf, double baseAngle, int dirX, int dirY, SDL_FRect* playerDest, Player* player)
	:BaseEnemy(s, d, r, t, sstart, smin, smax, nf, baseAngle, dirX, dirY, playerDest) {
	m_isInAttackRange = false;
	m_isNearToPlayer = false;
	m_player = player;
	m_IsReloading = RELOADING;
	m_pBulletText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Enemies.png");
	m_pBullet = new Bullet({ 0,439,9,29 }, { (float)(16) * 32, (float)(12) * 32, 32, 32 }, Engine::Instance().GetRenderer(), m_pBulletText, 0, 0, 0, 4);
}

bool RangedCombatEnemy::Attack()
{
	SDL_Rect src = { 96, 343, 96,96 };
	SDL_FRect dest = { GetDstP()->x - 32, GetDstP()->y - 32, GetDstP()->w + 64, GetDstP()->h + 64 };
	SDL_RenderCopyExF(m_pRend, m_pText, &src, &dest, 0, 0, static_cast<SDL_RendererFlip>(1));
	SOMA::PlaySound("Boom", 0, 5);
	return true;
}

void RangedCombatEnemy::Update() {
	if (m_isFleeing) {
		BaseEnemy::Update();
		return;
	}
	int enemyX = GetDstP()->x + GetDstP()->w / 2;
	int enemyY = GetDstP()->y + GetDstP()->h / 2;

	int playerX = m_playerDest->x + m_playerDest->w / 2;
	int playerY = m_playerDest->y + m_playerDest->h / 2;

	m_isLOS = CheckLOS(enemyX, enemyY, playerX, playerY);

	if (m_isLOS && m_isBehindCover) {
		MoveToLOSPoint();
		return;
	}

	if (!m_isLOS && m_isBehindCover) {
		WaitBehindCover();
	}

	if (m_isLOS) {
		if (m_isNearToPlayer)
			MoveFromPosition(m_playerDest->x, m_playerDest->y);
		if (m_isFarFromPlayer) {
			Move2Position(m_playerDest->x, m_playerDest->y);
		}
	}
	else
		BaseEnemy::Update();

	double distance = MAMA::Distance(enemyX, playerX, enemyY, playerY);
	m_isInAttackRange = (distance < (double)MAXRADIUS) && distance > (double)MINRADIUS ? true : false;
	m_isNearToPlayer = (distance <= (double)MINRADIUS) ? true : false;
	m_isFarFromPlayer = (distance >= (double)MAXRADIUS) ? true : false;


	m_pBullet->Update();

	if (!m_pBullet->isDied())
	{
		if (COMA::AABBCheck(*m_pBullet->GetDstP(), *m_player->GetDstP()))
		{
			m_player->GetAttacked();
			FindLOS(enemyX, enemyY, false);
			m_pBullet->Collision();
		}
	}
}

void RangedCombatEnemy::Render()
{
	BaseEnemy::Render();
	m_pBullet->Render();

	if (m_isInAttackRange && m_isLOS && m_IsReloading == RELOADING) {
		double shootAngle = MAMA::AngleBetweenPoints(m_dst.x, m_dst.y, m_player->GetDstP()->x, m_player->GetDstP()->y);
		m_pBullet->Shoot({ (int)(m_dst.x + m_dst.w / 2) , (int)(m_dst.y + m_dst.h / 2) }, 0, shootAngle);
		m_IsReloading = 0;
	}
	if (m_IsReloading < RELOADING) {
		m_IsReloading += 10;
	}
}

void RangedCombatEnemy::MoveBehindCover()
{

}

void RangedCombatEnemy::WaitBehindCover()
{
	m_WaitInBehindCover = true;
	m_isBehindCover = false;
}
