#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <swilib.h>

#include "usart.h"

typedef struct {
	CSM_RAM csm;
	int gui_id;
} MAIN_CSM;

typedef struct {
	GUI base;
	WSHDR *ws;
} MAIN_GUI;

static uint16_t prev_bitmap[128 * 128];
static uint32_t prev_bitmap_size = 0;
static int g_state = 0;
static int g_unicode_off = 0;
static int g_font = 0;

static void gui_oncreate(GUI *data, malloc_func_t malloc_fn) {
	MAIN_GUI *gui = (MAIN_GUI *) data;
	
	gui->base.state = CSM_GUI_STATE_UNFOCUSED;
	
	gui->ws = AllocWS(128);
}

static void gui_onredraw(GUI *data) {
	MAIN_GUI *gui = (MAIN_GUI *) data;
	
	int x = 0;
	int y = 0;
	int font = g_font;
	
	const char bg[] = { 0xFF, 0x00, 0xFF, 0x64 };
	const char txt_bg[] = { 0xFF, 0xFF, 0xFF, 0x64 };
	const char txt_fg[] = { 0, 0, 0, 0x64 };
	
	uint16_t *screen = (uint16_t *) RamScreenBuffer();
	
	if (g_state == 2)
		return;
	
	if (g_state == 0) {
		DrawRoundedFrame(0, 0, ScreenW() - 1, ScreenH() - 1, 0, 0, 0, bg, bg);
	}
	
	int next_unicode_off = 0;
	
	for (int i = g_unicode_off; i <= 0xFFFF; i++) {
		/*
		if ((i >= 0x0000 && i <= 0xD7FF) || (i >= 0xE000 && i <= 0xFFFF)) {
			gui->ws->wsbody[0] = 1;
			gui->ws->wsbody[1] = i;
		} else {
			gui->ws->wsbody[0] = 2;
			uint32_t double_codepoint = i - 0x10000;
			gui->ws->wsbody[1] = ((double_codepoint >> 10) & 0x3FF) | 0xD800;
			gui->ws->wsbody[2] = (double_codepoint & 0x3FF) | 0xDC00;
		}
		*/
		
		gui->ws->wsbody[0] = 1;
		gui->ws->wsbody[1] = i;
		
		int w = Get_WS_width(gui->ws, font);
		int w2 = GetSymbolWidth(i, font);
		
		if (w != w2) {
			printf("U+%04X: %d != %d\r\n", i, w, w2);
		//	g_state = 2;
		//	return;
		}
		
		int h = GetFontYSIZE(font);
		
		if ((i & 0xFF00) == 0xE100) { // smiles & icons
			/*
			int id = GetPicNByUnicodeSymbol(i);
			if (id != 0xFFFF) {
				w = GetImgWidth(i);
				h = GetImgHeight(i);
			}
			*/
			int id = GetPicNByUnicodeSymbol(i);
			printf("%X,%d,%d#%d\r\n", i, w, h, id);
			continue;
		}
		
		if ((i & 0xFF00) == 0xE200) { // dyn images
			/*
			IMGHDR *img = GetPitAdrBy0xE200Symbol(i);
			if (img) {
				w = img->w;
				h = img->h;
			}
			*/
			continue;
		}
		
		if (x + w > ScreenW()) {
			x = 0;
			y += h;
		}
		
		if (y + h > ScreenH())
			break;
		
		if (g_state == 0) {
			DrawString(gui->ws, x, y, x + w, y + h, font, TEXT_ALIGNLEFT, txt_bg, txt_fg);
		} else if (g_state == 1) {
			next_unicode_off = i + 1;
			
			uint16_t *pixels = malloc(w * h * 2);
			char *b64 = malloc(w * h * 4 + 1);
			
			for (int ch_y = 0; ch_y < h; ch_y++) {
				for (int ch_x = 0; ch_x < w; ch_x++) {
					int s_x = x + ch_x;
					int s_y = y + ch_y;
					
					pixels[ch_x + ch_y * w] = screen[s_y * ScreenW() + s_x];
				}
			}
			
			if (prev_bitmap_size == w * h * 2 && memcmp(prev_bitmap, pixels, w * h * 2) == 0) {
				printf("%X,%d,%d&\r\n", i, w, h);
			} else {
				int n = Base64Encode(pixels, w * h * 2, b64, w * h * 4);
				b64[n] = 0;
				printf("%X,%d,%d:%s\r\n", i, w, h, b64);
				
				prev_bitmap_size = w * h * 2;
				memcpy(prev_bitmap, pixels, prev_bitmap_size);
			}
			
			free(pixels);
			free(b64);
		}
		
		x += w;
	}
	
	if (g_state == 0) {
		g_state = 1;
		SUBPROC((void*) REDRAW);
	} else if (g_state == 1) {
		g_unicode_off = next_unicode_off;
		
		if (g_unicode_off <= 0xFFFF) {
			g_state = 0;
			SUBPROC((void*) REDRAW);
		} else {
			if (g_font < 8) {
				g_font++;
				printf("FONT:%d\r\n", g_font);
				g_state = 0;
				SUBPROC((void*) REDRAW);
			} else {
				printf("DONE\r\n");
				g_state = 2;
			}
		}
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
		switch (msg->gbsmsg->submess) {
			case RIGHT_SOFT:
			return 1;
			
			case '5':
				REDRAW();
			break;
			
			case '1':
				ShowMSG(1, (int) "Текст в cp1251!");
			break;
		}
	}
	return 0;
}

static void gui_onclose(GUI *data, mfree_func_t mfree_fn) {
	MAIN_GUI *gui = (MAIN_GUI *) data;
	
	gui->base.state = CSM_GUI_STATE_CLOSED;
	
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
	
	printf("FONT:%d\n", g_font);
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

static ssize_t my_write(int fd, const void *buf, size_t sz) {
	const uint8_t *u8 = (const uint8_t *) buf;
	for (int i = 0; i < sz; i++)
		usart_putc(u8[i]);
	return sz;
}

int main(char *exe, char *fname, void *p1) {
	stdout = fsetopen(-1, O_WRONLY, 0, my_write, 0, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	
	usart_init();
	usart_set_speed(1600000);
	
	printf("Running main()...\r\n");
	
	MAIN_CSM main_csm;
	LockSched();
	UpdateCSMname();
	CreateCSM(&MAINCSM.maincsm, &main_csm, 0);
	UnlockSched();
	
	return 0;
}
