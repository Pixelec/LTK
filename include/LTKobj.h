#ifndef __LTKOBJ_H__
#define __LTKOBJ_H__

#define PRINT64		"%llu"
#define PRINT64_5	"%5llu"
#define PRINT64_8	"%8llu"

#define PRINT32		"%llu"
#define PRINT32_3	"%3llu"

#if (LONG_BIT == 64)
#undef PRINT64
#undef PRINT64_5
#undef PRINT64_8
#define PRINT64		"%lu"
#define PRINT64_5	"%5lu"
#define PRINT64_8	"%8lu"
#endif

#if (WORD_BIT == 64)
#undef PRINT64
#undef PRINT64_5
#undef PRINT64_8
#define PRINT64		"%u"
#define PRINT64_5	"%5u"
#define PRINT64_8	"%8u"
#endif

#if (LONG_BIT == 32)
#undef PRINT32
#undef PRINT32_3
#define PRINT32		"%lu"
#define PRINT32_3	"%3lu"
#endif

#if (WORD_BIT == 32)
#undef PRINT32
#undef PRINT32_3
#define PRINT32		"%u"
#define PRINT32_3	"%3u"
#endif

struct LTKobj_s;

typedef void (*LTKfct)(struct LTKobj_s *, va_list); // General method definition
typedef void (*LTKhdl)(struct LTKobj_s **); // General event handler including free

typedef struct LTKbuf_s {
	struct LTKbuf_s *_nxt; /* Next pool */
	struct LTKctr_s *_ctr; /* Constructor */
	uint64_t _pcd; /* Number of cumulated instance */
	uint64_t _fre[]; /* Bitewise array of free flags */
	/* Instances comes here after _fre */
} LTKbuf;

typedef struct LTKobj_s {
	LTKbuf *_buf; /* Pool containing this instance */
	LTKfct *_fct; /* Function pointer to constructor's to function array */
    /* Object data */
} LTKobj;

typedef struct LTKctr_s {
	struct LTKctr_s *_slv; /* Slave */
	struct LTKctr_s *_nxt; /* Next */
	LTKbuf *_buf; /* Pool list */
	uint64_t _cnt; /* Highest object id + 1 */
	uint64_t _fnb; /* Number of function */
	uint64_t _dsz; /* Data size multiple of sizeof(void *) */
	uint32_t _vrt; /* Is this a virtual class */
	LTKhdl _hdl; /* Event handler */
	LTKfct _fct[]; /* Function array */
	/* Take care, function ID are absolute and positive, it is never relative to a derived object */
} LTKctr;

/* To add a constructor use : mother class constructor, funcnum, datsize, LTKhdl hdl) */
/* To define default pool size use : X, defpoolsize, 0, X */
LTKctr *LTKCtr(LTKctr *, uint64_t, uint64_t, LTKhdl);
LTKobj *LTKAdd(LTKctr *); /* Allocate an object or free all memory with 0 as argument */
void LTKRun(LTKobj *, uint64_t, ...);
uint64_t LTKNum(LTKobj *); /* Get object number of a given constructor */
LTKobj *LTKOid(LTKctr *, uint64_t); /* Get object from it's number in the pool */
void LTKClr(LTKobj *obj); /* Clear an object content */
void LTKFre(LTKobj *); /* Runs ctr._hdl method with obj._buf field reseted if ctr._hld defined */
/* Free all memory if 0 as argument */

int LTKTrg(LTKobj *, int, uint32_t);
LTKobj *LTKWte(int *);

#endif /* __LTKOBJ_H__ */
