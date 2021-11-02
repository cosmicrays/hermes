#ifdef HERMES_HAVE_CFITSIO

#include "hermes/neutralgas/RingModel.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <utility>

#include "hermes/Common.h"

namespace hermes { namespace neutralgas {

Ring::Ring(std::size_t index_, std::shared_ptr<RingData> dataPtr_, QLength innerR_, QLength outerR_,
           QRingX0Unit XCOvalue_)
    : index(index_), dataPtr(std::move(dataPtr_)), innerR(innerR_), outerR(outerR_), XCOvalue(XCOvalue_) {}

Ring::~Ring() {}

std::size_t Ring::getIndex() const { return index; }

std::pair<QLength, QLength> Ring::getBoundaries() const { return std::make_pair(innerR, outerR); }

bool Ring::isInside(const Vector3QLength &pos) const {
	QLength rho = pos.getRho();
	return (rho > innerR && rho < outerR);
}

GasType Ring::getGasType() const { return dataPtr->getGasType(); }

QColumnDensity Ring::getHIColumnDensity(const QDirection &dir_) const {
	return dataPtr->getHIColumnDensityInRing(index, dir_);
}

QColumnDensity Ring::getH2ColumnDensity(const QDirection &dir_) const {
	return 2 * XCOvalue * dataPtr->getCOIntensityInRing(index, dir_);
}

QColumnDensity Ring::getColumnDensity(const QDirection &dir_) const {
	if (getGasType() == GasType::HI) return getHIColumnDensity(dir_);
	if (getGasType() == GasType::H2) return getH2ColumnDensity(dir_);
	return QColumnDensity(0);
}

RingModel::RingModel(GasType gas) : NeutralGasAbstract(), dataPtr(std::make_shared<RingData>(RingData(gas))) {
	std::fill(XCOvalues.begin(), XCOvalues.end(),
	          XcoDefaultValue);  // default value for XCO
	std::fill(enabledRings.begin(), enabledRings.end(),
	          true);  // enable all by default
	fillRingContainer();
}

// RingModel::RingModel(GasType gas, std::array<QRingX0Unit, 12> XCOvalues_)
//     : NeutralGasAbstract(), dataPtr(std::make_shared<RingData>(RingData(gas))) {
// 	std::copy(XCOvalues_.begin(), XCOvalues_.end(), XCOvalues.begin());
// 	std::fill(enabledRings.begin(), enabledRings.end(),
// 	          true);  // enable all by default
// 	fillRingContainer();
// }

RingModel::RingModel(GasType gas, std::array<double, 12> XCOfactors)
    : NeutralGasAbstract(), dataPtr(std::make_shared<RingData>(RingData(gas))) {
	std::fill(XCOvalues.begin(), XCOvalues.end(),
	          XcoDefaultValue);  // default value for XCO
	std::transform(XCOfactors.begin(), XCOfactors.end(), XCOvalues.begin(), XCOvalues.begin(), std::multiplies<>{});
	std::fill(enabledRings.begin(), enabledRings.end(),
	          true);  // enable all by default
	fillRingContainer();
}

void RingModel::fillRingContainer() {
	for (std::size_t i = 0; i < dataPtr->getRingNumber(); ++i) {
		if (getGasType() == GasType::HI) {
			auto lowBoundary = boundariesHI.at(i);
			auto highBoundary = (i < 11) ? boundariesHI.at(i + 1) : boundariesHI.at(i) + 1_kpc;
			ringContainer.push_back(std::make_shared<Ring>(Ring(i, dataPtr, lowBoundary, highBoundary)));
		}
		if (getGasType() == GasType::H2) {
			auto lowBoundary = boundariesH2.at(i);
			auto highBoundary = (i < 11) ? boundariesH2.at(i + 1) : boundariesH2.at(i) + 1_kpc;
			auto XCOvalue = XCOvalues.at(i);
			ringContainer.push_back(std::make_shared<Ring>(Ring(i, dataPtr, lowBoundary, highBoundary, XCOvalue)));
		}
	}
}

std::array<bool, 12> RingModel::getEnabledRings() const { return enabledRings; }

void RingModel::disableRingNo(int i) {
	if (i >= enabledRings.size()) {
		throw std::runtime_error(
		    "Provided number is bigger than the total "
		    "number of rings. Aborted.");
	}

	enabledRings[i] = false;
}

void RingModel::enableRingNo(int i) {
	if (i >= enabledRings.size()) {
		throw std::runtime_error(
		    "Provided number is bigger than the total "
		    "number of rings. Aborted.");
	}

	enabledRings[i] = true;
}

void RingModel::setEnabledRings(std::array<bool, 12> list) { enabledRings = list; }

bool RingModel::isRingEnabled(int i) const {
	if (i >= enabledRings.size()) return false;
	return enabledRings[i];
}

GasType RingModel::getGasType() const { return dataPtr->getGasType(); }

int RingModel::getRingNumber() const { return dataPtr->getRingNumber(); }

std::vector<std::pair<PID, double>> RingModel::getAbundanceFractions() const { return abundanceFractions; }

std::shared_ptr<Ring> RingModel::operator[](const std::size_t i) const { return ringContainer[i]; }

std::size_t RingModel::size() const { return ringContainer.size(); }

RingModel::iterator RingModel::begin() { return ringContainer.begin(); }

RingModel::const_iterator RingModel::begin() const { return ringContainer.begin(); }

RingModel::iterator RingModel::end() { return ringContainer.end(); }

RingModel::const_iterator RingModel::end() const { return ringContainer.end(); }

}}  // namespace hermes::neutralgas

#endif  // HERMES_HAVE_CFITSIO
