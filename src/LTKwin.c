#include"LTKlib.h"

void LTKDpyHdl(LTKobj **obj) {
	LTKdisplay *dpy = (LTKdisplay *) obj[0];
	unsigned char idx;
	char buf;

	/* Screen is about to be destroyed */
	if(!obj[0]->_buf) {
		int dpyfd = ConnectionNumber(dpy->_dpy);

		while(dpy->_slv) {
			LTKFre(&dpy->_slv->_obj);
		}

		if(dpy->_dpy) {
			XFreeColormap(dpy->_dpy, dpy->_atr[0].colormap);
			if(dpy->_atr[1].colormap) XFreeColormap(dpy->_dpy, dpy->_atr[1].colormap);
			XCloseDisplay(dpy->_dpy);
		}

		LTKTrg(0, dpyfd, 0);

		return;
	}

	if((buf = recv(ConnectionNumber(dpy->_dpy), &buf, 1, MSG_PEEK | MSG_DONTWAIT)) < 0) {
		LTKrint(0, "LTKDpyHdl : Connection probing failed : %s", strerror(errno));
	}

	if(!buf) {
		LTKrint(0, "LTKDpyHdl : Connection closed, there will be memory leak");
	}

	if(buf < 1) {
		dpy->_dpy = 0;

		LTKFre(obj[0]);
		return;
	}

	while(XPending(dpy->_dpy)) {
		XNextEvent(dpy->_dpy, &dpy->_lst);

		switch(dpy->_lst.type) {
			case KeymapNotify:
				for(idx = 0; idx < 32; idx++) {
					dpy->_Keypad[idx + 32] = dpy->_Keypad[idx];
					dpy->_Keypad[idx] = dpy->_lst.xkeymap.key_vector[idx];
				}

				LTKrint(1, "LTKDpyHdl : Keymap event happened "
						"on connection %d", XConnectionNumber(dpy->_dpy));
				break;

			case ClientMessage:
				if(dpy->_lst.xclient.message_type == dpy->_WMm[0]) {
					if(((unsigned long) dpy->_lst.xclient.data.l[0]) == dpy->_WMm[1]) {
						if(dpy->_slv) {
							if(!dpy->_slv->_nxt) {
								LTKFre(&dpy->_obj);
								obj[0] = &dpy->_obj;
								return;
							}
							else {
								LTKRun(&dpy->_slv->_obj, LTKWND_EVENT, obj);
							}
						}
					}
				}

				break;

			default:
				if(dpy->_slv) {
					LTKRun(&dpy->_slv->_obj, LTKWND_EVENT, obj);
				}
		}
	}
}

void LTKdpy_flush(LTKobj *obj, va_list arg) {
	XSync(((LTKdisplay *) obj)->_dpy, False);
}

LTKctr *LTKDpyCtr() {
	static LTKctr *ret = 0;

	if(ret) return ret;
	ret = LTKCtr(0, 1, sizeof(LTKdisplay), LTKDpyHdl);

	ret->_fct[LTKDPY_FLUSH] = LTKdpy_flush;

	return ret;
}

LTKdisplay *LTKDpyOf(LTKwindow *wnd) {
	return wnd->_dpy;
}

LTKdisplay *LTKDpy(char *dpyname) {
	LTKdisplay *ret = (LTKdisplay *) LTKAdd(LTKDpyCtr());
	Drawable rtwnd;
#ifdef LTK_XDBE_SUPPORT
	XdbeScreenVisualInfo *vfolst;
	XVisualInfo *vfomtc;

	int idx;
	int noret;
	int perf24 = -1;
	int perf = -1;
	int vfoidx24 = -1;
	int vfoidx = -1;
#endif

/*	Should we use it...
	char **extension_list = XListExtensions(dpy->_dpy, &noret);

	for(idx = 0; idx < noret; idx++) {
		printf("%d : %s\n",idx , extension_list[idx]);
	}

	XFreeExtensionList(extension_list);
*/

	if(!(ret->_dpy = XOpenDisplay(dpyname))) {
		/* Display not found */
		LTKrint(0, "LTKdisplay : Unable to connect to display");
		LTKFre(&ret->_obj);
		return 0;
	}

	ret->_scn = XDefaultScreen(ret->_dpy);
	rtwnd = DefaultRootWindow(ret->_dpy);

#ifdef LTK_XDBE_SUPPORT
	noret = ret->_scn;
	vfolst = XdbeGetVisualInfo(ret->_dpy, &rtwnd, &noret);
	
	if (!vfolst || (noret < 1) || (vfolst->count < 1)) {
		LTKrint(0, "LTKdisplay : Double buffer unable");
		if(vfolst) XdbeFreeVisualInfo(vfolst);
		LTKFre(&ret->_obj);
		return 0;
	}
	
	for(idx = 0; idx < noret; idx++) {
		switch(vfolst->visinfo[idx].depth) {
			case 24:
				vfoidx24 = idx;
				perf24 = vfolst->visinfo[idx].perflevel > perf24 ?
						vfolst->visinfo[idx].perflevel : perf24;
				break;
			case 32:
				vfoidx = idx;
				perf = vfolst->visinfo[idx].perflevel > perf ?
						vfolst->visinfo[idx].perflevel : perf;
			default:;
		}
	}
	
	if(vfoidx == -1) {
		if(vfoidx24 == -1) {
			LTKrint(0, "LTKdisplay : Screen not compatible with 32 or 24 bits depth");
			XdbeFreeVisualInfo(vfolst);
			LTKFre(&ret->_obj);
			return 0;
		}
	
		LTKrint(1, "LTKdisplay : 32 bits depth unavailable, using 24 bits depth");
		vfoidx = vfoidx24;
	}
	
	ret->_vfo[0].visualid = vfolst->visinfo[vfoidx].visual;
	ret->_vfo[0].screen = ret->_scn; 
	ret->_vfo[0].depth = vfolst->visinfo[vfoidx].depth;
	
	XdbeFreeVisualInfo(vfolst);
	
	vfomtc = XGetVisualInfo(ret->_dpy, VisualIDMask | VisualScreenMask | VisualDepthMask,
							ret->_vfo, &noret);
	
	if(!vfomtc || (noret < 1)) {
		LTKrint(0, "LTKdisplay : Double buffer visual not mached");
		LTKFre(&ret->_obj);
		return 0;
	}
	
	memcpy(ret->_vfo, vfomtc, sizeof(XVisualInfo));
	ret->_vfo[1].visual = 0;
	
	for(idx = 0; idx < noret; idx++) {
		XFree(&vfomtc[idx]);
	}
#else
	if(!XMatchVisualInfo(ret->_dpy, ret->_scn, 32, TrueColor, ret->_vfo)) {
		LTKrint(1, "LTKdisplay : 32 bits color not alloed");
		if(!XMatchVisualInfo(ret->_dpy, ret->_scn, 24, TrueColor, ret->_vfo)) {
			/* No proper color depth available */
			LTKrint(0, "LTKdisplay : Screen not compatible with 32 or 24 bits true color");
			LTKFre(&ret->_obj);
			return 0;
		}
	}
#endif

	ret->_atr[0].colormap = XCreateColormap(ret->_dpy, rtwnd, ret->_vfo[0].visual, AllocNone);
	ret->_atr[0].border_pixel = 0;
	ret->_atr[0].event_mask =	KeyPressMask | /* Keyboard down events wanted */
								KeyReleaseMask | /* Keyboard up events wanted */
								ButtonPressMask | /* Pointer button down events wanted */
								ButtonReleaseMask | /* Pointer button up events wanted */
								EnterWindowMask | /* Pointer window entry events wanted */
								LeaveWindowMask | /* Pointer window leave events wanted */
								PointerMotionMask | /* Pointer motion events wanted */
								PointerMotionHintMask | /* Pointer motion hints wanted */
								Button1MotionMask | /* Pointer motion while button 1 down */
								Button2MotionMask | /* Pointer motion while button 2 down */
								Button3MotionMask | /* Pointer motion while button 3 down */
								Button4MotionMask | /* Pointer motion while button 4 down */
								Button5MotionMask | /* Pointer motion while button 5 down */
								ButtonMotionMask | /* Pointer motion while any button down */
								KeymapStateMask | /* Keyboard state wanted at window entry and focus in */
								ExposureMask; /* Any exposure wanted */

								/* VisibilityChangeMask | Any change in visibility wanted */
								/* StructureNotifyMask | Any change in window structure wanted */
								/* ResizeRedirectMask | Redirect resize of this window */
								/* SubstructureNotifyMask | Substructure notification wanted */
								/* SubstructureRedirectMask | Redirect structure requests on children */
								/* FocusChangeMask | Any change in input focus wanted */
								/* PropertyChangeMask | Any change in property wanted */
								/* ColormapChangeMask | Any change in colormap wanted */
								/* OwnerGrabButtonMask; Automatic grabs should activate with owner_events set to True */
	ret->_atr[1].colormap = 0;

	ret->_WMm[0] = XInternAtom(ret->_dpy, "WM_PROTOCOLS", 1);
	ret->_WMm[1] = XInternAtom(ret->_dpy, "WM_DELETE_WINDOW", 1);

	ret->_slv = 0;

	LTKTrg(&ret->_obj, ConnectionNumber(ret->_dpy),
			EPOLLIN | EPOLLHUP);

	return ret;
}

