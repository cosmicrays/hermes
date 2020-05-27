#ifndef HERMES_UNITSSIMPLE_H
#define HERMES_UNITSSIMPLE_H

#define HERMES_UNITSDISABLE

#include <array>
#include <cmath>

#include "UnitsSIPrefixes.h"

namespace hermes { namespace units {

typedef double QAMomentum;
typedef double QAcceleration;
typedef double QAngle;
typedef double QAngularMomentum;
typedef double QArea;
typedef double QColumnDensity;
typedef double QMassDensity;
typedef double QDiffCrossSection;
typedef double QDiffFlux;
typedef double QDiffIntensity;
typedef double QDispersionMeasure;
typedef double QECapacitance;
typedef double QECharge;
typedef double QECurrent;
typedef double QEPotential;
typedef double QEResistance;
typedef double QEmissivity;
typedef double QEnergy;
typedef double QEnergyDensity;
typedef double QForce;
typedef double QFrequency;
typedef double QIntensity;
typedef double QInverseEnergy;
typedef double QInverseLength;
typedef double QLIntensity;
typedef double QLength;
typedef double QMField;
typedef double QMass;
typedef double QMomentum;
typedef double QNumber;
typedef double QPDensity;
typedef double QPDensityPerEnergy;
typedef double QPower;
typedef double QPressure;
typedef double QRotationMeasure;
typedef double QSolidAngle;
typedef double QSpectralEnergyDensity;
typedef double QSpeed;
typedef double QSubstance;
typedef double QTemperature;
typedef double QTime;
typedef double QVolume;

typedef double QRMIntegral;
typedef double QSynchroConstant;
typedef double QPiZeroIntegral;
typedef double QRingCOIntensity;
typedef double QRingX0Unit;
typedef double QICInnerIntegral;
typedef double QGREmissivity;
typedef double QEnergyOverTemperature;

constexpr double squared(const QNumber &num) { return num * num; }

}}  // namespace hermes::units

#endif  // HERMES_UNITSSIMPLE_H
