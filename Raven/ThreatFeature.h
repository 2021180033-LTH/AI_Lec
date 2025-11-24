#pragma once
#include <math.h>
#include <algorithm>

class Raven_Bot;

namespace ThreatParam {
	static constexpr double RailValue = 1.00;
	static constexpr double RocketValue = 0.90;
	static constexpr double ShotgunValue = 0.85;
	static constexpr double BalsterValue = 0.50;
	static constexpr double UnknownValue = 0.30;

	static constexpr double RAIL_FLAT_BASE = 0.85;
	static constexpr double RAIL_FLAT_GAIN = 0.15;
	static constexpr double RAIL_EXP_D0 = 1500;

	static constexpr double ROCKET_LOGI_K = 0.008;
	static constexpr double ROCKET_LOGI_M = 700.0;

	static constexpr double	SHOT_LOGI_K = 0.035;
	static constexpr double SHOT_LOGI_M = 150.0;

	static constexpr double BLAST_EXP_D0 = 500.0;

	static constexpr double PROX_DMAX = 600.0;
}

struct ThreatFeature {
	static double WeaponThreat(int weaponClass, double dist);
	static double FacingAtMe(const Raven_Bot& me, const Raven_Bot& enemy);
	static double Proximity(const Raven_Bot& me, const Raven_Bot& enemy);
	static double Freshness(double dt, double tau = 1.5);
};