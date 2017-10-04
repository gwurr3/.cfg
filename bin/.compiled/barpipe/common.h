// max number of bspwm "monitors" ( and "desktops," per each monitor ) to care about
#ifndef MAXMONS
#define MAXMONS 8
#endif
#ifndef MAXMONLEN
#define MAXMONLEN 32
#endif
// max length of command string and a few other general strings used in this program
#ifndef MAXCMDLEN
#define MAXCMDLEN 512
#endif
// this isn't defined on all systems. notably FreeBSD. ( or maybe im not including he right file on FreeBSD )
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX MAXCMDLEN
#endif

// max number of bspwm "monitors" to care about
#ifndef MAXBARS
#define MAXBARS 16
#endif

#ifndef SHARED_STRING_SIZE
#define SHARED_STRING_SIZE 1024
#ifndef SHARED_STRING_LEN
#define SHARED_STRING_LEN SHARED_STRING_SIZE-1
#endif
#endif

#ifndef LINEBUF
#define LINEBUF 4096
#endif

#ifndef DEFAULT_PATH
#define DEFAULT_PATH "/usr/bin/lemonbar"
#endif
#ifndef DEFAULT_FLAGS
#define DEFAULT_FLAGS "-a 32 -d -n ignore"
#endif
#ifndef DEFAULT_DATEFMT
#define DEFAULT_DATEFMT "%H:%M:%S"
#endif
#ifndef USAGE_STRING
#define USAGE_STRING  "Usage:\n	%s [CONFIGFILE]\nExample:\n	bspc subscribe report | %s ~/%s.conf &\n	If [CONFIGFILE] is omitted, %s will look for $XDG_CONFIG_HOME/%s/HOSTNAME.conf,\n	where HOSTNAME is the short hostname of your machine.\n	If it doesn't exist, %s will still run and rely on compiled-in defaults."
#endif
#ifndef DEFAULT_FG
#define DEFAULT_FG  "#FFFFFF"
#endif
#ifndef DEFAULT_BG
#define DEFAULT_BG  "#000000"
#endif

#define TRUE 1
#define FALSE 0
typedef unsigned char BOOL;

#define OCCU 2
#define URGT 1
#define FREE 0
typedef unsigned char DSTATE;


typedef struct Bar {
	int index;
	cfg_t *cfg;
	const char *name;
	//const char *cmd;
	char cmd[MAXCMDLEN+1];
	int nummons;
	const char * mons[MAXMONS];
	FILE *pipe;
	char *line;
	BOOL hasclock;
	BOOL hastitle;
	BOOL *hasfocus;
} Bar;


typedef struct Desktop {
	BOOL focused;
	DSTATE state;
	char name[MAXMONLEN];

} Desktop;

typedef struct Monitor {
	BOOL focused;
	char name[MAXMONLEN];
	char layout;
	char state;
	char flags[8];
	int numdesks;
	Desktop desks[MAXMONS];
} Monitor;

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

#define MIN(A, B)         ((A) < (B) ? (A) : (B))

#define TITLEFORMAT         L"< %ls >"
/* Reference: http://www.sensi.org/~alec/locale/other/ctext.html */
#define CT_UTF8_BEGIN   "\x1b\x25\x47"
#define CT_UTF8_END     "\x1b\x25\x40"


