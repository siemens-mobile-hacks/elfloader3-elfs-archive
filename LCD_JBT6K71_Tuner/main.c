#include <swilib.h>
#include <pmb887x.h>
#include "sys.h"

typedef struct {
	CSM_RAM csm;
	int gui_id;
} MAIN_CSM;

typedef struct {
	GUI base;
	WSHDR *ws;
	GBSTMR tmr;
} MAIN_GUI;

static bool error = false;
static int old_reinit_cnt = 0;
static int cnt = 0;
static bool ac_mode = true;
static bool blue_off_enabled = false;
static bool hide_osd = false;
static int curr_display_index = 0;
static IMGHDR *image = NULL;
static int display_values[] = {
	// power
	0x06, 0x0F, 0x00,
	
	// gamma
	0x00, 0x02,
	0x02, 0x00,
	0x00, 0x00,
	0x00, 0x03,
	0x00, 0x07,
	
	// blue off
	0x0, 0x7
};

static void test_lcd() {
	const char *model = Get_Phone_Info(PI_MODEL);
	const char *sw = Get_Phone_Info(PI_SW_NUMBER);
	
	void (*dif_write_CMD)(int cmd);
	void (*dif_write_DATA)(int cmd);
	void (*getDisplayInfo)(void *);
	
	if (strcmp(model, "EL71") == 0 && strcmp(sw, "45") == 0) {
		dif_write_CMD= (void (*)(int)) 0xA077EAEC;
		dif_write_DATA = (void (*)(int)) 0xA077EB08;
		getDisplayInfo = (void (*)(void *)) 0xA04BBE18;
	} else if (strcmp(model, "E71") == 0 && strcmp(sw, "45") == 0) {
		dif_write_CMD = (void (*)(int)) 0xA076B650;
		dif_write_DATA = (void (*)(int)) 0xA076B66C;
		getDisplayInfo = (void (*)(void *)) 0xA04B5A50;
	} else {
		error = 1;
		return;
	}
	
	char display_info[0x20];
	display_info[0] = 2;
	display_info[1] = 0xE;
	getDisplayInfo(display_info);
	
	int reinit_cnt = display_info[1] + display_info[2] * 0x100;
	
	if (old_reinit_cnt == reinit_cnt)
		return;
	
	system_mode();
	disable_irq();
	uint32_t old_sleep_mode = PLL_CON3 & 1;
	PLL_CON3 &= ~1;
	
	uint32_t old_clc = DIF_CLC;
	if (!(DIF_CLC & MOD_CLC_RMC)) {
		old_clc = DIF_CLC;
		DIF_CLC = 0x100;
	}
	
	if (!DIF_RUNCTRL) {
		if (ac_mode) {
			dif_write_CMD(0x2);
			dif_write_DATA(0x200);
		} else {
			dif_write_CMD(0x2);
			dif_write_DATA(0);
		}
		
		dif_write_CMD(0x102);
		dif_write_DATA((display_values[2] << 8) | (display_values[1] << 4) | display_values[0]);
		
		dif_write_CMD(0x300);
		dif_write_DATA((display_values[4] << 8) | display_values[3]);
		
		dif_write_CMD(0x301);
		dif_write_DATA((display_values[6] << 8) | display_values[5]);
		
		dif_write_CMD(0x302);
		dif_write_DATA((display_values[8] << 8) | display_values[7]);
		
		dif_write_CMD(0x303);
		dif_write_DATA((display_values[10] << 8) | display_values[9]);
		
		dif_write_CMD(0x304);
		dif_write_DATA((display_values[12] << 8) | display_values[11]);
		
		dif_write_CMD(0x305);
		dif_write_DATA((display_values[14] << 4) | display_values[13] | (blue_off_enabled ? (1 << 8) : 0));
		
		old_reinit_cnt = reinit_cnt;
	}
	
	DIF_CLC = old_clc;
	
	PLL_CON3 |= old_sleep_mode;
	enable_irq();
}

static void on_timer(GBSTMR *tmr) {
	SUBPROC((void *) test_lcd);
	GBS_StartTimerProc(tmr, 2, on_timer);
}

static void gui_oncreate(GUI *data, malloc_func_t malloc_fn) {
	MAIN_GUI *gui = (MAIN_GUI *) data;
	
	gui->base.state = CSM_GUI_STATE_UNFOCUSED;
	
	gui->ws = AllocWS(128);
	GBS_StartTimerProc(&gui->tmr, 2, on_timer);
}

static void loadImage(const char *path) {
	if (image) {
		free(image->bitmap);
		free(image);
		image = NULL;
	}
	image = CreateIMGHDRFromPngFile(path, 0);
}

