#include "BaseEnemy.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "DebugManager.h"
#include "Engine.h"
#include <queue>

#define SPEED 2
#define RADIUS 100

BaseEnemy::BaseEnemy(SDL_Rect s, SDL_FRect d, SDL_Renderer* r, SDL_Texture* t, int sstart, int smin, int smax, int nf, double baseAngle, int dirX, int dirY, SDL_FRect* playerDest)
	:AnimatedSprite(s, d, r, t, sstart, smin, smax, nf), m_state(running), m_dir(0), m_baseAngle(baseAngle), m_dirX(dirX), m_dirY(dirY), m_playerDest(playerDest)
{
	m_dx = m_dy = m_accel = m_vel = 0.0;
	m_velMax = 5.0;
	m_rotMax = 2.5;
	SetState(running);
	m_isLOS = false;
	m_inRange = false;
	m_turnAngle = 0;
	m_isCollisioned = false;
	m_isDestroyed = false;
	m_isDestroying = false;
	m_isFleeing = false;
	m_isEscaped = false;
	m_isDetectedLOSposition = false;
	m_isOnLOSDetection = false;
	m_isBehindCover = false;
	SetLocalMatrix();
}

int BaseEnemy::gameMatrix[24][32] = { {0} };

void BaseEnemy::SetLocalMatrix() {
	for (int i = 0; i < 24; i++)
		for (int j = 0; j < 32; j++) {
			m_localMatrix[i][j] = gameMatrix[i][j];
		}
}

void BaseEnemy::Update()
{
	if (m_isFleeing) {
		Flee();
		return;
	}
	if (EVMA::KeyPressed(SDL_SCANCODE_P))// Toggle enemies idle/patrol mode.
	{
		//ToggleState();
	}

	if (true)
	{
		int enemyX = GetDstP()->x + GetDstP()->w / 2;
		int enemyY = GetDstP()->y + GetDstP()->h / 2;

		int playerX = m_playerDest->x + m_playerDest->w / 2;
		int playerY = m_playerDest->y + m_playerDest->h / 2;

		m_inRange = (MAMA::Distance(enemyX, playerX, enemyY, playerY) < (double)RADIUS) ? true : false;

		m_isLOS = CheckLOS(enemyX, enemyY, playerX, playerY);

		if (!m_isLOS && m_WaitInBehindCover) {
			return;
		}
		if (m_isLOS) {
			m_WaitInBehindCover = false;
			m_waitedInCover = -1;
		}

		if (m_isLOS && !m_isBehindCover)
			m_isDetectedLOSposition = false;

		if (m_inRange && !m_isLOS) {
			if (!m_isDetectedLOSposition && !m_isOnLOSDetection) {
				FindLOS(m_dst.x, m_dst.y, true);
			}
		}
		if (m_isDetectedLOSposition && !m_isLOS && !m_isBehindCover) {
			MoveToLOSPoint();
			return;
		}
	}

	if (m_state == running) {
		int playerX = m_playerDest->x + m_playerDest->w / 2;
		int playerY = m_playerDest->y + m_playerDest->h / 2;
		if (m_isLOS)
		{
			int m_dir_x = 0;
			int m_dir_y = 0;
			Move2Position(playerX, playerY);

		}
		if (!m_isLOS) {
			if (m_dirY == -1)
			{
				if (m_dst.y > 0 && !COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, -SPEED))
				{
					if (!m_isLOS)
						m_dst.y += -SPEED;
				}
				else
				{
					m_dirY = 1;
					m_baseAngle += 180.0;
				}
			}
			else if (m_dirY == 1)
			{
				if (m_dst.y < 768 - 32 && !COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, SPEED))
				{
					if (!m_isLOS)
						m_dst.y += SPEED;
				}
				else
				{
					m_dirY = -1;
					m_baseAngle -= 180.0;
				}
			}

			if (m_dirX == -1)
			{
				if (m_dst.x > 0 && !COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, -SPEED, 0))
				{
					if (!m_isLOS)
						m_dst.x += -SPEED;
					m_dir = 1;
				}
				else
				{
					m_dirX = 1;
					m_baseAngle += 180.0;
				}
			}
			else if (m_dirX == 1)
			{
				if (m_dst.x < 1024 - 32 && !COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, SPEED, 0))
				{
					if (!m_isLOS)
						m_dst.x += SPEED;
					m_dir = 0;
				}
				else
				{
					m_dirX = -1;
					m_baseAngle -= 180.0;
				}
			}
			SetAngle(m_baseAngle);
		}
	}
	else
	{
		m_turnAngle += 4.0f;
		if (m_turnAngle >= 360.0f)
		{
			m_turnAngle = m_turnAngle - 360.0f;
		}
		SetAngle(m_baseAngle + m_turnAngle);
		Animate();
	}
}

