#include "States.h"
#include "StateManager.h" // Make sure this is NOT in "States.h" or circular reference.
#include "Engine.h"
#include "Button.h"
#include <iostream>
#include <fstream>

#include "CollisionManager.h"
#include "DebugManager.h"
#include "EventManager.h"
#include "FontManager.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include "PathManager.h"
#include <ctime>

using namespace std;
// Begin State. CTRL+M+H and CTRL+M+U to turn on/off collapsed code.
void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}
void State::Resume() {}
// End State.


// Begin GameState
GameState::GameState() {}

void GameState::Enter()
{
	std::cout << "Entering GameState..." << std::endl;
	srand((unsigned)time(NULL));
	m_pTileText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Tiles.png");
	m_pPlayerText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Maga.png");
	m_pEnemyText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Enemies.png");
	m_pCircleText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Circle.png");
	m_pBulletText = IMG_LoadTexture(Engine::Instance().GetRenderer(), "Img/Enemies.png");

	SOMA::Load("Aud/adventure.wav", "adventure", SOUND_MUSIC);
	SOMA::Load("Aud/boom.wav", "Boom", SOUND_SFX);
	SOMA::Load("Aud/death.wav", "Death", SOUND_SFX);
	SOMA::Load("Aud/Click.wav", "Click", SOUND_SFX);
	SOMA::Load("Aud/PressM.wav", "PressM", SOUND_SFX);
	SOMA::Load("Aud/PressR.wav", "PressR", SOUND_SFX);
	SOMA::Load("Aud/PressF.wav", "PressF", SOUND_SFX);
	SOMA::Load("Aud/PressH.mp3", "PressH", SOUND_SFX);

	SOMA::SetMusicVolume(15);
	SOMA::PlayMusic("adventure", -1, 3000);

	SDL_Color textColor = { 250, 250, 250, 250 };

	m_pInstruct[0] = new Label("standard", 380, 40, "Press R to restart the play scene", textColor);
	m_pInstruct[1] = new Label("standard", 380, 55, "Press H to toggle the Debug view", textColor);
	m_pInstruct[2] = new Label("standard", 380, 70, "Press P to toggle idle/patrol mode of enemies ", textColor);

	// Add player 
	m_pPlayer = new Player({ 0,0,32,32 }, { (float)(16) * 32, (float)(12) * 32, 32, 32 }, Engine::Instance().GetRenderer(), m_pPlayerText, 0, 0, 0, 4);

	// Add enemies
	m_enemies.push_back(new RangedCombatEnemy({ 0,57,64,99 }, { (float)(19) * 32, (float)(4) * 32, 32, 32 }, Engine::Instance().GetRenderer(), m_pEnemyText, 0, 0, 0, 4, 0.0f, 0, -1, m_pPlayer->GetDstP(), m_pPlayer));
	m_enemies.push_back(new RangedCombatEnemy({ 0,57,64,99 }, { (float)(6) * 32, (float)(10) * 32, 32, 32 }, Engine::Instance().GetRenderer(), m_pEnemyText, 0, 0, 0, 4, 90.0f, 1, 0, m_pPlayer->GetDstP(), m_pPlayer));
	m_enemies.push_back(new CloseCombatEnemy({ 0,0,40,57 }, { (float)(22) * 32, (float)(11) * 32, 32, 32 }, Engine::Instance().GetRenderer(), m_pEnemyText, 0, 0, 0, 4, 180.0f, 0, 1, m_pPlayer->GetDstP(), m_pPlayer));
	m_enemies.push_back(new CloseCombatEnemy({ 0,0,40,57 }, { (float)(23) * 32, (float)(15) * 32, 32, 32 }, Engine::Instance().GetRenderer(), m_pEnemyText, 0, 0, 0, 4, 270.0f, -1, 0, m_pPlayer->GetDstP(), m_pPlayer));

	// Add bullet
	m_pBullet = new Bullet({ 0,439,9,29 }, { (float)(16) * 32, (float)(12) * 32, 32, 32 }, Engine::Instance().GetRenderer(), m_pBulletText, 0, 0, 0, 4);

	ifstream inFile("Dat/Tiledata.txt");
	if (inFile.is_open())
	{ // Create map of Tile prototypes.
		char key;
		int x, y;
		bool o, h;
		while (!inFile.eof())
		{
			inFile >> key >> x >> y >> o >> h;
			m_tiles.emplace(key, new Tile({ x * 32, y * 32, 32, 32 }, { 0,0,32,32 }, Engine::Instance().GetRenderer(), m_pTileText, o, h));
		}
	}
	inFile.close();
	inFile.open("Dat/Level1.txt");
	if (inFile.is_open())
	{ // Build the level from Tile prototypes.
		char key;
		for (int row = 0; row < ROWS; row++)
		{
			for (int col = 0; col < COLS; col++)
			{
				inFile >> key;
				if (m_tiles[key]->IsObstacle())
					BaseEnemy::gameMatrix[row][col] = -1;
				Engine::Instance().GetLevel()[row][col] = m_tiles[key]->Clone(); // Prototype design pattern used.
				Engine::Instance().GetLevel()[row][col]->GetDstP()->x = (float)(32 * col);
				Engine::Instance().GetLevel()[row][col]->GetDstP()->y = (float)(32 * row);
				// Instantiate the labels for each tile.
				Engine::Instance().GetLevel()[row][col]->m_lCost = new Label("tile", Engine::Instance().GetLevel()[row][col]->GetDstP()->x + 4, Engine::Instance().GetLevel()[row][col]->GetDstP()->y + 18, " ", { 0,0,0,255 });
				Engine::Instance().GetLevel()[row][col]->m_lX = new Label("tile", Engine::Instance().GetLevel()[row][col]->GetDstP()->x + 18, Engine::Instance().GetLevel()[row][col]->GetDstP()->y + 2, to_string(col).c_str(), { 0,0,0,255 });
				Engine::Instance().GetLevel()[row][col]->m_lY = new Label("tile", Engine::Instance().GetLevel()[row][col]->GetDstP()->x + 2, Engine::Instance().GetLevel()[row][col]->GetDstP()->y + 2, to_string(row).c_str(), { 0,0,0,255 });
				// Construct the Node for a valid tile.
				if (!Engine::Instance().GetLevel()[row][col]->IsObstacle() && !Engine::Instance().GetLevel()[row][col]->IsHazard())
					Engine::Instance().GetLevel()[row][col]->m_node = new PathNode((int)(Engine::Instance().GetLevel()[row][col]->GetDstP()->x), (int)(Engine::Instance().GetLevel()[row][col]->GetDstP()->y));
			}
		}
	}
	inFile.close();
	// Now build the graph from ALL the non-obstacle and non-hazard tiles. Only N-E-W-S compass points.
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			if (Engine::Instance().GetLevel()[row][col]->Node() == nullptr) // Now we can test for nullptr.
				continue; // An obstacle or hazard tile has no connections.
			// Make valid connections. Inside map and a valid tile.
			if (row - 1 != -1 && Engine::Instance().GetLevel()[row - 1][col]->Node() != nullptr) // Tile above. 
				Engine::Instance().GetLevel()[row][col]->Node()->AddConnection(new PathConnection(Engine::Instance().GetLevel()[row][col]->Node(), Engine::Instance().GetLevel()[row - 1][col]->Node(),
					MAMA::Distance(Engine::Instance().GetLevel()[row][col]->Node()->x, Engine::Instance().GetLevel()[row - 1][col]->Node()->x, Engine::Instance().GetLevel()[row][col]->Node()->y, Engine::Instance().GetLevel()[row - 1][col]->Node()->y)));
			if (row + 1 != ROWS && Engine::Instance().GetLevel()[row + 1][col]->Node() != nullptr) // Tile below.
				Engine::Instance().GetLevel()[row][col]->Node()->AddConnection(new PathConnection(Engine::Instance().GetLevel()[row][col]->Node(), Engine::Instance().GetLevel()[row + 1][col]->Node(),
					MAMA::Distance(Engine::Instance().GetLevel()[row][col]->Node()->x, Engine::Instance().GetLevel()[row + 1][col]->Node()->x, Engine::Instance().GetLevel()[row][col]->Node()->y, Engine::Instance().GetLevel()[row + 1][col]->Node()->y)));
			if (col - 1 != -1 && Engine::Instance().GetLevel()[row][col - 1]->Node() != nullptr) // Tile to Left.
				Engine::Instance().GetLevel()[row][col]->Node()->AddConnection(new PathConnection(Engine::Instance().GetLevel()[row][col]->Node(), Engine::Instance().GetLevel()[row][col - 1]->Node(),
					MAMA::Distance(Engine::Instance().GetLevel()[row][col]->Node()->x, Engine::Instance().GetLevel()[row][col - 1]->Node()->x, Engine::Instance().GetLevel()[row][col]->Node()->y, Engine::Instance().GetLevel()[row][col - 1]->Node()->y)));
			if (col + 1 != COLS && Engine::Instance().GetLevel()[row][col + 1]->Node() != nullptr) // Tile to right.
				Engine::Instance().GetLevel()[row][col]->Node()->AddConnection(new PathConnection(Engine::Instance().GetLevel()[row][col]->Node(), Engine::Instance().GetLevel()[row][col + 1]->Node(),
					MAMA::Distance(Engine::Instance().GetLevel()[row][col]->Node()->x, Engine::Instance().GetLevel()[row][col + 1]->Node()->x, Engine::Instance().GetLevel()[row][col]->Node()->y, Engine::Instance().GetLevel()[row][col + 1]->Node()->y)));
		}
	}
}

