//------------------------------------------------------------------------------------------------------------------------------
#include "Game/Game.hpp"
//Engine Systems
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystems.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/Disc2D.hpp"
#include "Engine/Math/PhysicsSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
//Game systems
#include "Game/GameCursor.hpp"
#include "Game/Geometry.hpp"

//Globals
Rgba* g_clearScreenColor = nullptr;
Camera *g_mainCamera = nullptr; 
float g_shakeAmount = 0.0f;
RandomNumberGenerator* g_randomNumGen;
bool g_debugMode = false;

//Externs
extern RenderContext* g_renderContext;
extern AudioSystem* g_audio;

//------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	m_isGameAlive = true;
	m_squirrelFont = g_renderContext->CreateOrGetBitmapFontFromFile("SquirrelFixedFont");

	g_devConsole->SetBitmapFont(*m_squirrelFont);
	g_randomNumGen = new RandomNumberGenerator();
}

Game::~Game()
{
	m_isGameAlive = false;
}

void Game::StartUp()
{
	g_clearScreenColor = new Rgba(0.f, 0.f, 0.f, 1.f);
	m_gameCursor = new GameCursor();

	Geometry* geometry = new Geometry(*g_physicsSystem, STATIC_SIMULATION, AABB2_GEOMETRY, Vec2(100.f, 10.f), true);
	m_allGeometry.push_back(geometry);
}

STATIC bool Game::TestEvent(EventArgs& args)
{
	UNUSED(args);
	g_devConsole->PrintString(Rgba::YELLOW, "This a test event called from Game.cpp");
	return true;
}

void Game::HandleKeyPressed(unsigned char keyCode)
{
	switch( keyCode )
	{
		case UP_ARROW:
		case RIGHT_ARROW:
		case LEFT_ARROW:
		case DOWN_ARROW:
		m_gameCursor->HandleKeyPressed(keyCode);
		break;
		case SPACE_KEY:
		{
			//Deselect object
			m_selectedGeometry = nullptr;
			m_selectedIndex = -1;
			break;
		}
		case DEL_KEY:
		{
			//Destroy selected object
			delete m_selectedGeometry;
			m_selectedGeometry = nullptr;
			//Set the reference to the object in vector to be nullptr
			m_allGeometry[m_selectedIndex] = nullptr;
			break;
		}
		case TAB_KEY:
		{
			//Select object for possession
			float distMinSq = 200.f;
			m_selectedIndex = 0;
			int numGeometry = static_cast<int>(m_allGeometry.size()) ;
			for(int geometryIndex = 0; geometryIndex < numGeometry; geometryIndex++)
			{
				if(m_allGeometry[geometryIndex] == nullptr)
				{
					continue;
				}

				float distSq = GetDistanceSquared2D(m_gameCursor->GetCursorPositon(), m_allGeometry[geometryIndex]->m_transform.m_position);
				if(distMinSq > distSq)
				{
					distMinSq = distSq;
					m_selectedIndex = geometryIndex;
				}
			}

			//Now select the actual object
			m_selectedGeometry = m_allGeometry[m_selectedIndex];
			m_selectedGeometry->m_rigidbody->SetSimulationMode(STATIC_SIMULATION);
			m_gameCursor->SetCursorPosition(m_selectedGeometry->m_transform.m_position);
			break;
		}
		case A_KEY:
		case N_KEY:
		break;
		case F1_KEY:
		{
			//F1 spawns a static box on the cursor position
			Geometry* geometry = new Geometry(*g_physicsSystem, STATIC_SIMULATION, AABB2_GEOMETRY, m_gameCursor->GetCursorPositon());
			m_allGeometry.push_back(geometry);
		}
		break;
		case F2_KEY:
		{
			//F2 spawns a static disc on the cursor position
			Geometry* geometry = new Geometry(*g_physicsSystem, STATIC_SIMULATION, DISC_GEOMETRY, m_gameCursor->GetCursorPositon());
			m_allGeometry.push_back(geometry);
		}
		break;
		case F3_KEY:
		{
			//F3 spawns a dynamic box on the cursor position
			Geometry* geometry = new Geometry(*g_physicsSystem, DYNAMIC_SIMULATION, AABB2_GEOMETRY, m_gameCursor->GetCursorPositon());
			m_allGeometry.push_back(geometry);
		}
		break;
		case F4_KEY:
		{
			//F4 spawns a dynamic disc on the cursor position
			Geometry* geometry = new Geometry(*g_physicsSystem, DYNAMIC_SIMULATION, DISC_GEOMETRY, m_gameCursor->GetCursorPositon());
			m_allGeometry.push_back(geometry);
		}
		break;
		case F5_KEY:
		break;
		case F6_KEY:
		break;
		case F7_KEY:
		//Quit Debug
		g_eventSystem->FireEvent("Quit");
		break;
		default:
		break;
	}
}

