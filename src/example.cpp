#include "hermes/example.h"

#include <iostream>
#include <memory>

namespace hermes {

class DummyGasDensity: public GasDensity {
public:
        DummyGasDensity() { };
        QPDensity getDensity(const Vector3QLength& pos) const {
		return QPDensity(1);
	}
};

class TestMagneticField: public MagneticField {
        Vector3QMField value;
public:
        TestMagneticField() { }
        Vector3QMField getField(const Vector3QLength &pos) const {
                Vector3QLength pos_gc(0_pc, 0_pc, 0_pc);

                if (fabs(pos.x) < 10_kpc && pos.y < 1_kpc && fabs(pos.z) < 0.5_kpc)
                        return Vector3QMField(QMField(static_cast<double>(pos.getR())));
                else
                        return Vector3QMField(0_T);
        }
};


void playground() {

	// magnetic field models
	auto B = Vector3QMField(1_muG, 0_muG, 0_muG);
	auto ufield = std::make_shared<UniformMagneticField>(UniformMagneticField(B));
	auto JF12 = std::make_shared<JF12Field>(JF12Field());
	auto PT11 = std::make_shared<PT11Field>(PT11Field());
	//auto testField = std::make_shared<TestMagneticField>(TestMagneticField());

	// cosmic ray density models
	auto simpleModel = std::make_shared<SimpleCRDensity>(SimpleCRDensity(1_GeV, 1e4_GeV, 10));
	auto dragonModel = std::make_shared<DragonCRDensity>(DragonCRDensity("/home/andy/Work/notebooks/Hermes/run_3D.fits", Electron)); 
	
	// gas models
	//auto gas = std::make_shared<HII_Cordes91>(HII_Cordes91());
	auto gas = std::make_shared<YMW16>(YMW16());
	//auto gas = std::make_shared<DummyGasDensity>(DummyGasDensity());

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
				//auto density = dragonModel->getDensityPerEnergy(*energy, pos);
				//auto density = simpleModel->getDensityPerEnergy(*energy, pos);
				//auto density = (JF12->getField(pos)).getR();
				//auto density = (PT11->getField(pos)).getR();
				//auto density = (testField->getField(pos)).getR();
				auto density = gas->getDensity(pos);
				std::cout << x.getValue()/1_pc << "\t" <<
					     y.getValue()/1_pc << "\t" <<
					     z.getValue()/1_pc << "\t" <<
					     density << std::endl;
			}
	*/
	
	

	// integrators
	//auto synchro = std::make_shared<SynchroIntegrator>(SynchroIntegrator(JF12, dragonModel));
	auto RM = std::make_shared<RMIntegrator>(RMIntegrator(JF12, gas));

	int nside = 32;	

	//auto skymaps = std::make_shared<SynchroSkymapRange>(SynchroSkymapRange(nside, 1_MHz, 500_MHz, 10));
	//auto skymap = std::make_shared<SynchroSkymap>(SynchroSkymap(nside, 408_MHz));
	auto skymap = std::make_shared<RMSkymap>(RMSkymap(nside));
	//skymap->setIntegrator(synchro);
	skymap->setIntegrator(RM);

	skymap->compute();

	//skymap->printPixels();
	
#ifdef HERMES_HAVE_CFITSIO
	auto output = std::make_shared<FITSOutput>(FITSOutput("!example.fits.gz"));
	skymap->save(output);
#endif // HERMES_HAVE_CFITSIO

}

} // namespace hermes

int main(void){

	hermes::playground();

	return 0;
}

