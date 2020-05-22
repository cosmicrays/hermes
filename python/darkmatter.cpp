#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/darkmatter/DarkMatterSpectrum.h"
#include "hermes/darkmatter/GalacticProfile.h"
#include "hermes/darkmatter/NFWGProfile.h"
#include "hermes/darkmatter/PPPC4DMIDSpectrum.h"

namespace py = pybind11;

namespace hermes { namespace darkmatter {

void init(py::module &m) {
	py::module subm = m.def_submodule("darkmatter");
	subm.doc() = "dark matter package";

	py::enum_<Channel>(subm, "Channel")
	    .value("e", Channel::e)
	    .value("mu", Channel::mu)
	    .value("tau", Channel::tau)
	    .value("q", Channel::q)
	    .value("c", Channel::c)
	    .value("b", Channel::b)
	    .value("t", Channel::t)
	    .value("W", Channel::W)
	    .value("Z", Channel::Z)
	    .value("h", Channel::h);

	py::enum_<Mass>(subm, "Mass", py::arithmetic())
	    .value("m5GeV", Mass::m5GeV)
	    .value("m6GeV", Mass::m6GeV)
	    .value("m8GeV", Mass::m8GeV)
	    .value("m10GeV", Mass::m10GeV)
	    .value("m15GeV", Mass::m15GeV)
	    .value("m20GeV", Mass::m20GeV)
	    .value("m25GeV", Mass::m25GeV)
	    .value("m30GeV", Mass::m30GeV)
	    .value("m40GeV", Mass::m40GeV)
	    .value("m50GeV", Mass::m50GeV)
	    .value("m60GeV", Mass::m60GeV)
	    .value("m70GeV", Mass::m70GeV)
	    .value("m80GeV", Mass::m80GeV)
	    .value("m90GeV", Mass::m90GeV)
	    .value("m100GeV", Mass::m100GeV)
	    .value("m110GeV", Mass::m110GeV)
	    .value("m120GeV", Mass::m120GeV)
	    .value("m130GeV", Mass::m130GeV)
	    .value("m140GeV", Mass::m140GeV)
	    .value("m150GeV", Mass::m150GeV)
	    .value("m160GeV", Mass::m160GeV)
	    .value("m180GeV", Mass::m180GeV)
	    .value("m200GeV", Mass::m200GeV)
	    .value("m220GeV", Mass::m220GeV)
	    .value("m240GeV", Mass::m240GeV)
	    .value("m260GeV", Mass::m260GeV)
	    .value("m280GeV", Mass::m280GeV)
	    .value("m300GeV", Mass::m300GeV)
	    .value("m330GeV", Mass::m330GeV)
	    .value("m360GeV", Mass::m360GeV)
	    .value("m400GeV", Mass::m400GeV)
	    .value("m450GeV", Mass::m450GeV)
	    .value("m500GeV", Mass::m500GeV)
	    .value("m550GeV", Mass::m550GeV)
	    .value("m600GeV", Mass::m600GeV)
	    .value("m650GeV", Mass::m650GeV)
	    .value("m700GeV", Mass::m700GeV)
	    .value("m750GeV", Mass::m750GeV)
	    .value("m800GeV", Mass::m800GeV)
	    .value("m900GeV", Mass::m900GeV)
	    .value("m1TeV", Mass::m1TeV)
	    .value("m1100GeV", Mass::m1100GeV)
	    .value("m1200GeV", Mass::m1200GeV)
	    .value("m1300GeV", Mass::m1300GeV)
	    .value("m1500GeV", Mass::m1500GeV)
	    .value("m1700GeV", Mass::m1700GeV)
	    .value("m2TeV", Mass::m2TeV)
	    .value("m3TeV", Mass::m3TeV)
	    .value("m4TeV", Mass::m4TeV)
	    .value("m5TeV", Mass::m5TeV)
	    .value("m6TeV", Mass::m6TeV)
	    .value("m7TeV", Mass::m7TeV)
	    .value("m8TeV", Mass::m8TeV)
	    .value("m9TeV", Mass::m9TeV)
	    .value("m10TeV", Mass::m10TeV)
	    .value("m12TeV", Mass::m12TeV)
	    .value("m15TeV", Mass::m15TeV)
	    .value("m20TeV", Mass::m20TeV)
	    .value("m30TeV", Mass::m30TeV)
	    .value("m50TeV", Mass::m50TeV)
	    .value("m100TeV", Mass::m100TeV);

	// charged gas density models
	py::class_<DarkMatterSpectrum, std::shared_ptr<DarkMatterSpectrum>>(
	    subm, "DarkMatterSpectrum")
	    .def("getRestMassEnergy", &DarkMatterSpectrum::getRestMassEnergy)
	    .def("getParticlesPerEnergy",
	         &DarkMatterSpectrum::getParticlesPerEnergy);
	py::class_<PPPC4DMIDSpectrum, std::shared_ptr<PPPC4DMIDSpectrum>,
	           DarkMatterSpectrum>(subm, "PPPC4DMIDSpectrum")
	    .def(py::init<Channel, Mass>(), py::arg("Channel"), py::arg("Mass"))
	    .def("getRestMassEnergy", &PPPC4DMIDSpectrum::getRestMassEnergy)
	    .def("getParticlesPerEnergy",
	         &PPPC4DMIDSpectrum::getParticlesPerEnergy);
	py::class_<GalacticProfile, std::shared_ptr<GalacticProfile>>(
	    subm, "GalacticProfile");
	py::class_<NFWGProfile, std::shared_ptr<NFWGProfile>, GalacticProfile>(
	    subm, "NFWGProfile")
	    .def(py::init<double, double, QMass>(), py::arg("gamma"),
	         py::arg("concentration"), py::arg("M200"))
	    .def("getMassDensity", &NFWGProfile::getMassDensity)
	    .def("getRhoSun", &NFWGProfile::getRhoSun);
}

}}  // namespace hermes::darkmatter
