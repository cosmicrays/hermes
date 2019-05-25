#ifdef HERMES_HAVE_CFITSIO

#include "hermes/neutralGasDensity/RingModelDensity.h"
#include "hermes/Common.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <array>
#include <algorithm>

namespace hermes {

RingData::RingData() {
	readDataFile(GasType::HI, "NHrings_Ts300K.fits.gz");
	readDataFile(GasType::CO, "WCOrings_COGAL.fits.gz");
}

void RingData::readDataFile(GasType t, std::string filename) {

	ffile = std::make_unique<FITSFile>(
			FITSFile(getDataPath("GasDensity/Remy18/" + filename))); 
	ffile->openFile(FITS::READ);

	n_lon[t] = ffile->readKeyValueAsInt("NAXIS1");
	n_lat[t] = ffile->readKeyValueAsInt("NAXIS2");
	n_rings[t] = ffile->readKeyValueAsInt("NAXIS3");
	
	min_lon[t] = ffile->readKeyValueAsDouble("CRVAL1");
	delta_lon[t] = ffile->readKeyValueAsDouble("CDELT1");
	min_lat[t] = ffile->readKeyValueAsDouble("CRVAL2");
	delta_lat[t] = ffile->readKeyValueAsDouble("CDELT2");
	
	std::cerr << "Number of rings: " << n_rings[t] << std::endl;

	int firstElement = 1;
	int nElements = n_lon[t]*n_lat[t]*n_rings[t];
	dataVector[t] = ffile->readImageAsFloat(firstElement, nElements);
}

int RingData::getRingNumber(GasType t) const {
	return n_rings[t];
}

double RingData::getRawValue(
		GasType t, int ring, const QDirection& dir) const {

	QAngle lat = dir[0];
    	QAngle lon = dir[1] + 180_deg; // becaue the galactic centre of
				       // the ring model is in the middle of the map

	int pxl_lat = static_cast<int>(round(lat / 180_deg * n_lat[t]));
	int pxl_lon = static_cast<int>(round(lon / 360_deg * n_lon[t]));

	// NAXIS1 x NAXIS2 x NAXIS3 => lon x lat x ring
	return dataVector[t][
		(ring * n_lat[t] + pxl_lat) * n_lon[t] + pxl_lon
	];
}

QColumnDensity RingData::getHIColumnDensityInRing(
		int ring, const QDirection& dir) const {
	// the data is given in cm^-2
	return getRawValue(GasType::HI, ring, dir) / 1_cm2;
}

QRingCOIntensity RingData::getCOIntensityInRing(
		int ring, const QDirection& dir) const {
	// the data is given in K km s^-2
	if(ring == 10 && ring == 11)
	       return QRingCOIntensity(0);	
	return getRawValue(GasType::CO, ring, dir) * 1_K * 1_km / 1_s;
}

Ring::Ring(std::size_t index_, std::shared_ptr<RingData> dataPtr_,
                        QLength innerR_, QLength outerR_) :
	index(index_), dataPtr(dataPtr_), innerR(innerR_), outerR(outerR_) {
}

std::size_t Ring::getIndex() const {
	return index;
}

std::pair<QLength, QLength> Ring::getBoundaries() const {
	return std::make_pair(innerR, outerR);
}

bool Ring::isInside(const Vector3QLength &pos) const {
	QLength rho = sqrt(pos.x*pos.x + pos.y*pos.y);
	return (rho > innerR && rho < outerR);
}

QColumnDensity Ring::getHIColumnDensity(
		const QDirection& dir_) const {
	return dataPtr->getHIColumnDensityInRing(index, dir_);
}

QRingCOIntensity Ring::getCOIntensity(
		const QDirection& dir_) const {
	return dataPtr->getCOIntensityInRing(index, dir_);
}

RingModelDensity::RingModelDensity() {
	dataPtr = std::make_shared<RingData>(RingData());
	fillRingContainer();	
}

void RingModelDensity::fillRingContainer() {
	for(std::size_t i = 0; i < dataPtr->getRingNumber(GasType::HI); ++i) {
		ringContainer.push_back(
			std::make_shared<Ring>(Ring(i, dataPtr, boundaries[i], boundaries[i+1])));
	}
}

int RingModelDensity::getRingNumber(GasType t) const {
	return dataPtr->getRingNumber(t);
}

std::shared_ptr<Ring> RingModelDensity::operator[](const std::size_t i) const {
        return ringContainer[i];
}

RingModelDensity::iterator RingModelDensity::begin() {
        return ringContainer.begin();
}

RingModelDensity::const_iterator RingModelDensity::begin() const {
        return ringContainer.begin();
}

RingModelDensity::iterator RingModelDensity::end() {
        return ringContainer.end();
}

RingModelDensity::const_iterator RingModelDensity::end() const {
        return ringContainer.end();
}

} // namespace hermes

#endif // HERMES_HAVE_CFITSIO
