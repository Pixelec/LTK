#include"../include/LTKlib.h"

void LTKDevHdl(LTKobj **obj) {
	LTKdev *dev = (LTKdev *) obj[0];

	if(dev->_path) {
		free(dev->_path);
	}

	if(dev->_fd >= 0) {
		close(dev->_fd);
	}
}

/* Made for encapsulate ioctl or fcntl calls */
void LTKDevControl(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevControl : not implemented on this device");
}

/* Made for encapsulate tcsetattr or equivalent */
void LTKDevSetASynq(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevSetASynq : not implemented on this device");
}

/* Made for encapsulate tcsetattr or equivalent */
void LTKDevSetSynq(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevSetSynq : not implemented on this device");
}

/* Made for encapsulate fchown and/or fchmod calls */
void LTKDevSetStat(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevSetStat : not implemented on this device");
}

/* Made for encapsulate fstat */
void LTKDevGetStat(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevGetStat : not implemented on this device");
}

/* Made for encapsulate flock calls */
void LTKDevLock(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevLock : not implemented on this device");
}

/* Made for encapsulate flock calls */
void LTKDevWaitLock(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevWaitLock : not implemented on this device");
}

/* Made for encapsulate flock calls */
void LTKDevUnLock(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevUnLock : not implemented on this device");
}

/* Made for encapsulate read calls */
void LTKDevRead(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevRead : not implemented on this device");
}

/* Made for encapsulate blocking read calls */
void LTKDevFRead(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevFRead : not implemented on this device");
}

/* Made for encapsulate write calls */
void LTKDevWrite(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevWrite : not implemented on this device");
}

/* Made for encapsulate blocking write calls */
void LTKDevFWrite(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevFWrite : not implemented on this device");
}

/* Made for encapsulate fseek calls */
void LTKDevSeek(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevSeek : not implemented on this device");
}

/* Made for encapsulate ftruncate calls */
void LTKDevTrunc(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevTrunc : not implemented on this device");
}

/* Made for encapsulate mmap calls */
void LTKDevMap(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevMap : not implemented on this device");
}

/* Made for encapsulate munmap calls */
void LTKDevUnMap(LTKobj *obj, va_list arg) {
	LTKrint(0, "LTKDevUnMap : not implemented on this device");
}

LTKctr *LTKCtrdev() {
	static LTKctr *ctr = 0;
	
	if(!ctr) {
		ctr = LTKCtr(0, 16, sizeof(LTKdev), LTKDevHdl);
		ctr->_vrt = 1;
		
		ctr->_fct[LTKDEVCONTROL] = LTKDevControl;
		
		ctr->_fct[LTKDEVSETASYNQ] = LTKDevSetASynq;
		ctr->_fct[LTKDEVSETSYNQ] = LTKDevSetSynq;
		
		ctr->_fct[LTKDEVSETSTAT] = LTKDevSetStat;
		ctr->_fct[LTKDEVGETSTAT] = LTKDevGetStat;
		
		ctr->_fct[LTKDEVLOCK] = LTKDevLock;
		ctr->_fct[LTKDEVWAITLOCK] = LTKDevWaitLock;
		ctr->_fct[LTKDEVUNLOCK] = LTKDevUnLock;
		
		ctr->_fct[LTKDEVREAD] = LTKDevRead;
		ctr->_fct[LTKDEVFREAD] = LTKDevFRead;
		ctr->_fct[LTKDEVWRITE] = LTKDevWrite;
		ctr->_fct[LTKDEVFWRITE] = LTKDevFWrite;

		ctr->_fct[LTKDEVSEEK] = LTKDevSeek;
		ctr->_fct[LTKDEVTRUNCATE] = LTKDevTrunc;

		ctr->_fct[LTKDEVMAP] = LTKDevMap;
		ctr->_fct[LTKDEVUNMAP] = LTKDevUnMap;
	}
	
	return ctr;
}
