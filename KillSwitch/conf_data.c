#include <swilib.h>
#include <conf_loader.h>
#include <cfg_items.h>

__CFG_BEGIN(0)

__root const CFG_HDR cfghdr0 = { CFG_UINT, "Polling period (ms)", 100, 10000};
__root const unsigned int TIMER_PERIOD = 2000;

__root const CFG_HDR cfghdr1 = { CFG_UINT, "Press timeout (ms)", 0, 10000};
__root const unsigned int PRESS_TIMEOUT = 3000;

#if ELKA
__root const CFG_HDR cfghdr2 = { CFG_STR_WIN1251, "PORT0", 0, 8};
__root const char PORT0_STR_VAL[9] = "F7E7F7F7";

__root const CFG_HDR cfghdr3 = { CFG_STR_WIN1251, "PORT1", 0, 8};
__root const char PORT1_STR_VAL[9] = "F7F7F7F7";

__root const CFG_HDR cfghdr4 = { CFG_STR_WIN1251, "PORT2", 0, 8};
__root const char PORT2_STR_VAL[9] = "FFFFFFF7";
#elif NEWSGOLD
__root const CFG_HDR cfghdr2 = { CFG_STR_WIN1251, "PORT0", 0, 8};
__root const char PORT0_STR_VAL[9] = "FBEBFBFB";

__root const CFG_HDR cfghdr3 = { CFG_STR_WIN1251, "PORT1", 0, 8};
__root const char PORT1_STR_VAL[9] = "FBFBFBFB";

__root const CFG_HDR cfghdr4 = { CFG_STR_WIN1251, "PORT2", 0, 8};
__root const char PORT2_STR_VAL[9] = "FFFFFFFB";
#else
#error Not supported
#endif

__CFG_END(0)
