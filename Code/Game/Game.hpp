//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine systems
#include "Engine/Math/Vertex_PCU.hpp"
#include "Engine/Audio/AudioSystem.hpp"

//Game systems
#include "Game/GameCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class Texture;
class BitmapFont;
class SpriteAnimDefenition;
class Image;
class GameCursor;
class Geometry;

//------------------------------------------------------------------------------------------------------------------------------
class Game
{

public:

	Game();
	~Game();
	
	static bool				TestEvent(EventArgs& args);
	
	void					StartUp();
	void					HandleKeyPressed( unsigned char keyCode );
	void					DebugEnabled();

	void					HandleKeyReleased( unsigned char keyCode );
	void					Render() const;
	void					RenderAllGeometry() const;
	void					PostRender();

	void					Update( float deltaTime );
	void					UpdateGeometry( float deltaTime );
	void					UpdateCamera( float deltaTime );
	
	void					ClearGarbageEntities();
	void					CheckCollisions();

	bool					IsAlive();

private:

	bool					m_isGameAlive = false;
	bool					m_consoleDebugOnce = false;

public:

	BitmapFont*				m_squirrelFont = nullptr;
	GameCursor*				m_gameCursor = nullptr;
	std::vector<Geometry*>  m_allGeometry;
	Geometry*				m_selectedGeometry = nullptr;
	int						m_selectedIndex;
};
