#ifndef FUNCS_H
#define FUNCS_H
pid_t  startworker(void (*func)(char*, char*) , char *string, unsigned int seconds, BOOL *running, char *optarg);
void time_to_string(char* output, char* fmt);
void setup_signal(int sig, void (*func)(int));
//char** explode_str(char* input, const char a_delim);
int parse_bspwm(Monitor ** mons, char* input);
int snprintf_monitor(char *dst, size_t size, Monitor *mon);
#endif
