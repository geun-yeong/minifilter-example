#include "miniflt.h"

FLT_REGISTRATION registration = {
	sizeof(FLT_REGISTRATION),          // size
	FLT_REGISTRATION_VERSION,          // version
	0,                                 // flags
	NULL,                              // context registration
	operations,                        // operation registration
	MinifltExampleFilterUnloadRoutine, // filter unload callback
	NULL,                              // instance setup callback
	NULL,                              // instance query teardown callback
	NULL,                              // instance teardown start callback
	NULL,                              // instance teardown complete callback
	NULL,                              // generate file name callback
	NULL,                              // normalize name component callback
	NULL,                              // normalize context cleanup callback
	NULL,                              // transaction notification callback
	NULL,                              // normalize name component ex callback
	NULL                               // section notification callback
};