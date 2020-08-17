#pragma once
#ifndef _BASE_ENEMY_H_
#define _BASE_ENEMY_H_

#include "Sprite.h"
#include <vector>

class BaseEnemy : public AnimatedSprite
{
public:
	BaseEnemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf, double baseAngle, int dirX, int dirY, SDL_FRect* playerDest);
	void SetLocalMatrix();
	virtual void Update();
	virtual void Render();
	void Start();
	void SetDebugMode(bool enable);
	void Collision();
	void Flee();
	bool IsDestroyed();
	bool IsEsceaped();
	virtual bool Atack();
	static int gameMatrix[24][32];
protected:
	void FindLOS(int, int, bool);
	void MoveFromPosition(int, int);
	SDL_FRect* m_playerDest;
	bool m_isFleeing;
	bool m_isLOS;
	bool CheckLOS(int, int, int, int);
	bool m_isEscaped;
	bool Move2Position(int, int);
	void MoveToLOSPoint();
	bool m_isBehindCover;
	bool m_WaitInBehindCover;
private:
	int m_waitedInCover = -1;
	enum state { idle, running, destroyed } m_state;
	bool m_dir;
	void Move2Stop(const double angle);
	void SetState(int s);
	void ToggleState();
	void Stop();
	void SetVs(const double angle);
	void SetPath(int, int);
	int m_localMatrix[24][32];
	std::vector<std::pair<int, int>> pathToLOS;
	double m_dx, m_dy,
		m_accel,
		m_vel,
		m_velMax,
		m_rotMax;
	double m_baseAngle;
	double m_turnAngle;
	int m_dirX;
	int m_dirY;
	bool m_isDebugEnable;
	bool m_inRange;
	bool m_isCollisioned;
	bool m_isDestroyed;
	bool m_isDestroying;
	bool m_isDetectedLOSposition;
	bool m_isOnLOSDetection;
	int m_LOS_x;
	int m_LOS_y;
	int m_LOS_x_Dir;
	int m_LOS_y_Dir;
	int  m_eslapsedFromCollision;
	int  m_eslapsedFromDestroy;
};

#endif

