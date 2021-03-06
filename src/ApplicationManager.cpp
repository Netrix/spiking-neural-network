#include "ApplicationManager.hpp"
#include "PlotImpulseHandlerProxy.hpp"
#include <iostream>

using namespace NLib;
using namespace NLib::Math;

namespace {

// Framework settings
Framework::FrameworkSettings FRAMEWORK_SETTINGS = { 800, 600, 32 };

// World settings
const float STEP = 1.0f / 60.0f;
const float WORLD_SCALE = 0.2f;

// Sprites
const std::string CAR_SPRITE_IMAGE_PATH = "../../data/car.png";
const std::string BACKGROUND_SPRITE_IMAGE_PATH = "../../data/background.png";

// Tracks
const std::string SIMPLE_TRACK_PATH = "../../data/simple_track.txt";
const std::string COMPLEX_TRACK_PATH = "../../data/complex_track.txt";

// Neural network
const NSize_t NETWORK_SIZE = 4;

}

ApplicationManager::ApplicationManager()
	: m_framework(FRAMEWORK_SETTINGS, WORLD_SCALE)
	, m_fWorldScale(WORLD_SCALE)
	, m_pApplicationController(null)
	, m_bShowPlots(true)
	, m_currentTrackPath(COMPLEX_TRACK_PATH)
	, m_uCurrentLinePlot(0)
	, m_impulsePlotBundle(m_framework)
	, m_linePlot(m_framework, NMVector2fLoad(90.0f, 10.0f),  NMVector2fLoad(60.0f, 50.0f), 160)
	, m_testWorld(m_framework, WORLD_SCALE, STEP)
	, m_spikingNetwork(NETWORK_SIZE, NEURON_INPUT_COUNT, STEP)								// TODO: CHANGE STEP TO 1.0f (if not working of course)
	, m_userController(m_framework, m_testWorld)
	, m_networkController(m_framework, m_testWorld, m_spikingNetwork)
{
	m_framework.setDebugDraw(true);

	for(NSize_t i = 0; i < m_aNeuronImpulseHandlers.size(); ++i)
	{
		m_aNeuronImpulseHandlers[i] = new NeuronImpulseHandler(m_spikingNetwork, i);
	}
}

ApplicationManager::~ApplicationManager()
{
	for(NSize_t i = 0; i < m_aNeuronImpulseHandlers.size(); ++i)
	{
		delete m_aNeuronImpulseHandlers[i];
	}
}

void ApplicationManager::initSprites()
{
	// Loading sprites
	m_carSprite = m_framework.createSprite(CAR_SPRITE_IMAGE_PATH);
	m_backgroundSprite = m_framework.createSprite(BACKGROUND_SPRITE_IMAGE_PATH);

	// Prepar car sprite
	NMVector2f offset = -m_carSprite->getSize() * 0.1f;
	m_carSprite->setOffset(offset);
	m_carSprite->setSize(m_carSprite->getSize() * m_fWorldScale);

	// Prepare backgroudn sprite
	NMVector2f backgroundSize = { 800 * m_fWorldScale, 600 * m_fWorldScale };
	m_backgroundSprite->setSize(backgroundSize);
}

void ApplicationManager::initSpikingNetwork()
{
	m_testWorld.setCarTrackDistanceProbeAHandler(*m_aNeuronImpulseHandlers[0]);
	m_testWorld.setCarTrackDistanceProbeBHandler(*m_aNeuronImpulseHandlers[1]);
	m_testWorld.setLeftDistanceProbeHandler(*m_aNeuronImpulseHandlers[2]);
	m_testWorld.setCarTrackAngleProbeHandler(*m_aNeuronImpulseHandlers[3]);
	m_testWorld.setCarTrackLeftSideProbeHandle(*m_aNeuronImpulseHandlers[4]);
	m_testWorld.setCarTrackRightSideProbeHandle(*m_aNeuronImpulseHandlers[5]);
	m_testWorld.setCarVelocityProbeHandler(*m_aNeuronImpulseHandlers[6]);
}

