#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/interactions/AAfragGamma.h"
#include "hermes/interactions/BreitWheeler.h"
#include "hermes/interactions/BremsstrahlungGALPROP.h"
#include "hermes/interactions/BremsstrahlungTsai74.h"
#include "hermes/interactions/DiffCrossSection.h"
#include "hermes/interactions/DummyCrossSection.h"
#include "hermes/interactions/Kamae06Gamma.h"
#include "hermes/interactions/Kamae06Neutrino.h"
#include "hermes/interactions/KelnerAharonianGamma.h"
#include "hermes/interactions/KelnerAharonianNeutrino.h"
#include "hermes/interactions/KleinNishina.h"

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

	py::class_<DummyCrossSection, std::shared_ptr<DummyCrossSection>, DifferentialCrossSection>(subm,
	                                                                                            "DummyCrossSection")
	    .def(py::init<const QDiffCrossSection &>(), py::arg("constant"));

	py::class_<KleinNishina, std::shared_ptr<KleinNishina>, DifferentialCrossSection>(subm, "KleinNishina")
	    .def(py::init<>())
	    .def("getDiffCrossSection",
	         static_cast<QDiffCrossSection (KleinNishina::*)(const QEnergy &, const QEnergy &, const QEnergy &) const>(
	             &KleinNishina::getDiffCrossSection));

	py::class_<AAfragGamma, std::shared_ptr<AAfragGamma>, DifferentialCrossSection>(subm, "AAfragGamma")
	    .def(py::init<>())
	    .def("getDiffCrossSection",
	         static_cast<QDiffCrossSection (AAfragGamma::*)(const QEnergy &, const QEnergy &) const>(
	             &AAfragGamma::getDiffCrossSection))
	    .def("getDiffCrossSection",
	         static_cast<QDiffCrossSection (AAfragGamma::*)(const PID &, const PID &, const QEnergy &, const QEnergy &)
	                         const>(&AAfragGamma::getDiffCrossSection));

	py::class_<AAfragNeutrino, std::shared_ptr<AAfragNeutrino>, DifferentialCrossSection>(subm, "AAfragNeutrino")
	    .def(py::init<>())
	    .def("getDiffCrossSection",
	         static_cast<QDiffCrossSection (AAfragNeutrino::*)(const QEnergy &, const QEnergy &) const>(
	             &AAfragNeutrino::getDiffCrossSection))
	    .def("getDiffCrossSection", static_cast<QDiffCrossSection (AAfragNeutrino::*)(
	                                    const PID &, const PID &, const QEnergy &, const QEnergy &) const>(
	                                    &AAfragNeutrino::getDiffCrossSection));

	py::class_<Kamae06Gamma, std::shared_ptr<Kamae06Gamma>, DifferentialCrossSection>(subm, "Kamae06Gamma")
	    .def(py::init<>())
	    .def("getDiffCrossSection",
	         static_cast<QDiffCrossSection (Kamae06Gamma::*)(const QEnergy &, const QEnergy &) const>(
	             &Kamae06Gamma::getDiffCrossSection))
	    .def("getDiffCrossSection",
	         static_cast<QDiffCrossSection (Kamae06Gamma::*)(const PID &, const PID &, const QEnergy &, const QEnergy &)
	                         const>(&Kamae06Gamma::getDiffCrossSection));

	py::class_<Kamae06Neutrino, std::shared_ptr<Kamae06Neutrino>, DifferentialCrossSection>(subm, "Kamae06Neutrino")
	    .def(py::init<>())
	    .def("getDiffCrossSection",
	         static_cast<QDiffCrossSection (Kamae06Neutrino::*)(const QEnergy &, const QEnergy &) const>(
	             &Kamae06Neutrino::getDiffCrossSection))
	    .def("getDiffCrossSection", static_cast<QDiffCrossSection (Kamae06Neutrino::*)(
	                                    const PID &, const PID &, const QEnergy &, const QEnergy &) const>(
	                                    &Kamae06Neutrino::getDiffCrossSection));

	py::class_<KelnerAharonianGamma, std::shared_ptr<KelnerAharonianGamma>, DifferentialCrossSection>(
	    subm, "KelnerAharonianGamma")
	    .def(py::init<>())
	    .def_static("sigmaInelastic", &KelnerAharonianGamma::sigmaInelastic)
	    .def("getDiffCrossSection",
	         static_cast<QDiffCrossSection (KelnerAharonianGamma::*)(const QEnergy &, const QEnergy &) const>(
	             &KelnerAharonianGamma::getDiffCrossSection))
	    .def("getDiffCrossSection", static_cast<QDiffCrossSection (KelnerAharonianGamma::*)(
	                                    const PID &, const PID &, const QEnergy &, const QEnergy &) const>(
	                                    &KelnerAharonianGamma::getDiffCrossSection));

	py::class_<KelnerAharonianNeutrino, std::shared_ptr<KelnerAharonianNeutrino>, DifferentialCrossSection>(
	    subm, "KelnerAharonianNeutrino")
	    .def(py::init<>())
	    .def_static("sigmaInelastic", &KelnerAharonianGamma::sigmaInelastic)
	    .def("getDiffCrossSection",
	         static_cast<QDiffCrossSection (KelnerAharonianNeutrino::*)(const QEnergy &, const QEnergy &) const>(
	             &KelnerAharonianNeutrino::getDiffCrossSection))
	    .def("getDiffCrossSection", static_cast<QDiffCrossSection (KelnerAharonianNeutrino::*)(
	                                    const PID &, const PID &, const QEnergy &, const QEnergy &) const>(
	                                    &KelnerAharonianNeutrino::getDiffCrossSection));

	py::class_<BremsstrahlungAbstract, std::shared_ptr<BremsstrahlungAbstract>>(subm, "BremsstrahlungAbstract");

	py::enum_<BremsstrahlungAbstract::Target>(subm, "BremTarget")
	    .value("HI", BremsstrahlungAbstract::Target::HI)
	    .value("HII", BremsstrahlungAbstract::Target::HII)
	    .value("He", BremsstrahlungAbstract::Target::He);

	py::class_<BremsstrahlungGALPROP, std::shared_ptr<BremsstrahlungGALPROP>, BremsstrahlungAbstract>(
	    subm, "BremsstrahlungGALPROP")
	    .def(py::init<>())
	    .def("getDiffCrossSectionForTarget", &BremsstrahlungGALPROP::getDiffCrossSectionForTarget);

	py::class_<BremsstrahlungTsai74, std::shared_ptr<BremsstrahlungTsai74>, BremsstrahlungAbstract>(
	    subm, "BremsstrahlungTsai74")
	    .def(py::init<>())
	    .def("getDiffCrossSectionForTarget", &BremsstrahlungTsai74::getDiffCrossSectionForTarget);

	py::class_<BreitWheeler, std::shared_ptr<BreitWheeler>>(subm, "BreitWheeler")
	    .def(py::init<>())
	    .def("getCrossSection", &BreitWheeler::getCrossSection)
	    .def("integratedOverTheta", &BreitWheeler::integratedOverTheta);
}

}}  // namespace hermes::interactions
