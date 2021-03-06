#include "PassageEvaluator.hpp"

using namespace NLib::Math;

namespace {
	const float CHECK_DELTA = 0.5f;
	const float PENALTY_MULTIPLIER = 10.0f;
	const float OUTROAD_PENALTY = 1000.0f * PENALTY_MULTIPLIER;
}

namespace Simulation {

PassageEvaluator::PassageEvaluator(Track& track)
	: m_track(track)
	, m_fTime(0.0f)
	, m_fPoints(0.0f)
	, m_bRunning(false)
{
}

void PassageEvaluator::start()
{
	m_fTime = 0.0f;
	m_fPoints = 0.0f;
	m_bRunning = true;
}

void PassageEvaluator::update(float fDelta)
{
	if(m_bRunning)
	{
		m_fTime += fDelta;

		m_fPoints += evaluateCarTrackDistance() * fDelta;
		m_fPoints += (m_track.getTrackLength() - m_track.getTravelledDistance()) * m_fTime * PENALTY_MULTIPLIER;
		m_fPoints += NMVector2fLength(m_track.getCurrentPointOnTrack() - m_track.getCurrentPosition()) * m_fTime * PENALTY_MULTIPLIER;

		if(m_track.getTravelledDistance() >= m_track.getTrackLength())
		{
			m_bRunning = false;
		}
	}
}

float PassageEvaluator::evaluateCarTrackDistance() const
{
	float fCarTrackDistance = m_track.getCurrentDistanceFromTrack();

	return fCarTrackDistance > m_track.getTrackWidth() ? fCarTrackDistance * OUTROAD_PENALTY : fCarTrackDistance;
}

} // Simulation 
