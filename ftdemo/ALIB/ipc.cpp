#include "platform.h"
//#include "ui.h"


IPC_REQ msg;
void SendIPC (char *from, char *to, int ipc, void* data){

  msg.name_to=to;
  msg.name_from=from;
  msg.data=data;
  GBS_SendMessage (MMI_CEPID, MSG_IPC, ipc, &msg);
}


//SendIPC (ALIB_CLIENT, ALIB_SERVER, IPC_GET_BG, 0);