void GameState::Update()
{

	// Reset the scene
	if (EVMA::KeyPressed(SDL_SCANCODE_R))
	{
		SOMA::PlaySound("PressR", 0, 1);
		cout << "Reset Game State" << endl;
		STMA::PushState(new GameState);
	}

	if (m_isGameOver)
		return;

	if (EVMA::KeyPressed(SDL_SCANCODE_H))// Toggle debug mode.
	{
		m_showCosts = !m_showCosts; //=> Not support grisp now
		for (auto it = m_enemies.begin(); it != m_enemies.end(); ++it) {
			(*it)->SetDebugMode(m_showCosts);
		}
		SOMA::PlaySound("PressH", 0, 3);
	}

	if (1)
	{
		if (EVMA::KeyPressed(SDL_SCANCODE_SPACE)) // Toggle the heuristic used for pathfinding.
		{
			m_hEuclid = !m_hEuclid;
			std::cout << "Setting " << (m_hEuclid ? "Euclidian" : "Manhattan") << " heuristic..." << std::endl;
		}
		if (EVMA::MousePressed(1) || EVMA::MousePressed(3)) // If user has clicked.
		{

			if (EVMA::MousePressed(1)) // Move the player with left-click.
			{
				//m_pPlayer->GetDstP()->x = (float)(xIdx * 32);
				//m_pPlayer->GetDstP()->y = (float)(yIdx * 32);
				m_pBullet->Shoot({ (int)(m_pPlayer->GetDstP()->x + m_pPlayer->GetDstP()->w / 2) , (int)(m_pPlayer->GetDstP()->y + m_pPlayer->GetDstP()->h / 2) }, m_pPlayer->GetDirection());
				SOMA::PlaySound("Click", 0, 4);
			}

		}
	}
	m_pPlayer->Update(); // Just stops MagaMan from moving.

	for (auto it = m_enemies.begin(); it != m_enemies.end(); ++it) {
		(*it)->Update();
	}

	m_pBullet->Update();

	if (!m_pBullet->isDied())
	{
		for (auto it = m_enemies.begin(); it != m_enemies.end(); ++it) {
			if (COMA::AABBCheck(*m_pBullet->GetDstP(), *((*it)->GetDstP())))
			{
				(*it)->Collision();
				m_pBullet->Collision();
			}

		}
	}
}

