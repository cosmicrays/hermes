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

void exampleRM() {
	// magnetic field models
	auto B = Vector3QMField(0_muG, 0_muG, 1_muG);
	auto ufield = std::make_shared<UniformMagneticField>(UniformMagneticField(B));
	auto JF12 = std::make_shared<JF12Field>(JF12Field());

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
	auto PT11 = std::make_shared<PT11Field>(PT11Field());
	
	// cosmic ray density models
	auto dummyModel = std::make_shared<DummyCRDensity>(DummyCRDensity());
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity());
	auto dragonModel = std::make_shared<DragonCRDensity>(DragonCRDensity("/home/andy/Work/notebooks/Hermes/run_3D.fits", Electron)); 
	
	// integrator
	auto intSynchro = std::make_shared<SynchroIntegrator>(SynchroIntegrator(JF12, dragonModel));
	
	/*	
	std::cout << "# X\tY\tZ\teta" << std::endl;
	auto energy = std::next(dragonModel->begin());
	energy++;
	QLength x = 1_kpc;
	QLength y = 1_kpc;
	for (QLength x = -12_kpc; x < 12_kpc; x += 0.5_kpc)
		for (QLength y = -12_kpc; y < 12_kpc; y += 0.5_kpc)
			for (QLength z = -4_kpc; z < 4_kpc; z += 0.2_kpc) {
				Vector3QLength pos(x.getValue(), y.getValue(), z.getValue());
				auto density = dragonModel->getDensityPerEnergy(*energy, pos);
				//auto density = simpleModel->getDensityPerEnergy(*energy, pos);
				//auto density = (JF12->getField(pos)).getR();
				//auto density = (PT11->getField(pos)).getR();
				//auto density = (testField->getField(pos)).getR();
				if (density.getValue() == 0) continue;
				std::cout << x.getValue()/1_pc << "\t" <<
					     y.getValue()/1_pc << "\t" <<
					     z.getValue()/1_pc << "\t" <<
					     density << std::endl;
			}
	*/

	// skymap
	int nside = 20;
	auto skymaps = std::make_shared<SynchroSkymapRange>(SynchroSkymapRange(nside, 1_MHz, 408_MHz, 10));
	auto skymap = std::make_shared<SynchroSkymap>(SynchroSkymap(nside, 408_MHz));
	skymap->setIntegrator(intSynchro);
	//skymap->compute();

	//skymap->printPixels();
	
	auto output = std::make_shared<FITSOutput>(FITSOutput("!example.fits.gz"));
	//skymap->save(output);
}

void playground() {

	//exampleRM();
	exampleSynchro();

}

} // namespace hermes

int main(void){

	hermes::playground();

	return 0;
}