//Function that handles debug mode enabled
void Game::DebugEnabled()
{
	g_debugMode = !g_debugMode;
}


void Game::HandleKeyReleased(unsigned char keyCode)
{

	switch( keyCode )
	{
		case UP_ARROW:
		case DOWN_ARROW:
		case RIGHT_ARROW:
		case LEFT_ARROW:
		m_gameCursor->HandleKeyReleased(keyCode);

		break;
		default:
		break;
	}
}

void Game::Render() const
{
	g_renderContext->BeginFrame();

	g_renderContext->BeginCamera(*g_mainCamera);

	g_renderContext->BindTexture(nullptr);
		
	g_renderContext->ClearScreen(*g_clearScreenColor);

	RenderAllGeometry();

	m_gameCursor->Render();

	g_renderContext->BindTexture(nullptr);

	g_renderContext->EndCamera(*g_mainCamera);

	g_renderContext->EndFrame();

}

void Game::RenderAllGeometry() const
{
	// display debug information
	g_physicsSystem->DebugRender( g_renderContext ); 

	// overwrite the selected object with a white draw; 
	if (m_selectedGeometry != nullptr) 
	{
		m_selectedGeometry->m_rigidbody->DebugRender( g_renderContext, Rgba::WHITE ); 
	}

}

void Game::PostRender()
{
	//Debug bools
	m_consoleDebugOnce = true;
}

//Calls the UpdateShip function in playerShip
void Game::Update( float deltaTime )
{
	UpdateCamera(deltaTime);

	m_gameCursor->Update(deltaTime);

	UpdateGeometry(deltaTime);

	if(m_selectedGeometry != nullptr)
	{
		m_selectedGeometry->m_transform.m_position = m_gameCursor->GetCursorPositon();
		//m_selectedGeometry->m_rigidbody->m_transform.m_position = m_gameCursor->GetCursorPositon();
		//m_selectedGeometry->m_rigidbody->m_object_transform->m_position = m_gameCursor->GetCursorPositon();
	}

	ClearGarbageEntities();	
}

void Game::UpdateGeometry( float deltaTime )
{
	// let physics system play out
	g_physicsSystem->Update(deltaTime);
}

void Game::UpdateCamera(float deltaTime)
{
	g_mainCamera = new Camera();
	Vec2 orthoBottomLeft = Vec2(0.f,0.f);
	Vec2 orthoTopRight = Vec2(WORLD_WIDTH, WORLD_HEIGHT);
	g_mainCamera->SetOrthoView(orthoBottomLeft, orthoTopRight);

	float shakeX = 0.f;
	float shakeY = 0.f;

	if(g_shakeAmount > 0)
	{
		shakeX = g_randomNumGen->GetRandomFloatInRange(-g_shakeAmount, g_shakeAmount);
		shakeY = g_randomNumGen->GetRandomFloatInRange(-g_shakeAmount, g_shakeAmount);

		g_shakeAmount -= deltaTime * CAMERA_SHAKE_REDUCTION_PER_SECOND;
	}
	else
	{
		g_shakeAmount = 0;
	}

	Vec2 translate2D = Vec2(shakeX, shakeY);
	translate2D.ClampLength(MAX_SHAKE);
	g_mainCamera->Translate2D(translate2D);
}



void Game::ClearGarbageEntities()
{
	//Kill any entity off screen
	AABB2 bounds = AABB2(g_mainCamera->GetOrthoBottomLeft(), g_mainCamera->GetOrthoTopRight()); 

	int numGeometry = static_cast<int>(m_allGeometry.size());
	for (int geometryIndex = 0; geometryIndex < numGeometry; geometryIndex++)
	{
		if(m_allGeometry[geometryIndex] == nullptr)
		{
			continue;
		}

		//Kill object if below screen
		if(m_allGeometry[geometryIndex]->m_transform.m_position.y < 0.f)
		{	
			delete m_allGeometry[geometryIndex];
			m_allGeometry[geometryIndex] = nullptr;
		}
	}
}

void Game::CheckCollisions()
{
	//Look for any collisions and call required methods for collision handling
	
}

bool Game::IsAlive()
{
	//Check if alive
	return m_isGameAlive;
}