void GameState::Render()
{
	// Draw anew.

	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			Engine::Instance().GetLevel()[row][col]->Render(); // Render each tile.
			// Render the debug data...
#if 0
			if (m_showCosts && Engine::Instance().GetLevel()[row][col]->Node() != nullptr)
			{
				Engine::Instance().GetLevel()[row][col]->m_lCost->Render();
				Engine::Instance().GetLevel()[row][col]->m_lX->Render();
				Engine::Instance().GetLevel()[row][col]->m_lY->Render();
				// I am also rendering out each connection in blue. If this is a bit much for you, comment out the for loop below.
				for (unsigned i = 0; i < Engine::Instance().GetLevel()[row][col]->Node()->GetConnections().size(); i++)
				{
					DEMA::QueueLine({ Engine::Instance().GetLevel()[row][col]->Node()->GetConnections()[i]->GetFromNode()->x + 16,Engine::Instance().GetLevel()[row][col]->Node()->GetConnections()[i]->GetFromNode()->y + 16 },
						{ Engine::Instance().GetLevel()[row][col]->Node()->GetConnections()[i]->GetToNode()->x + 16, Engine::Instance().GetLevel()[row][col]->Node()->GetConnections()[i]->GetToNode()->y + 16 }, { 0,0,255,255 });
				}
			}
#endif
		}
	}

	for (int i = 0; i < 3; i++)
	{
		m_pInstruct[i]->Render();
	}

	m_pPlayer->Render();

	if (m_isGameOver) {
		m_GameOverLabel->Render();
		return;
	}

	DrawHealthBar({ (int)m_pPlayer->GetDstP()->x - 16, (int)m_pPlayer->GetDstP()->y - 16, 64,16 }, m_pPlayer->getHealthLevel());

	auto it = m_enemies.begin();
	while (it != m_enemies.end()) {
		if ((*it)->IsDestroyed())
		{
			it = m_enemies.erase(it);
			continue;
		}
		if ((*it)->IsEsceaped()) {
			it = m_enemies.erase(it);
			m_EnemyRecovering = true;
			SDL_Thread* thread = SDL_CreateThread(AddNewEnemy, "AddNewEnemy", this);
			continue;
		}
		(*it)->Render();
		DrawHealthBar({ (int)(*it)->GetDstP()->x - 16, (int)(*it)->GetDstP()->y - 16, 64,16 }, (*it)->getHealthLevel());
		++it;
	}

	if (m_enemies.empty() && !m_EnemyRecovering) {
		GameOver(true);
	}

	if (m_pPlayer->m_isDestroyed) {
		GameOver(false);
	}

	m_pBullet->Render();

	PAMA::DrawPath(); // I save the path in a static vector to be drawn here.
	DEMA::FlushLines(); // And... render ALL the queued lines. Phew.
	if (dynamic_cast<GameState*>(STMA::GetStates().back()))
		State::Render();
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}

