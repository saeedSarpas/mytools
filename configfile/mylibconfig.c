/*
 * configfile/mylibconfig.c
 * test_file: configfile/mylibconfig_test.c
 *
 * Wrapping libconfig functions
 *
 * author: Saeed Sarpas
 */


#include <stdlib.h>
#include <libconfig.h>
#include "./../memory/allocate.h"


/*
 * Initializing config by creating a new config object
 */
config_t* new_cfg()
{
  config_t *cfg = allocate(1, sizeof(config_t));
	config_init(cfg);
  return cfg;
}


/*
 * Loading a config file
 */
void cfg_loadfile(config_t *cfg, const char *file_path)
{
	if(!config_read_file(cfg, file_path)){
		printf("[%s:%d - %s]\n", config_error_file(cfg),
           config_error_line(cfg), config_error_text(cfg));
		config_destroy(cfg);
		exit(EXIT_FAILURE);
	}
}


/*
 * Finding a path (setting) inside a config
 */
config_setting_t* cfg_findsetting(config_t *cfg, char *path)
{
	config_setting_t *setting = config_lookup(cfg, path);

	if(setting != NULL) return setting;

  printf("[Did not find '%s' in config file]\n", path);
  config_destroy(cfg);
  exit(EXIT_FAILURE);
}


/*
 * Getting an element inside a setting
 */
config_setting_t* cfg_getelem(config_setting_t *setting, int index)
{
	config_setting_t *elem = config_setting_get_elem(setting, index);
	if(elem != NULL) return elem;

  printf("[Cannot find the element]\n");
  exit(EXIT_FAILURE);
}


/*
 * Getting a string from a specific field inside a setting
 */
const char* cfg_getstring(config_setting_t *setting, const char *field)
{
	const char *value;
	if(config_setting_lookup_string(setting, field, &value)) return value;

  printf("[Unable to find the field: %s]\n", field);
  exit(EXIT_FAILURE);
}


/*
 * Getting a string from a specific field inside a setting
 */
int cfg_getint(config_setting_t *setting, const char *field)
{
	int value;
	if(config_setting_lookup_int(setting, field, &value)) return value;

  printf("[Unable to find the field: %s]\n", field);
  exit(EXIT_FAILURE);
}


/*
 * Getting a double from a specific field inside a setting
 */
double cfg_getdouble(config_setting_t *setting, const char *field)
{
	double value;
	if(config_setting_lookup_float(setting, field, &value)) return value;

  printf("[Unable to find the field: %s]\n", field);
  exit(EXIT_FAILURE);
}


/*
 * Getting the length of the setting
 */
int cfg_settinglength(config_setting_t *setting){
	int len = config_setting_length(setting);
	if(len) return len;

  printf("[The length of the setting is zero or unavailable]\n");
  exit(EXIT_FAILURE);
}


/*
 * Add a new setting
 */
config_setting_t* cfg_addsetting(config_setting_t *parent, const char *name,
                                 int type)
{
	config_setting_t *new_setting = config_setting_add(parent, name, type);
	if(new_setting) return new_setting;

  printf("[Unable to add new setting]\n");
  exit(EXIT_FAILURE);
}


/*
 * Set an integer value to a given setting
 */
void cfg_setint(config_setting_t *setting, int value)
{
	if(config_setting_set_int(setting, value)) return;

  printf("[Unable to set integer value]\n");
  exit(EXIT_FAILURE);
}


/*
 * Set a double value to a given setting
 */
void cfg_setdouble(config_setting_t *setting, double value)
{
	if(config_setting_set_float(setting, value)) return;

  printf("[Unable to set double value]\n");
  exit(EXIT_FAILURE);
}


/*
 * Set a string to a given setting
 */
void cfg_setstring(config_setting_t *setting, const char *value)
{
	if(config_setting_set_string(setting, value) == CONFIG_TRUE) return;

  printf("[Unable to set the string value]\n");
  exit(EXIT_FAILURE);
}


/*
 * Destroying a config object
 */
void cfg_destroy(config_t *cfg)
{
	config_destroy(cfg);
}
