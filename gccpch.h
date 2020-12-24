// gccpch.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently.
// gccpch.h.gch will contain the pre-compiled type information
//

#ifndef __GCCPCH__
#define __GCCPCH__

// TODO: reference additional headers your program requires here
int console_write(const char * s);
int (*plugin_SetInterface)(void * view, int interface, void * Handler) = 0;
int (*plugin_SetInterface2)(void * view, int interface, void * Handler) = 0;
int (*vsh_sprintf)( char*, const char*,...) = 0;

void log(char * buffer);
void log(char * format, char * param1);
void log(char * format, const char * param1);
void log(char * format, const wchar_t * param1);
void log(char * format, char param1);
void log(char * format, int param1);
void log(char * format, float param1);


#define SYS_SOFT_REBOOT 				0x0200
#define SYS_HARD_REBOOT					0x1200


class xai_plugin_interface_action
{	
public:
	static void xai_plugin_action(const char * action);
};

void * xai_plugin_action_if[3] = {(void*)xai_plugin_interface_action::xai_plugin_action,
							0,
							0};

class xai_plugin_interface
{
public:	
	static void xai_plugin_init(void * view);
	static void xai_plugin_start(void * view);
	static void xai_plugin_stop(void);
	static void xai_plugin_exit(void);
};

void * xai_plugin_functions[4] = {(void*)xai_plugin_interface::xai_plugin_init,
	(void*)xai_plugin_interface::xai_plugin_start,
	(void*)xai_plugin_interface::xai_plugin_stop,
	(void*)xai_plugin_interface::xai_plugin_exit};


#endif
