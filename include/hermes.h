#ifndef HERMES_H
#define HERMES_H

#include "hermes/Units.h"
#include "hermes/Vector3.h"
#include "hermes/Vector3Quantity.h"
#include "hermes/Grid.h"
#include "hermes/GridTools.h"
#include "hermes/Random.h"
#include "hermes/HEALPixBits.h"
#include "hermes/FITSWrapper.h"
#include "hermes/ProgressBar.h"
#include "hermes/ParticleID.h"
#include "hermes/Common.h"
#include "hermes/CacheTools.h"
#include "hermes/Signals.h"

#include "hermes/skymaps/Skymap.h"
#include "hermes/skymaps/SkymapMask.h"
#include "hermes/skymaps/SkymapTemplate.h"
#include "hermes/skymaps/DMSkymap.h"
#include "hermes/skymaps/RMSkymap.h"
#include "hermes/skymaps/GammaSkymap.h"
#include "hermes/skymaps/GammaSkymapRange.h"
#include "hermes/skymaps/RadioSkymap.h"
#include "hermes/skymaps/RadioSkymapRange.h"

#include "hermes/integrators/IntegratorTemplate.h"
#include "hermes/integrators/LOSIntegrationMethods.h"
#include "hermes/integrators/DMIntegrator.h"
#include "hermes/integrators/RMIntegrator.h"
#include "hermes/integrators/SynchroIntegrator.h"
#include "hermes/integrators/FreeFreeIntegrator.h"
#include "hermes/integrators/SynchroAbsorptionIntegrator.h"
#include "hermes/integrators/PiZeroIntegrator.h"
#include "hermes/integrators/InverseComptonIntegrator.h"

#include "hermes/magneticfields/MagneticField.h"
#include "hermes/magneticfields/MagneticFieldGrid.h"
#include "hermes/magneticfields/WMAP07Field.h"
#include "hermes/magneticfields/Sun08Field.h"
#include "hermes/magneticfields/PT11Field.h"
#include "hermes/magneticfields/JF12Field.h"

#include "hermes/chargedgas/ChargedGasDensity.h"
#include "hermes/chargedgas/HII_Cordes91.h"
#include "hermes/chargedgas/NE2001Simple.h"
#include "hermes/chargedgas/YMW16.h"

#include "hermes/neutralgas/NeutralGasDensity.h"
#include "hermes/neutralgas/RingModel.h"

#include "hermes/cosmicrays/CosmicRayDensity.h"
#include "hermes/cosmicrays/DummyCRDensity.h"
#include "hermes/cosmicrays/SimpleCRDensity.h"
#include "hermes/cosmicrays/Sun08CRDensity.h"
#include "hermes/cosmicrays/WMAP07CRDensity.h"
#include "hermes/cosmicrays/Dragon2D.h"
#include "hermes/cosmicrays/Dragon3D.h"

#include "hermes/photonfields/PhotonField.h"
#include "hermes/photonfields/CMB.h"
#include "hermes/photonfields/ISRF.h"

#include "hermes/interactions/DifferentialCrossSection.h"
#include "hermes/interactions/DummyCrossSection.h"
#include "hermes/interactions/Kamae06.h"
#include "hermes/interactions/KleinNishina.h"
#include "hermes/interactions/BremsstrahlungSimple.h"

#include "hermes/outputs/Output.h"
#include "hermes/outputs/FITSOutput.h"

#endif // HERMES_H
