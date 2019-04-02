//------------------------------------------------------------------------------------------------------------------------------
#include "Game/Game.hpp"
//Engine Systems
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystems.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Math/Disc2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/PhysicsSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/RigidBodyBucket.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/ColorTargetView.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Shader.hpp"

//Game systems
#include "Game/GameCursor.hpp"

//Globals
Rgba* g_clearScreenColor = nullptr;
float g_shakeAmount = 0.0f;
RandomNumberGenerator* g_randomNumGen;
bool g_debugMode = false;

eSimulationType g_selectedSimType = STATIC_SIMULATION;

//Extern 
extern RenderContext* g_renderContext;
extern AudioSystem* g_audio;

//------------------------------------------------------------------------------------------------------------------------------

Game::Game()
{
	m_isGameAlive = true;
	m_squirrelFont = g_renderContext->CreateOrGetBitmapFontFromFile("SquirrelFixedFont");

	g_devConsole->SetBitmapFont(*m_squirrelFont);
	g_debugRenderer->SetDebugFont(m_squirrelFont);
	g_randomNumGen = new RandomNumberGenerator();
}

Game::~Game()
{
	m_isGameAlive = false;
	delete m_mainCamera;
	m_mainCamera = nullptr;
}

void Game::StartUp()
{
	//Setup mouse startup values
	IntVec2 clientCenter = g_windowContext->GetClientCenter();
	g_windowContext->SetClientMousePosition(clientCenter);

	g_windowContext->SetMouseMode(MOUSE_MODE_ABSOLUTE);
	g_windowContext->HideMouse();

	g_clearScreenColor = new Rgba(0.f, 0.f, 0.f, 1.f);
	m_gameCursor = new GameCursor();

	//Create the world bounds AABB2
	Vec2 minWorldBounds = Vec2(20.f, -20.f);
	Vec2 maxWorldBounds = Vec2(WORLD_WIDTH, WORLD_HEIGHT) + Vec2(-20.f, 20.f);
	m_worldBounds = AABB2(minWorldBounds, maxWorldBounds);

	//Create the static floor object
	Geometry* geometry = new Geometry(*g_physicsSystem, STATIC_SIMULATION, BOX_GEOMETRY, Vec2(100.f, 10.f), 0.f, 0.f, Vec2(100.f, 10.f), true);
	m_allGeometry.push_back(geometry);

	//Create the Camera and setOrthoView
	m_mainCamera = new Camera();
	m_mainCamera->SetColorTarget(nullptr);

	//Create a devConsole Cam
	m_devConsoleCamera = new Camera();
	m_devConsoleCamera->SetColorTarget(nullptr);

	Vec2 orthoBottomLeft = Vec2(0.f,0.f);
	Vec2 orthoTopRight = Vec2(WORLD_WIDTH, WORLD_HEIGHT);
	m_mainCamera->SetOrthoView(orthoBottomLeft, orthoTopRight);

	//Get the Shader
	m_shader = g_renderContext->CreateOrGetShaderFromFile(m_xmlShaderPath);
	m_shader->SetDepth(eCompareOp::COMPARE_LEQUAL, true);
}

