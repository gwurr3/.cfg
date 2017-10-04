#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include <string.h>

#ifdef __linux_
#include <bsd/string.h>
#endif

#include <confuse.h>

#include "common.h"
#include "config.h"
#include "funcs.h"

/*              function to run, string to update , secs to wait , pointer to BOOL to check in while loop, optional argument string */
pid_t  startworker(void (*func)(char*, char*) , char *string, unsigned int seconds, BOOL *running, char *optarg)
{
	pid_t pid = fork();

	switch (pid) {
		case -1:
			errx(1, "could not fork worker proc");

		case 0:
			while(*running) {
				func(string, optarg);
				sleep(seconds);
			}
			exit(EXIT_SUCCESS);
		
		default:
			warnx("Worker proc %d forked", pid);
			return pid;
	}
	return -1;
}

void time_to_string(char* output, char* fmt)
{
	time_t rawtime;
	struct tm *info;

	time( &rawtime );
	info = localtime( &rawtime );

	strftime(output,SHARED_STRING_LEN,fmt, info);

}

void setup_signal(int sig, void (*func)(int)) {
	struct sigaction sa;
	sigfillset(&sa.sa_mask);
	sa.sa_handler = func;
	sa.sa_flags = 0;
	sigaction(sig, &sa, NULL);
}


static char** explode_str(char* input, const char a_delim)
{
	char** result    = 0;
	size_t count     = 0;
	char* tmp        = input;
	char* last_comma = 0;
	char delim[2];
	delim[0] = a_delim;
	delim[1] = 0;
	
	/* Count how many elements will be extracted. */
	while (*tmp) {
		if (a_delim == *tmp) {
			count++;
			last_comma = tmp;
		}
		tmp++;
	}
	
	/* Add space for trailing token. */
	count += last_comma < (input + strlen(input) - 1);
	
	/* Add space for terminating null string so caller
	   knows where the list of returned strings ends. */
	count++;
	
	result = malloc(sizeof(char*) * count);
	
	if (result) {
		size_t idx  = 0;
		char* token = strtok(input, delim);
		
		while (token) {
			assert(idx < count);
			*(result + idx++) = strdup(token);
			token = strtok(0, delim);
		}
		assert(idx == count - 1);
		*(result + idx) = 0;
	}
	
	return result;

}

/*
 *  example input string: "MLEFT:oI:OII:fIRC1:oIRC2:oK:fmisc:LT:TF:G:mRIGHT:fmisc2:fWEB2:OWEB1:LM:TT:G:mBOTH:FBIG:LT" 
 *  note that the leading 'W' is missing. it is detected and removed elsewhere.
 *  PCRE examples:
 *  whole line: ((([mM])([^:]+))((?::[fFoOuU][^:]+)*):L([^:])(:T([^:]))?(:G([^:]?))?) 
 *  "monitor" section of above ( group 5 ) : (:([fFoOuU])([^:]+)) 
 *
 */
int parse_bspwm(Monitor ** monsptr, char* input)
{
	Monitor *mons = NULL;
	int monidx = 0;
	size_t nummons = 0;
	char** tokens;
	tokens = explode_str(input, ':');
	if(tokens) {
		int j;
		for (j = 0; *(tokens + j); j++)
			if(*(tokens + j)[0] == 'M' || *(tokens + j)[0] == 'm')
				nummons++;
		if(nummons > MAXMONS)
			nummons = MAXMONS;
		if(nummons)
			mons = malloc(nummons * sizeof(Monitor));
	}
	if (mons) {
		int i;
		int deskidx = 0;
		Monitor *mon;
		BOOL focus;
		DSTATE state;
		mon = NULL;
		for (i = 0; *(tokens + i); i++) {
			//warnx("token=[%s]\n", *(tokens + i));
			focus = FALSE;
			state = FREE;
			switch(*(*(tokens + i)) ) {
				case 'M':
					focus = TRUE;
				case 'm':
					if(monidx < MAXMONS) {
						monidx++;
						deskidx = 0;
						mon = &mons[monidx-1];
						mon->focused = focus;
						strlcpy(mon->name, *(tokens + i)+1, MAXMONLEN-1);
						mon->layout = ' ';
						mon->state = ' ';
						strlcpy(mon->flags, " ", 7);
						mon->numdesks = 0;
					}
					break;
				case 'O':
					focus = TRUE;
				case 'o':
					state = OCCU;
					goto newdesk;
					break;
				case 'U':
					focus = TRUE;
				case 'u':
					state = URGT;
					goto newdesk;
					break;
				case 'F':
					focus = TRUE;
				case 'f':
					state = FREE;
newdesk:
					if(mon)
						if(deskidx < MAXMONS) {
							deskidx++;
							mon->numdesks++;
							mon->desks[deskidx-1].focused = focus;
							mon->desks[deskidx-1].state = state;
							strlcpy(mon->desks[deskidx-1].name, *(tokens + i)+1, MAXMONLEN-1);
						}
					break;

				case 'L':
					if(mon) {
						if(tokens[i][1] != '\0')
							mon->layout = tokens[i][1];
					}
					break;
				case 'T':
					if(mon)
						if(tokens[i][1] != '\0')
							mon->state = tokens[i][1];
							//mon->state = *(*(tokens + i)+1);
					break;
				case 'G':
					if(mon)
						if(*(*(tokens + i)+1) != '\0')
							strlcpy(mon->flags, tokens[i]+1, 4);
					break;


				default:
					warnx("got invalid bspwm token");
					break;
			}
			//warnx("done with token %s", tokens[i]);
			free(tokens[i]);
		}
		free(tokens);
		*monsptr = (Monitor *)mons;
	}
	return nummons;
}

