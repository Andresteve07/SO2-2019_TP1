#include "log.h"
#include "vista.h"

int main(void)
{
	log_set_level(LOG_TRACE);
	log_set_quiet(0);
	init_vista();
}