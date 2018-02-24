typedef union {
    int    Node;        /* Index of Node */
    double dbl;         /* real value    */
    long   lng;         /* integer value */
    char   log;         /* logical value */
    char   str[256];    /* string value  */
} FFSTYPE;
#define	BOOLEAN	258
#define	LONG	259
#define	DOUBLE	260
#define	STRING	261
#define	BITSTR	262
#define	FUNCTION	263
#define	BFUNCTION	264
#define	GTIFILTER	265
#define	REGFILTER	266
#define	COLUMN	267
#define	BCOLUMN	268
#define	SCOLUMN	269
#define	BITCOL	270
#define	ROWREF	271
#define	NULLREF	272
#define	SNULLREF	273
#define	OR	274
#define	AND	275
#define	EQ	276
#define	NE	277
#define	GT	278
#define	LT	279
#define	LTE	280
#define	GTE	281
#define	POWER	282
#define	NOT	283
#define	INTCAST	284
#define	FLTCAST	285
#define	UMINUS	286
#define	ACCUM	287
#define	DIFF	288


extern FFSTYPE fflval;
