#include "Raven_TargetingSystem.h"
#include "Raven_Bot.h"
#include "Raven_SensoryMemory.h"
#include <algorithm>

#include "goals/Raven_Feature.h"



//-------------------------------- ctor ---------------------------------------
//-----------------------------------------------------------------------------
Raven_TargetingSystem::Raven_TargetingSystem(Raven_Bot* owner):m_pOwner(owner),
                                                               m_pCurrentTarget(0)
{}



//----------------------------- Update ----------------------------------------

//-----------------------------------------------------------------------------
void Raven_TargetingSystem::Update()
{
    double MaxThreat = -1.0;
    Raven_Bot* pBestTarget = 0;

    std::list<Raven_Bot*> SensedBots = m_pOwner->GetSensoryMem()->GetListOfRecentlySensedOpponents();

    for (std::list<Raven_Bot*>::iterator curBot = SensedBots.begin(); curBot != SensedBots.end(); ++curBot)
    {
        if ((*curBot)->isAlive() && (*curBot != m_pOwner))
        {
            double w_weapon = Raven_Feature::WeaponThreat(m_pOwner, *curBot);
            double w_facing = Raven_Feature::FacingAtMe(m_pOwner, *curBot);
            double w_prox = Raven_Feature::Proximity(m_pOwner, *curBot);
            double w_damage = Raven_Feature::RecentDamageFrom(m_pOwner, *curBot);

            double Threat = (0.4 * w_weapon) + (0.3 * w_facing) + (0.2 + w_prox) + (0.1 * w_damage);

            if (Threat > MaxThreat)
            {
                MaxThreat = Threat;
                pBestTarget = *curBot;
            }
        }
    }

    m_pCurrentTarget = pBestTarget;
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
