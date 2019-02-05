//------------------------------------------------------------------------------------------------------------------------------
#include "Game/Geometry.hpp"
//Engine Systems
#include "Engine/Math/PhysicsSystem.hpp"
#include "Engine/Math/Rigidbody2D.hpp"
#include "Engine/Math/Collider2D.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
//Game Systems
#include "Game/GameCommon.hpp"

Geometry::Geometry(PhysicsSystem& physicsSystem, eSimulationType simulationType, eGeometryType geometryType, const Vec2& cursorPosition, bool staticFloor)
{
	// First, give it a rigidbody to represent itself in the physics system
	m_rigidbody = physicsSystem.CreateRigidbody(simulationType);
	m_rigidbody->SetSimulationMode( simulationType ); 

	m_transform.m_position = cursorPosition;

	// give it a shape
	switch( geometryType )
	{
	case TYPE_UNKNOWN:
	ERROR_AND_DIE("Unkown Geometry type");
	break;
	case AABB2_GEOMETRY:
	{
		float thickness;
		Vec2 minBounds;
		Vec2 maxBounds;

		if(!staticFloor)
		{
			thickness = g_randomNumGen->GetRandomFloatInRange(BOX_MIN_WIDTH, BOX_MAX_WIDTH);

			minBounds = Vec2(cursorPosition.x - thickness, cursorPosition.y - thickness);
			maxBounds = Vec2(cursorPosition.x + thickness, cursorPosition.y + thickness);
		}
		else
		{
			thickness = 80.f;
			float height = 10.f;

			minBounds = Vec2(cursorPosition.x - thickness, cursorPosition.y - height);
			maxBounds = Vec2(cursorPosition.x + thickness, cursorPosition.y + height);
		}

		
		m_collider = m_rigidbody->SetCollider( new AABB2Collider(minBounds, maxBounds) );  
		m_collider->m_colliderType = COLLIDER_AABB2;
		m_collider->m_rigidbody = m_rigidbody;
	}
	break;
	case DISC_GEOMETRY:
	{
		float radius = g_randomNumGen->GetRandomFloatInRange(DISC_MIN_RADIUS, DISC_MAX_RADIUS);

		m_collider = m_rigidbody->SetCollider(new Disc2DCollider(cursorPosition, radius));
		m_collider->m_colliderType = COLLIDER_DISC;
		m_collider->m_rigidbody = m_rigidbody;
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
	physicsSystem.AddRigidbodyToVector(m_rigidbody);
}

Geometry::~Geometry()
{

}
