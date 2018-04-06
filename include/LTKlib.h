#ifndef __LTKLIB_H__
#define __LTKLIB_H__

#include<time.h>
#include<math.h>
#include<errno.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdarg.h>
#include<unistd.h>
#include<string.h>
#include<termios.h>
#include<X11/Xlib.h>
#include<X11/Xutil.h>			/* For visual informations */
#include<X11/keysym.h>			/* For ??? */
#include<X11/extensions/Xdbe.h>
#include<X11/extensions/XShm.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glxext.h>			/* For V-sync */
#include<vulkan/vulkan.h>		/* Doomed to replace GL */
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<sys/time.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
/* #include<.h> */

#include"../include/LTKobj.h"	/* General object implementation */
#include"../include/LTKutl.h"	/* General functions with no dependence */
#include"../include/LTKdev.h"	/* Device virtual class defines default system call encapsulation */
#include"../include/LTKskt.h"	/* Device derived class that holds network communications */
#include"../include/LTKwin.h"	/* Windowing and basical graphic functions */
#include"../include/LTKvlk.h"	/* Advenced gpu functions */
//	#include"LTKdev.h"			/* Generic devices, never implement as is */
//	#include"LTKsrv.h"			/* Server implementation */

#endif /* __LTKLIB_H__ */