bool BaseEnemy::Move2Position(int x, int y) {
	if (x == m_dst.x && y == m_dst.y)
		return false;
	if (x != m_dst.x) {
		if (x > m_dst.x)
		{
			if (!COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, SPEED / 2, 0)) {
				m_dst.x += SPEED / 2;
			}
		}
		else {
			if (!COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, -SPEED / 2, 0)) {
				m_dst.x += -SPEED / 2;
			}
		}
	}
	if (y != m_dst.y) {
		if (y > m_dst.y) {
			if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, SPEED / 2)) {
				m_dst.y += SPEED / 2;
			}
		}
		else {
			if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, -SPEED / 2)) {
				m_dst.y += -SPEED / 2;
			}
		}
	}
	return true;
}

bool BaseEnemy::CheckLOS(int enemyX, int enemyY, int playerX, int playerY) {
	bool LOS = true;
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			auto tile = Engine::Instance().GetLevel()[row][col];
			if (tile->IsObstacle())
			{
				SDL_Rect rect = { (int)tile->GetDstP()->x, (int)tile->GetDstP()->y, (int)tile->GetDstP()->w, (int)tile->GetDstP()->h };
				if (SDL_IntersectRectAndLine((const SDL_Rect*)&rect, &enemyX, &enemyY, &playerX, &playerY))
				{
					LOS = false;
					break;
				}
			}

		}

		if (!LOS)
		{
			break;
		}
	}
	return LOS;
}

void BaseEnemy::MoveToLOSPoint()
{
	m_isDetectedLOSposition = true;
	if (pathToLOS.empty()) {
		m_isDetectedLOSposition = false;
		m_isBehindCover = false;
		return;
	}
	auto it = pathToLOS.rbegin();
	bool isLastPoint = false;
	int cur_row = it->first;
	int cur_col = it->second;
	int next_col, next_row;
	if (++it != pathToLOS.rend()) {
		next_row = it->first;
		next_col = it->second;
	}
	else {
		isLastPoint = true;
		next_row = cur_row;
		next_col = cur_col;
	}
	int enemyX = m_dst.x; // GetDstP()->x + GetDstP()->w / 2;
	int enemyY = m_dst.y; //GetDstP()->y + GetDstP()->h / 2;
	if (enemyX >= cur_col * 32 - 16 && enemyX <= cur_col * 32 + 48 &&
		enemyY >= cur_row * 32 - 16 && enemyY <= cur_row * 32 + 48) {
		bool res = Move2Position(next_col * 32, next_row * 32);
		if (res == false)
			pathToLOS.pop_back();
	}
	else {
		pathToLOS.pop_back();
	}
}

void BaseEnemy::SetPath(int row, int col) {
	while (m_localMatrix[row][col] != 1) {
		pathToLOS.push_back(std::pair<int, int>(row, col));
		if (m_localMatrix[row - 1][col] == m_localMatrix[row][col] - 1) {
			row--;
			continue;
		}
		if (m_localMatrix[row][col - 1] == m_localMatrix[row][col] - 1) {
			col--;
			continue;
		}
		if (m_localMatrix[row + 1][col] == m_localMatrix[row][col] - 1) {
			row++;
			continue;
		}
		if (m_localMatrix[row][col + 1] == m_localMatrix[row][col] - 1) {
			col++;
			continue;
		}
	}
	if (m_localMatrix[row][col] == 1) {
		pathToLOS.push_back(std::pair<int, int>(row, col));
	}
	SetLocalMatrix();
	m_isOnLOSDetection = false;
}


