extern int havesecstore(char *addr, char *owner);
extern char *secstore;
extern char *secstorefetch(char *addr, char *owner, char *passwd);
extern char *authserver;
extern int exportfs(int);
extern int dialfactotum(void);
extern char *getuser(void);
extern void cpumain(int, char**);
extern char *estrdup(char*);
extern int aanclient(char*, int);
extern int drawdbg;
extern FILE *dtlog;
extern void printloginfo(void);

#define LOG(...) printloginfo(); fprintf(dtlog, __VA_ARGS__); fprintf(dtlog, "\n"); fflush(dtlog);
