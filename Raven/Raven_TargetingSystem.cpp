#include "Raven_TargetingSystem.h"
#include "Raven_Bot.h"
#include "Raven_SensoryMemory.h"
#include <algorithm>



//-------------------------------- ctor ---------------------------------------
//-----------------------------------------------------------------------------
Raven_TargetingSystem::Raven_TargetingSystem(Raven_Bot* owner):m_pOwner(owner),
                                                               m_pCurrentTarget(0)
{}



//----------------------------- Update ----------------------------------------

//-----------------------------------------------------------------------------
void Raven_TargetingSystem::Update()
{
    m_pCurrentTarget = 0;

    std::list<Raven_Bot* > SensedBots = m_pOwner->GetSensoryMem()->GetListOfRecentlySensedOpponents();

    auto ThreatScore = [&](Raven_Bot* opp)->double 
        {
            if (!opp) return false;

            double recent = m_pOwner->GetSensoryMem()->RecentDamageFrom(opp);

            double recentNorm = recent / 100.0;
            recentNorm = (recentNorm > 1.0 ? 1.0 : (recentNorm < 0.0 ? 0.0 : recentNorm));

            double dist = Vec2DDistance(m_pOwner->Pos(), opp->Pos());
            double prox = 1.0 / (1.0 + dist / 1000.0);

            const double wRecent = 0.99;
            const double wDist = 0.01;

            return wRecent * recentNorm + wDist * prox;
        };

    double best = -1e9;

    for (auto it = SensedBots.begin(); it != SensedBots.end(); ++it)
    {
        Raven_Bot* bot = *it;

        if (bot->isAlive() && bot != m_pOwner)
        {
            double sc = ThreatScore(bot);

            if (sc > best)
            {
                best = sc;
                m_pCurrentTarget = bot;
            }
        }
    }
}


bool Raven_TargetingSystem::isTargetWithinFOV()const
{
  return m_pOwner->GetSensoryMem()->isOpponentWithinFOV(m_pCurrentTarget);
}

bool Raven_TargetingSystem::isTargetShootable()const
{
  return m_pOwner->GetSensoryMem()->isOpponentShootable(m_pCurrentTarget);
}

Vector2D Raven_TargetingSystem::GetLastRecordedPosition()const
{
  return m_pOwner->GetSensoryMem()->GetLastRecordedPositionOfOpponent(m_pCurrentTarget);
}

double Raven_TargetingSystem::GetTimeTargetHasBeenVisible()const
{
  return m_pOwner->GetSensoryMem()->GetTimeOpponentHasBeenVisible(m_pCurrentTarget);
}

double Raven_TargetingSystem::GetTimeTargetHasBeenOutOfView()const
{
  return m_pOwner->GetSensoryMem()->GetTimeOpponentHasBeenOutOfView(m_pCurrentTarget);
}