static int bytes_added( int result_of_sprintf ) {
	    return (result_of_sprintf > 0) ? result_of_sprintf : 0;
}

// for reference
//#define COLOR(arg1) cfg_getstr(cfg_colors, arg1)
//#define COLORRESET "%%{B-}%%{F-}"
int snprintf_monitor(char *dst, size_t size, Monitor *mon)
{
	if((mon == NULL) || (dst == NULL) || (cfg_colors == NULL))
		return -1;
	if(size == 0)
		return 0;

	int i, length = 0;

	//warnx("now in snprintf_monitor");

	length += bytes_added(snprintf(dst+length, size-length,
				mon->focused ? "%c%%{B%s}%%{F%s}  < %s >  %%{B-}%%{F-}" : "%c%%{B%s}%%{F%s} %s %%{B-}%%{F-}",
				mon->focused ? 'F' : ' ',
				mon->focused ? COLOR("FOCUSED_MONITOR_BG") : COLOR("MONITOR_BG"),
				mon->focused ? COLOR("FOCUSED_MONITOR_FG") : COLOR("MONITOR_FG"),
				mon->name
				)
			);

	for(i = 0; i < mon->numdesks ; i++ )
		if(mon->desks[i].state == OCCU)
			length += bytes_added(snprintf(dst+length, size-length,
						"%%{B%s}%%{F%s} %s %%{B-}%%{F-}",
						mon->desks[i].focused ? COLOR("FOCUSED_OCCUPIED_BG") : COLOR("OCCUPIED_BG"),
						mon->desks[i].focused ? COLOR("FOCUSED_OCCUPIED_FG") : COLOR("OCCUPIED_FG"),
						mon->desks[i].name
						)
					);
		else if(mon->desks[i].state == URGT)
			length += bytes_added(snprintf(dst+length, size-length,
						"%%{B%s}%%{F%s} %s %%{B-}%%{F-}",
						mon->desks[i].focused ? COLOR("FOCUSED_URGENT_BG") : COLOR("URGENT_BG"),
						mon->desks[i].focused ? COLOR("FOCUSED_URGENT_FG") : COLOR("URGENT_FG"),
						mon->desks[i].name
						)
					);
		else
			length += bytes_added(snprintf(dst+length, size-length,
						"%%{B%s}%%{F%s} %s %%{B-}%%{F-}",
						mon->desks[i].focused ? COLOR("FOCUSED_FREE_BG") : COLOR("FREE_BG"),
						mon->desks[i].focused ? COLOR("FOCUSED_FREE_FG") : COLOR("FREE_FG"),
						mon->desks[i].name
						)
					);

	length += bytes_added(snprintf(dst+length, size-length,
				"%%{B%s}%%{F%s} %c %c %s %%{B-}%%{F-}",
				COLOR("LAYOUT_BG"),
				COLOR("LAYOUT_FG"),
				(mon->layout == ' ' || mon->layout == '\0') ? '?' : mon->layout,
				(mon->state == ' ' || mon->state == '\0') ? '?' : mon->state,
				mon->flags
				)
			);




	return length;


}

