//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine systems
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Audio/AudioSystem.hpp"

//Game systems
#include "Game/GameCommon.hpp"
#include "Game/Geometry.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class Texture;
class BitmapFont;
class SpriteAnimDefenition;
class Image;
class GameCursor;
class Geometry;
class Shader;
struct Camera;
struct IntVec2;

//------------------------------------------------------------------------------------------------------------------------------
class Game
{

public:

	Game();
	~Game();
	
	static bool				TestEvent(EventArgs& args);
	
	void					StartUp();
	void					ShutDown();
	int						GetNextValidGeometryIndex(int index);
	void					DebugEnabled();

	void					HandleKeyPressed( unsigned char keyCode );
	void					HandleKeyReleased( unsigned char keyCode );

	bool					HandleMouseLBDown();
	bool					HandleMouseLBUp();
	bool					HandleMouseRBDown();
	bool					HandleMouseRBUp();
	bool					HandleMouseScroll(float wheelDelta);

	void					Render() const;
	void					RenderOnScreenInfo() const;
	void					RenderAllGeometry() const;
	void					PostRender();

	void					Update( float deltaTime );
	void					UpdateGeometry( float deltaTime );
	void					UpdateCamera( float deltaTime );
	
	void					ClearGarbageEntities();
	void					CheckCollisions();

	bool					IsAlive();

	static Vec2				GetClientToWorldPosition2D(IntVec2 mousePosInClient, IntVec2 ClientBounds);

	void					ToggleSimType();
	void					ChangeCurrentGeometry();

private:

	bool					m_isGameAlive = false;
	bool					m_consoleDebugOnce = false;

public:

	BitmapFont*				m_squirrelFont = nullptr;
	GameCursor*				m_gameCursor = nullptr;
	std::vector<Geometry*>  m_allGeometry;
	Geometry*				m_selectedGeometry = nullptr;
	int						m_selectedIndex;
	float					m_fontHeight = 2.0f;

	float					m_objectMass = 1.0f;
	float					m_objectRestitution = 0.5f;

	float					m_massStep = 0.1f;
	float					m_restitutionStep = 0.1f;

	Camera*					m_mainCamera = nullptr;
	Camera*					m_devConsoleCamera = nullptr;

	Shader*					m_shader = nullptr;
	std::string				m_xmlShaderPath = "default_unlit.xml";

	eGeometryType			m_geometryType = BOX_GEOMETRY;
	bool					m_isStatic = false;

	Vec2					m_mouseStart = Vec2::ZERO;
	Vec2					m_mouseEnd = Vec2::ZERO;

	float					m_zoomLevel = 0.0f;
	float					m_zoomMultiplier = 10.f;
};
