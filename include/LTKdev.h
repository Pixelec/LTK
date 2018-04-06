#ifndef __LTKDEV_H__
#define __LTKDEV_H__

/*	LTKobj _obj
		LTKdev Add file descriptor and all unimplemented methods
*/

typedef struct {
	LTKobj _obj;
	char *_path;
	int _fd;
} LTKdev;

#define LTKDEVCONTROL	0

#define LTKDEVSETASYNQ	1
#define LTKDEVSETSYNQ	2

#define LTKDEVSETSTAT	3
#define LTKDEVGETSTAT	4

#define LTKDEVLOCK		5
#define LTKDEVWAITLOCK	6
#define LTKDEVUNLOCK	7

#define LTKDEVREAD		8
#define LTKDEVFREAD		9
#define LTKDEVWRITE		10
#define LTKDEVFWRITE	11

#define LTKDEVSEEK		12
#define LTKDEVTRUNCATE	13

#define LTKDEVMAP		14
#define LTKDEVUNMAP		15

LTKctr *LTKCtrdev();

#endif /* __LTKDEV_H__ */