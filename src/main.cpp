#include <iostream>
#include <vector>
#include <Box2D/Box2D.h>
#include "SpikingNeuron.hpp"
#include "Framework.hpp"

#include "PhysicsCar.hpp"
#include "PhysicsContacts.hpp"


using namespace std;
using namespace NLib::Math;

const float WORLD_SCALE = 0.2f;


int SDL_main(int argc, char* args[])
{
    FrameworkSettings settings = { 800, 600, 32 };
    Framework game(settings, WORLD_SCALE);

    MyDestructionListener destructionListener;

	b2World& world = game.getPhysicsWorld();
	
	game.setDebugDraw(true);
    world.SetDestructionListener(&destructionListener);

    b2Body* groundBody = null;

    //set up ground areas
    //{
    //    b2BodyDef bodyDef;
    //    groundBody = world.CreateBody( &bodyDef );

    //    b2PolygonShape polygonShape;
    //    b2FixtureDef fixtureDef;
    //    fixtureDef.shape = &polygonShape;
    //    fixtureDef.isSensor = true;

    //    polygonShape.SetAsBox( 9, 7, b2Vec2(-10,15), 20*DEGTORAD );
    //    b2Fixture* groundAreaFixture = groundBody->CreateFixture(&fixtureDef);
    //    groundAreaFixture->SetUserData( new GroundAreaFUD( 0.5f, false ) );

    //    polygonShape.SetAsBox( 9, 5, b2Vec2(5,20), -40*DEGTORAD );
    //    groundAreaFixture = groundBody->CreateFixture(&fixtureDef);
    //    groundAreaFixture->SetUserData( new GroundAreaFUD( WORLD_SCALE, false ) );
    //}

    //PhysicsCar physicsCar(&world, 5.0f);
	PhysicsCar physicsCar(&world);
	physicsCar.setPosition(b2Vec2(126 * WORLD_SCALE, 578 * WORLD_SCALE) , 180.0f * DEGTORAD);

    int controlState = 0;

    SpriteAPtr car = game.createSprite("../../data/car.png");
    SpriteAPtr background = game.createSprite("../../data/background.png");

    // Background sprite
    NMVector2f backgroundSize = { 800 * WORLD_SCALE, 600 * WORLD_SCALE};
    background->setSize(backgroundSize);

    // Offset of car
    NMVector2f offset = -car->getSize() * 0.1f;
    car->setOffset(offset);
	car->setSize(car->getSize() * WORLD_SCALE);

    // Car position
    NMVector2f carPos = NMVector2fLoad(126, 578);

    typedef std::vector<NMVector2f> PositionVector;
    PositionVector vPositions;
    vPositions.push_back(carPos);

    NLib::NSize_t uIndex = 0;
    float fProcess = 0.0f;
    float fSpeed = 1.0f;

    float fAngle = 0.0f;
    while(game.update())
    {
		b2Vec2 physCarPos = physicsCar.getBody()->GetWorldCenter();
		fAngle = physicsCar.getBody()->GetAngle() * RADTODEG + 180.0f;

        game.drawSprite(0, 0, 0.0f, *background.get());
        game.drawSprite(physCarPos.x, physCarPos.y, fAngle, *car.get());



        if(game.isMouseButtonLeftClicked())
        {
            NMVector2f mousePos = game.getMouseCoords();

            if(NMVector2fLength(vPositions.back() - mousePos) > 0.001f)
            {
                vPositions.push_back(mousePos);
            }
        }

        if(uIndex < vPositions.size() - 1)
        {
            if(fProcess < 1.0f)
            {
                NMVector2f previous = vPositions[uIndex];
                NMVector2f next = vPositions[uIndex + 1];

                NMVector2f direction = NMVector2fNormalize(next - previous);
                float fLength = NMVector2fLength(next - previous);

                fProcess += fSpeed / fLength;
                carPos += direction * fSpeed;

                fAngle = acosf(NMVector2fDot(NMVector2fLoad(0.0f, -1.0f), direction));
                fAngle = (fAngle * 360.0f) / (2 * NM_PI_F);

                if(direction.x < 0.0f)
                {
                    fAngle = -fAngle;
                }

                std::cout << carPos.x << " " << carPos.y << " " << fLength << " " << uIndex << " "  << vPositions.size() << std::endl;
            }
            else
            {
                uIndex++;
                fProcess = 0.0f;
            }
        }

		controlState = 0;
		controlState |= game.checkKeyDown(SDLK_w) ? TDC_UP : 0;
		controlState |= game.checkKeyDown(SDLK_s) ? TDC_DOWN : 0;
		controlState |= game.checkKeyDown(SDLK_a) ? TDC_RIGHT : 0;
		controlState |= game.checkKeyDown(SDLK_d) ? TDC_LEFT : 0;

		physicsCar.update(controlState);

		game.physicsStep(60.0f);

//		printf("car velocity: %f\n", physicsCar.getBody()->GetLinearVelocity());
//		printf("force %f\n", m_currentTraction * force * currentForwardNormal);
//		printf("velocity %f\n", m_body->GetLinearVelocity());

        game.flipScreen();
    }


    //SpikingNeuron sn(2);
    //sn.setDecayTime(2.0);
    //sn.setRelaxation(5.0);
    //sn.setThreshold(10.0);

    //sn.setInputWeight(0, -5.0);
    //sn.setInputDecay(0, 5.0);

    //sn.setInputWeight(1, 5.0);
    //sn.setInputDecay(1, 5.0);

    //std::vector<real> temp;
    //temp.resize(sn.getParametersCount());

    //sn.getParameters(&temp[0]);

    //for(int i = 0; i < 100; ++i)
    //{
    //	if(i == 10)
    //	{
    //		sn.handleInput(0);
    //	}
    //	if(i > 10 && (i % 4) == 0)
    //	{
    //		sn.handleInput(1);
    //	}	

    //	sn.process();
    //	cout << sn.getValue() << endl;
    //}

    //iforce2d_TopdownCar testbed;
    //testbed.

    //SDL_S* pHelloSurface = null;

    return 0;
}