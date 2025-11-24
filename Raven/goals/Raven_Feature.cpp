#include "Raven_Feature.h"
#include "../Raven_Bot.h"
#include "../navigation/Raven_PathPlanner.h"
#include "../armory/Raven_Weapon.h"
#include "../Raven_WeaponSystem.h"
#include "../Raven_ObjectEnumerations.h"
#include "../lua/Raven_Scriptor.h"
#include "../Raven_SensoryMemory.h"

//-----------------------------------------------------------------------------
double Raven_Feature::DistanceToItem(Raven_Bot* pBot, int ItemType)
{
  //determine the distance to the closest instance of the item type
  double DistanceToItem = pBot->GetPathPlanner()->GetCostToClosestItem(ItemType);

  //if the previous method returns a negative value then there is no item of
  //the specified type present in the game world at this time.
  if (DistanceToItem < 0 ) return 1;

  //these values represent cutoffs. Any distance over MaxDistance results in
  //a value of 0, and value below MinDistance results in a value of 1
  const double MaxDistance = 500.0;
  const double MinDistance = 50.0;

  Clamp(DistanceToItem, MinDistance, MaxDistance);

  return DistanceToItem / MaxDistance;
}


//----------------------- GetMaxRoundsBotCanCarryForWeapon --------------------
//
//  helper function to tidy up IndividualWeapon method
//  returns the maximum rounds of ammo a bot can carry for the given weapon
//-----------------------------------------------------------------------------
double GetMaxRoundsBotCanCarryForWeapon(int WeaponType)
{
  switch(WeaponType)
  {
  case type_rail_gun:

    return script->GetDouble("RailGun_MaxRoundsCarried");

  case type_rocket_launcher:

    return script->GetDouble("RocketLauncher_MaxRoundsCarried");

  case type_shotgun:

    return script->GetDouble("ShotGun_MaxRoundsCarried");

  default:

    throw std::runtime_error("trying to calculate  of unknown weapon");

  }//end switch
}


//----------------------- IndividualWeaponStrength ----------------------
//-----------------------------------------------------------------------------
double Raven_Feature::IndividualWeaponStrength(Raven_Bot* pBot,
                                               int        WeaponType)
{
  //grab a pointer to the gun (if the bot owns an instance)
  Raven_Weapon* wp = pBot->GetWeaponSys()->GetWeaponFromInventory(WeaponType);

  if (wp)
  {
    return wp->NumRoundsRemaining() / GetMaxRoundsBotCanCarryForWeapon(WeaponType);
  }

  else
  {
   return 0.0;
  }
}

//--------------------- TotalWeaponStrength --------------
//-----------------------------------------------------------------------------
double Raven_Feature::TotalWeaponStrength(Raven_Bot* pBot)
{
  const double MaxRoundsForShotgun = GetMaxRoundsBotCanCarryForWeapon(type_shotgun);
  const double MaxRoundsForRailgun = GetMaxRoundsBotCanCarryForWeapon(type_rail_gun);
  const double MaxRoundsForRocketLauncher = GetMaxRoundsBotCanCarryForWeapon(type_rocket_launcher);
  const double TotalRoundsCarryable = MaxRoundsForShotgun + MaxRoundsForRailgun + MaxRoundsForRocketLauncher;

  double NumSlugs      = (double)pBot->GetWeaponSys()->GetAmmoRemainingForWeapon(type_rail_gun);
  double NumCartridges = (double)pBot->GetWeaponSys()->GetAmmoRemainingForWeapon(type_shotgun);
  double NumRockets    = (double)pBot->GetWeaponSys()->GetAmmoRemainingForWeapon(type_rocket_launcher);

  //the value of the tweaker (must be in the range 0-1) indicates how much
  //desirability value is returned even if a bot has not picked up any weapons.
  //(it basically adds in an amount for a bot's persistent weapon -- the blaster)
  const double Tweaker = 0.1;

  return Tweaker + (1-Tweaker)*(NumSlugs + NumCartridges + NumRockets)/(MaxRoundsForShotgun + MaxRoundsForRailgun + MaxRoundsForRocketLauncher);
}

//------------------------------- HealthScore ---------------------------------
//
//-----------------------------------------------------------------------------
double Raven_Feature::Health(Raven_Bot* pBot)
{
  return (double)pBot->Health() / (double)pBot->MaxHealth();

}

double Raven_Feature::WeaponThreat(Raven_Bot* pBot, Raven_Bot* pTarget)
{
    if (!pTarget) return 0.0;

    int weapon = pTarget->GetWeaponSys()->GetCurrentWeapon()->GetType();
    double dist = pBot->Pos().Distance(pTarget->Pos());

    double maxThreat = 0.0;
    double dMax = 1000.0;

    switch (weapon) {
    case type_rail_gun:         maxThreat = 1.0; dMax = 1000.0; break;
    case type_rocket_launcher:  maxThreat = 0.9; dMax =  800.0; break;
    case type_shotgun:          maxThreat = 0.8; dMax =  300.0; break;
    default:                    maxThreat = 0.3; dMax =  500.0; break;
    }

    double proximity = 1.0 - (dist / dMax);
    if (proximity < 0.0) proximity = 0.0;

    return maxThreat * proximity;
}

double Raven_Feature::FacingAtMe(Raven_Bot* pBot, Raven_Bot* pTarget)
{
    if (!pTarget) return 0.0;

    Vector2D toMe = pBot->Pos() = pTarget->Pos();
    toMe.Normalize();

    double dot = pTarget->Facing().Dot(toMe);
    return (dot + 1.0) / 2.0;
}

double Raven_Feature::Proximity(Raven_Bot* pBot, Raven_Bot* pTarget)
{
    const double Dmax = 500.0;
    double dist = pBot->Pos().Distance(pTarget->Pos());

    double val = 1.0 - (dist / Dmax);
    if (val < 0.0)val = 0.0;

    return val;
}

double Raven_Feature::RecentDamageFrom(Raven_Bot* pBot, Raven_Bot* pTarget)
{
    return pBot->GetSensoryMem()->RecentDamageFrom(pTarget, 1.5, 100.0);
}