void LTKWndHdl(LTKobj **obj) {
	LTKwindow *wnd = (LTKwindow *) obj[0];

	if(wnd->_mtr->_slv == wnd) {
		if(wnd->_nxt) wnd->_mtr->_slv = wnd->_nxt;
		else wnd->_mtr->_slv = wnd->_prv;
	}

	if(wnd->_nxt) wnd->_nxt->_prv = wnd->_prv;
	if(wnd->_prv) wnd->_prv->_nxt = wnd->_nxt;
	while(wnd->_slv) LTKFre(&wnd->_slv->_obj);

#ifdef LTK_XDBE_SUPPORT
	XdbeDeallocateBackBufferName(wnd->_dpy->_dpy, wnd->_bbf);
#endif

	XFreeGC(wnd->_dpy->_dpy, wnd->_gc);
	XUnmapWindow(wnd->_dpy->_dpy, wnd->_wnd);
	XDestroyWindow(wnd->_dpy->_dpy, wnd->_wnd);
	XFlush(wnd->_dpy->_dpy);
}

void LTKwnd_event(LTKobj *obj, va_list arg) {
	char buffer[20];
	KeySym key;
	LTKobj **ret = va_arg(arg, LTKobj **);
	LTKwindow *wnd = (LTKwindow *) obj;

	if(wnd->_dpy->_lst.xany.window != wnd->_wnd) {
		LTKobj *lstret = ret[0];

		if(wnd->_slv) {
			LTKRun(&wnd->_slv->_obj, LTKWND_EVENT, ret);
		}

		if(ret[0] != lstret) return;

		if(wnd->_nxt) {
			LTKRun(&wnd->_nxt->_obj, LTKWND_EVENT, ret);
		}

		return;
	}

	ret[0] = obj;

	switch(wnd->_dpy->_lst.type) {
		case KeyPress:
			XLookupString(&wnd->_dpy->_lst.xkey, buffer, 20, &key, 0);
			LTKrint(1, "LTKwnd_event : '%s' pressed "
						"on window %d on connection %d",
						buffer, wnd->_wnd, XConnectionNumber(wnd->_dpy->_dpy));
			break;

		case KeyRelease:
			XLookupString(&wnd->_dpy->_lst.xkey, buffer, 20, &key, 0);
			LTKrint(1, "LTKwnd_event : '%s' released "
						"on window %d on connection %d",
						buffer, wnd->_wnd, XConnectionNumber(wnd->_dpy->_dpy));
			break;

		case ButtonPress:
		case ButtonRelease:
			wnd->_btn = (wnd->_btn << 8) | (wnd->_btn & 0x00FF);
			wnd->_msx = wnd->_dpy->_lst.xbutton.x;
			wnd->_msy = wnd->_dpy->_lst.xbutton.y;

			if(wnd->_dpy->_lst.type == ButtonPress) {
				wnd->_btn |= 1 << (wnd->_dpy->_lst.xbutton.button - 1);
			}
			else{
				wnd->_btn &= ~(1 << (wnd->_dpy->_lst.xbutton.button - 1));
			}

			LTKrint(1, "LTKwnd_event : button state is now '%c%c%c%c%c' "
						"on window %d on connection %d",
						wnd->_btn & (Button5Mask / Button1Mask) ? '-' : '_',
						wnd->_btn & (Button4Mask / Button1Mask) ? '-' : '_',
						wnd->_btn & (Button3Mask / Button1Mask) ? '-' : '_',
						wnd->_btn & (Button2Mask / Button1Mask) ? '-' : '_',
						wnd->_btn & (Button1Mask / Button1Mask) ? '-' : '_',
						wnd->_wnd, XConnectionNumber(wnd->_dpy->_dpy));
			break;

		case MotionNotify:
			wnd->_msx = wnd->_dpy->_lst.xmotion.x;
			wnd->_msy = wnd->_dpy->_lst.xmotion.y;
			wnd->_dpy->_msx = wnd->_dpy->_lst.xmotion.x_root;
			wnd->_dpy->_msy = wnd->_dpy->_lst.xmotion.y_root;

			LTKrint(1, "LTKwnd_event : Pointer moved at %d %d "
						"on window %d on connection %d",
						wnd->_msx, wnd->_msy, wnd->_wnd,
						XConnectionNumber(wnd->_dpy->_dpy));
			break;

		case EnterNotify:
			wnd->_msx = wnd->_dpy->_lst.xcrossing.x;
			wnd->_msy = wnd->_dpy->_lst.xcrossing.y;
			wnd->_dpy->_msx = wnd->_dpy->_lst.xcrossing.x_root;
			wnd->_dpy->_msy = wnd->_dpy->_lst.xcrossing.y_root;

			LTKrint(1, "LTKwnd_event : Pointer came in at %d %d "
						"on window %d on connection %d",
						wnd->_msx, wnd->_msy, wnd->_wnd,
						XConnectionNumber(wnd->_dpy->_dpy));
			break;

		case LeaveNotify:
			wnd->_msx = -1;
			wnd->_msy = -1;
			wnd->_dpy->_msx = wnd->_dpy->_lst.xcrossing.x_root;
			wnd->_dpy->_msy = wnd->_dpy->_lst.xcrossing.y_root;

			LTKrint(1, "LTKwnd_event : Pointer moved out "
						"of window %d on connection %d",
						wnd->_wnd, XConnectionNumber(wnd->_dpy->_dpy));
			break;

		case ClientMessage:
			if(wnd->_dpy->_lst.xclient.message_type == wnd->_dpy->_WMm[0]) {
				if(((unsigned long) wnd->_dpy->_lst.xclient.data.l[0]) == wnd->_dpy->_WMm[1]) {
					LTKFre(&wnd->_obj);
				}
			}

			LTKrint(1, "LTKwnd_event : Window %d destroyed "
						"on connection %d", wnd->_wnd,
						XConnectionNumber(wnd->_dpy->_dpy));
			break;

		case FocusIn:
			LTKrint(1, "LTKwnd_event : Window %d on connection %d "
						"got focused", wnd->_wnd,
						XConnectionNumber(wnd->_dpy->_dpy));
			break;

		case FocusOut:
			LTKrint(1, "LTKwnd_event : Window %d on connection %d "
						"lost focus", wnd->_wnd,
						XConnectionNumber(wnd->_dpy->_dpy));
			break;

		case Expose:
			LTKrint(1, "LTKwnd_event : Window %d on connection %d "
						"got exposed", wnd->_wnd,
						XConnectionNumber(wnd->_dpy->_dpy));
#ifdef LTK_XDBE_SUPPORT
			LTKRun(&wnd->_obj, LTKWND_SWAP);
#endif
			break;

		case DestroyNotify:
		case GraphicsExpose:
		case NoExpose:
		case CirculateRequest:
		case ConfigureRequest:
		case MapRequest:
		case ResizeRequest:
		case CirculateNotify:
		case ConfigureNotify:
		case CreateNotify:
		case GravityNotify:
		case MapNotify:
		case MappingNotify:
		case ReparentNotify:
		case UnmapNotify:
		case VisibilityNotify:
		case ColormapNotify:
		case PropertyNotify:
		case SelectionClear:
		case SelectionNotify:
		case SelectionRequest:
		default:
			LTKrint(1, "LTKwnd_event : Happened unmanaged event "
						"on window %d on connection %d",
						wnd->_wnd, XConnectionNumber(wnd->_dpy->_dpy));
	}
}

