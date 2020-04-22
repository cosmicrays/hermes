#include "hermes/Signals.h"

namespace hermes {

int g_cancel_signal_flag = 0;

void g_cancel_signal_callback(int sig) {
	std::cerr << "hermes::Skymap: Signal " << sig
		  << " (SIGINT/SIGTERM) received" << std::endl;
	g_cancel_signal_flag = sig;
}

} // namespace hermes
