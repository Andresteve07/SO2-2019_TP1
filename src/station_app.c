#include "log.h"
#include "vista.h"
#include <stdlib.h>

int main( int argc, char *argv[] ){
	log_set_level(LOG_TRACE);
	log_set_quiet(0);
	init_vista(argv,argc);
}