void LTKwnd_getkey(LTKobj *obj, va_list arg) {
	uint8_t *ret = va_arg(arg, uint8_t *);
	LTKwindow *wnd = (LTKwindow *) obj;
	uint8_t it = 32;

	while(it--) {
		ret[it + 32] = wnd->_dpy->_Keypad[it + 32];
		ret[it] = wnd->_dpy->_Keypad[it];

		wnd->_dpy->_Keypad[it + 32] = ret[it];
	}
}

void LTKwnd_getbtn(LTKobj *obj, va_list arg) {
	uint16_t *ret = va_arg(arg, uint16_t *);
	LTKwindow *wnd = (LTKwindow *) obj;

	ret[0] = wnd->_btn;
	wnd->_btn = (wnd->_btn << 8) | (wnd->_btn & 0x00FF);
}

#ifdef LTK_XDBE_SUPPORT
void LTKwnd_swap(LTKobj *obj, va_list arg) {
	LTKwindow *wnd = (LTKwindow *) obj;
	XdbeSwapInfo swpifo;

	swpifo.swap_window = wnd->_wnd;
	swpifo.swap_action = XdbeUndefined;

	if(XdbeSwapBuffers(wnd->_dpy->_dpy, &swpifo, 1)) {
/*		LTKrint(1, "LTKwnd_swap : Window %d swapped "
					"on connection %d",
					wnd->_wnd, XConnectionNumber(wnd->_dpy->_dpy)); */
		XFlush(wnd->_dpy->_dpy);
	}
	else {
		LTKrint(1, "LTKwnd_swap : Unable to swap window %d "
					"on connection %d",
					wnd->_wnd, XConnectionNumber(wnd->_dpy->_dpy));
	}
}
#endif

LTKctr *LTKWndCtr() {
	static LTKctr *ret = 0;

	if(ret) return ret;
#ifdef LTK_XDBE_SUPPORT
	ret = LTKCtr(0, 4, sizeof(LTKwindow), LTKWndHdl);
#else
	ret = LTKCtr(0, 3, sizeof(LTKwindow), LTKWndHdl);
#endif

	ret->_fct[LTKWND_EVENT] = LTKwnd_event;
	ret->_fct[LTKWND_GETKEY] = LTKwnd_getkey;
	ret->_fct[LTKWND_GETBTN] = LTKwnd_getbtn;
#ifdef LTK_XDBE_SUPPORT
	ret->_fct[LTKWND_SWAP] = LTKwnd_swap;
#endif

	return ret;
}

