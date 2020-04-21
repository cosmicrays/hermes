#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/interactions/DifferentialCrossSection.h"
#include "hermes/interactions/DummyCrossSection.h"
#include "hermes/interactions/BremsstrahlungSimple.h"
#include "hermes/interactions/KleinNishina.h"
#include "hermes/interactions/Kamae06Gamma.h"
#include "hermes/interactions/Kamae06Neutrino.h"
#include "hermes/interactions/KelnerAharonianGamma.h"
#include "hermes/interactions/KelnerAharonianNeutrino.h"

namespace py = pybind11;

namespace hermes { namespace interactions {

void init(py::module &m) {
    
    py::module subm = m.def_submodule("interactions");
    subm.doc() = "interactions package";

    // charged gas density models
    py::class_<DifferentialCrossSection, std::shared_ptr<DifferentialCrossSection>>(subm, "DifferentialCrossSection")
	      .def(py::init<bool>(), py::arg("cachingEnabled"))
	      .def("enableCaching", &DifferentialCrossSection::enableCaching)
	      .def("disableCaching", &DifferentialCrossSection::disableCaching);
    py::class_<DummyCrossSection, std::shared_ptr<DummyCrossSection>, DifferentialCrossSection>(subm, "DummyCrossSection")
	      .def(py::init<const QDifferentialCrossSection &>(), py::arg("constant"));
    py::class_<KleinNishina, std::shared_ptr<KleinNishina>, DifferentialCrossSection>(subm, "KleinNishina")
	      .def(py::init<>())
	      .def("getDiffCrossSection",
		   (QDifferentialCrossSection (KleinNishina::*)(const QEnergy &, const QEnergy &, const QEnergy &) const)
		   &KleinNishina::getDiffCrossSection);
    py::class_<Kamae06Gamma, std::shared_ptr<Kamae06Gamma>, DifferentialCrossSection>(subm, "Kamae06Gamma")
	      .def(py::init<>())
	      .def("getDiffCrossSection",
		   (QDifferentialCrossSection (Kamae06Gamma::*)(const QEnergy &, const QEnergy &) const)
		   &Kamae06Gamma::getDiffCrossSection);
    py::class_<Kamae06Neutrino, std::shared_ptr<Kamae06Neutrino>, DifferentialCrossSection>(subm, "Kamae06Neutrino")
	      .def(py::init<>())
	      .def("getDiffCrossSection",
		   (QDifferentialCrossSection (Kamae06Neutrino::*)(const QEnergy &, const QEnergy &) const)
		   &Kamae06Neutrino::getDiffCrossSection);
    py::class_<KelnerAharonianGamma, std::shared_ptr<KelnerAharonianGamma>, DifferentialCrossSection>(subm, "KelnerAharonianGamma")
	      .def(py::init<>())
	      .def_static("sigmaInelastic", &KelnerAharonianGamma::sigmaInelastic)
	      .def("getDiffCrossSection",
		   (QDifferentialCrossSection (KelnerAharonianGamma::*)(const QEnergy &, const QEnergy &) const)
		   &KelnerAharonianGamma::getDiffCrossSection);
    py::class_<KelnerAharonianNeutrino, std::shared_ptr<KelnerAharonianNeutrino>, DifferentialCrossSection>(subm, "KelnerAharonianNeutrino")
	      .def(py::init<>())
	      .def_static("sigmaInelastic", &KelnerAharonianGamma::sigmaInelastic)
	      .def("getDiffCrossSection",
		   (QDifferentialCrossSection (KelnerAharonianNeutrino::*)(const QEnergy &, const QEnergy &) const)
		   &KelnerAharonianNeutrino::getDiffCrossSection);
    py::class_<BremsstrahlungSimple, std::shared_ptr<BremsstrahlungSimple>, DifferentialCrossSection>(subm, "BremsstrahlungSimple")
	      .def(py::init<>())
	      .def("getDiffCrossSection",
		   (QDifferentialCrossSection (BremsstrahlungSimple::*)(const QEnergy &, const QEnergy &) const)
		   &BremsstrahlungSimple::getDiffCrossSection);
}

} // namespace interactions
} // namespace hermes
