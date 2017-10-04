#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <err.h>
#include <confuse.h>

#include "common.h"
#include "config.h"


char configfile[MAXCMDLEN+1];

cfg_t *cfg;
cfg_t *cfg_colors;


void read_config(void)
{

	cfg_opt_t color_opts[] =
	{
		CFG_STR("FG", DEFAULT_FG, CFGF_NONE),
		CFG_STR("BG", DEFAULT_BG, CFGF_NONE),
		CFG_STR("FOCUSED_MONITOR_FG", DEFAULT_FG, CFGF_NONE),
		CFG_STR("FOCUSED_MONITOR_BG", DEFAULT_BG, CFGF_NONE),
		CFG_STR("MONITOR_FG", DEFAULT_FG, CFGF_NONE),
		CFG_STR("MONITOR_BG", DEFAULT_BG, CFGF_NONE),
		CFG_STR("FOCUSED_OCCUPIED_FG", DEFAULT_FG, CFGF_NONE),
		CFG_STR("FOCUSED_OCCUPIED_BG", DEFAULT_BG, CFGF_NONE),
		CFG_STR("FOCUSED_FREE_FG", DEFAULT_FG, CFGF_NONE),
		CFG_STR("FOCUSED_FREE_BG", DEFAULT_BG, CFGF_NONE),
		CFG_STR("FOCUSED_URGENT_FG", DEFAULT_FG, CFGF_NONE),
		CFG_STR("FOCUSED_URGENT_BG", DEFAULT_BG, CFGF_NONE),
		CFG_STR("OCCUPIED_FG", DEFAULT_FG, CFGF_NONE),
		CFG_STR("OCCUPIED_BG", DEFAULT_BG, CFGF_NONE),
		CFG_STR("FREE_FG", DEFAULT_FG, CFGF_NONE),
		CFG_STR("FREE_BG", DEFAULT_BG, CFGF_NONE),
		CFG_STR("URGENT_FG", DEFAULT_FG, CFGF_NONE),
		CFG_STR("URGENT_BG", DEFAULT_BG, CFGF_NONE),
		CFG_STR("LAYOUT_FG", DEFAULT_FG, CFGF_NONE),
		CFG_STR("LAYOUT_BG", DEFAULT_BG, CFGF_NONE),
		CFG_STR("TITLE_FG", DEFAULT_FG, CFGF_NONE),
		CFG_STR("TITLE_BG", DEFAULT_BG, CFGF_NONE),
		CFG_STR("DATE_FG", DEFAULT_FG, CFGF_NONE),
		CFG_STR("DATE_BG", DEFAULT_BG, CFGF_NONE),
		CFG_END()
	};
	cfg_opt_t command_opts[] =
	{
		CFG_STR("path", "/usr/bin/true", CFGF_NONE),
		CFG_INT("refresh", 15, CFGF_NONE),
		CFG_END()
	};
	cfg_opt_t bar_opts[] =
	{
		CFG_STR("barpath", DEFAULT_PATH, CFGF_NONE),
		CFG_STR("flags", DEFAULT_FLAGS, CFGF_NONE),
		CFG_STR("coordinates", "x16", CFGF_NONE),
		CFG_STR_LIST("fonts", "{}", CFGF_NONE),
		CFG_STR_LIST("monitors", "{\"*\"}", CFGF_NONE),
		CFG_SEC("command", command_opts, CFGF_TITLE | CFGF_MULTI),
		CFG_INT("hasclock", 1, CFGF_NONE),
		CFG_INT("hastitle", 1, CFGF_NONE),
		CFG_END()
	};
	cfg_opt_t opts[] =
	{
		CFG_SEC("colors", color_opts, CFGF_NONE),
		CFG_SEC("bar", bar_opts, CFGF_TITLE | CFGF_MULTI),
		CFG_STR("datefmt", DEFAULT_DATEFMT, CFGF_NONE), // for strftime()
		CFG_INT("refresh", 1, CFGF_NONE),
		CFG_FUNC("include", cfg_include),
		CFG_END()
	};
	
	cfg = cfg_init(opts, CFGF_NONE);
	if(cfg_parse(cfg, configfile) == CFG_PARSE_ERROR)
		errx(1, "Could not parse config file: %s", configfile);

	if(cfg_size(cfg, "bar") == 0)
		cfg_parse_buf(cfg, "bar one {}");
	if(cfg_size(cfg, "colors") == 0)
		cfg_parse_buf(cfg, "colors {}");
	cfg_colors = cfg_getsec(cfg, "colors");

}