void GameState::DrawHealthBar(SDL_Rect rect, int level)
{
	SDL_Rect liveRect;
	liveRect.x = rect.x;
	liveRect.y = rect.y;
	liveRect.w = (rect.w * level) / 100;
	liveRect.h = rect.h;

	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 250, 0, 0, 255);
	SDL_RenderFillRect(Engine::Instance().GetRenderer(), &rect);
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 0, 255, 0, 255);
	SDL_RenderFillRect(Engine::Instance().GetRenderer(), &liveRect);
}

void GameState::Exit()
{
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			delete Engine::Instance().GetLevel()[row][col];
			Engine::Instance().GetLevel()[row][col] = nullptr; // Wrangle your dangle.
		}
	}
	for (auto const& i : m_tiles)
		delete m_tiles[i.first];
	m_tiles.clear();
}

void GameState::Resume() {}

void GameState::GameOver(bool PlayerWin)
{
	SDL_Color textColor = { 200, 0, 0, 0 };
	if (PlayerWin) {
		m_GameOverLabel = new Label("UI", 430, 500, "You Win!!!", textColor);
	}
	else
	{
		m_GameOverLabel = new Label("UI", 430, 500, "You Lose!!!", textColor);
	}
	m_isGameOver = true;
}

void GameState::PushToEnemies(BaseEnemy* enemy)
{
	m_enemies.push_back(enemy);
}

int GameState::AddNewEnemy(void* data)
{
	GameState* self = static_cast<GameState*>(data);
	SDL_Texture* l_texture = self->m_pEnemyText;
	Player* player = self->m_pPlayer;
	srand((unsigned int)time(NULL));
	int x = rand() % 2;
	srand((unsigned int)time(NULL));
	int pos_x = 1 + rand() % 29;
	srand((unsigned int)time(NULL));
	int pos_y = 1 + rand() % 21;
	BaseEnemy* be = nullptr;
	switch (x)
	{
	case 0:
		be = new RangedCombatEnemy({ 0,57,64,99 }, { (float)(pos_x) * 32, (float)(pos_y) * 32, 32, 32 }, Engine::Instance().GetRenderer(), l_texture, 0, 0, 0, 4, 270.0f, -1, 0, player->GetDstP(), player);
		break;
	case 1:
		be = new CloseCombatEnemy({ 0,0,40,57 }, { (float)(pos_x) * 32, (float)(pos_y) * 32, 32, 32 }, Engine::Instance().GetRenderer(), l_texture, 0, 0, 0, 4, 270.0f, -1, 0, player->GetDstP(), player);
		break;
	default:
		break;
	}
	SDL_Delay(2000);
	self->PushToEnemies(be);
	self->m_EnemyRecovering = false;
	return 0;
}

// Begin TitleState
TitleState::TitleState() {}

void TitleState::Enter()
{
	SDL_Color black = { 0, 0, 0, 0 };
	
	m_pNameLabel = new Label("UI", 240, 90, "Mingkun Yang (ID: 101235517)", black);
	m_playBtn = new PlayButton({ 0,0,400,100 }, { 320,320,400,80 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("playBut"));
	m_pGameStart = new Sprite({ 0,0, 700, 600 }, { 0,0,1024, 768 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("StartScene"));
	
	SOMA::Load("Aud/opening.mp3", "Opening", SOUND_MUSIC);
	SOMA::SetMusicVolume(15);
	SOMA::PlayMusic("Opening", -1, 3000);
}
void TitleState::Update()
{
	if (m_playBtn->Update() == 1)
		return;
}

void TitleState::Render()
{

	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 255, 115, 90, 90);
	SDL_RenderClear(Engine::Instance().GetRenderer());

	//m_pStartLabel->Render();
	m_pGameStart->Render();
	m_pNameLabel->Render();
	m_playBtn->Render();
	State::Render();
}


void TitleState::Exit()
{
	std::cout << "Exiting TitleState..." << std::endl;
}
// End GameState