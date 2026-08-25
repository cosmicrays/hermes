#include <hermes.h>

#include <string>

int main(int argc, char **argv) {
	if (hermes::getThreadsNumber() == 0) return 1;
	if (argc == 3 && hermes::getDataPath(argv[1]) != argv[2]) return 2;
	return argc == 1 || argc == 3 ? 0 : 3;
}
