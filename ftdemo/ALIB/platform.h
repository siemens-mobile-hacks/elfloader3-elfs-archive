#include "include.h"

//#define WIN
#define SIEMENS

#define ALIB_VER "\n based on\nALib 0.7alpha"

#define ALIB_SERVER "ALibServer"
#define ALIB_CLIENT "ALibClient"

#define IPC_GET_BG 0
#define IPC_SET_BG 1

#define IPC_GET_FUNC 3
#define IPC_SET_FUNC 4

void SendIPC (char *from, char *to, int ipc, void* data);
void GetIPC_Client (GBS_MSG *msg);
void GetIPC_Server (GBS_MSG *msg);

