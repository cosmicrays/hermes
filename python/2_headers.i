/* 2: CRPropa headers and Python extensions */

/* Python slots */
%feature("python:slot", "sq_length", functype="lenfunc") __len__;
%feature("python:slot", "mp_subscript", functype="binaryfunc") __getitem__;
%feature("python:slot", "tp_iter", functype="unaryfunc") __iter__;
#ifdef SWIG_PYTHON3
%feature("python:slot", "tp_iternext", functype="iternextfunc") __next__;
#else
%feature("python:slot", "tp_iternext", functype="iternextfunc") next;
#endif

/* Include headers */

%{
#include "hermes/Units.h"
#include "hermes/units/UnitsBasic.h"
#include "hermes/units/UnitsDerived.h"
#include "hermes/units/UnitsLiterals.h"
%}

%ignore operator<<;
%ignore operator>>;
%ignore *::operator=;

%rename(m) operator"" _m(unsigned long long int x);

%include "hermes/Units.h"
%include "hermes/units/UnitsBasic.h"
%include "hermes/units/UnitsDerived.h"
%include "hermes/units/UnitsLiterals.h"

/*
%include "hermes/Vector3.h"
%include "hermes/Vector3Quantity.h"
%include "hermes/Grid.h"
%include "hermes/GridTools.h"
%include "hermes/Random.h"
%include "hermes/HEALPixBits.h"
%include "hermes/FITSWrapper.h"
%include "hermes/ProgressBar.h"
%include "hermes/Common.h"

%include "hermes/skymaps/Skymap.h"
%include "hermes/skymaps/SkymapTemplate.h"
%include "hermes/skymaps/RMSkymap.h"
%include "hermes/skymaps/SynchroSkymap.h"
%include "hermes/skymaps/SynchroSkymapRange.h"

%include "hermes/integrators/Integrator.h"
%include "hermes/integrators/RMIntegrator.h"
%include "hermes/integrators/SynchroIntegrator.h"

%include "hermes/magneticField/MagneticField.h"
%include "hermes/magneticField/MagneticFieldGrid.h"
%include "hermes/magneticField/JF12Field.h"
%include "hermes/magneticField/PT11Field.h"

%include "hermes/gasDensity/GasDensity.h"
%include "hermes/gasDensity/HII_Cordes91.h"

%include "hermes/cosmicRayDensity/CosmicRayDensity.h"
%include "hermes/cosmicRayDensity/SimpleCRDensity.h"
%include "hermes/cosmicRayDensity/DragonCRDensity.h"

%include "hermes/outputs/Output.h"
%include "hermes/outputs/FITSOutput.h"
*/