static void gui_onredraw(GUI *data) {
	MAIN_GUI *gui = (MAIN_GUI *) data;
	
	const char bg[] = { 0xFF, 0xFF, 0xFF, 0x64 };
	const char black[] = { 0, 0, 0, 0x64 };
	const char yellow[] = { 0xFF, 0xFF, 0, 0x64 };
	const char yellow_alpha[] = { 0xFF, 0xFF, 0, 0x64 / 2 };
	const char transparent[] = { 0, 0, 0, 0 };
	
	if (!image) {
		loadImage("0:\\Pictures\\wallpaper.png");
	}
	
	if (image) {
		DRWOBJ drw;
		RECT rc = { 0, 0, ScreenW() - 1, ScreenH() - 1 };
		SetProp2ImageOrCanvas(&drw, &rc, 0, image, 0, 0);
		DrawObject(&drw);
	}
	
	if (error) {
		wsprintf(gui->ws, "Your phone model is not supported.");
		DrawString(gui->ws, 5, 5, ScreenW() - 5, ScreenH() - 5, FONT_SMALL, 0, yellow, transparent);
	} else if (!hide_osd) {
		wsprintf(gui->ws, "%cVGM:%X %cVCS:%X %cVCOM:%X%c",
			(curr_display_index == 0 ? UTF16_UNDERLINE : UTF16_NO_UNDERLINE),
			display_values[0],
			(curr_display_index == 1 ? UTF16_UNDERLINE : UTF16_NO_UNDERLINE),
			display_values[1],
			(curr_display_index == 2 ? UTF16_UNDERLINE : UTF16_NO_UNDERLINE),
			display_values[2],
			UTF16_NO_UNDERLINE
		);
		DrawString(gui->ws, 5, 5, ScreenW() - 5, ScreenH() - 5, FONT_SMALL, 0, yellow, transparent);
		
		wsprintf(gui->ws, "GM:%c %X%c %X%c %X%c %X%c %X%c %X%c %X%c %X%c %X%c %X%c",
			(curr_display_index == 3 ? UTF16_UNDERLINE : UTF16_NO_UNDERLINE),
			display_values[3],
			(curr_display_index == 4 ? UTF16_UNDERLINE : UTF16_NO_UNDERLINE),
			display_values[4],
			(curr_display_index == 5 ? UTF16_UNDERLINE : UTF16_NO_UNDERLINE),
			display_values[5],
			(curr_display_index == 6 ? UTF16_UNDERLINE : UTF16_NO_UNDERLINE),
			display_values[6],
			(curr_display_index == 7 ? UTF16_UNDERLINE : UTF16_NO_UNDERLINE),
			display_values[7],
			(curr_display_index == 8 ? UTF16_UNDERLINE : UTF16_NO_UNDERLINE),
			display_values[8],
			(curr_display_index == 9 ? UTF16_UNDERLINE : UTF16_NO_UNDERLINE),
			display_values[9],
			(curr_display_index == 10 ? UTF16_UNDERLINE : UTF16_NO_UNDERLINE),
			display_values[10],
			(curr_display_index == 11 ? UTF16_UNDERLINE : UTF16_NO_UNDERLINE),
			display_values[11],
			(curr_display_index == 12 ? UTF16_UNDERLINE : UTF16_NO_UNDERLINE),
			display_values[12],
			UTF16_NO_UNDERLINE
		);
		DrawString(gui->ws, 5, 10 + GetFontYSIZE(FONT_SMALL), ScreenW() - 5, ScreenH() - 5, FONT_SMALL, 0, yellow, transparent);
		
		wsprintf(gui->ws, "BOFF:%c %X%c %X%c %t | AC: %d",
			(curr_display_index == 13 ? UTF16_UNDERLINE : UTF16_NO_UNDERLINE),
			display_values[13],
			(curr_display_index == 14 ? UTF16_UNDERLINE : UTF16_NO_UNDERLINE),
			display_values[14],
			UTF16_NO_UNDERLINE,
			(blue_off_enabled ? "(ON)" : "(OFF)"),
			ac_mode
		);
		DrawString(gui->ws, 5, 15 + GetFontYSIZE(FONT_SMALL) * 2, ScreenW() - 5, ScreenH() - 5, FONT_SMALL, 0, yellow, transparent);
	}
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
		old_reinit_cnt = -1;
		
		switch (msg->gbsmsg->submess) {
			case RIGHT_SOFT:
			return 1;
			
			case RIGHT_BUTTON:
				curr_display_index++;
				if (curr_display_index >= (sizeof(display_values) / sizeof(display_values[0])))
					curr_display_index = 0;
			break;
			
			case LEFT_BUTTON:
				curr_display_index--;
				if (curr_display_index < 0)
					curr_display_index = (sizeof(display_values) / sizeof(display_values[0]) - 1);
			break;
			
			case UP_BUTTON:
				display_values[curr_display_index]++;
				if (display_values[curr_display_index] > 0xF)
					display_values[curr_display_index] = 0xF;
			break;
			
			case DOWN_BUTTON:
				display_values[curr_display_index]--;
				if (display_values[curr_display_index] < 0)
					display_values[curr_display_index] = 0;
			break;
			
			case '0':
				loadImage("0:\\Pictures\\wallpaper.png");
			break;
			
			case '1':
				loadImage("0:\\Pictures\\wallpaper-1.png");
			break;
			
			case '2':
				loadImage("0:\\Pictures\\wallpaper-2.png");
			break;
			
			case '3':
				loadImage("0:\\Pictures\\wallpaper-3.png");
			break;
			
			case '9':
				ac_mode = !ac_mode;
			break;
			
			case '#':
				blue_off_enabled = !blue_off_enabled;
			break;
			
			case '*':
				hide_osd = !hide_osd;
			break;
	
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
	
	if (image) {
		free(image->bitmap);
		free(image);
		image = NULL;
	}
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
	wsprintf(&MAINCSM.maincsm_name, "LCD crack.elf");
}

int main(char *exe, char *fname, void *p1) {
	MAIN_CSM main_csm;
	LockSched();
	UpdateCSMname();
	CreateCSM(&MAINCSM.maincsm, &main_csm, 0);
	UnlockSched();
	
	return 0;
}
