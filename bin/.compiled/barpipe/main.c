#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#define _BSD_SOURCE             /* Get MAP_ANONYMOUS definition */
#include <sys/mman.h>
#include <signal.h>
#include <sys/wait.h>
#include <setjmp.h>
#include <confuse.h>
#include <string.h>
#include <limits.h>
#include <assert.h>


#ifdef __linux_
#include <bsd/string.h>
#endif

#include "common.h"
#include "funcs.h"
#include "config.h"
#include "title.h"

//volatile char buffer[LINEBUF];
char buffer[LINEBUF];
char tmpbuffer[SHARED_STRING_SIZE];
sigjmp_buf envstopping;
BOOL stopping = FALSE;
BOOL restarting = FALSE;
BOOL firstrun = TRUE;
Bar Bars[MAXBARS];
int numbars;
char *title, *date;

static void handle_reload(int signum)
{
	restarting = TRUE;
	siglongjmp(envstopping,1);
}
static void handle_quit(int signum)
{
	setup_signal(SIGCHLD, SIG_DFL);
	stopping = TRUE;
	siglongjmp(envstopping,1);
}
/*
static void handle_zombies(int signum)
{
	pid_t kidpid;
	int status;
	
	warnx("Worker process has died with signal: %d", signum);
	while ((kidpid = waitpid(-1, &status, WNOHANG)) > 0) {
		warnx("Child %d terminated", kidpid);
	}
	stopping = TRUE;
	siglongjmp(envstopping,1);
}
*/

static int pipe_to_bars(void)
{
	int j, k = numbars;
	for(j = 0; j < numbars; j++)
		if( fputs( Bars[j].hastitle ? "   %{l}" : "%{c}", Bars[j].pipe ) != EOF )
			if( fputs(Bars[j].line, Bars[j].pipe) != EOF )
				if( fprintf(Bars[j].pipe,
							"%s%%{F%s}%%{B%s}%s%%{B-}%%{F-}%s",
						       ( Bars[j].hastitle && *(Bars[j].hasfocus) ) ? "%{c}" : "%{r}",
							COLOR("TITLE_FG"),
							COLOR("TITLE_BG"),
						       ( Bars[j].hastitle && *(Bars[j].hasfocus) ) ? title : " ",
						       ( Bars[j].hastitle && *(Bars[j].hasfocus) ) ? "%{r}" : " "
						       ) != EOF )
					if( (Bars[j].hasclock && fprintf(Bars[j].pipe,
										"%%{F%s}%%{B%s}%s%%{B-}%%{F-}",
										COLOR("DATE_FG"),
										COLOR("DATE_BG"),
										date
									) > 0 ) || TRUE )
						if( fputc('\n', Bars[j].pipe) != EOF )
							if( fflush(Bars[j].pipe) != EOF )
								k--;
	return k;
}


