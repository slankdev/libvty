
#pragma once

#include <vty_server.h>
#include <stddef.h>
#include <unistd.h>


void ssn_vty_poll_thread(void* arg);
void ssn_vty_poll_thread_stop();


