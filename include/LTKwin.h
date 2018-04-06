#ifndef __LTKWIN_H__
#define __LTKWIN_H__

#define LTKDPY_FLUSH			0

#define LTKWND_EVENT			0
#define LTKWND_GETKEY			1
#define LTKWND_GETBTN			2
#define LTKWND_SWAP				3
#define LTKSCN_FOCUS			4
#define LTKSCN_LOOK				5

#define LTKIMG_GETDATA			0

struct LTKwindow_s;
struct LTKscelem_s;

typedef struct LTKdisplay_s {
	LTKobj _obj;

	struct LTKwindow_s *_slv;

	Display *_dpy;
	XEvent _lst;
	XVisualInfo _vfo[2];
	XSetWindowAttributes _atr[2];

	int _scn;
	int _msx;
	int _msy;

	Atom _WMm[2];

	uint8_t _Keypad[64];
} LTKdisplay;

typedef struct LTKwindow_s {
	LTKobj _obj;

	struct LTKwindow_s *_slv; /* Can be an LTKscene * */
	struct LTKwindow_s *_mtr; /* Is the LTKdisplay * if parent is root window */
	struct LTKwindow_s *_prv;
	struct LTKwindow_s *_nxt;

	LTKdisplay *_dpy;
	Window _wnd;

	int _x;
	int _y;
	int _w;
	int _h;
	int _msx;
	int _msy;
	uint16_t _btn;

#ifdef LTK_XDBE_SUPPORT
	XdbeBackBuffer _bbf;
#endif
	GC _gc;
} LTKwindow;

typedef struct LTKscene_s {
	LTKobj _obj;

	struct LTKwindow_s *_slv;
	struct LTKwindow_s *_mtr;
	struct LTKwindow_s *_prv;
	struct LTKwindow_s *_nxt;

	LTKdisplay *_dpy;
	Window _wnd;

	int _x;
	int _y;
	int _w;
	int _h;
	int _msx;
	int _msy;
	uint16_t _btn;

	struct LTKscelem_s *_mvw; /* First element in a model view */
	GLXContext _glc;
	int _fd;

	uint8_t _stat;
} LTKscene;

typedef struct LTKimage_s {
	LTKobj _obj;

	LTKdisplay *_dpy;
	XImage *_img;
	XShmSegmentInfo _shm;

	uint32_t **_data;
	uint8_t _stat;
} LTKimage;

LTKctr *LTKDpyCtr();
LTKdisplay *LTKDpyOf(LTKwindow *wnd);
LTKdisplay *LTKDpy(char *dpyname);

LTKctr *LTKWndCtr();
LTKwindow *LTKWnd(void *, char *, int, int, int, int, uint32_t);

LTKctr *LTKScnCtr();
LTKscene *LTKScn(LTKwindow *, int, int, int, int, uint32_t);

LTKctr *LTKImgCtr();
LTKimage *LTKImg(void *, int, int);

void LTKBlit(void *, void *, int, int, int, int, unsigned int, unsigned int);

LTKimage *LTKImgLoad(void *, const char *, int *);
int LTKImgSave(LTKimage *, int);

#endif /* __LTKWIN_H__ */