void Game::ShutDown()
{
	delete m_mainCamera;
	m_mainCamera = nullptr;

	delete m_devConsoleCamera;
	m_devConsoleCamera = nullptr;
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
		case W_KEY:
		case S_KEY:
		case A_KEY:
		case D_KEY:
		//m_gameCursor->HandleKeyPressed(keyCode);
		UpdateCameraMovement(keyCode);
		break;		
		case SPACE_KEY:
		{
			ToggleSimType();
			break;
		}
		case DEL_KEY:
		{
			if(m_selectedGeometry == nullptr)
			{
				return;
			}

			//Destroy selected object
			delete m_selectedGeometry;
			m_selectedGeometry = nullptr;
			//Set the reference to the object in vector to be nullptr
			m_allGeometry[m_selectedIndex] = nullptr;
			break;
		}
		case G_KEY:
		{
			ChangeCurrentGeometry();
			break;
		}
		case TAB_KEY:
		{
			break;
		}
		case N_KEY:
		m_objectMass -= m_massStep;
		m_objectMass = Clamp(m_objectMass, 0.1f, 10.f);
		break;
		case M_KEY:
		m_objectMass += m_massStep;
		m_objectMass = Clamp(m_objectMass, 0.1f, 10.f);
		break;
		case KEY_LESSER:
		m_objectRestitution -= m_restitutionStep;
		m_objectRestitution = Clamp(m_objectRestitution, 0.f, 1.f);
		break;
		case KEY_GREATER:
		m_objectRestitution += m_restitutionStep;
		m_objectRestitution  = Clamp(m_objectRestitution, 0.f, 1.f);
		break;
		case F1_KEY:
		{
			break;
		}
		break;
		case F2_KEY:
		{
			//F2 spawns a static disc on the cursor position
			Geometry* geometry = new Geometry(*g_physicsSystem, STATIC_SIMULATION, CAPSULE_GEOMETRY, m_gameCursor->GetCursorPositon());
			geometry->m_rigidbody->m_material.restitution = m_objectRestitution;

			m_allGeometry.push_back(geometry);
		}
		break;
		case F3_KEY:
		{
			//F3 spawns a dynamic box on the cursor position
			Geometry* geometry = new Geometry(*g_physicsSystem, DYNAMIC_SIMULATION, BOX_GEOMETRY, m_gameCursor->GetCursorPositon());
			geometry->m_rigidbody->m_mass = m_objectMass;
			geometry->m_rigidbody->m_material.restitution = m_objectRestitution;

			m_allGeometry.push_back(geometry);
		}
		break;
		case F4_KEY:
		{
			break;
		}
		break;
		case F5_KEY:
		break;
		case F6_KEY:
		break;
		case F7_KEY:
		break;
		default:
		break;
	}
}

