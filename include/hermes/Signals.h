#ifndef HERMES_SIGNALS_H
#define HERMES_SIGNALS_H

#include <csignal>
#include <iostream>
#ifndef sighandler_t
typedef void (*sighandler_t)(int);
#endif

namespace hermes {

extern int g_cancel_signal_flag;
void g_cancel_signal_callback(int sig);

} // namespace hermes

#endif // HERMES_SIGNALS_H