void ApplicationManager::initPlots()
{
	PlotImpulseHandlerProxyAPtr plotCarTrackDistanceA(new PlotImpulseHandlerProxy(m_framework, NMVector2fLoad(2.0f, 5.0f), NMVector2fLoad(75.0, 10.0f)));
	PlotImpulseHandlerProxyAPtr plotCarTrackDistanceB(new PlotImpulseHandlerProxy(m_framework, NMVector2fLoad(2.0f, 16.0f), NMVector2fLoad(75.0, 10.0f)));
	PlotImpulseHandlerProxyAPtr plotLeftDistance(new PlotImpulseHandlerProxy(m_framework, NMVector2fLoad(2.0f, 27.0f), NMVector2fLoad(75.0, 10.0f)));
	PlotImpulseHandlerProxyAPtr plotCarTrackAngle(new PlotImpulseHandlerProxy(m_framework, NMVector2fLoad(2.0f, 38.0f), NMVector2fLoad(75.0, 10.0f)));
	PlotImpulseHandlerProxyAPtr plotCarTrackLeftSide(new PlotImpulseHandlerProxy(m_framework, NMVector2fLoad(2.0f, 49.0f), NMVector2fLoad(75.0, 10.0f)));
	PlotImpulseHandlerProxyAPtr plotCarTrackRightSide(new PlotImpulseHandlerProxy(m_framework, NMVector2fLoad(2.0f, 60.0f), NMVector2fLoad(75.0, 10.0f)));
	PlotImpulseHandlerProxyAPtr plotCarVelocity(new PlotImpulseHandlerProxy(m_framework, NMVector2fLoad(2.0f, 71.0f), NMVector2fLoad(75.0, 10.0f)));
	PlotImpulseHandlerProxyAPtr plotInputForward(new PlotImpulseHandlerProxy(m_framework, NMVector2fLoad(2.0f, 83.0f), NMVector2fLoad(75.0, 10.0f)));
	PlotImpulseHandlerProxyAPtr plotInputBackward(new PlotImpulseHandlerProxy(m_framework, NMVector2fLoad(82.0f, 83.0f), NMVector2fLoad(75.0, 10.0f)));
	PlotImpulseHandlerProxyAPtr plotInputLeft(new PlotImpulseHandlerProxy(m_framework, NMVector2fLoad(2.0f, 94.0f), NMVector2fLoad(75.0, 10.0f)));
	PlotImpulseHandlerProxyAPtr plotInputRight(new PlotImpulseHandlerProxy(m_framework, NMVector2fLoad(82.0f, 94.0f), NMVector2fLoad(75.0, 10.0f)));

	plotCarTrackDistanceA->setImpulseHandler(m_testWorld.getCarTrackDistanceProbeAHandler());
	plotCarTrackDistanceB->setImpulseHandler(m_testWorld.getCarTrackDistanceProbeBHandler());
	plotLeftDistance->setImpulseHandler(m_testWorld.getLeftDistanceProbeHandler());
	plotCarTrackAngle->setImpulseHandler(m_testWorld.getCarTrackAngleProbeHandler());
	plotCarTrackLeftSide->setImpulseHandler(m_testWorld.getCarTrackLeftSideProbeHandle());
	plotCarTrackRightSide->setImpulseHandler(m_testWorld.getCarTrackRightSideProbeHandle());
	plotCarVelocity->setImpulseHandler(m_testWorld.getCarVelocityProbeHandler());

	m_testWorld.setCarTrackDistanceProbeAHandler(*plotCarTrackDistanceA.get());
	m_testWorld.setCarTrackDistanceProbeBHandler(*plotCarTrackDistanceB.get());
	m_testWorld.setLeftDistanceProbeHandler(*plotLeftDistance.get());
	m_testWorld.setCarTrackAngleProbeHandler(*plotCarTrackAngle.get());
	m_testWorld.setCarTrackLeftSideProbeHandle(*plotCarTrackLeftSide.get());
	m_testWorld.setCarTrackRightSideProbeHandle(*plotCarTrackRightSide.get());
	m_testWorld.setCarVelocityProbeHandler(*plotCarVelocity.get());

	m_testWorld.setForwardImpulseHandler(*plotInputForward.get());
	m_testWorld.setBackwardImpulseHandler(*plotInputBackward.get());
	m_testWorld.setLeftImpulseHandler(*plotInputLeft.get());
	m_testWorld.setRightImpulseHandler(*plotInputRight.get());

	m_impulsePlotBundle.addPlot(Plots::ImpulsePlotAPtr(plotCarTrackDistanceA));
	m_impulsePlotBundle.addPlot(Plots::ImpulsePlotAPtr(plotCarTrackDistanceB));
	m_impulsePlotBundle.addPlot(Plots::ImpulsePlotAPtr(plotLeftDistance));
	m_impulsePlotBundle.addPlot(Plots::ImpulsePlotAPtr(plotCarTrackAngle));
	m_impulsePlotBundle.addPlot(Plots::ImpulsePlotAPtr(plotCarTrackLeftSide));
	m_impulsePlotBundle.addPlot(Plots::ImpulsePlotAPtr(plotCarTrackRightSide));
	m_impulsePlotBundle.addPlot(Plots::ImpulsePlotAPtr(plotCarVelocity));
	m_impulsePlotBundle.addPlot(Plots::ImpulsePlotAPtr(plotInputForward));
	m_impulsePlotBundle.addPlot(Plots::ImpulsePlotAPtr(plotInputBackward));
	m_impulsePlotBundle.addPlot(Plots::ImpulsePlotAPtr(plotInputLeft));
	m_impulsePlotBundle.addPlot(Plots::ImpulsePlotAPtr(plotInputRight));
}

