#include <swilib.h>
#include <pmb887x.h>

#ifdef NEWSGOLD
	#define CBOX_CHECKED 0xE116
	#define CBOX_UNCHECKED 0xE117
#else
	#define CBOX_CHECKED 0xE10B
	#define CBOX_UNCHECKED 0xE10C
#endif

typedef struct {
	CSM_RAM csm;
	int gui_id;
} MAIN_CSM;

typedef struct {
	const char *type;
	const char *name;
	int *value;
} JVMSettingsItem;

static JVMSettingsItem *jvm_settings = NULL;

static HEADER_DESC header = {{0, 0, 0, 0}, NULL, (int) "JVM Settings", LGP_NULL};
static SOFTKEY_DESC softkeys[3] = {
	{0x0018, 0, LGP_NULL},
	{0x0001, 0, LGP_NULL},
	{0x003D, 0, LGP_DOIT_PIC},
};
static SOFTKEYSTAB softkeys_tab = { softkeys, 0 };
static INPUTDIA_DESC input_dialog = {};

static void update_jvm_settings(GUI *data) {
	char tmp[128];
	
	int setting_id = 0;
	
	JVMSettingsItem *setting = jvm_settings;
	while (setting->name) {
		EDITCONTROL ec;
		ExtractEditControl(data, setting_id * 2 + 2, &ec);
		
		int new_len;
		ws_2utf8(ec.pWS, tmp, &new_len, sizeof(tmp) - 1);
		tmp[new_len] = 0;
		
		if (strcmp(setting->type, "bool") == 0) {
			*setting->value = ec.pWS->wsbody[1] == CBOX_CHECKED ? 1 : 0;
		} else {
			*setting->value = strtol(tmp, NULL, 10);
		}
		
		setting++;
		setting_id++;
	}
}

static void ui_locret() {
	
}

static int ui_on_key(GUI *data, GUI_MSG *msg) {
	if ((msg->gbsmsg->msg == KEY_DOWN || msg->gbsmsg->msg == LONG_PRESS)) {
		switch (msg->gbsmsg->submess) {
			case ENTER_BUTTON:
			case RIGHT_SOFT:
			{
				int id_focused = EDIT_GetFocus(data);
				
				int setting_id = (id_focused / 2) - 1;
				if (strcmp(jvm_settings[setting_id].type, "bool") == 0) {
					EDITCONTROL ec;
					ExtractEditControl(data, id_focused, &ec);
					if (ec.pWS->wsbody[1] == CBOX_CHECKED) {
						ec.pWS->wsbody[1] = CBOX_UNCHECKED;
					} else {
						ec.pWS->wsbody[1] = CBOX_CHECKED;
					}
					StoreEditControl(data, id_focused, &ec);
					RefreshGUI();
				}
			}
			break;
		}
	}
	return 0;
}

static bool isJavaSettings(void *ptr) {
	JVMSettingsItem *settings = ptr;
	for (int i = 0; i < 3; i++) {
		if ((((uint32_t) settings[i].type & 0xF0000000) != 0xA0000000))
			return false;
		if ((((uint32_t) settings[i].name & 0xF0000000) != 0xA0000000))
			return false;
		if (memcmp(settings[i].type, "int", 3) != 0 && memcmp(settings[i].type, "bool", 4) != 0)
			return false;
	}
	return true;
}

static void *findJavaSettings() {
	const char *memory = (const char *) 0xA0000000;
	const char *memory_end = (const char *) 0xA1000000;
	
	while (memory < memory_end && (memory[0] != 'J' || memcmp(memory, "JVM_TICK_PROCESS", 16) != 0)) {
		memory += 4;
	}
	
	memory -= 0x1000;
	
	for (int i = 0; i < 0x2000; i += 4) {
		uint32_t addr = (uint32_t) &memory[i];
		if ((addr & 0xF0000000) == 0xA0000000 && addr <= 0xA1000000) {
			if (isJavaSettings((void *) addr))
				return (void *) addr;
		}
	}
	
	return NULL;
}

static void ui_on_command(GUI *data, int cmd) {
	if (cmd == TI_CMD_DESTROY) {
		update_jvm_settings(data);
	}
}

static void maincsm_oncreate(CSM_RAM *data) {
	MAIN_CSM *csm = (MAIN_CSM *) data;
	
	csm->csm.state = CSM_STATE_OPEN;
	csm->csm.unk1 = 0;
	
	EDITQ *eq = AllocEQueue(malloc_adr(), mfree_adr());
	WSHDR *ws = AllocWS(256);
	
	findJavaSettings();
	
	JVMSettingsItem *setting = jvm_settings;
	while (setting->name) {
		EDITCONTROL ec;
		
		wsprintf(ws, "%t (%s):", setting->name, setting->type ?: "readonly");
		PrepareEditControl(&ec);
		ConstructEditControl(&ec, ECT_HEADER, ECF_APPEND_EOL, ws, ws->wsbody[0]);
		AddEditControlToEditQend(eq, &ec, malloc_adr());
		
		if (strcmp(setting->type, "bool") == 0) {
			wsprintf(ws, "%c", *setting->value ? CBOX_CHECKED : CBOX_UNCHECKED);
			PrepareEditControl(&ec);
			ConstructEditControl(&ec, ECT_LINK, ECF_APPEND_EOL, ws, 64);
			AddEditControlToEditQend(eq, &ec, malloc_adr());
		} else {
			wsprintf(ws, "%d", *setting->value);
			PrepareEditControl(&ec);
			ConstructEditControl(&ec, ECT_NORMAL_NUM, (setting->type ? 0 : ECF_GRAY) | ECF_APPEND_EOL, ws, 64);
			AddEditControlToEditQend(eq, &ec, malloc_adr());
		}
		
		setting++;
	}
	
	FreeWS(ws);
	
	input_dialog.onKey = ui_on_key;
	input_dialog.global_hook_proc = ui_on_command;
	input_dialog.locret = ui_locret;
	input_dialog.softkeystab = &softkeys_tab;
	input_dialog.one = 1;
	input_dialog._100 = 100;
	input_dialog._101 = 101;
	input_dialog.font = FONT_SMALL;
	input_dialog._0x40000000 = 0x40000000;
	
	StoreXYXYtoRECT(&header.rc, 0, YDISP, ScreenW() - 1, HeaderH() + YDISP);
	StoreXYXYtoRECT(&input_dialog.rc, 0, HeaderH() + 1 + YDISP, ScreenW() - 1, ScreenH() - SoftkeyH() - 1);
	
	csm->gui_id = CreateInputTextDialog(&input_dialog, &header, eq, 1, NULL);
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
	wsprintf(&MAINCSM.maincsm_name, "JVM Settings");
}

int main(char *exe, char *fname, void *p1) {
	jvm_settings = (JVMSettingsItem *) findJavaSettings();
	if (!jvm_settings) {
		ShowMSG(0, (int) "JVM settings not found!");
		kill_elf();
		return 0;
	}
	
	MAIN_CSM main_csm;
	LockSched();
	UpdateCSMname();
	CreateCSM(&MAINCSM.maincsm, &main_csm, 0);
	UnlockSched();
	
	return 0;
}
