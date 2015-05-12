// external success code
#define SUCCESS						0
#define SUCCESS_SOME_ERRORS			SUCCESS + 1

// internally used
#define PARAMETERS_OK				101
#define STEP_SUCCESS				102
#define STEP_BADSELECTOR			103
#define STEP_KEYVALUENOTFOUND		104

// possible external return codes
#define ERR_BASE					SUCCESS_SOME_ERRORS
#define ERR_NO_PARAMETERS			ERR_BASE + 1
#define ERR_MISSEDARG				ERR_BASE + 2
#define ERR_ILLEGAL_OPTION			ERR_BASE + 3
#define ERR_NO_OPTION_HANDLER		ERR_BASE + 4

#define ERR_FAILED					ERR_BASE + 10
#define ERR_INIT_FAILED				ERR_BASE + 11