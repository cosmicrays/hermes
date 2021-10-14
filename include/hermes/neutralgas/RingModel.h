#ifdef HERMES_HAVE_CFITSIO

#ifndef HERMES_RINGMODEL_H
#define HERMES_RINGMODEL_H

#include <array>
#include <utility>

#include "hermes/FITSWrapper.h"
#include "hermes/Grid.h"
#include "hermes/ParticleID.h"
#include "hermes/Units.h"
#include "hermes/neutralgas/GasType.h"
#include "hermes/neutralgas/NeutralGasAbstract.h"
#include "hermes/neutralgas/RingData.h"

namespace hermes { namespace neutralgas {
/**
 * \addtogroup NeutralGas
 * @{
 */

class Ring {
  private:
	std::size_t index;
	std::shared_ptr<RingData> dataPtr;
	QLength innerR, outerR;
	QRingX0Unit XCOvalue;

  public:
	Ring(std::size_t index_, std::shared_ptr<RingData> RingModelPtr_, QLength innerR_, QLength outerR_,
	     QRingX0Unit XCOvalue_ = QRingX0Unit(1.8e20));
	~Ring();

	std::size_t getIndex() const;
	std::pair<QLength, QLength> getBoundaries() const;
	bool isInside(const Vector3QLength &) const;
	GasType getGasType() const;

	QColumnDensity getHIColumnDensity(const QDirection &) const;
	QColumnDensity getH2ColumnDensity(const QDirection &) const;
	QColumnDensity getColumnDensity(const QDirection &) const;
};

class RingModel : public NeutralGasAbstract {
  private:
	std::shared_ptr<RingData> dataPtr;
	std::array<QLength, 12> boundariesHI = {0_kpc, 2_kpc, 3_kpc,  4_kpc,  5_kpc,  6_kpc,
	                                        7_kpc, 9_kpc, 12_kpc, 15_kpc, 18_kpc, 35_kpc};
	std::array<QLength, 12> boundariesH2 = {0_kpc, 0.86_kpc, 2_kpc, 3_kpc,  4_kpc,  5_kpc,
	                                        6_kpc, 7_kpc,    9_kpc, 12_kpc, 15_kpc, 18_kpc};
	std::array<QRingX0Unit, 12> XCOvalues;

	std::vector<std::pair<PID, double>> abundanceFractions = {{Proton, 1}, {Helium, 0.1}};

	typedef std::vector<std::shared_ptr<Ring>> tRingContainer;
	mutable tRingContainer ringContainer;
	mutable std::array<bool, 12> enabledRings;

	void fillRingContainer();

  public:
	RingModel(GasType gas);
	RingModel(GasType gas, std::array<QRingX0Unit, 12> XCOvalues_);
	~RingModel() {}

	std::array<bool, 12> getEnabledRings() const;
	void setEnabledRings(std::array<bool, 12> list);
	void disableRingNo(int i);
	void enableRingNo(int i);
	bool isRingEnabled(int i) const;

	GasType getGasType() const;
	int getRingNumber() const override;
	std::vector<std::pair<PID, double>> getAbundanceFractions() const;

	/** iterator goodies */
	std::size_t size() const;
	std::shared_ptr<Ring> operator[](const std::size_t i) const;
	typedef tRingContainer::iterator iterator;
	typedef tRingContainer::const_iterator const_iterator;
	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
};

/** @}*/
}}  // namespace hermes::neutralgas

#endif  // HERMES_RINGMODEL_H
#endif  // HERMES_HAVE_CFITSIO