LTKwindow *LTKWnd(void *master, char *wndname, int x, int y,
					int width, int height, uint32_t back) {
	LTKdisplay *dpy;
	LTKwindow *mtr = (LTKwindow *) master;
	LTKwindow *ret;
	Window prt;

	/* If no connection specified, create a new local one */
	if(!mtr) mtr = (LTKwindow *) LTKDpy(0);
	if(!mtr) return 0;

	if(mtr->_obj._fct == LTKDpyCtr()->_fct) {
		/* Create a window */
		dpy = (LTKdisplay *) mtr;
		prt = DefaultRootWindow(dpy->_dpy);
	}
	else {
		/* Create a subwindow */
		if(mtr->_obj._fct != LTKWndCtr()->_fct) {
			return 0;
		}

		prt = mtr->_wnd;
		dpy = mtr->_dpy;
	}

	ret = (LTKwindow *) LTKAdd(LTKWndCtr());
	dpy->_atr[0].background_pixel = back;

	ret->_dpy = dpy;
	ret->_wnd = XCreateWindow(dpy->_dpy, prt, x, y, width, height, 0,
								dpy->_vfo[0].depth, InputOutput,
								dpy->_vfo[0].visual, CWColormap |
								CWBorderPixel | CWBackPixel |
								CWEventMask, &dpy->_atr[0]);

#ifdef LTK_XDBE_SUPPORT
	ret->_bbf = XdbeAllocateBackBufferName(dpy->_dpy, ret->_wnd, XdbeUndefined);
#endif

	if(mtr->_obj._fct == LTKDpyCtr()->_fct) {
		/* Initialize window */
		XWindowAttributes size;
		XSizeHints sizehints;

		/* Subscribe to window closing event */
		XSetWMProtocols(dpy->_dpy, ret->_wnd, dpy->_WMm, 2);

		/* Lock window size */
		sizehints.flags = PMinSize | PMaxSize;
		sizehints.min_width = width;
		sizehints.min_height = height;
		sizehints.max_width = width;
		sizehints.max_height = height;
		XSetWMNormalHints(dpy->_dpy, ret->_wnd, &sizehints);

		if(wndname) XStoreName(dpy->_dpy, ret->_wnd, wndname);

		XGetWindowAttributes(dpy->_dpy, ret->_wnd, &size);

		ret->_x = size.x;
		ret->_y = size.y;
	}
	else {
		/* Initialize subwindow */
		ret->_x = x;
		ret->_y = y;
	}

	ret->_gc = XCreateGC(dpy->_dpy, ret->_wnd, 0, 0);

	ret->_w = width;
	ret->_h = height;

	ret->_mtr = mtr;
	ret->_nxt = mtr->_slv;
	ret->_prv = ret->_prv ? ret->_prv->_prv : 0;
	if(ret->_nxt) ret->_nxt->_prv = ret;
	if(ret->_prv) ret->_prv->_nxt = ret;
	mtr->_slv = ret;
	while(mtr->_slv->_prv) mtr->_slv = mtr->_slv->_prv;

	XMapWindow(dpy->_dpy, ret->_wnd);

	return ret;
}

#define SCN_WINDOW	0x01

void LTKScnHdl(LTKobj **obj) {
	LTKscene *scn = (LTKscene *) obj[0];

	if(scn->_mtr->_slv == (LTKwindow *) scn) {
		if(scn->_nxt) scn->_mtr->_slv = scn->_nxt;
		else scn->_mtr->_slv = scn->_prv;
	}

	if(scn->_nxt) scn->_nxt->_prv = scn->_prv;
	if(scn->_prv) scn->_prv->_nxt = scn->_nxt;

	if(scn->_glc) glXDestroyContext(scn->_dpy->_dpy, scn->_glc);
	if(scn->_stat & SCN_WINDOW) XDestroyWindow(scn->_dpy->_dpy, scn->_wnd);
	XFlush(scn->_dpy->_dpy);
}

void LTKscn_swap(LTKobj *obj, va_list arg) {
	LTKscene *scn = (LTKscene *) obj;

	glXSwapBuffers(scn->_dpy->_dpy, scn->_wnd);
}

void LTKscn_focus(LTKobj *obj, va_list arg) {
	LTKscene *scn = (LTKscene *) obj;

	glXMakeCurrent(scn->_dpy->_dpy, scn->_wnd, scn->_glc);
}

void LTKscn_look(LTKobj *obj, va_list arg) {
	double side[3], matrix[16] = {0};

	double eyePosition3D[3] = {va_arg(arg, double),
							va_arg(arg, double),
							va_arg(arg, double)};
	double center3D[3] = {va_arg(arg, double),
							va_arg(arg, double),
							va_arg(arg, double)};
	double upVector3D[3] = {va_arg(arg, double),
							va_arg(arg, double),
							va_arg(arg, double)};

	double forward[3] = {center3D[0] - eyePosition3D[0],
							center3D[1] - eyePosition3D[1],
							center3D[2] - eyePosition3D[2]};

	double normal = sqrt((forward[0] * forward[0]) +
						(forward[1] * forward[1]) +
						(forward[2] * forward[2]));

	forward[0] /= normal;
	forward[1] /= normal;
	forward[2] /= normal;

	side[0] = (forward[1] * upVector3D[2]) - (forward[2] * upVector3D[1]);
	side[1] = (forward[2] * upVector3D[0]) - (forward[0] * upVector3D[2]);
	side[2] = (forward[0] * upVector3D[1]) - (forward[1] * upVector3D[0]);

	normal = sqrt((side[0] * side[0]) +
					(side[1] * side[1]) +
					(side[2] * side[2]));

	side[0] /= normal;
	side[1] /= normal;
	side[2] /= normal;

	upVector3D[0] = (side[1] * forward[2]) - (side[2] * forward[1]);
	upVector3D[1] = (side[2] * forward[0]) - (side[0] * forward[2]);
	upVector3D[2] = (side[0] * forward[1]) - (side[1] * forward[0]);

	matrix[0] = -side[0];	matrix[1] = upVector3D[0];	matrix[2] = -forward[0];	matrix[3] = 0.0;
	matrix[4] = -side[1];	matrix[5] = upVector3D[1];	matrix[6] = -forward[1];	matrix[7] = 0.0;
	matrix[8] = -side[2];	matrix[9] = upVector3D[2];	matrix[10] = -forward[2];	matrix[11] = 0.0;
	matrix[12] = 0.0;		matrix[13] = 0.0;	matrix[14] = 0.0;			matrix[15] = 1.0;

	glMultMatrixd(matrix);
	glTranslated(-eyePosition3D[0], -eyePosition3D[1], -eyePosition3D[2]);
}

