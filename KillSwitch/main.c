#include <swilib.h>
#include <nu_swilib.h>
#include <pmb887x.h>

#include "stopwatch.h"
#include "conf_loader.h"

typedef struct {
	CSM_RAM csm;
	int gui_id;
} MAIN_CSM;

static uint64_t last_press_time = 0;
static uint32_t config_keys[3] = { 0 };
static NU_TIMER timer;

static void update_settings() {
	InitConfig();
	sscanf(PORT0_STR_VAL, "%08X", &config_keys[0]);
	sscanf(PORT1_STR_VAL, "%08X", &config_keys[1]);
	sscanf(PORT2_STR_VAL, "%08X", &config_keys[2]);
}

static void on_timer_expire(unsigned long id) {
	bool pressed = true;
	for (int j = 0; j < 3; j++) {
		if (KEYPAD_PORT(j) != config_keys[j]) {
			pressed = false;
			break;
		}
	}
	
	int next_timeout = TIMER_PERIOD;
	if (pressed) {
		if (!last_press_time)
			last_press_time = stopwatch_get();
		
		if (stopwatch_elapsed_ms(last_press_time) >= PRESS_TIMEOUT) {
			volatile void (*reset_func)() = (void (*)()) 0xDEAD926E;
			reset_func();
		}
		next_timeout = PRESS_TIMEOUT / 2;
	} else {
		last_press_time = 0;
	}
	
	if (next_timeout < 100)
		next_timeout = 100;
	
	// 1 period = 4.615ms
	NU_Reset_Timer(&timer, on_timer_expire, next_timeout * 1000 / 4615, 0, NU_ENABLE_TIMER);
}

static void maincsm_oncreate(CSM_RAM *data) {
	MAIN_CSM *csm = (MAIN_CSM *) data;
	
	csm->csm.state = CSM_STATE_OPEN;
	csm->csm.unk1 = 0;
	
	int ret = NU_Create_Timer(&timer, "KillSwitchTimer", on_timer_expire, 0, 1, 0, NU_ENABLE_TIMER);
	if (ret != 0) {
		ShowMSG(0, (int) "NU timer create error!!!");
		csm->csm.state = CSM_STATE_CLOSED;
	}
}

static void maincsm_onclose(CSM_RAM *data) {
	MAIN_CSM *csm = (MAIN_CSM *) data;
	NU_Control_Timer(&timer, NU_DISABLE_TIMER);
	NU_Delete_Timer(&timer);
	kill_elf();
}

static int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg) {
	MAIN_CSM *csm = (MAIN_CSM *) data;
	if ((msg->msg == MSG_GUI_DESTROYED) && ((int) msg->data0 == csm->gui_id)) {
		csm->csm.state = CSM_STATE_CLOSED;
	}
	
	if (msg->msg == MSG_RECONFIGURE_REQ) {
		if (strcmpi(successed_config_filename, (char *) msg->data0) == 0) {
			update_settings();
			NU_Reset_Timer(&timer, on_timer_expire, 1, 0, NU_ENABLE_TIMER);
			ShowMSG(1, (int) "KillSwitch config updated!");
        }
    }
	
	return 1;
}

static const int minus11 = -11;
static uint16_t maincsm_name_body[140];

static const struct {
	CSM_DESC maincsm;
	WSHDR maincsm_name;
} MAINCSM = {
	.maincsm = {
		maincsm_onmessage,
		maincsm_oncreate,
#ifdef NEWSGOLD
		0, 0, 0, 0, 
#endif
		maincsm_onclose,
		sizeof(MAIN_CSM),
		1, &minus11
	},
	.maincsm_name = { maincsm_name_body, NAMECSM_MAGIC1, NAMECSM_MAGIC2, 0x0, sizeof(maincsm_name_body) - 1, 0}
};

int main(char *exe, char *fname, void *p1) {
	stopwatch_init();
	update_settings();
	wsprintf(&MAINCSM.maincsm_name, "KillSwitchD");
	
	MAIN_CSM main_csm;
	LockSched();
	CSM_RAM *save_cmpc = CSM_root()->csm_q->current_msg_processing_csm;
	CSM_root()->csm_q->current_msg_processing_csm = CSM_root()->csm_q->csm.first;
	CreateCSM(&MAINCSM.maincsm, &main_csm, 0);
	CSM_root()->csm_q->current_msg_processing_csm = save_cmpc;
	UnlockSched();
	
	return 0;
}