int main( int argc, char *argv[] )
{
	unsigned int refresh;
	int j;
	BOOL *running;
	char *tmp1, *tmp2, hostname[HOST_NAME_MAX+1], progname[MAXCMDLEN+1];
	pid_t wpid;

	if (gethostname(hostname, sizeof(hostname)))
		err(1, "gethostname");

	// find the location of the first '.' in the string, and if found, truncate the string at that location
	if ((tmp1 = strchr(hostname, '.')))
		*tmp1 = '\0';

	strlcpy(progname, argv[0], sizeof(progname));
	if ((tmp1 = strrchr(progname, '/')))
		strlcpy(progname, ++tmp1, sizeof(progname));

	tmp1=NULL;

	if ( argc == 1 ) {
		if (!(tmp2 = getenv("XDG_CONFIG_HOME"))) {
			if(!(tmp2 = getenv("HOME"))) {
				errx(1, "Couldn't determine XDG_CONFIG_HOME path.\nCheck your ENV variables!");
			} else {
				strlcpy(configfile, tmp2, sizeof(configfile));
				strlcat(configfile, "/.config", sizeof(configfile));
			}
		} else {
			strlcpy(configfile, tmp2, sizeof(configfile));
		}
		strlcat(configfile, "/", sizeof(configfile));
		strlcat(configfile, progname, sizeof(configfile));
		// libconfuse "include()" function within a config file is relative to CWD, so chdir now:
		if(chdir(configfile))
			warnx("could not set CWD to %s", configfile);
		strlcat(configfile, "/", sizeof(configfile));
		strlcat(configfile, hostname, sizeof(configfile));
		strlcat(configfile, ".conf", sizeof(configfile));

	} else if ( argc == 2 && (strcmp(argv[1], "-h") != 0) ) {
		strlcpy(configfile, argv[1], sizeof(configfile));
	} else {
		errx(1, USAGE_STRING, progname, progname, progname, progname, progname, progname);
	}

	warnx("Using config file %s ", configfile);
	read_config();

	// 
	running = mmap(NULL, sizeof(BOOL), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0); 
	title = (char*)mmap(NULL, SHARED_STRING_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	date = (char*)mmap(NULL, SHARED_STRING_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	if (running == MAP_FAILED || title == MAP_FAILED || date == MAP_FAILED)
		errx(1, "mmap failure");



	// first run bybasses restart cleanup
	if(firstrun)
		goto REG_INIT;
RELOAD:	

	for(j = 0; j < numbars; j++)
		if(pclose(Bars[j].pipe) == -1)
			warnx("error killing bar process for: %s ", Bars[j].name);

	// make sure we dont leave with children still running...
	warnx("waiting for pids to die...");
	while ((wpid = wait(NULL)) > 0);

	for(j = 0; j < numbars; j++)
		if (munmap(Bars[j].line, SHARED_STRING_SIZE) == -1)
			errx(1, "munmap failure");

	read_config();
REG_INIT:
	firstrun = FALSE;

	refresh = (unsigned int) cfg_getint(cfg, "refresh");
	numbars = cfg_size(cfg, "bar");
	if(numbars >= MAXBARS)
		numbars = MAXBARS-1 ;
	refresh = (unsigned int) cfg_getint(cfg, "refresh");


	*running = TRUE;

	assert(numbars < MAXBARS);
	
	for(j = 0; j < numbars; j++)
	{
		int k;
		Bars[j].cfg = cfg_getnsec(cfg, "bar", j);
		Bars[j].name = cfg_title(Bars[j].cfg);
		Bars[j].index = j;

		Bars[j].hasclock = (BOOL) cfg_getint(Bars[j].cfg, "hasclock");
		Bars[j].hastitle = (BOOL) cfg_getint(Bars[j].cfg, "hastitle");

		snprintf(Bars[j].cmd, sizeof(Bars[j].cmd)-1, "%s %s -g %s -F%s -B%s %s%s", 
				cfg_getstr(Bars[j].cfg, "barpath"),
				cfg_getstr(Bars[j].cfg, "flags"),
				cfg_getstr(Bars[j].cfg, "coordinates"),
				COLOR("FG"),
				COLOR("BG"),
				(cfg_size(Bars[j].cfg, "fonts") > 0) ? "-f" : " ",
				(cfg_size(Bars[j].cfg, "fonts") > 0) ? cfg_getnstr(Bars[j].cfg, "fonts", 0) : " "
			);
		// ^ need to make a format_fonts function to make the string according to how many fonts are specified

		Bars[j].pipe = popen(Bars[j].cmd, "w");
		if(Bars[j].pipe == NULL)
			errx(1, "popen failed for bar %s ", Bars[j].name);

		Bars[j].line = (char*)mmap(NULL, SHARED_STRING_SIZE, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
		if (Bars[j].line == MAP_FAILED)
			errx(1, "mmap failure");
		Bars[j].hasfocus = mmap(NULL, sizeof(BOOL), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
		if (Bars[j].hasfocus == MAP_FAILED)
			errx(1, "mmap failure");
		*(Bars[j].hasfocus) = FALSE;

		//strlcpy(Bars[j].line, "!", SHARED_STRING_LEN);
		snprintf(Bars[j].line, SHARED_STRING_LEN-1, "%s started. Waiting for input...", progname);

		Bars[j].nummons = cfg_size(Bars[j].cfg, "monitors");
		if(Bars[j].nummons >= MAXMONS)
			Bars[j].nummons = MAXMONS-1;
		for(k = 0; k < Bars[j].nummons; k++)
			Bars[j].mons[k] = cfg_getnstr(Bars[j].cfg, "monitors", k);

		// loop and start worker for each "custom command" for bar, if any
		//
	}



	startworker(&time_to_string, date, refresh, running, cfg_getstr(cfg, "datefmt"));
	titleworker(title, running);
	
	wpid = fork();
	switch (wpid) {
		case -1:
			errx(1, "could not fork bar updater worker");

		case 0:
			while(*running) {
				if(pipe_to_bars())
					warnx("Error writing to pipe");
				sleep(refresh);
			}
			exit(EXIT_SUCCESS);
		
		default:
			warnx("Bar updater proc %d forked", wpid);
	}

	setup_signal(SIGINT, &handle_quit);
	setup_signal(SIGTERM, &handle_quit);
	setup_signal(SIGCHLD, &handle_reload);

	sigsetjmp(envstopping, 1);
	while(*running == TRUE){
		if(stopping) {
			// disable signal handler
			setup_signal(SIGCHLD, SIG_DFL);
			*running = FALSE;
			goto END;
		} else if(restarting) {
			restarting = FALSE;
			// disable signal handler
			setup_signal(SIGCHLD, SIG_DFL);
			*running = FALSE;
			goto RELOAD;
		}

		if( fgets( buffer, LINEBUF, stdin ) != NULL ) {
			Monitor *mons = NULL;
			int k, q, nummons;

			if ((tmp1 = strchr(buffer, '\r')) || (tmp1 = strchr(buffer, '\n')))
				*tmp1 = '\0';

			// from bspwm
			if (buffer[0] == 'W' && strlcpy(tmpbuffer, buffer+1, SHARED_STRING_LEN)) 
				if((nummons = parse_bspwm(&mons, tmpbuffer)) && mons )
					for(j = 0; j < numbars; j++)
						for(Bars[j].line[0] = '\0', *(Bars[j].hasfocus) = FALSE,  k = 0; k < Bars[j].nummons ; k++)
							for(q = 0; q < nummons ; q++)
								if(Bars[j].mons[k][0] == '*' || strcmp(Bars[j].mons[k], mons[q].name) == 0)
									if(snprintf_monitor(tmpbuffer, SHARED_STRING_LEN, &mons[q]))
										if(strlcat(Bars[j].line, tmpbuffer+1, SHARED_STRING_LEN))
											if(*tmpbuffer == 'F')
												*(Bars[j].hasfocus) = TRUE;
			free(mons);
			if(pipe_to_bars())
				warnx("Error writing to pipe");
		} else {
			stopping = TRUE;
		}
	}



END:
	// cleanup	
	for(j = 0; j < numbars; j++)
		if(pclose(Bars[j].pipe) == -1)
			warnx("error killing bar process for: %s ", Bars[j].name);

	// make sure we dont leave with children still running...
	while ((wpid = wait(NULL)) > 0);

	for(j = 0; j < numbars; j++)
		if (munmap(Bars[j].line, SHARED_STRING_SIZE) == -1 || munmap(Bars[j].hasfocus, sizeof(BOOL)) == -1) 
			warnx("munmap failure for %s bar shared memeory", Bars[j].name);

	if (munmap(running, sizeof(BOOL)) == -1 || munmap(title, SHARED_STRING_SIZE) == -1 || munmap(date, SHARED_STRING_SIZE) == -1)
		errx(1, "munmap failure for global shared vars");

	cfg_free(cfg);

	return(EXIT_SUCCESS);
}