LTKctr *LTKScnCtr() {
	static LTKctr *ret = 0;

	if(ret) return ret;
	ret = LTKCtr(LTKWndCtr(), 6, sizeof(LTKscene), LTKScnHdl);

	ret->_fct[LTKWND_SWAP] = LTKscn_swap;
	ret->_fct[LTKSCN_FOCUS] = LTKscn_focus;
	ret->_fct[LTKSCN_LOOK] = LTKscn_look;

	return ret;
}

LTKscene *LTKScn(LTKwindow *wnd, int x, int y, int width, int height, uint32_t back) {
	LTKscene *ret;

	if(!wnd) return 0;

	if(wnd->_obj._fct != LTKWndCtr()->_fct) {
		LTKrint(0, "LTKscene : Need a window as first param");
		return 0;
	}

	ret = (LTKscene *) LTKAdd(LTKScnCtr());
	ret->_dpy = wnd->_dpy;

	if(!wnd->_dpy->_vfo[1].visual) {
		/* Open GL visual needs to be found */
		int attribs[] = {
			GLX_X_RENDERABLE,	True,
			GLX_DRAWABLE_TYPE,	GLX_WINDOW_BIT | GLX_PIXMAP_BIT,
			GLX_RENDER_TYPE,	GLX_RGBA_BIT,
			GLX_X_VISUAL_TYPE,	GLX_TRUE_COLOR,
			GLX_RED_SIZE,		8,
			GLX_GREEN_SIZE,		8,
			GLX_BLUE_SIZE,		8,
			GLX_ALPHA_SIZE,		8,
			GLX_DEPTH_SIZE,		24,
			GLX_STENCIL_SIZE,	8,
			GLX_DOUBLEBUFFER,	True, None
		};
		int best_fbc = -1, best_num_samp = -1, idx, vcnt;
		XVisualInfo *vfo;

		GLXFBConfig* fbc = glXChooseFBConfig(ret->_dpy->_dpy, ret->_dpy->_scn, attribs, &vcnt);
		if(!vcnt) {
			LTKrint(0, "LTKscene : Scene not available");

			LTKFre(&ret->_obj);
			return 0;
		}

		for(idx = 0; idx < vcnt; idx++) {
			XVisualInfo *vi = glXGetVisualFromFBConfig(ret->_dpy->_dpy, fbc[idx]);

			if(vi) {
				int samples;
				glXGetFBConfigAttrib(ret->_dpy->_dpy, fbc[idx], GLX_SAMPLES, &samples);

				if(samples > best_num_samp) {
					best_fbc = idx;
					best_num_samp = samples;
				}
			}

			XFree(vi);
		}

		if(best_fbc == -1) {
			XFree(fbc);

			LTKFre(&ret->_obj);
			return 0;
		}

		vfo = glXGetVisualFromFBConfig(ret->_dpy->_dpy, fbc[best_fbc]);
		memcpy(&ret->_dpy->_vfo[1], vfo, sizeof(XVisualInfo));

		/* Be sure to free the FBConfig list allocated by glXChooseFBConfig() */
		XFree(fbc);

		/* window attributes */
		ret->_dpy->_atr[1].colormap = XCreateColormap(ret->_dpy->_dpy,
												wnd->_wnd,
												vfo->visual, AllocNone);
		XFree(vfo);
		ret->_dpy->_atr[1].border_pixel = 0;
		ret->_dpy->_atr[1].event_mask = KeyPressMask | /* Keyboard down events wanted */
										KeyReleaseMask | /* Keyboard up events wanted */
										ButtonPressMask | /* Pointer button down events wanted */
										ButtonReleaseMask | /* Pointer button up events wanted */
										EnterWindowMask | /* Pointer window entry events wanted */
										LeaveWindowMask | /* Pointer window leave events wanted */
										PointerMotionMask | /* Pointer motion events wanted */
										PointerMotionHintMask | /* Pointer motion hints wanted */
										Button1MotionMask | /* Pointer motion while button 1 down */
										Button2MotionMask | /* Pointer motion while button 2 down */
										Button3MotionMask | /* Pointer motion while button 3 down */
										Button4MotionMask | /* Pointer motion while button 4 down */
										Button5MotionMask | /* Pointer motion while button 5 down */
										ButtonMotionMask | /* Pointer motion while any button down */
										KeymapStateMask | /* Keyboard state wanted at window entry and focus in */
										ExposureMask; /* Any exposure wanted */

										/* VisibilityChangeMask | Any change in visibility wanted */
										/* StructureNotifyMask | Any change in window structure wanted */
										/* ResizeRedirectMask | Redirect resize of this window */
										/* SubstructureNotifyMask | Substructure notification wanted */
										/* SubstructureRedirectMask | Redirect structure requests on children */
										/* FocusChangeMask | Any change in input focus wanted */
										/* PropertyChangeMask | Any change in property wanted */
										/* ColormapChangeMask | Any change in colormap wanted */
										/* OwnerGrabButtonMask; Automatic grabs should activate with owner_events set to True */
	}

	ret->_dpy->_atr[1].background_pixel = back;

	ret->_dpy = wnd->_dpy;
	ret->_wnd = XCreateWindow(ret->_dpy->_dpy, wnd->_wnd, x, y, width, height, 0,
								ret->_dpy->_vfo[1].depth, InputOutput,
								ret->_dpy->_vfo[1].visual, CWColormap |
								CWBorderPixel | CWBackPixel |
								CWEventMask, &ret->_dpy->_atr[1]);

	ret->_stat &= SCN_WINDOW;

	ret->_glc = glXCreateContext(ret->_dpy->_dpy, &ret->_dpy->_vfo[1], 0, True);
	if(!ret->_glc) {
		LTKFre(&ret->_obj);
		return 0;
	}

	ret->_x = x;
	ret->_y = y;
	ret->_w = width;
	ret->_h = height;

	ret->_mtr = wnd;
	ret->_nxt = wnd->_slv;
	ret->_prv = wnd->_slv ? wnd->_slv->_prv : 0;
	if(ret->_nxt) ret->_nxt->_prv = (LTKwindow *) ret;
	if(ret->_prv) ret->_prv->_nxt = (LTKwindow *) ret;
	wnd->_slv = (LTKwindow *) ret;
	while(wnd->_slv->_prv) wnd->_slv = wnd->_slv->_prv;

	XMapWindow(ret->_dpy->_dpy, ret->_wnd);

	LTKRun(&ret->_obj, LTKSCN_FOCUS);

	return ret;
}

#undef SCN_WINDOW

#define SHM_ATTR	0x01
#define SHM_ALLOC	0x02
#define SHM_ATTCH	0x04