void BaseEnemy::FindLOS(int x, int y, bool isFindLos)
{
	if (m_isOnLOSDetection)
		return;
	else
		m_isOnLOSDetection = true;
	SetLocalMatrix();
	int col = x / 32;
	int row = y / 32;
	int val = 1;
	std::queue<std::pair<int, int>> que;
	if (m_localMatrix[row][col] == 0) {
		m_localMatrix[row][col] = 1;
		que.push(std::pair<int, int>(row, col));
	}
	while (!que.empty()) {
		std::pair<int, int> curPos = que.front();
		que.pop();
		int enemyX = curPos.second * 32 + GetDstP()->w / 2;
		int enemyY = curPos.first * 32 + GetDstP()->h / 2;

		int playerX = m_playerDest->x + m_playerDest->w / 2;
		int playerY = m_playerDest->y + m_playerDest->h / 2;
		val = m_localMatrix[curPos.first][curPos.second];
		if (isFindLos) {
			if (CheckLOS(enemyX, enemyY, playerX, playerY)) {
				SetPath(curPos.first, curPos.second);
				MoveToLOSPoint();
				return;
			}
		}
		else {
			if (!CheckLOS(enemyX, enemyY, playerX, playerY)) {
				m_isBehindCover = true;
				SetPath(curPos.first, curPos.second);
				MoveToLOSPoint();
				return;
			}
		}
		if (m_localMatrix[curPos.first + 1][curPos.second] != -1 &&
			(m_localMatrix[curPos.first + 1][curPos.second] == 0 || m_localMatrix[curPos.first + 1][curPos.second] > val + 1)) {
			m_localMatrix[curPos.first + 1][curPos.second] = val + 1;
			que.push(std::pair<int, int>(curPos.first + 1, curPos.second));
		}
		if (m_localMatrix[curPos.first - 1][curPos.second] != -1 &&
			(m_localMatrix[curPos.first - 1][curPos.second] == 0 || m_localMatrix[curPos.first - 1][curPos.second] > val + 1)) {
			m_localMatrix[curPos.first - 1][curPos.second] = val + 1;
			que.push(std::pair<int, int>(curPos.first - 1, curPos.second));
		}
		if (m_localMatrix[curPos.first][curPos.second + 1] != -1 &&
			(m_localMatrix[curPos.first][curPos.second + 1] == 0 || m_localMatrix[curPos.first][curPos.second + 1] > val + 1)) {
			m_localMatrix[curPos.first][curPos.second + 1] = val + 1;
			que.push(std::pair<int, int>(curPos.first, curPos.second + 1));
		}
		if (m_localMatrix[curPos.first][curPos.second - 1] != -1 &&
			(m_localMatrix[curPos.first][curPos.second - 1] == 0 || m_localMatrix[curPos.first][curPos.second - 1] > val + 1)) {
			m_localMatrix[curPos.first][curPos.second - 1] = val + 1;
			que.push(std::pair<int, int>(curPos.first, curPos.second - 1));
		}
	}
	m_isOnLOSDetection = false;
}

void BaseEnemy::MoveFromPosition(int x, int y)
{
	if (x < m_dst.x)
	{
		if (!COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, SPEED / 2, 0)) {
			m_dst.x += SPEED / 2;
		}
	}
	else {
		if (!COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, -SPEED / 2, 0)) {
			m_dst.x += -SPEED / 2;
		}
	}
	if (y < m_dst.y) {
		if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, SPEED / 2)) {
			m_dst.y += SPEED / 2;
		}
	}
	else {
		if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, -SPEED / 2)) {
			m_dst.y += -SPEED / 2;
		}
	}
}

void BaseEnemy::Render()
{
	if (m_WaitInBehindCover) {
		if (m_waitedInCover == -1)
			m_waitedInCover = 0;
		else
			m_waitedInCover += 1;
		if (m_waitedInCover > 250)
		{
			m_waitedInCover = -1;
			m_WaitInBehindCover = false;
		}
	}
	// enable debug mode
	if (m_isDebugEnable) {
		double circle_x = GetDstP()->x + GetDstP()->w / 2;
		double circle_y = GetDstP()->y + GetDstP()->h / 2;
		double circle_radius = RADIUS; // Increase this later
		if (m_inRange)
		{
			SDL_SetRenderDrawColor(m_pRend, 250, 0, 0, 255);
		}
		else
		{
			SDL_SetRenderDrawColor(m_pRend, 0, 0, 250, 255);
		}

		double point1_x;
		double point1_y;
		double point2_x;
		double point2_y;

		// Draw circle
		for (int t = 1; t < 360; t++)
		{
			point1_x = circle_x + circle_radius * cos(t - 1);
			point1_y = circle_y + circle_radius * sin(t - 1);

			point2_x = circle_x + circle_radius * cos(t);
			point2_y = circle_y + circle_radius * sin(t);
			SDL_RenderDrawLineF(m_pRend, point1_x, point1_y, point2_x, point2_y);
		}

		// Draw LOS
		point1_x = m_playerDest->x + m_playerDest->w / 2;
		point1_y = m_playerDest->y + m_playerDest->h / 2;
		if (m_isLOS)
		{
			SDL_SetRenderDrawColor(m_pRend, 250, 0, 0, 255);
		}
		else
		{
			SDL_SetRenderDrawColor(m_pRend, 0, 0, 250, 255);
		}
		SDL_RenderDrawLineF(m_pRend, point1_x, point1_y, circle_x, circle_y);
	}
	SDL_RenderCopyExF(m_pRend, m_pText, GetSrcP(), GetDstP(), m_angle, 0, static_cast<SDL_RendererFlip>(m_dir));

	if (m_isCollisioned)
	{
		m_eslapsedFromCollision += 10;
		if (m_eslapsedFromCollision > RADIUS)
		{
			m_isCollisioned = false;
		}
		SDL_Rect src = { 0, 343, 96,96 };
		SDL_FRect dest = { GetDstP()->x - 32, GetDstP()->y - 32, GetDstP()->w + 64, GetDstP()->h + 64 };
		SDL_RenderCopyExF(m_pRend, m_pText, &src, &dest, 0, 0, static_cast<SDL_RendererFlip>(m_dir));
	}

	if (m_isDestroying) {
		m_eslapsedFromDestroy += 10;
		if (m_eslapsedFromDestroy > RADIUS) {
			m_isDestroyed = true;
			m_isDestroying = false;
		}
		SDL_Rect src = { (int)(m_eslapsedFromDestroy / 25) * 96, 343, 96,96 };
		SDL_FRect dest = { GetDstP()->x - 32, GetDstP()->y - 32, GetDstP()->w + 64, GetDstP()->h + 64 };
		SDL_RenderCopyExF(m_pRend, m_pText, &src, &dest, 0, 0, static_cast<SDL_RendererFlip>(m_dir));
	}
}

