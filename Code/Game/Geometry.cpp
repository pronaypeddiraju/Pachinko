//------------------------------------------------------------------------------------------------------------------------------
#include "Game/Geometry.hpp"
//Engine Systems
#include "Engine/Math/PhysicsSystem.hpp"
#include "Engine/Math/Rigidbody2D.hpp"
#include "Engine/Math/Collider2D.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
//Game Systems
#include "Game/GameCommon.hpp"

Geometry::Geometry(const PhysicsSystem& physicsSystem, eSimulationType simulationType, eGeometryType geometryType, const Vec2& cursorPosition)
{
	// setup the physics for htis; 
	PhysicsSystem physics = physicsSystem; 

	// First, give it a rigidbody to represent itself in the physics system
	m_rigidbody = physics.CreateRigidbody();
	m_rigidbody->SetSimulationMode( simulationType ); 

	// give it a shape
	switch( geometryType )
	{
	case TYPE_UNKNOWN:
	ERROR_AND_DIE("Unkown Geometry type");
	break;
	case AABB2_GEOMETRY:
	{
		float thickness = g_randomNumGen->GetRandomFloatInRange(BOX_MIN_WIDTH, BOX_MAX_WIDTH);

		Vec2 minBounds = Vec2(cursorPosition.x - thickness, cursorPosition.y - thickness);
		Vec2 maxBounds = Vec2(cursorPosition.x + thickness, cursorPosition.y + thickness);
		m_collider = m_rigidbody->SetCollider( new AABB2Collider(minBounds, maxBounds) );  
	}
	break;
	case DISC_GEOMETRY:
	{
		float radius = g_randomNumGen->GetRandomFloatInRange(DISC_MIN_RADIUS, DISC_MAX_RADIUS);

		m_collider = m_rigidbody->SetCollider(new Disc2DCollider(cursorPosition, radius));
	}
	break;
	case NUM_GEOMETRY_TYPES:
	ERROR_AND_DIE("Unkown Geometry type");
	break;
	default:
	break;
	}
	
	// give it a for the physics system to affect our object
	m_rigidbody->SetObject( this, &m_transform ); 
}

Geometry::~Geometry()
{

}
