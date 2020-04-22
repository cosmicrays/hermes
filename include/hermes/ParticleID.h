#ifndef HERMES_PARTICLEID_H
#define HERMES_PARTICLEID_H

#include <cassert>

namespace hermes {

class PID {
      protected:
	int Z;
	int A;
	int id;

      public:
	PID(int Z_, int A_) : Z(Z_), A(A_) {
		assert(A_ >= 0);
		id = Z * 100 + A;
	}

	int getID() const { return id; }

	virtual ~PID() {}

	bool isNucleus() { return (A > 0); }

	int atomicNr() const { return Z; }

	int massNr() const { return A; }

	bool operator<(const PID &p2) const { return id < p2.getID(); }

	bool operator>(const PID &p2) const { return id > p2.getID(); }

	bool operator==(const PID &p2) const { return id == p2.getID(); }
};

static const PID Electron(-1, 0);
static const PID Positron(1, 0);
static const PID Proton(1, 1);
static const PID Helium(2, 4);

} // namespace hermes

#endif // HERMES_PARTICLEID_H