void BaseEnemy::Start()
{
	m_sprite = 0;
	m_accel = 0.2;
}

void BaseEnemy::SetState(int s)
{
	m_state = static_cast<state>(s);
	m_frame = 0;
	if (m_state == idle || m_state == destroyed)
	{
		m_sprite = m_spriteMin = m_spriteMax = 0;
	}
	else // Only other is running for now...
	{
		Start();
		m_sprite = m_spriteMin = 1;
		m_spriteMax = 4;
	}
}

void BaseEnemy::ToggleState()
{
	if (m_state == idle)
	{
		SetState(running);
	}
	else
	{
		SetState(idle);
	}
	m_turnAngle = 0;
}

void BaseEnemy::SetDebugMode(bool enable) {
	m_isDebugEnable = enable;
}

void BaseEnemy::Stop()
{
	m_dx = m_dy = 0.0;
	m_vel = 0;
	m_frame = 0;
	m_frameMax = 4;
	m_sprite = 4;
}

void BaseEnemy::SetVs(const double angle)
{
	double destAngle = MAMA::Rad2Deg(angle) + 90;
	m_angle += std::min(std::max(MAMA::Angle180(destAngle - m_angle), -m_rotMax), m_rotMax); // Only rotate slightly towards the destination angle.
	// Now use the new slight rotation to generate dx and dy as normal.
	m_vel += m_accel;
	m_vel = std::min(m_vel, m_velMax);
}

void BaseEnemy::Move2Stop(const double angle)
{

}

void BaseEnemy::Collision()
{
	if (m_healthLevel <= 0) {
		// Do nothing
	}
	else
	{

		m_healthLevel -= 20;
		if (m_healthLevel <= 0) {
			SOMA::PlaySound("Death", 0, 6);
			//TODO: Death Animation here
			m_isDestroying = true;
			m_eslapsedFromDestroy = 0;
			SetState(destroyed);
			m_healthLevel = 0;
		}
		else {
			SOMA::PlaySound("Boom", 0, 5);
			if (m_healthLevel <= 25)
				Flee();
		}
		m_isCollisioned = true;
		m_eslapsedFromCollision = 0;
	}

}

void BaseEnemy::Flee()
{
	if (m_isDestroyed)
		return;
	m_isFleeing = true;
	int playerX = m_playerDest->x + m_playerDest->w / 2;
	int playerY = m_playerDest->y + m_playerDest->h / 2;
	int m_dir_x = 0;
	int m_dir_y = 0;
	if (m_dst.x < 25 || m_dst.x > 967 || m_dst.y < 27 || m_dst.y > 707) {
		m_isEscaped = true;
	}
	if (playerX < m_dst.x)
	{
		if (!COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, SPEED / 2, 0)) {
			m_dst.x += SPEED / 2;
		}
	}
	else {
		if (!COMA::PlayerCollision({ (int)m_dst.x, (int)m_dst.y, (int)32, (int)32 }, -SPEED / 2, 0)) {
			m_dst.x += -SPEED / 2;
		}
	}
	if (playerY < m_dst.y) {
		if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, SPEED / 2)) {
			m_dst.y += SPEED / 2;
		}
	}
	else {
		if (!COMA::PlayerCollision({ (int)m_dst.x, (int)(m_dst.y), (int)32, (int)32 }, 0, -SPEED / 2)) {
			m_dst.y += -SPEED / 2;
		}
	}
}

bool BaseEnemy::IsDestroyed()
{
	return m_isDestroyed;
}

bool BaseEnemy::IsEsceaped()
{
	return m_isEscaped;
}

bool BaseEnemy::Atack()
{
	return false;
}
