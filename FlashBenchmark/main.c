#include <swilib.h>
#include <pmb887x.h>

#include "sys.h"
#include "stopwatch.h"

#define BENCH_MMC_WR_SIZE	4 * 1024 * 1024
#define BENCH_MMC_RD_SIZE	4 * 1024 * 1024

#define BENCH_FFS_WR_SIZE	4 * 1024 * 1024
#define BENCH_FFS_RD_SIZE	4 * 1024 * 1024

typedef struct {
	CSM_RAM csm;
	int gui_id;
} MAIN_CSM;

typedef struct {
	GUI base;
	WSHDR *ws;
} MAIN_GUI;

enum {
	BENCH_WRITE,
	BENCH_READ,
};

enum {
	BENCH_STATE_NONE = 0,
	BENCH_STATE_DONE,
	BENCH_STATE_SELECT_CHUNK,
	BENCH_STATE_ERROR,
	BENCH_STATE_DEBUG,
	BENCH_STATE_PREPARING,
	BENCH_STATE_WORKING,
};

typedef struct {
	int type;
	int size;
	int chunk;
	char disk;
} BenchParams;

typedef struct {
	BenchParams params;
	int state;
	uint32_t speed;
	uint32_t written;
	uint32_t readed;
	uint32_t elapsed;
	const char *error;
} BenchState;

static BenchState bench_state = {};

static void benchmark(void) {
	BenchParams *p = &bench_state.params;
	
	bool error = false;
	uint32_t err;
	int written = 0;
	int readed = 0;
	uint8_t *tmp = malloc(p->chunk);
	
	char tmp_filename[4096];
	sprintf(tmp_filename, "%c:\\test.bin", p->disk);
	
	switch (p->type) {
		case BENCH_WRITE:
		{
			_unlink(tmp_filename, &err);
			
			uint64_t start = stopwatch_get();
			
			bench_state.state = BENCH_STATE_WORKING;
			REDRAW();
			
			int fd = _open(tmp_filename, A_WriteOnly + A_BIN + A_Create, P_WRITE, 0);
			if (fd >= 0) {
				while (written < p->size) {
					int ret = _write(fd, tmp, p->chunk, &err);
					if (ret < 0) {
						bench_state.error = "IO error, _write() failed";
						error = true;
						break;
					}
					written += ret;
				}
				_close(fd, &err);
			} else {
				bench_state.error = "Can't open test.bin for writing!";
				error = true;
			}
			
			uint32_t elapsed = stopwatch_elapsed_ms(start);
			if (error) {
				bench_state.state = BENCH_STATE_ERROR;
			} else {
				bench_state.elapsed = elapsed;
				bench_state.written = written / 1024;
				bench_state.speed = written * 1000 / elapsed / 1024;
				bench_state.state = BENCH_STATE_DONE;
			}
			REDRAW();
		}
		break;
		
		case BENCH_READ:
		{
			uint64_t start = stopwatch_get();
			
			bench_state.state = BENCH_STATE_WORKING;
			REDRAW();
			
			int fd = _open(tmp_filename, A_ReadOnly + A_BIN, P_READ, 0);
			if (fd >= 0) {
				while (readed < p->size) {
					int ret = _read(fd, tmp, p->chunk, &err);
					if (ret < 0) {
						bench_state.error = "IO error, _read() failed";
						error = true;
						break;
					}
					readed += ret;
				}
				_close(fd, &err);
			} else {
				bench_state.error = "Cannot open test.bin for reading! (please run write test first!)";
				error = true;
			}
			
			uint32_t elapsed = stopwatch_elapsed_ms(start);
			if (error) {
				bench_state.state = BENCH_STATE_ERROR;
			} else {
				bench_state.elapsed = elapsed;
				bench_state.readed = readed / 1024;
				bench_state.speed = readed * 1000 / elapsed / 1024;
				bench_state.state = BENCH_STATE_DONE;
			}
			REDRAW();
		}
		break;
	}
	
	free(tmp);
}

static void gui_oncreate(GUI *data, malloc_func_t malloc_fn) {
	MAIN_GUI *gui = (MAIN_GUI *) data;
	
	gui->base.state = CSM_GUI_STATE_UNFOCUSED;
	gui->ws = AllocWS(1024);
}

