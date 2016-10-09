#ifndef MYLIBCONFIG_H
#define MYLIBCONFIG_H


#include <libconfig.h>


config_t new_cfg();
void cfg_loadfile(config_t*, const char*);
config_setting_t* cfg_findsetting(config_t*, char*);
config_setting_t* cfg_getelem(config_setting_t*, int);
const char* cfg_getstring(config_setting_t*, const char*);
int cfg_getint(config_setting_t*, const char*);
double cfg_getdouble(config_setting_t*, const char*);
int cfg_settinglength(config_setting_t*);
config_setting_t* cfg_addsetting(config_setting_t*, const char*, int);
void cfg_setint(config_setting_t*, int);
void cfg_setdouble(config_setting_t*, double);
void cfg_setstring(config_setting_t*, const char*);
void cfg_destroy(config_t*);


#endif /* MYLIBCONFIG_H */
