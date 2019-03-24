#include "hermes/example.h"

#include <iostream>
#include <memory>

namespace hermes {

class DummyCRDensity: public CosmicRayDensity {
QEnergy minE, maxE;
int steps;
public:
	DummyCRDensity() {
        	minE = 1_GeV;
	        maxE = 1e4_GeV;
        	steps = 10;
	        makeEnergyRange();
	}

	void makeEnergyRange() {
        	QEnergy energy = minE;
	        float energyRatio =
        	        exp(1./static_cast<double>(steps-1) * log(maxE/minE));

	        for (int i = 0; i < steps; ++i) {
        	        energyRange.push_back(energy);
                	energy = energy * energyRatio;
	        }
	}

	QPDensityPerEnergy getDensityPerEnergy(const QEnergy& E_, const Vector3QLength& pos_) const {
        	constexpr int alpha = 3;
	        auto Phi0 = 0.1 / (1_GeV * 1_cm*1_cm * 1_s * c_light);// * 4_pi; 
        	auto E0 = 1_GeV;
	        auto E_cutoff = 5_TeV;

        	QNumber profile = 1.;
	        QPDensityPerEnergy spectrum = Phi0;

        	return profile * spectrum;
	}
};

class DummyField: public MagneticField {
	Vector3QMField getField(const Vector3QLength& pos_) const {
		Vector3QMField bOuter(0.);
		Vector3QMField bInner(0,0,1e-10);

		if (pos_.getR() < 7_kpc)
			return bInner;
		else
			return bOuter;
	}
};


void exampleRM() {
	// magnetic field models
	auto B = Vector3QMField(0_muG, 0_muG, 1_muG);
	auto ufield = std::make_shared<UniformMagneticField>(UniformMagneticField(B));
	auto WMAP07 = std::make_shared<WMAP07Field>(WMAP07Field());
	auto Sun08 = std::make_shared<Sun08Field>(Sun08Field());
	//Sun08->randomTurbulent(1337);
	auto JF12 = std::make_shared<JF12Field>(JF12Field());
	JF12->randomStriated(137);
	JF12->randomTurbulent(1337);
	auto PT11 = std::make_shared<PT11Field>(PT11Field());

	// gas models
	auto gasCordes91 = std::make_shared<HII_Cordes91>(HII_Cordes91());
	auto gasYMW16 = std::make_shared<YMW16>(YMW16());
	
	// integrator
	auto intRM = std::make_shared<RMIntegrator>(RMIntegrator(JF12, gasYMW16));

	// skymap
	int nside = 32;	
	auto skymap = std::make_shared<RMSkymap>(RMSkymap(nside));
	skymap->setIntegrator(intRM);
	skymap->compute();

	// save
	auto output = std::make_shared<FITSOutput>(FITSOutput("!example.fits.gz"));
	skymap->save(output);
}

void exampleSynchro() {

	// magnetic field models
	auto B = Vector3QMField(0_muG, 0_muG, 1_muG);
	auto ufield = std::make_shared<UniformMagneticField>(UniformMagneticField(B));
	auto JF12 = std::make_shared<JF12Field>(JF12Field());
	JF12->randomStriated(137);
	JF12->randomTurbulent(1337);
	auto PT11 = std::make_shared<PT11Field>(PT11Field());
	auto WMAP07 = std::make_shared<WMAP07Field>(WMAP07Field());
	auto Sun08 = std::make_shared<Sun08Field>(Sun08Field());
	auto dfield = std::make_shared<DummyField>(DummyField());
	
	// cosmic ray density models
	auto dummyModel = std::make_shared<DummyCRDensity>(DummyCRDensity());
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto WMAP07Model = std::make_shared<WMAP07CRDensity>(WMAP07CRDensity());
	auto Sun08Model = std::make_shared<Sun08CRDensity>(Sun08CRDensity());
	//auto dragonModel = std::make_shared<DragonCRDensity>(DragonCRDensity("/home/andy/Work/notebooks/Hermes/run_2D.fits", Electron, DragonFileType::_2D)); 
	
	// integrator
	auto intSynchro = std::make_shared<SynchroIntegrator>(SynchroIntegrator(WMAP07, WMAP07Model));
	//auto intFreeFree = std::make_shared<FreeFreeIntegrator>(FreeFreeIntegrator(ufield, simpleModel));

/*				
	std::cout << "# X\tY\tZ\teta" << std::endl;
	auto energy = std::next(dragonModel->begin());
	//energy++;
	QLength x = 1_kpc;
	QLength y = 1_kpc;
#pragma omp critical(print)
	for (QLength x = -12_kpc; x < 12_kpc; x += 0.5_kpc)
		for (QLength y = -12_kpc; y < 12_kpc; y += 0.5_kpc)
			for (QLength z = -4_kpc; z < 4_kpc; z += 0.2_kpc) {
				Vector3QLength pos(x.getValue(), y.getValue(), z.getValue());
				auto density = dragonModel->getDensityPerEnergy(*energy, pos);
				//auto density = simpleModel->getDensityPerEnergy(*energy, pos);
				//auto density = (JF12->getField(pos)).getZ();
				//auto density = (PT11->getField(pos)).getR();
				//auto density = (testField->getField(pos)).getR();
				//if (density.getValue() == 0) continue;
				std::cout << x.getValue()/1_pc << "\t" <<
					     y.getValue()/1_pc << "\t" <<
					     z.getValue()/1_pc << "\t" <<
					     density << std::endl;
			}
*/	

	// skymap
	int nside = 16;
	auto skymaps = std::make_shared<RadioSkymapRange>(RadioSkymapRange(nside, 100_MHz, 100_GHz, 10));
	auto skymap = std::make_shared<RadioSkymap>(RadioSkymap(nside, 408_MHz));
	skymap->setIntegrator(intSynchro);

	auto output = std::make_shared<FITSOutput>(FITSOutput("!example.fits.gz"));
	
	skymap->compute();
	//skymap->printPixels();
	skymap->save(output);
}

void exampleFreeFree() {
	// gas models
	auto gasCordes91 = std::make_shared<HII_Cordes91>(HII_Cordes91());
	auto gasYMW16 = std::make_shared<YMW16>(YMW16());
	
	// integrator
	auto intFreeFree = std::make_shared<FreeFreeIntegrator>(FreeFreeIntegrator(gasYMW16));

	// skymap
	int nside = 8;	
	auto skymaps = std::make_shared<RadioSkymapRange>(RadioSkymapRange(nside, 100_MHz, 100_GHz, 10));
	skymaps->setIntegrator(intFreeFree);
	skymaps->compute();

	// save
	auto output = std::make_shared<FITSOutput>(FITSOutput("!example-multi.fits.gz"));
	skymaps->save(output);
}

void playground() {

	//exampleRM();
	//exampleSynchro();
	exampleFreeFree();
}

} // namespace hermes

int main(void){

	hermes::playground();

	return 0;
}