static void gui_onredraw(GUI *data) {
	MAIN_GUI *gui = (MAIN_GUI *) data;
	
	const char bg[] = { 0xFF, 0xFF, 0xFF, 0x64 };
	const char fg[] = { 0, 0, 0, 0x64 };
	
	switch (bench_state.state) {
		case BENCH_STATE_NONE:
			wsprintf(gui->ws,
				"At least 4 MB of free space required!!!!\n\n"
				"[1] Read bench (SD)\n"
				"[2] Write bench (SD)\n"
				"[3] Read bench (FFS)\n"
				"[4] Write bench (FFS)\n"
				"[#] SDIO info\n"
				"[*] %s SD overclock",
				(mmci_read_reg(&MCI_CLOCK) & MCI_CLOCK_BYPASS) != 0 ? "Disable" : "Enable"
			);
		break;
		
		case BENCH_STATE_DEBUG:
			wsprintf(gui->ws,
				"MCI_CLOCK: %08X\n"
				"MCI_POWER: %08X\n"
				"MCI_DATATIMER: %08X\n",
				mmci_read_reg(&MCI_CLOCK),
				mmci_read_reg(&MCI_POWER),
				mmci_read_reg(&MCI_DATATIMER)
			);
		break;
		
		case BENCH_STATE_SELECT_CHUNK:
		{
			uint32_t val = bench_state.params.chunk;
			char *postfix = "";
			
			if (bench_state.params.chunk >= 1024 * 1024) {
				val = bench_state.params.chunk / 1024 / 1024;
				postfix = " Mb";
			} else if (bench_state.params.chunk >= 1024 * 10) {
				val = bench_state.params.chunk / 1024;
				postfix = " Kb";
			}
			
			wsprintf(gui->ws,
				"Select the chunk size (UP/DOWN)\n\n"
				"Chunk size: %d%s\n",
				val, postfix
			);
		}
		break;
		
		case BENCH_STATE_PREPARING:
			wsprintf(gui->ws, "Preparing....\n");
		break;
		
		case BENCH_STATE_WORKING:
			wsprintf(gui->ws, "Testing....\n");
		break;
		
		case BENCH_STATE_ERROR:
			wsprintf(gui->ws, "ERROR:\n%s\n", bench_state.error ?: "UNKNOWN");
		break;
		
		case BENCH_STATE_DONE:
		{
			if (bench_state.params.type == BENCH_WRITE) {
				wsprintf(gui->ws,
					"Disk: %c:\\\n"
					"Written: %u Kb\n"
					"Chunk %d\n"
					"Elapsed: %u ms\n"
					"Write speed: %u Kb/s\n",
					bench_state.params.disk,
					bench_state.written, bench_state.params.chunk, bench_state.elapsed, bench_state.speed
				);
			} else {
				wsprintf(gui->ws,
					"Disk: %c:\\\n"
					"Readed: %u Kb\n"
					"Chunk %d\n"
					"Elapsed: %u ms\n"
					"Read speed: %u Kb/s\n",
					bench_state.params.disk,
					bench_state.readed, bench_state.params.chunk, bench_state.elapsed, bench_state.speed
				);
			}
		}
		break;
	}
	
	DrawRoundedFrame(0, 0, ScreenW() - 1, ScreenH() - 1, 0, 0, 0, bg, bg);
	DrawString(gui->ws, 0, 0, ScreenW() - 1, ScreenH() - 1, FONT_SMALL, TEXT_ALIGNLEFT, fg, NULL);
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
				if (bench_state.state < BENCH_STATE_PREPARING)
					return 1;
			break;
		}
		
		if (bench_state.state == BENCH_STATE_NONE) {
			switch (msg->gbsmsg->submess) {
				case '1': // MMC read
				{
					bench_state.params.type = BENCH_READ;
					bench_state.params.size = BENCH_MMC_RD_SIZE;
					bench_state.params.chunk = 4096;
					bench_state.params.disk = '4';
					bench_state.state = BENCH_STATE_SELECT_CHUNK;
				}
				break;
				
				case '2': // MMC write
				{
					bench_state.params.type = BENCH_WRITE;
					bench_state.params.size = BENCH_MMC_RD_SIZE;
					bench_state.params.chunk = 4096;
					bench_state.params.disk = '4';
					bench_state.state = BENCH_STATE_SELECT_CHUNK;
				}
				break;
				
				case '3': // FFS read
				{
					bench_state.params.type = BENCH_READ;
					bench_state.params.size = BENCH_FFS_RD_SIZE;
					bench_state.params.chunk = 4096;
					bench_state.params.disk = '0';
					bench_state.state = BENCH_STATE_SELECT_CHUNK;
				}
				break;
				
				case '4': // FFS write
				{
					bench_state.params.type = BENCH_WRITE;
					bench_state.params.size = BENCH_FFS_RD_SIZE;
					bench_state.params.chunk = 4096;
					bench_state.params.disk = '0';
					bench_state.state = BENCH_STATE_SELECT_CHUNK;
				}
				break;
				
				case '*':
				{
					uint32_t clock = mmci_read_reg(&MCI_CLOCK);
					if ((clock & 0xFF) == 0) {
						if ((clock & MCI_CLOCK_BYPASS)) {
							clock &= ~MCI_CLOCK_BYPASS;
							mmci_write_reg(&MCI_DATATIMER, 0x3938700);
						} else {
							clock |= MCI_CLOCK_BYPASS;
							mmci_write_reg(&MCI_DATATIMER, 0x3938700*4);
						}
						mmci_write_reg(&MCI_CLOCK, clock);
					} else {
						char tmp[1024];
						sprintf(tmp, "Unknown MMCI_CLOCK: %d\n", (clock & 0xFF));
						ShowMSG(0, (int) tmp);
					}
				}
				break;
				
				case '#':
					bench_state.state = BENCH_STATE_DEBUG;
				break;
			}
		} else if (bench_state.state == BENCH_STATE_SELECT_CHUNK) {
			switch (msg->gbsmsg->submess) {
				case DOWN_BUTTON:
					bench_state.params.chunk = bench_state.params.chunk / 2;
					if (!bench_state.params.chunk)
						bench_state.params.chunk = 1;
				break;
				
				case UP_BUTTON:
					bench_state.params.chunk = bench_state.params.chunk * 2;
					if (bench_state.params.chunk >= bench_state.params.size)
						bench_state.params.chunk = bench_state.params.size;
				break;
				
				case ENTER_BUTTON:
					bench_state.state = BENCH_STATE_PREPARING;
					SUBPROC(benchmark);
				break;
			}
		}
		
		REDRAW();
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
	wsprintf(&MAINCSM.maincsm_name, "crack.elf");
}

int main(char *exe, char *fname, void *p1) {
	stopwatch_init();
	
	MAIN_CSM main_csm;
	LockSched();
	UpdateCSMname();
	CreateCSM(&MAINCSM.maincsm, &main_csm, 0);
	UnlockSched();
	
	return 0;
}