void LTKImgHdl(LTKobj **obj) {
	LTKimage *img = (LTKimage *) obj[0];

	if(img->_stat & SHM_ATTCH) XShmDetach(img->_dpy->_dpy, &img->_shm);
	if(img->_stat & SHM_ALLOC) shmdt(img->_shm.shmaddr);
	if(img->_stat & SHM_ATTR) shmctl(img->_shm.shmid, IPC_RMID, 0);

/*	if(img->_img->data) free(img->_img->data); Notice that neither XCreateImage
							nor XInitImage allocate datas
							while XDestroyImage frees it */
	if(img->_img) XDestroyImage(img->_img);
}

void LTKimg_getdata(LTKobj *obj, va_list arg) {
	LTKimage *img = (LTKimage *) obj;
	uint32_t ***ret = va_arg(arg, uint32_t ***);

	ret[0] = img->_data;
}

LTKctr *LTKImgCtr() {
	static LTKctr *ret = 0;

	if(ret) return ret;
	ret = LTKCtr(0, 1, sizeof(LTKimage), LTKImgHdl);

	ret->_fct[LTKIMG_GETDATA] = LTKimg_getdata;

	return ret;
}

LTKimage *LTKImg(void *from, int width, int height) {
	LTKimage *ret = (LTKimage *) LTKAdd(LTKImgCtr());
	char *datptr;
	int idx;

	if(!(width && height)) {
		LTKrint(0, "LTKImg : Width and height must be not null");
		return 0;
	}

	if(from) {
		if(((LTKobj *) from)->_fct == LTKDpyCtr()->_fct) {
			ret->_dpy = (LTKdisplay *) from;
		}
		else {
			if(((LTKobj *) from)->_fct != LTKWndCtr()->_fct) {
				LTKrint(0, "LTKImg : Bad first argument, display or window needed");
				return 0;
			}

			ret->_dpy = ((LTKwindow *) from)->_dpy;
		}
	}
	else {
		LTKrint(0, "LTKImg : Absent first argument, display or window needed");
		return 0;
	}

	ret->_img = XShmCreateImage(ret->_dpy->_dpy, ret->_dpy->_vfo[0].visual,
								ret->_dpy->_vfo[0].depth, ZPixmap, 0, &ret->_shm,
								width, height);

	if(ret->_img) {
		ret->_shm.shmid = shmget(IPC_PRIVATE, height * (ret->_img->bytes_per_line +
								sizeof(void *)), IPC_CREAT | 0666);

		if(ret->_shm.shmid < 0) {
			LTKrint(0, "LTKImg : Unable to get shared memory segment "
						"on connection %d : %s",
						XConnectionNumber(ret->_dpy->_dpy), strerror(errno));

			LTKFre(&ret->_obj);
			return 0;
		}

		ret->_stat = SHM_ATTR;
		ret->_shm.shmaddr = ret->_img->data = shmat(ret->_shm.shmid, 0, 0);

		if(ret->_shm.shmaddr == (void *) -1) {
			LTKrint(0, "LTKImg : Unable to allocate shared memory segment "
						"data on connection %d : %s",
						XConnectionNumber(ret->_dpy->_dpy), strerror(errno));

			LTKFre(&ret->_obj);
			return 0;
		}

		ret->_stat |= SHM_ALLOC;
		ret->_shm.readOnly = False;

		if(!XShmAttach(ret->_dpy->_dpy, &ret->_shm)) {
			LTKrint(0, "LTKImg : Unable to attach shared memory segment "
						"to connection %d",
						XConnectionNumber(ret->_dpy->_dpy));

			LTKFre(&ret->_obj);
			return 0;
		}

		ret->_stat |= SHM_ALLOC;
	}
	else {
		ret->_img = XCreateImage(ret->_dpy->_dpy, ret->_dpy->_vfo[0].visual,
								ret->_dpy->_vfo[0].depth, ZPixmap, 0, 0,
								width, height, 32, 0);

		if(!ret->_img) {
			LTKrint(0, "LTKImg : Unable to create image on connection %d",
						XConnectionNumber(ret->_dpy->_dpy));
			LTKFre(&ret->_obj);
			return 0;
		}

		if(!XInitImage(ret->_img)) {
			LTKrint(0, "LTKImg : Unable to init image on connection %d",
						XConnectionNumber(ret->_dpy->_dpy));
			LTKFre(&ret->_obj);
			return 0;
		}
		ret->_img->data = malloc(height * (ret->_img->bytes_per_line +
								sizeof(void *)));

		if(!ret->_img->data) {
			LTKrint(0, "LTKImg : Unable to allocate image on connection %d",
						XConnectionNumber(ret->_dpy->_dpy));
			LTKFre(&ret->_obj);
			return 0;
		}
	}

	ret->_data = (uint32_t **)
				&((uint8_t *) ret->_img->data)[height * ret->_img->bytes_per_line];
	datptr = ret->_img->data;

	for(idx = 0; idx < height; idx++) {
		ret->_data[idx] = (uint32_t *) datptr;
		datptr += ret->_img->bytes_per_line;
	}

	return ret;
}

