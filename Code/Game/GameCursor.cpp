//------------------------------------------------------------------------------------------------------------------------------
#include "Game/GameCursor.hpp"
//Engine Systems
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//------------------------------------------------------------------------------------------------------------------------------

GameCursor::GameCursor()
{
	StartUp();
}

GameCursor::~GameCursor()
{

}

void GameCursor::StartUp()
{
	m_cursorPosition = Vec2(WORLD_CENTER_X, WORLD_CENTER_Y);
}

void GameCursor::Update( float deltaTime )
{
	m_cursorPosition += m_movementVector * m_cursorSpeed;
}

void GameCursor::Render() const
{
	std::vector<Vertex_PCU> ringVerts;
	AddVertsForRing2D(ringVerts, m_cursorPosition, m_cursorRingRadius, m_cursorThickness, m_cursorColor);

	std::vector<Vertex_PCU>	lineVerts;
	Vec2 vertLineOffset = Vec2(0.f, m_cursorRingRadius);
	Vec2 horLineOffset = Vec2(m_cursorRingRadius, 0.f);

	AddVertsForLine2D(lineVerts, m_cursorPosition - vertLineOffset, m_cursorPosition + vertLineOffset, m_cursorThickness, m_cursorColor);
	AddVertsForLine2D(lineVerts, m_cursorPosition - horLineOffset, m_cursorPosition + horLineOffset, m_cursorThickness, m_cursorColor);

	g_renderContext->BindTexture(nullptr);
	g_renderContext->DrawVertexArray(lineVerts);
	g_renderContext->DrawVertexArray(ringVerts);
}

void GameCursor::HandleKeyPressed( unsigned char keyCode )
{
	switch( keyCode )
	{
		case UP_ARROW:
		m_movementVector.y = 1.f;
		break;
		case DOWN_ARROW:
		m_movementVector.y = -1.f;
		break;
		case RIGHT_ARROW:
		m_movementVector.x = 1.f;
		break;
		case LEFT_ARROW:
		m_movementVector.x = -1.f;
		break;
	}
}

void GameCursor::HandleKeyReleased( unsigned char keyCode )
{
	switch( keyCode )
	{
		case UP_ARROW:
		case DOWN_ARROW:
		m_movementVector.y = 0.f;
		break;
		case RIGHT_ARROW:
		case LEFT_ARROW:
		m_movementVector.x = 0.f;
		break;
	}
}

