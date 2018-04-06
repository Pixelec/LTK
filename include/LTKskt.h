#ifndef __LTKSKT_H__
#define __LTKSKT_H__

#define LTKCON_LOCK				0
#define LTKCON_UNLOCK			1

#define LTKCON_REGULAR			0
#define LTKCON_SOCKET			1
#define LTKCON_SOCKET_OPEN		1
#define LTKCON_COUNTED_OPEN		2
#define LTKCON_SOCKET_CLOSED	3 /* Manually closed */
#define LTKCON_COUNTED_CLOSED	4 /* Automatically closed */

#define LTKNDP_REGULAR			0
#define LTKNDP_SOCKET			1

typedef struct LTKcontainer_s {
	LTKdev _dev;

	struct LTKendpoint_s *_slv; /* Can be an LTKcontainer * */

	uint16_t _port;
	uint16_t _status;
	/*	Folder => LTKCON_REGULAR
		Socket => LTKCON_SOCKET_OPEN
		Socket => LTKCON_COUNTED_OPEN (with counted slots) */
	uint64_t _guest_num;
	struct sockaddr _conf;
	socklen_t _len;
} LTKcontainer;

typedef struct LTKendpoint_s {
	LTKobj _obj;

	LTKcontainer *_mtr;
	struct LTKendpoint_s *_prv; /* Can be an LTKcontainer * */
	struct LTKendpoint_s *_nxt; /* Can be an LTKcontainer * */

	int _fd;
	char *_path;
	int _type;
	/*	Not applicable for regular file,
		TCP => SOCK_STREAM
		UDP => SOCK_DGRAM */
	uint16_t _port;
	uint16_t _status;
	/*	File => LTKNDP_REGULAR
		Socket => LTKNDP_SOCKET */
	struct sockaddr _conf;
	socklen_t _len;
} LTKendpoint;

#define LTKTCP 0
#define LTKUDP 1

LTKctr *LTKCntCtr();
LTKobj *LTKCnt(uint8_t, char *);

LTKctr *LTKNdpCtr();
LTKobj *LTKNdp(uint8_t, int);

#endif /* __LTKSKT_H__ */