#include <swilib.h>
#include <pmb887x.h>

typedef struct {
	CSM_RAM csm;
	int gui_id;
} MAIN_CSM;

typedef struct {
	GUI base;
	WSHDR *ws;
	GBSTMR tmr;
} MAIN_GUI;

static void on_timer(GBSTMR *tmr) {
	SUBPROC(REDRAW);
	GBS_StartTimerProc(tmr, 50, on_timer);
}

static void gui_oncreate(GUI *data, malloc_func_t malloc_fn) {
	MAIN_GUI *gui = (MAIN_GUI *) data;
	
	gui->base.state = CSM_GUI_STATE_UNFOCUSED;
	
	gui->ws = AllocWS(128);
	GBS_StartTimerProc(&gui->tmr, 50, on_timer);
}

static void gui_onredraw(GUI *data) {
	MAIN_GUI *gui = (MAIN_GUI *) data;
	
	const char bg[] = { 0xFF, 0xFF, 0xFF, 0x64 };
	const char fg[] = { 0, 0, 0, 0x64 };
	
	volatile uint32_t ports[3] = {KEYPAD_PORT(0), KEYPAD_PORT(1), KEYPAD_PORT(2)};
	wsprintf(gui->ws, "PORT0: %08X\nPORT1: %08X\nPORT2: %08X", ports[0], ports[1], ports[2]);
	
	DrawRoundedFrame(0, 0, ScreenW() - 1, ScreenH() - 1, 0, 0, 0, bg, bg);
	DrawString(gui->ws, 0, 0, ScreenW() - 1, ScreenH() - 1, FONT_MEDIUM, TEXT_ALIGNMIDDLE, fg, NULL);
}

static void gui_onfocus(GUI *data, malloc_func_t malloc_fn, mfree_func_t mfree_fn) {
	MAIN_GUI *gui = (MAIN_GUI *) data;
	
	gui->base.state = CSM_GUI_STATE_FOCUSED;
	
	DisableIDLETMR();
	
#ifdef ELKA
	DisableIconBar(1);
#endif
}

static void gui_onunfocus(GUI *data, mfree_func_t mfree_fn) {
	MAIN_GUI *gui = (MAIN_GUI *) data;
	
	if (gui->base.state != CSM_GUI_STATE_FOCUSED)
		return;
	
	gui->base.state = CSM_GUI_STATE_UNFOCUSED;
}

static int gui_onkey(GUI *data, GUI_MSG *msg) {
	MAIN_GUI *gui = (MAIN_GUI *) data;
	
	if ((msg->gbsmsg->msg == KEY_DOWN || msg->gbsmsg->msg == LONG_PRESS)) {
		switch (msg->gbsmsg->submess) {
			case RIGHT_SOFT:
			return 1;
		}
		REDRAW();
	}
	return 0;
}

static void gui_onclose(GUI *data, mfree_func_t mfree_fn) {
	MAIN_GUI *gui = (MAIN_GUI *) data;
	
	gui->base.state = CSM_GUI_STATE_CLOSED;
	
	GBS_DelTimer(&gui->tmr);
	
	FreeWS(gui->ws);
}

static int gui_method8(void) {
	return 0;
}

static int gui_method9(void) {
	return 0;
}

static GUI_METHODS gui_methods = {
	gui_onredraw,
	gui_oncreate,
	gui_onclose,
	gui_onfocus,
	gui_onunfocus,
	gui_onkey,
	NULL,
	kill_data,
	gui_method8,
	gui_method9,
	NULL,
};

static RECT gui_rect;

static void maincsm_oncreate(CSM_RAM *data) {
	MAIN_CSM *csm = (MAIN_CSM *) data;
	
	csm->csm.state = CSM_STATE_OPEN;
	csm->csm.unk1 = 0;
	
	StoreXYWHtoRECT(&gui_rect, 0, 0, ScreenW(), ScreenH());
	
	MAIN_GUI *gui = malloc(sizeof(MAIN_GUI));
	zeromem(gui, sizeof(MAIN_GUI));
	gui->base.canvas = &gui_rect;
	gui->base.methods = &gui_methods;
	gui->base.item_ll.data_mfree = mfree_adr();
	
	csm->gui_id = CreateGUI(gui);
}

static void maincsm_onclose(CSM_RAM *csm) {
	kill_elf();
}

static int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg) {
	MAIN_CSM *csm = (MAIN_CSM *) data;
	if ((msg->msg == MSG_GUI_DESTROYED) && ((int) msg->data0 == csm->gui_id)) {
		csm->csm.state = CSM_STATE_CLOSED;
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

static void UpdateCSMname(void) {
	wsprintf(&MAINCSM.maincsm_name, "Hello World CSM");
}

int main(char *exe, char *fname, void *p1) {
	MAIN_CSM main_csm;
	LockSched();
	UpdateCSMname();
	CreateCSM(&MAINCSM.maincsm, &main_csm, 0);
	UnlockSched();
	
	return 0;
}
