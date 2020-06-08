#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hermes/neutralgas/GasType.h"
#include "hermes/neutralgas/NeutralGasAbstract.h"
#include "hermes/neutralgas/RingModel.h"

namespace py = pybind11;

namespace hermes { namespace neutralgas {

void init(py::module &m) {
	py::module subm = m.def_submodule("neutralgas");
	subm.doc() = "neutral gas package";

	py::enum_<GasType>(subm, "GasType")
	    .value("HI", GasType::HI)
	    .value("H2", GasType::H2);

	// neutral gas density models
	py::class_<RingData, std::shared_ptr<RingData>>(subm, "RingData")
	    .def(py::init<GasType>())
	    .def("getHIColumnDensityInRing", &RingData::getHIColumnDensityInRing)
	    .def("getCOIntensityInRing", &RingData::getCOIntensityInRing);
	py::class_<Ring, std::shared_ptr<Ring>>(subm, "Ring")
	    .def("getBoundaries", &Ring::getBoundaries)
	    .def("isInside", &Ring::isInside)
	    .def("getHIColumnDensity", &Ring::getHIColumnDensity)
	    .def("getH2ColumnDensity", &Ring::getH2ColumnDensity);
	// NOLINTNEXTLINE(bugprone-unused-raii)
	py::class_<NeutralGasAbstract, std::shared_ptr<NeutralGasAbstract>>(
	    subm, "NeutralGasAbstract");
	py::class_<RingModel, std::shared_ptr<RingModel>, NeutralGasAbstract>(
	    subm, "RingModel")
	    .def(py::init<GasType>(), py::arg("ring_type"))
	    .def("getEnabledRings", &RingModel::getEnabledRings)
	    .def("setEnabledRings", &RingModel::setEnabledRings)
	    .def("disableRingNo", &RingModel::disableRingNo)
	    .def("enableRingNo", &RingModel::enableRingNo)
	    .def("isRingEnabled", &RingModel::isRingEnabled)
	    .def("getGasType", &RingModel::getGasType)
	    .def("getRingNumber", &RingModel::getRingNumber)
	    .def("__getitem__",
	         [](const RingModel &r, std::size_t i) -> std::shared_ptr<Ring> {
		         if (i >= r.size()) throw py::index_error();
		         return r[i];
	         })
	    .def("__len__", &RingModel::size)
	    .def(
	        "__iter__",
	        [](const RingModel &r) {
		        return py::make_iterator(r.begin(), r.end());
	        },
	        py::keep_alive<
	            0,
	            1>() /* Essential: keep object alive while iterator exists */);
}

}}  // namespace hermes::neutralgas
