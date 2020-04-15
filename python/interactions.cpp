#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/interactions/DifferentialCrossSection.h"
#include "hermes/interactions/DummyCrossSection.h"
#include "hermes/interactions/KleinNishina.h"
#include "hermes/interactions/BremsstrahlungSimple.h"
#include "hermes/interactions/Kamae06.h"

namespace py = pybind11;

namespace hermes { namespace interactions {

void init(py::module &m) {
    
    py::module subm = m.def_submodule("interactions");
    subm.doc() = "interactions package";

    // charged gas density models
    py::class_<DifferentialCrossSection, std::shared_ptr<DifferentialCrossSection>>(subm, "DifferentialCrossSection");
    py::class_<DummyCrossSection, std::shared_ptr<DummyCrossSection>, DifferentialCrossSection>(subm, "DummyCrossSection")
	      .def(py::init<const QDifferentialCrossSection &>(), py::arg("constant"));
    py::class_<KleinNishina, std::shared_ptr<KleinNishina>, DifferentialCrossSection>(subm, "KleinNishina")
	      .def(py::init<>())
	      .def("getDiffCrossSection",
		   (QDifferentialCrossSection (KleinNishina::*)(const QEnergy &, const QEnergy &, const QEnergy &) const)
		   &KleinNishina::getDiffCrossSection);
    py::class_<BremsstrahlungSimple, std::shared_ptr<BremsstrahlungSimple>, DifferentialCrossSection>(subm, "BremsstrahlungSimple")
	      .def(py::init<>())
	      .def("getDiffCrossSection",
		   (QDifferentialCrossSection (BremsstrahlungSimple::*)(const QEnergy &, const QEnergy &) const)
		   &BremsstrahlungSimple::getDiffCrossSection);
    py::class_<Kamae06, std::shared_ptr<Kamae06>, DifferentialCrossSection>(subm, "Kamae06")
	      .def(py::init<>())
	      .def("getDiffCrossSection",
		   (QDifferentialCrossSection (Kamae06::*)(const QEnergy &, const QEnergy &) const)
		   &Kamae06::getDiffCrossSection);
}

} // namespace interactions
} // namespace hermes
