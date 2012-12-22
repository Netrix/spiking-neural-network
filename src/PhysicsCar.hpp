/* http://www.iforce2d.net/b2dtut/top-down-car
*/
#ifndef SNN_PHYSICS_CAR
#define SNN_PHYSICS_CAR

#include <NLib/Math/nMath.hpp>
#include <NLib/Base/nNonCopyable.hpp>
#include <Box2D/Box2D.h>
#include <array>
#include "PhysicsTire.hpp"

#ifndef DEGTORAD
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#endif

class PhysicsCar 
{
	static const NLib::NUint32 NUM_TIRES = 4;

public:
    PhysicsCar(b2World& world);

    ~PhysicsCar();

	void setTransform(const b2Vec2& pos, float fAngle= 0.0f);

	NLib::Math::NMVector2f getPosition() const;

    void update(int controlState);

	b2Body* getBody()				{ return m_body; }
	const b2Body* getBody()	const	{ return m_body; }

private:
    b2Body* m_body;
	std::array<PhysicsTire*, NUM_TIRES> m_tires;
    b2RevoluteJoint *flJoint, *frJoint;
};

#endif // SNN_PHYSICS_CAR