int Game::GetNextValidGeometryIndex(int index)
{
	int vectorSize = static_cast<int>(m_allGeometry.size());

	int end = index;
	index = (index + 1) % vectorSize;

	while(index != end)
	{
		if(m_allGeometry[index] != nullptr)
		{
			return index;
		}
		else
		{
			index = (index + 1) % vectorSize;
		}
	}

	return end;
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

bool Game::HandleMouseLBDown()
{
	int numGeometry = static_cast<int>(m_allGeometry.size()) ;

	if(numGeometry == 0)
	{
		return true;
	}

	//Select object for possession
	float distMinSq = 200.f;
	m_selectedIndex = 0;
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
	g_selectedSimType = m_selectedGeometry->m_rigidbody->GetSimulationType();

	m_selectedGeometry->m_rigidbody->SetSimulationMode(STATIC_SIMULATION);
	m_gameCursor->SetCursorPosition(m_selectedGeometry->m_transform.m_position);
	return true;
}

bool Game::HandleMouseLBUp()
{
	/*
	m_allGeometry[m_selectedIndex]->m_rigidbody->SetSimulationMode(g_selectedSimType);
	m_allGeometry[m_selectedIndex]->m_rigidbody->m_velocity = Vec2::ZERO;
	m_allGeometry[m_selectedIndex]->m_rigidbody->m_mass = m_objectMass;
	m_allGeometry[m_selectedIndex]->m_rigidbody->m_material.restitution = m_objectRestitution;

	m_selectedIndex = GetNextValidGeometryIndex(m_selectedIndex);

	//Now select the actual object
	m_selectedGeometry = m_allGeometry[m_selectedIndex];
	g_selectedSimType = m_selectedGeometry->m_rigidbody->GetSimulationType();

	m_selectedGeometry->m_rigidbody->SetSimulationMode(STATIC_SIMULATION);
	m_gameCursor->SetCursorPosition(m_selectedGeometry->m_transform.m_position);
	*/

	if(m_selectedGeometry == nullptr)
	{
		return true;
	}

	//De-select object
	m_selectedGeometry->m_rigidbody->SetSimulationMode(g_selectedSimType);
	m_selectedGeometry->m_rigidbody->m_velocity = Vec2::ZERO;
	m_selectedGeometry->m_rigidbody->m_mass = m_objectMass;
	m_selectedGeometry->m_rigidbody->m_material.restitution = m_objectRestitution;

	m_selectedGeometry = nullptr;
	m_selectedIndex = -1;

	return true;
}

bool Game::HandleMouseRBDown()
{
	m_mouseStart = GetClientToWorldPosition2D(g_windowContext->GetClientMousePosition(), g_windowContext->GetClientBounds());
	return true;
}

bool Game::HandleMouseRBUp()
{

	m_mouseEnd = GetClientToWorldPosition2D(g_windowContext->GetClientMousePosition(), g_windowContext->GetClientBounds());

	Geometry* geometry;

	switch( m_geometryType )
	{
	case TYPE_UNKNOWN:
	break;
	case AABB2_GEOMETRY:
	break;
	case DISC_GEOMETRY:
	break;
	case BOX_GEOMETRY:
	{
		//Calculate Centre from mouse Start and End
		Vec2 disp = m_mouseStart - m_mouseEnd;
		Vec2 norm = disp.GetNormalized();
		float length = disp.GetLength();

		Vec2 center = m_mouseEnd + length * norm * 0.5f;
		float rotationDegrees = disp.GetAngleDegrees() + 90.f;

		if(m_isStatic)
		{
			geometry = new Geometry(*g_physicsSystem, STATIC_SIMULATION, BOX_GEOMETRY, center, rotationDegrees, length);
		}
		else
		{
			geometry = new Geometry(*g_physicsSystem, DYNAMIC_SIMULATION, BOX_GEOMETRY, center, rotationDegrees, length);
		}
		geometry->m_rigidbody->m_material.restitution = m_objectRestitution;
		m_allGeometry.push_back(geometry);

	}
	break;
	case CAPSULE_GEOMETRY:
	{
		if(m_isStatic)
		{
			geometry = new Geometry(*g_physicsSystem, STATIC_SIMULATION, CAPSULE_GEOMETRY, m_mouseStart, 0.f, 0.f, m_mouseEnd);
		}
		else
		{
			geometry = new Geometry(*g_physicsSystem, DYNAMIC_SIMULATION, CAPSULE_GEOMETRY, m_mouseStart, 0.f, 0.f, m_mouseEnd);
		}
		geometry->m_rigidbody->m_mass = m_objectMass;
		geometry->m_rigidbody->m_material.restitution = m_objectRestitution;

		m_allGeometry.push_back(geometry);
	}
	break;
	case NUM_GEOMETRY_TYPES:
	break;
	default:
	break;
	}

	return true;
}

bool Game::HandleMouseScroll(float wheelDelta)
{
	m_zoomLevel += wheelDelta;
	m_zoomLevel = Clamp(m_zoomLevel, 0.00001f, MAX_ZOOM_STEPS);

	//Recalculate mins and maxes based on the zoom level
	float newMinX = 0.f + m_zoomLevel * SCREEN_ASPECT;
	float newMinY = 0.f + m_zoomLevel;

	float newMaxX = WORLD_WIDTH - m_zoomLevel * SCREEN_ASPECT;
	float newMaxY = WORLD_HEIGHT - m_zoomLevel;

	m_mainCamera->SetOrthoView(Vec2(newMinX, newMinY), Vec2(newMaxX, newMaxY));

	return true;
}

void Game::Render() const
{
	//Get the ColorTargetView from rendercontext
	ColorTargetView *colorTargetView = g_renderContext->GetFrameColorTarget();

	//Setup what we are rendering to
	m_mainCamera->SetColorTarget(colorTargetView);
	m_devConsoleCamera->SetColorTarget(colorTargetView);

	g_renderContext->BeginCamera(*m_mainCamera);

	g_renderContext->BindTextureViewWithSampler(0U, nullptr);
		
	g_renderContext->ClearColorTargets(*g_clearScreenColor);

	g_renderContext->BindShader( m_shader );

	RenderAllGeometry();

	RenderWorldBounds();

	RenderOnScreenInfo();

	m_gameCursor->Render();

	g_renderContext->BindTextureViewWithSampler(0U, nullptr);

	g_renderContext->EndCamera();

}

void Game::RenderWorldBounds() const
{
	std::vector<Vertex_PCU> boxVerts;
	AddVertsForBoundingBox(boxVerts, m_worldBounds, Rgba::DARK_GREY, 2.f);
	
	g_renderContext->DrawVertexArray(boxVerts);
}

void Game::RenderOnScreenInfo() const
{
	int staticVectorSize = static_cast<int>(g_physicsSystem->m_rbBucket->m_RbBucket[STATIC_SIMULATION].size());
	int dynamicVectorSize = static_cast<int>(g_physicsSystem->m_rbBucket->m_RbBucket[DYNAMIC_SIMULATION].size());
	int staticCount = 0;
	int dynamicCount = 0;

	Vec2 camMinBounds = m_mainCamera->GetOrthoBottomLeft();
	Vec2 camMaxBounds = m_mainCamera->GetOrthoTopRight();

	for(int staticIndex = 0; staticIndex < staticVectorSize; staticIndex++)
	{
		if(g_physicsSystem->m_rbBucket->m_RbBucket[STATIC_SIMULATION][staticIndex] != nullptr)
		{
			staticCount++;
		}
	}

	for(int dynamicIndex = 0; dynamicIndex < dynamicVectorSize; dynamicIndex++)
	{
		if(g_physicsSystem->m_rbBucket->m_RbBucket[DYNAMIC_SIMULATION][dynamicIndex] != nullptr)
		{
			dynamicCount++;
		}
	}

	std::string printStringStatic = "Number of Static Objects : ";
	printStringStatic += std::to_string(staticCount);

	std::string printStringDynamic = "Number of Dynamic Objects : ";
	printStringDynamic += std::to_string(dynamicCount);

	std::vector<Vertex_PCU> textVerts;
	m_squirrelFont->AddVertsForText2D(textVerts, Vec2(camMinBounds.x + m_fontHeight, camMaxBounds.y - m_fontHeight * 2), m_fontHeight, printStringStatic, Rgba::WHITE);
	m_squirrelFont->AddVertsForText2D(textVerts, Vec2(camMinBounds.x + m_fontHeight, camMaxBounds.y - m_fontHeight * 3), m_fontHeight, printStringDynamic, Rgba::WHITE);

	std::string printStringMassClamp = "Mass Clamped between 0.1 and 10.0";
	m_squirrelFont->AddVertsForText2D(textVerts, Vec2(camMinBounds.x + m_fontHeight, camMaxBounds.y - m_fontHeight * 4), m_fontHeight, printStringMassClamp, Rgba::WHITE);

	std::string printStringMass = "Object Mass (Adjust with N , M) : ";
	printStringMass += std::to_string(m_objectMass);

	m_squirrelFont->AddVertsForText2D(textVerts, Vec2(camMinBounds.x + m_fontHeight, camMaxBounds.y - m_fontHeight * 5), m_fontHeight, printStringMass, Rgba::YELLOW);

	std::string printStringRestitutionClamp = "Restitution Clamped between 0 and 1";
	m_squirrelFont->AddVertsForText2D(textVerts, Vec2(camMinBounds.x + m_fontHeight, camMaxBounds.y - m_fontHeight * 6), m_fontHeight, printStringRestitutionClamp, Rgba::WHITE);

	std::string printStringRestitution = "Object Restitution (Adjust with < , > ) : ";
	printStringRestitution += std::to_string(m_objectRestitution);

	m_squirrelFont->AddVertsForText2D(textVerts, Vec2(camMinBounds.x + m_fontHeight, camMaxBounds.y - m_fontHeight * 7), m_fontHeight, printStringRestitution, Rgba::YELLOW);

	std::string printStringSimType = "Simulation (Space Key) : ";
	if(m_isStatic)
	{
		printStringSimType += "Static";
	}
	else
	{
		printStringSimType += "Dynamic";
	}
	m_squirrelFont->AddVertsForText2D(textVerts, Vec2(camMinBounds.x + m_fontHeight, camMaxBounds.y - m_fontHeight * 9), m_fontHeight, printStringSimType, Rgba::ORANGE);

	std::string printStringObjType = "Geometry (G Key) : ";
	switch( m_geometryType )
	{
	case TYPE_UNKNOWN:
	break;
	case AABB2_GEOMETRY:
	printStringObjType += "AABB2";
	break;
	case DISC_GEOMETRY:
	printStringObjType += "DISC";
	break;
	case BOX_GEOMETRY:
	printStringObjType += "OBB";
	break;
	case CAPSULE_GEOMETRY:
	printStringObjType += "CAPSULE";
	break;
	case NUM_GEOMETRY_TYPES:
	break;
	default:
	break;
	}

	m_squirrelFont->AddVertsForText2D(textVerts, Vec2(camMinBounds.x + m_fontHeight, camMaxBounds.y - m_fontHeight * 10), m_fontHeight, printStringObjType, Rgba::YELLOW);

	//Mouse Debug

	std::string printStringMousePos = "Mouse Position : ";
	printStringMousePos += std::to_string(g_windowContext->GetClientMousePosition().x);
	printStringMousePos += ", ";
	printStringMousePos += std::to_string(g_windowContext->GetClientMousePosition().y);

	m_squirrelFont->AddVertsForText2D(textVerts, Vec2(camMinBounds.x + m_fontHeight, camMinBounds.y + 10.f), m_fontHeight, printStringMousePos, Rgba::WHITE);

	std::string printStringClientBounds = "Client Bounds: ";
	printStringClientBounds += std::to_string(g_windowContext->GetClientBounds().x);
	printStringClientBounds += ", ";
	printStringClientBounds += std::to_string(g_windowContext->GetClientBounds().y);

	m_squirrelFont->AddVertsForText2D(textVerts, Vec2(camMinBounds.x + m_fontHeight, camMinBounds.y + 10.f - m_fontHeight), m_fontHeight, printStringClientBounds, Rgba::WHITE);

	g_renderContext->BindTextureViewWithSampler(0U, m_squirrelFont->GetTexture());
	g_renderContext->DrawVertexArray(textVerts);
	g_renderContext->BindTextureViewWithSampler(0U, nullptr);
}

void Game::RenderAllGeometry() const
{
	// display debug information
	g_physicsSystem->DebugRender( g_renderContext ); 

	/*
	// overwrite the selected object with a white draw; 
	if (m_selectedGeometry != nullptr) 
	{
		if(m_selectedGeometry->m_rigidbody == nullptr)
		{
			return;
		}
		m_selectedGeometry->m_rigidbody->DebugRender( g_renderContext, Rgba::WHITE ); 
	}
	*/
}

void Game::DebugRenderToScreen() const
{
	Camera& debugCamera = g_debugRenderer->Get2DCamera();
	debugCamera.m_colorTargetView = g_renderContext->GetFrameColorTarget();

	g_renderContext->BindShader(m_shader);
	g_renderContext->BeginCamera(debugCamera);

	g_debugRenderer->DebugRenderToScreen();

	g_renderContext->EndCamera();

}

void Game::DebugRenderToCamera() const
{
	Camera& debugCamera3D = *m_mainCamera;
	debugCamera3D.m_colorTargetView = g_renderContext->GetFrameColorTarget();

	g_renderContext->BindShader(m_shader);
	g_renderContext->BeginCamera(debugCamera3D);

	g_debugRenderer->Setup3DCamera(&debugCamera3D);
	g_debugRenderer->DebugRenderToCamera();

	g_renderContext->EndCamera();
}

void Game::PostRender()
{
	//Debug bools
	m_consoleDebugOnce = true;

	if(!m_isDebugSetup)
	{
		//SetStartupDebugRenderObjects();

		ColorTargetView* ctv = g_renderContext->GetFrameColorTarget();
		//Setup debug render client data
		g_debugRenderer->SetClientDimensions( ctv->m_height, ctv->m_width );
		g_debugRenderer->SetWorldSize2D(m_mainCamera->GetOrthoBottomLeft(), m_mainCamera->GetOrthoTopRight());

		m_isDebugSetup = true;
	}

	//All screen Debug information
	DebugRenderToScreen();
}

//Calls the UpdateShip function in playerShip
void Game::Update( float deltaTime )
{
	//UpdateCamera(deltaTime);

	m_gameCursor->Update(deltaTime);

	UpdateGeometry(deltaTime);

	if(m_selectedGeometry != nullptr)
	{
		m_selectedGeometry->m_transform.m_position = m_gameCursor->GetCursorPositon();
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
	m_mainCamera = new Camera();
	Vec2 orthoBottomLeft = Vec2(0.f,0.f);
	Vec2 orthoTopRight = Vec2(WORLD_WIDTH, WORLD_HEIGHT);
	m_mainCamera->SetOrthoView(orthoBottomLeft, orthoTopRight);

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
	m_mainCamera->Translate2D(translate2D);
}


//Function to update movement of the camera within the game
void Game::UpdateCameraMovement( unsigned char keyCode )
{
	switch( keyCode )
	{
	case W_KEY:
	{
		if(m_mainCamera->GetOrthoTopRight().y < m_worldBounds.m_maxBounds.y)
		{
			//Move the camera up
			m_mainCamera->Translate2D(Vec2(0.f, 1.0f));
		}
	}
	break;
	case S_KEY:
	{
		if(m_mainCamera->GetOrthoBottomLeft().y > m_worldBounds.m_minBounds.y)
		{
			//Move the camera down
			m_mainCamera->Translate2D(Vec2(0.f, -1.0f));
		}
	}
	break;
	case A_KEY:
	{
		//For when we are over world bounds
		if(m_mainCamera->GetOrthoTopRight().x > m_worldBounds.m_maxBounds.x)
		{
			//Move the camera left
			m_mainCamera->Translate2D(Vec2(-1.f, 0.0f));
		}

		//For when we are inside the world bounds
		if(m_mainCamera->GetOrthoBottomLeft().x > m_worldBounds.m_minBounds.x)
		{
			//Move the camera left
			m_mainCamera->Translate2D(Vec2(-1.f, 0.0f));
		}
	}
	break;
	case D_KEY:
	{
		//When we are bigger than the world bounds
		if(m_mainCamera->GetOrthoBottomLeft().x < m_worldBounds.m_minBounds.x)
		{
			//Move the camera right
			m_mainCamera->Translate2D(Vec2(1.f, 0.0f));
		}

		//For when we are inside the world bounds
		if(m_mainCamera->GetOrthoTopRight().x < m_worldBounds.m_maxBounds.x)
		{
			//Move the camera right
			m_mainCamera->Translate2D(Vec2(1.f, 0.0f));
		}
	}
	break;
	default:
	break;
	}

}

void Game::ClearGarbageEntities()
{
	//Kill any entity off screen

	for (int geometryIndex = 0; geometryIndex < m_allGeometry.size(); geometryIndex++)
	{
		if(m_allGeometry[geometryIndex] == nullptr)
		{
			continue;
		}

		bool isOffRight = (m_allGeometry[geometryIndex]->m_transform.m_position.x > m_worldBounds.m_maxBounds.x);
		bool isOffLeft = (m_allGeometry[geometryIndex]->m_transform.m_position.x < m_worldBounds.m_minBounds.x);
		bool isOffBottom = (m_allGeometry[geometryIndex]->m_transform.m_position.y < m_worldBounds.m_minBounds.y);
		bool isOffTop = (m_allGeometry[geometryIndex]->m_transform.m_position.y > m_worldBounds.m_maxBounds.y);

		if(isOffRight || isOffLeft || isOffTop || isOffBottom)
		{
			if(m_allGeometry[geometryIndex] == m_selectedGeometry)
			{
				m_selectedGeometry = nullptr;
			}

			delete m_allGeometry[geometryIndex];
			m_allGeometry[geometryIndex] = nullptr;
			m_allGeometry.erase(m_allGeometry.begin() + geometryIndex);
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

STATIC Vec2 Game::GetClientToWorldPosition2D( IntVec2 mousePosInClient, IntVec2 ClientBounds )
{
	Clamp(static_cast<float>(mousePosInClient.x), 0.f, static_cast<float>(ClientBounds.x));
	Clamp(static_cast<float>(mousePosInClient.y), 0.f, static_cast<float>(ClientBounds.y));

	float posOnX = RangeMapFloat(static_cast<float>(mousePosInClient.x), 0.0f, static_cast<float>(ClientBounds.x), 0.f, WORLD_WIDTH);
	float posOnY = RangeMapFloat(static_cast<float>(mousePosInClient.y), static_cast<float>(ClientBounds.y), 0.f, 0.f, WORLD_HEIGHT);

	return Vec2(posOnX, posOnY);
}

void Game::ToggleSimType()
{
	m_isStatic = !m_isStatic;
}

void Game::ChangeCurrentGeometry()
{
	//For future use cases
	
	/*
	int geometryID = m_geometryType;

	if(geometryID < NUM_GEOMETRY_TYPES)
	{
		geometryID++;
	}
	else
	{
		geometryID = 0;
	}

	m_geometryType = eGeometryType(geometryID);
	*/

	//Only for A03
	if(m_geometryType == BOX_GEOMETRY)
	{
		m_geometryType = CAPSULE_GEOMETRY;
	}
	else
	{
		m_geometryType = BOX_GEOMETRY;
	}
}
