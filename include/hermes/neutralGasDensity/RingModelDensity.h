#ifdef HERMES_HAVE_CFITSIO

#ifndef HERMES_RINGMODELDENSITY_H
#define HERMES_RINGMODELDENSITY_H

#include "hermes/Units.h"
#include "hermes/Grid.h"
#include "hermes/FITSWrapper.h"

#include <utility>
#include <array>

namespace hermes {

enum GasType { HI = 0, CO = 1 };

class RingData {
private:
        std::unique_ptr<FITSFile> ffile;
	
	std::array<int, 2> n_lon, n_lat, n_rings;
	std::array<double, 2> min_lon, min_lat;
	std::array<double, 2> delta_lon, delta_lat;
	std::array<std::vector<float>, 2> dataVector;

	void readDataFile(GasType t, const std::string &filename);
	double getRawValue(GasType t, int ring, const QDirection& dir) const;
public:
	RingData();
	QColumnDensity getHIColumnDensityInRing(
			int ring, const QDirection& dir) const;
	QRingCOIntensity getCOIntensityInRing(
			int ring, const QDirection& dir) const;

	int getRingNumber(GasType t) const;
};

class Ring {
private:
	std::size_t index;
	std::shared_ptr<RingData> dataPtr;
	QLength innerR, outerR;
public:
	Ring(std::size_t index_, std::shared_ptr<RingData> RingModelPtr_,
			QLength innerR_, QLength outerR_);
	std::size_t getIndex() const;
	std::pair<QLength, QLength> getBoundaries() const;
	bool isInside(const Vector3QLength &pos) const;
	QColumnDensity getHIColumnDensity(const QDirection& dir_) const;
	QRingCOIntensity getCOIntensity(const QDirection& dir) const;
};

class RingModelDensity {
private:
	std::shared_ptr<RingData> dataPtr;
	std::array<QLength, 12> boundaries = {0_kpc, 2_kpc, 3_kpc, 4_kpc, 5_kpc, 6_kpc,
		7_kpc, 9_kpc, 12_kpc, 15_kpc, 18_kpc, 35_kpc};

	QTemperature gasTemp;
	typedef std::vector<std::shared_ptr<Ring> > tRingContainer;
	mutable tRingContainer ringContainer;
	
	void fillRingContainer();
public:
	RingModelDensity();
	~RingModelDensity() { }
	int getRingNumber(GasType t) const;

	inline void setTemperature(QTemperature T) {
		gasTemp = T;
	}
	inline QTemperature getTemperature() const {
		return gasTemp;
	}

	/** iterator goodies */
	std::shared_ptr<Ring> operator[](const std::size_t i) const;
        typedef tRingContainer::iterator iterator;
        typedef tRingContainer::const_iterator const_iterator;
        iterator begin();
        const_iterator begin() const;
        iterator end();
        const_iterator end() const;
};

} // namespace hermes

#endif // HERMES_RINGMODELDENSITY_H
#endif // HERMES_HAVE_CFITSIO
