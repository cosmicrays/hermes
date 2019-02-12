#include "gtest/gtest.h"

#include "hermes.h"

namespace hermes {

typedef SkymapTemplate<QNumber> SimpleSkymap;

class DummyIntegrator: public IntegratorTemplate<QNumber> {
public:
        DummyIntegrator() {
	};
        ~DummyIntegrator() {
	};
        QNumber integrateOverLOS(QDirection direction) const {
	
		QDirection galacticCentre = {90_deg, 0};
		QDirection galacticNorth = {0, 0};

		if(isWithinAngle(direction, galacticCentre, 20_deg))
			return QNumber(1);
		if(isWithinAngle(direction, galacticNorth, 30_deg))
			return QNumber(-1);
		return QNumber(0);

		/*
		Vector3QLength positionSun(8.5_kpc, 0, 0);
        	Vector3QLength pos(0.0), galacticCentre(0.0);
		Vector3QLength galacticNorth(8.5_kpc, 0, 4_kpc);
		QNumber sum(0);

		for(auto dist = 0_kpc; dist < 15_kpc; dist += 100_pc) {
			pos.setRThetaPhi(dist, direction[0], direction[1]);
        	        pos += positionSun;

			if(fabs((galacticCentre-pos).getR()) < 2_kpc)
				sum += QNumber(1);
			
			if(fabs((galacticNorth-pos).getR()) < 2_kpc)
				sum += QNumber(-1);
		}

		return sum;		
		*/
	};
};


TEST(Skymap, computePixel) {

	int nside = 8;
        auto skymap = std::make_shared<SimpleSkymap>(SimpleSkymap(nside));
	auto integrator = std::make_shared<DummyIntegrator>(DummyIntegrator());
	QDirection galacticCentre = {90_deg, 0};
	long int gcPixel = ang2pix_ring(nside, OffsetToHEALPix(galacticCentre));
	QDirection galacticNorth = {0, 0};
	long int gnPixel = ang2pix_ring(nside, OffsetToHEALPix(galacticNorth));

	//skymap->setIntegrator(integrator);
	skymap->computePixel(gcPixel, integrator);
	skymap->computePixel(gnPixel, integrator);

	EXPECT_EQ(static_cast<double>(skymap->getPixel(gcPixel)), 1);
	EXPECT_EQ(static_cast<double>(skymap->getPixel(gnPixel)), -1);

        //auto output = std::make_shared<FITSOutput>(FITSOutput("!test.fits.gz"));
        //skymap->save(output);
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

} // namespace hermes