void ApplicationManager::initWorld()
{
	m_testWorld.loadTrack(m_currentTrackPath);
	m_testWorld.reset();
}

bool ApplicationManager::handleApplicationKeys()
{
	// Handling controllers
	bool bReturn = m_pApplicationController->handleKeys();

	static bool sbEscape = false;
	static bool sbP = false;
	static bool sbK = false;

	if(!sbP && m_framework.checkKeyDown(SDLK_p))
	{
		m_bShowPlots = !m_bShowPlots;
	}

	if(!sbK && m_framework.checkKeyDown(SDLK_k))
	{
		if(m_uCurrentLinePlot + 1 >= m_spikingNetwork.getOutputCount())
		{
			setCurrentLinePlot(0);
		}
		else
		{
			setCurrentLinePlot(m_uCurrentLinePlot + 1);
		}
	}

	if(!bReturn)
	{
		// Handling common escape
		if(!sbEscape && m_framework.checkKeyDown(SDLK_ESCAPE))
		{
			if(m_pApplicationController == this)
			{
				m_pApplicationController = null;
				return true;
			}
			else
			{
				setApplicationController(*this);
			}
		}
	}
	sbEscape = m_framework.checkKeyDown(SDLK_ESCAPE);
	sbP = m_framework.checkKeyDown(SDLK_p);

	return bReturn;
}

bool ApplicationManager::handleKeys()
{
	bool bReturn = false;

	static bool sbF1 = false;
	static bool sbF2 = false;
	static bool sbF11 = false;
	static bool sbF12 = false;
	
	if(!bReturn && !sbF1 && m_framework.checkKeyDown(SDLK_F1))
	{
		setApplicationController(m_userController);
		bReturn = true;
	}
	if(!bReturn && !sbF2 && m_framework.checkKeyDown(SDLK_F2))
	{
		setApplicationController(m_networkController);
		bReturn = true;
	}

	// Track
	if(!sbF11 && m_framework.checkKeyDown(SDLK_F11))
	{
		m_currentTrackPath = "../../data/simple_track.txt";
		m_testWorld.loadTrack(m_currentTrackPath);
	}
	if(!sbF12 && m_framework.checkKeyDown(SDLK_F12))
	{
		m_currentTrackPath = "../../data/complex_track.txt";
		m_testWorld.loadTrack(m_currentTrackPath);
	}

	sbF1 = m_framework.checkKeyDown(SDLK_F1);
	sbF2 = m_framework.checkKeyDown(SDLK_F2);
	sbF11 = m_framework.checkKeyDown(SDLK_F11);
	sbF12 = m_framework.checkKeyDown(SDLK_F12);

	return bReturn;
}

void ApplicationManager::fixedStepUpdate()
{
}

void ApplicationManager::setApplicationController(IApplicationController& pController)
{
	m_pApplicationController = &pController;
	m_pApplicationController->initController();
}

void ApplicationManager::initController()
{
	m_framework.setWindowTitle("ApplicationManager choice menu");
	
	std::cout << std::endl
		<< "ApplicationManager choice menu:" << std::endl
		<< " [ F1 ] - User controlled simulation" << std::endl
		<< " [ F2 ] - SNN controlled simulation" << std::endl
		<< " [ F3 ] - SNN learning mode" << std::endl
		<< " [ F11 ] - Load simple track" << std::endl
		<< " [ F12 ] - Load complex track" << std::endl
		<< " [ ESC ] - Quit application" << std::endl
		<< std::endl;
}

void ApplicationManager::draw()
{
	m_testWorld.draw(*m_carSprite, *m_backgroundSprite);

	if(m_bShowPlots)
	{
		m_impulsePlotBundle.draw();
		m_linePlot.draw();
	}
}

void ApplicationManager::run()
{
	initSprites();
	initSpikingNetwork();
	initPlots();
	initWorld();

	setApplicationController(*this);

	// Application loop
	float fFixedTimer = 0.0f;
	while(m_pApplicationController != null && m_framework.update())
	{
		// Drawing
		draw();

		// Handling application keys
		if(handleApplicationKeys())
		{
			continue;
		}

		// Fixed time loop
		fFixedTimer += m_framework.getTimeDelta();
		if(fFixedTimer > 2.0f)
		{
				fFixedTimer = 0.0f;
		}

		while(fFixedTimer > STEP)
		{
			fFixedTimer -= STEP;

			// Update with fixed step
			m_pApplicationController->fixedStepUpdate();
			m_impulsePlotBundle.update(STEP);
			m_linePlot.pushValue(m_spikingNetwork.getNeuron(m_uCurrentLinePlot).getValue());
		}

		m_framework.flipScreen();
	}
}

void ApplicationManager::setCurrentLinePlot(NLib::NSize_t uPlotIndex)
{
	m_uCurrentLinePlot = uPlotIndex;
	m_linePlot.reset();
}

