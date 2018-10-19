#include "hermes/example.h"

#include <iostream>
#include <memory>

namespace hermes {

QEnergy einstein_formula(QMass m) {
	return m * (c_light*c_light);
}

auto a_0 = Qcube(e_plus)/(2_pi*Qsquare(m_electron)*Qsquare(Qsquare(c_light)));

void playground() {

	QLength x(2.3_km), y(3.2_mm), z(0.0);
	QMass m(2_kg), m2(3_g);
	QEnergy E;
	Vector3MField B(1.0_muG);
	Vector3Length vec_x(1.0_kpc);

	//UniformMagneticField UField = UniformMagneticField(B);
	
	std::shared_ptr<Skymap> ptr_skymap (new Skymap(4));
	std::shared_ptr<JF12Field> ptr_JF12 (new JF12Field());

	RMIntegrator RM = RMIntegrator(ptr_JF12);
	RM.set_skymap(ptr_skymap);
	RM.compute();
	
	B = ptr_JF12->getField(vec_x);

	std::cout << (B.getValue()).getX() << " J" << std::endl;
	
	z = (x + y);

	E = einstein_formula(m+m2);

	// valid and compile-time dimension checked
	std::cout << E.getValue() << " J" << std::endl;
	
}

} // namespace hermes

int main(void){

	hermes::playground();

	return 0;
}

