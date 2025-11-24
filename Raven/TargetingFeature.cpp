#include "ThreatFeature.h"
#include "Raven_Bot.h"

#include "2D/Vector2D.h"

using namespace ThreatParam;

static double clamp01(double x) {
	return (x < 0.0) ? 0.0 : (x > 1.0 ? 1.0 : x);
}
static double expFalloff(double d, double d0) {
	return std::exp(-d / d0);
}
static double logistic(double d, double k, double m) {
	return 1.0 / (1.0 + std::exp(k * (d - m)));
}


static double fRail(double d)	{
	return clamp01(RAIL_FLAT_BASE + RAIL_FLAT_GAIN * expFalloff(d, RAIL_EXP_D0));
}
static double fRocket(double d) {
	return clamp01(logistic(d, ROCKET_LOGI_K, ROCKET_LOGI_M));
}
static double fShotgun(double d) {
	return clamp01(logistic(d, SHOT_LOGI_K, SHOT_LOGI_M));
}
static double fBlaster(double d) {
	return clamp01(expFalloff(d, BLAST_EXP_D0));
}


double ThreatFeature::WeaponThreat(int weaponClass, double dist)
{
	double base = UnknownValue;
	double fall = fBlaster(dist);

	switch (weaponClass) {
	case 1: base = RailValue;		fall = fRail(dist);		break;
	case 2: base = RocketValue;		fall = fRocket(dist);	break;
	case 3: base = ShotgunValue;	fall = fShotgun(dist);	break;
	case 4: base = BalsterValue;	fall = fBlaster(dist);	break;
	default: break;
	}

	return clamp01(base * fall);
}

double ThreatFeature::FacingAtMe(const Raven_Bot& me, const Raven_Bot& enemy)
{
	Vector2D toMe = me.Pos() - enemy.Pos();
	double len = toMe.Length();
	if (len > 1e-9) toMe /= len; else toMe = Vector2D(0, 0);

	const Vector2D& head = enemy.Heading();
	const double dot = toMe.x * head.x + toMe.y * head.y;

	return clamp01((dot + 1.0) * 0.5);
}

double ThreatFeature::Proximity(const Raven_Bot& me, const Raven_Bot& enemy)
{
	const double d = (me.Pos() - enemy.Pos()).Length();
	return clamp01(1.0 - (d / PROX_DMAX));
}

double ThreatFeature::Freshness(double dt, double tau)
{
	if (dt <= 0.0)return 1.0;
	return clamp01(std::exp(-dt / tau));
}