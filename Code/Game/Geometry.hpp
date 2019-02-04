#pragma once
//------------------------------------------------------------------------------------------------------------------------------
#include <vector>
//Engine Systems
#include "Engine/Math/Vec2.hpp"
//------------------------------------------------------------------------------------------------------------------------------
struct AABB2;
class Disc2D;
//------------------------------------------------------------------------------------------------------------------------------

enum eGeometryType
{
	TYPE_UNKNOWN = -1,

	TYPE_BOX,
	TYPE_DISC,

	NUM_GEOMETRY_TYPES
};

enum ePhysicsType
{
	TYPE_UNKOWN = -1,

	TYPE_STATIC,
	TYPE_DYNAMIC,

	NUM_PHYSICS_TYPES
};

class Geometry
{
public:
	explicit Geometry( AABB2& box, const Vec2& centre);
	explicit Geometry( Disc2D& disc, const Vec2& centre);
	~Geometry();

	AABB2*				GetBox();
	Disc2D*				GetDisc();

	static std::vector<Geometry*>	s_allGeometry;

public:
	eGeometryType		m_geoType = TYPE_UNKNOWN;
	ePhysicsType		m_physicsType = TYPE_UNKOWN;

private:
	AABB2*				m_boxGeometry = nullptr;
	Disc2D*				m_discGeometry = nullptr;

	Vec2				m_geoCentre = Vec2::ZERO;
};
