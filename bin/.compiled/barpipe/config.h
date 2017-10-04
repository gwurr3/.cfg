#ifndef CONFIGFUNC_H
#define CONFIGFUNC_H


extern char configfile[MAXCMDLEN+1];

extern cfg_t *cfg;
extern cfg_t *cfg_colors;


void read_config(void);

#define COLOR(arg1) cfg_getstr(cfg_colors, arg1)
#define COLORRESET "%%{B-}%%{F-}"

#endif