void LTKBlit(void *orig, void *dest, int x_orig, int y_orig,
			int x_dest, int y_dest, unsigned int width, unsigned int height) {
	unsigned int x;
	unsigned int y;

	LTKimage *orig_as_image = (LTKimage *) orig;
	LTKwindow *orig_as_window = (LTKwindow *) orig;
	LTKimage *dest_as_image = (LTKimage *) dest;
	LTKwindow *dest_as_window = (LTKwindow *) dest;

	enum {
		image_to_image = 0,
		image_to_window = 1,
		window_to_image = 2,
		window_to_window = 3
	} mode = image_to_image;

	if(((LTKobj *) orig)->_fct == LTKWndCtr()->_fct) {
		mode |= window_to_image;
	}
	else if(((LTKobj *) orig)->_fct != LTKImgCtr()->_fct) {
		LTKrint(0, "LTKImgBlit : Source is neither a window nor an image");
		return;
	}

	if(((LTKobj *) dest)->_fct == LTKWndCtr()->_fct) {
		mode |= image_to_window;
	}
	else if(((LTKobj *) dest)->_fct != LTKImgCtr()->_fct) {
		LTKrint(0, "LTKImgBlit : Destination is neither a window nor an image");
		return;
	}

	switch(mode) {
		case image_to_image:
			/* This is the painter's algorithm */
			for(y = 0; y < height; y++) {
				for(x = 0; x < width; x++) {
					uint16_t oa = (orig_as_image->_data[y + y_orig][x + x_orig] >> 24) & 0x00FF;
					uint16_t or = (orig_as_image->_data[y + y_orig][x + x_orig] >> 16) & 0x00FF;
					uint16_t og = (orig_as_image->_data[y + y_orig][x + x_orig] >> 8) & 0x00FF;
					uint16_t ob = (orig_as_image->_data[y + y_orig][x + x_orig]) & 0x00FF;

					uint16_t da = (dest_as_image->_data[y + y_dest][x + x_dest] >> 24) & 0x00FF;
					uint16_t dr = (dest_as_image->_data[y + y_dest][x + x_dest] >> 16) & 0x00FF;
					uint16_t dg = (dest_as_image->_data[y + y_dest][x + x_dest] >> 8) & 0x00FF;
					uint16_t db = (dest_as_image->_data[y + y_dest][x + x_dest]) & 0x00FF;

					if(oa == 0xFF) {
						/* Src is opaque, no more computation needed */
						dest_as_image->_data[y + y_dest][x + x_dest] =
						orig_as_image->_data[y + y_orig][x + x_orig];
						continue;
					}

					if(!oa) {
						/* No copy has to be done if src is transparent */
						continue;
					}

					or = (or * oa) / 0xFF;
					og = (og * oa) / 0xFF;
					ob = (ob * oa) / 0xFF;

					dr = (dr * (0xFF - oa)) / 0xFF;
					dg = (dg * (0xFF - oa)) / 0xFF;
					db = (db * (0xFF - oa)) / 0xFF;

					da += (0xFF - da) * (oa / 0xFF);
					dr += or;
					dg += og;
					db += ob;

					dest_as_image->_data[y + y_dest][x + x_dest] =
													((da & 0xFF) << 24) |
													((dr & 0xFF) << 16) |
													((dg & 0xFF) << 8) |
													(db & 0xFF);
				}
			}

			break;
		case image_to_window:
			if(orig_as_image->_stat & SHM_ATTCH) {
				/* No error check since Xlib craches the process itself */
#ifdef LTK_XDBE_SUPPORT
				XShmPutImage(dest_as_window->_dpy->_dpy, dest_as_window->_bbf,
							dest_as_window->_gc, orig_as_image->_img,
							x_orig, y_orig, x_dest, y_dest, width, height, True);
#else
				XShmPutImage(dest_as_window->_dpy->_dpy, dest_as_window->_wnd,
							dest_as_window->_gc, orig_as_image->_img,
							x_orig, y_orig, x_dest, y_dest, width, height, True);
#endif
			}
			else {
				/* No error check since Xlib craches the process itself */
#ifdef LTK_XDBE_SUPPORT
				XPutImage(dest_as_window->_dpy->_dpy, dest_as_window->_bbf,
							dest_as_window->_gc, orig_as_image->_img,
							x_orig, y_orig, x_dest, y_dest, width, height);
#else
				XPutImage(dest_as_window->_dpy->_dpy, dest_as_window->_wnd,
							dest_as_window->_gc, orig_as_image->_img,
							x_orig, y_orig, x_dest, y_dest, width, height);
#endif
			}

			break;
		case window_to_image:
			if(orig_as_image->_stat & SHM_ATTCH) {
				/* No error check since Xlib craches the process itself */
#ifdef LTK_XDBE_SUPPORT
				XShmGetImage(orig_as_window->_dpy->_dpy, orig_as_window->_bbf,
							dest_as_image->_img, x_orig, y_orig, ~0);
#else
				XShmGetImage(orig_as_window->_dpy->_dpy, orig_as_window->_wnd,
							dest_as_image->_img, x_orig, y_orig, ~0);
#endif
			}
			else {
				/* No error check since Xlib craches the process itself */
#ifdef LTK_XDBE_SUPPORT
				XGetImage(orig_as_window->_dpy->_dpy, orig_as_window->_bbf,
							x_orig, y_orig, width, height, ~0, ZPixmap);
#else
				XGetImage(orig_as_window->_dpy->_dpy, orig_as_window->_wnd,
							x_orig, y_orig, width, height, ~0, ZPixmap);
#endif
			}

			break;
		case window_to_window:
#ifdef LTK_XDBE_SUPPORT
			XCopyArea(dest_as_window->_dpy->_dpy, orig_as_window->_bbf,
							dest_as_window->_bbf, dest_as_window->_gc,
							x_orig, y_orig, width, height,  x_dest, y_dest);
#else
			XCopyArea(dest_as_window->_dpy->_dpy, orig_as_window->_wnd,
							dest_as_window->_wnd, dest_as_window->_gc,
							x_orig, y_orig, width, height,  x_dest, y_dest);
#endif
		default:;
	}
}

#undef SHM_ATTR
#undef SHM_ALLOC
#undef SHM_ATTCH

typedef struct {
	uint8_t _id_size;
	uint8_t _colormap_type;
	uint8_t _image_type;

	uint8_t _colormap_start_l;
	uint8_t _colormap_start_h;
	uint8_t _colormap_length_l;
	uint8_t _colormap_length_h;
	uint8_t _colormap_depth;

	uint8_t _x_orig_l;
	uint8_t _x_orig_h;
	uint8_t _y_orig_l;
	uint8_t _y_orig_h;
	uint8_t _x_size_l;
	uint8_t _x_size_h;
	uint8_t _y_size_l;
	uint8_t _y_size_h;
	uint8_t _bit_per_sample;
	uint8_t _descriptor;
} tga_header_t;

