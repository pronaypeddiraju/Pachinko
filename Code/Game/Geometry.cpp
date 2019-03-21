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

			minBounds = Vec2(-thickness, -thickness);
			maxBounds = Vec2(thickness, thickness);
		}
		else
		{
			thickness = 80.f;
			float height = 10.f;
		
			minBounds = Vec2(-thickness, -height);
			maxBounds = Vec2(thickness, height);
		}

		
		m_collider = m_rigidbody->SetCollider( new AABB2Collider(minBounds, maxBounds) );  
		m_collider->m_colliderType = COLLIDER_AABB2;
		m_collider->m_rigidbody = m_rigidbody;
	}
	break;
	case DISC_GEOMETRY:
	{
		float radius = g_randomNumGen->GetRandomFloatInRange(DISC_MIN_RADIUS, DISC_MAX_RADIUS);

		m_collider = m_rigidbody->SetCollider(new Disc2DCollider(Vec2::ZERO, radius));
		m_collider->m_colliderType = COLLIDER_DISC;
		m_collider->m_rigidbody = m_rigidbody;
	}
	break;
	case BOX_GEOMETRY:
	{
		Vec2 size;
		float rotationDegrees = 0.f;

		if(!staticFloor)
		{
			size = Vec2(g_randomNumGen->GetRandomFloatInRange(BOX_MIN_WIDTH, BOX_MAX_WIDTH), g_randomNumGen->GetRandomFloatInRange(BOX_MIN_WIDTH, BOX_MAX_WIDTH));
			
			rotationDegrees = g_randomNumGen->GetRandomFloatInRange(0.f, 360.f);
			
			//For easy debugging let's make it 45 degrees
			//rotationDegrees = 0.f;
		}
		else
		{
			size = Vec2(80.f, 10.f);
		}

		m_collider = m_rigidbody->SetCollider( new BoxCollider2D(Vec2::ZERO, size, rotationDegrees) );  
		m_collider->m_colliderType = COLLIDER_BOX;
		m_collider->m_rigidbody = m_rigidbody;
	}
	break;
	case CAPSULE_GEOMETRY:
	{
		float radius = g_randomNumGen->GetRandomFloatInRange(DISC_MIN_RADIUS, DISC_MAX_RADIUS);

		m_collider = m_rigidbody->SetCollider( new CapsuleCollider2D(cursorPosition, cursorPosition + Vec2(0.f, -10.0f), radius) );  
		m_collider->m_colliderType = COLLIDER_CAPSULE;
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
	delete m_rigidbody;
	m_rigidbody = nullptr;
}
