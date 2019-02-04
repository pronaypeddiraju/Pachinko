//------------------------------------------------------------------------------------------------------------------------------
#include "Game/Geometry.hpp"
//Engine Systems
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2D.hpp"
//------------------------------------------------------------------------------------------------------------------------------

Geometry::Geometry( AABB2& box, const Vec2& centre)
{
	m_boxGeometry = &box;
	m_geoCentre = centre;
}

Geometry::Geometry( Disc2D& disc, const Vec2& centre)
{
	m_discGeometry = &disc;
	m_geoCentre = centre;
}

Geometry::~Geometry()
{

}

AABB2* Geometry::GetBox()
{
	return m_boxGeometry;
}

Disc2D* Geometry::GetDisc()
{
	return m_discGeometry;
}

STATIC std::vector<Geometry*> Geometry::s_allGeometry;