LTKimage *LTKImgLoad(void *context, const char *name, int *fd) {
	int file;
	uint64_t idx = 0;
	uint64_t size;
	uint16_t x;
	uint16_t y;

	tga_header_t header;
	LTKimage *ret;

	if(name) file = open(name, O_RDWR);
	else if(fd) file = fd[0];
	else {
		LTKrint(0, "LTKImgLoad : Valid path or valid file descriptor pointer needed");
		return 0;
	}

	if(file < 0) {
		LTKrint(0, "LTKImgLoad : Unable to open %s : %s", name, strerror(errno));
		return 0;
	}

#ifdef __BIG_ENDIAN__
	idx = 1;

	if(read(file, &header._id_size, 1) != 1) idx = 0;
	if(!idx && (read(file, &header._colormap_type, 1) != 1)) idx = 0;
	if(!idx && (read(file, &header._image_type, 1) != 1)) idx = 0;

	if(!idx && (read(file, &header._colormap_start_l, 1) != 1)) idx = 0;
	if(!idx && (read(file, &header._colormap_start_h, 1) != 1)) idx = 0;
	if(!idx && (read(file, &header._colormap_length_l, 1) != 1)) idx = 0;
	if(!idx && (read(file, &header._colormap_length, 1) != 1)) idx = 0;
	if(!idx && (read(file, &header._colormap_depth, 1) != 1)) idx = 0;

	if(!idx && (read(file, &header._x_orig_l, 1) != 1)) idx = 0;
	if(!idx && (read(file, &header._x_orig_h, 1) != 1)) idx = 0;
	if(!idx && (read(file, &header._y_orig_l, 1) != 1)) idx = 0;
	if(!idx && (read(file, &header._y_orig_h, 1) != 1)) idx = 0;
	if(!idx && (read(file, &header._x_size_l, 1) != 1)) idx = 0;
	if(!idx && (read(file, &header._x_size_h, 1) != 1)) idx = 0;
	if(!idx && (read(file, &header._y_size_l, 1) != 1)) idx = 0;
	if(!idx && (read(file, &header._y_size_h, 1) != 1)) idx = 0;

	if(!idx && (read(file, &header._bit_per_sample, 1) != 1)) idx = 0;
	if(!idx && (read(file, &header._descriptor, 1) != 1)) idx = 0;
#else
	while(idx < 18) {
		int rem = read(file, &((uint8_t *) &header)[idx], 18 - idx);

		if(rem < 0) {
			idx = 0;
			break;
		}

		idx += rem;
	}
#endif

	x = header._x_size_l | (header._x_size_h << 8);
	y = header._y_size_l | (header._y_size_h << 8);

	if(!idx) {
		close(file);
		LTKrint(0, "LTKImgLoad : Unable to read header : %s", strerror(errno));
		return 0;
	}

	if(header._id_size) {
		close(file);
		LTKrint(0, "LTKImgLoad : Id not allowed on tga files TODO");
		return 0;
	}

	if(header._colormap_type) {
		close(file);
		LTKrint(0, "LTKImgLoad : Palette not allowed on tga files TODO");
		return 0;
	}

	if((header._image_type != 2) ||
			(header._bit_per_sample != 32) ||
			((header._descriptor & 0x0F) != 0x08)) {

		close(file);
		LTKrint(0, "LTKImgLoad : Only truecolor 32 bits allowed on tga files TODO");
		return 0;
	}

	if((header._descriptor & 0xF0) != 0x20) {
		close(file);
		LTKrint(0, "LTKImgLoad : Pixel must start form the upper-left corner "
					"and no interleaving can appear on tga files TODO");
		return 0;
	}

	if(!(x && y)) {
		close(file);
		LTKrint(0, "LTKImgLoad : Both width and height cannot be 0");
		return 0;
	}

	ret = LTKImg(context, x, y);

	if(!ret) return 0;

	size = x * y * 4;
	idx = 0;

	while(idx < size) {
		int rem = read(file, &ret->_img->data[idx], size - idx);

		if(rem < 0) {
			close(file);
			LTKrint(0, "LTKImgLoad : Unable to read image content : %s", strerror(errno));
			return 0;
		}

		idx += rem;
	}

	if(fd) fd[0] = file;
	else close(file);

	return ret;
}

int LTKImgSave(LTKimage *img, int fd) {
	uint64_t idx = 0;
	uint64_t size;
	tga_header_t header;

	if(!img) {
		LTKrint(0, "LTKImgSave : First argument needed");
		return -1;
	}

	if(img->_obj._fct != LTKImgCtr()->_fct) {
		LTKrint(0, "LTKImgSave : First argument isn't an image");
		return -1;
	}

	header._id_size = 0;

	header._colormap_type = 0;
	header._colormap_start_l = 0;
	header._colormap_start_h = 0;
	header._colormap_length_l = 0;
	header._colormap_length_h = 0;
	header._colormap_depth = 0;

	header._image_type = 2;
	header._bit_per_sample = 32;
	header._descriptor = 0x28;

	header._x_orig_l = 0;
	header._x_orig_h = 0;
	header._y_orig_l = 0;
	header._y_orig_h = 0;
	header._x_size_l = img->_img->width;
	header._x_size_h = img->_img->width >> 8;
	header._y_size_l = img->_img->height;
	header._y_size_h = img->_img->height >> 8;

	size = img->_img->width * img->_img->height * 4;

#ifdef __BIG_ENDIAN__
	if(!idx && (write(fd, &header._colormap_type, 1) != 1)) idx = 1;
	if(!idx && (write(fd, &header._image_type, 1) != 1)) idx = 1;
	if(!idx && (write(fd, &header._colormap_start_l, 1) != 1)) idx = 1;
	if(!idx && (write(fd, &header._colormap_start_h, 1) != 1)) idx = 1;
	if(!idx && (write(fd, &header._colormap_length_l, 1) != 1)) idx = 1;
	if(!idx && (write(fd, &header._colormap_length_h, 1) != 1)) idx = 1;
	if(!idx && (write(fd, &header._colormap_depth, 1) != 1)) idx = 1;

	if(!idx && (write(fd, &header._x_orig_l, 1) != 1)) idx = 1;
	if(!idx && (write(fd, &header._x_orig_h, 1) != 1)) idx = 1;
	if(!idx && (write(fd, &header._y_orig_l, 1) != 1)) idx = 1;
	if(!idx && (write(fd, &header._y_orig_h, 1) != 1)) idx = 1;
	if(!idx && (write(fd, &header._x_size_l, 1) != 1)) idx = 1;
	if(!idx && (write(fd, &header._x_size_h, 1) != 1)) idx = 1;
	if(!idx && (write(fd, &header._y_size_l, 1) != 1)) idx = 1;
	if(!idx && (write(fd, &header._y_size_h, 1) != 1)) idx = 1;

	if(!idx && (write(fd, &header._bit_per_sample, 1) != 1)) idx = 1;
	if(!idx && (write(fd, &header._descriptor, 1) != 1)) idx = 1;
#else
	while(idx < 18) {
		int rem = write(fd, &((uint8_t *) &header)[idx], 18 - idx);

		if(rem < 0) {
			idx = 0;
			break;
		}

		idx += rem;
	}
#endif

	if(!idx) {
		LTKrint(0, "LTKImgSave : Unable to write header : %s", strerror(errno));
		return -1;
	}

	idx = 0;

	while(idx < size) {
		int rem = write(fd, &img->_img->data[idx], size - idx);

		if(rem < 0) {
			LTKrint(0, "LTKImgSave : Unable to write image content : %s", strerror(errno));
			return -1;
		}

		idx += rem;
	}

	return 0;
}
