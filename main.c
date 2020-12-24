#include <sdk_version.h>
#include <cellstatus.h>
#include <cell/cell_fs.h>
#include <cell/rtc.h>
#include <cell/gcm.h>
#include <cell/pad.h>
#include <sys/vm.h>
#include <sysutil/sysutil_common.h>

#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <sys/event.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/memory.h>
#include <sys/timer.h>
#include <sys/process.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netex/net.h>
#include <netex/errno.h>
#include <netex/libnetctl.h>
#include <netex/sockinfo.h>

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#include "types.h"

#include "rebug.h"

#include "inc/exports.h"
#include "printf.h"

#define SYS_SOFT_REBOOT 				0x8201
#define SYS_HARD_REBOOT					0x1200

SYS_MODULE_INFO(edy_plugin, 0, 1, 1);
SYS_MODULE_START(prx_start);
SYS_MODULE_STOP(prx_stop);
SYS_MODULE_EXIT(prx_stop);

int prx_start(size_t args, void *argp);
int prx_stop(void);

int sys_sm_shutdown(uint16_t op);
void xmb_reboot(uint16_t op);

static void wm_plugin_init (int view);
static int  wm_plugin_start(void * view);
static int  wm_plugin_stop (void);
static void wm_plugin_exit (void);
static void wm_plugin_action(const char * action);
int setInterface(unsigned int view);

static int (*plugin_SetInterface)(int view, int interface, void * Handler);
static int (*plugin_SetInterface2)(int view, int interface, void * Handler);

static void *wm_plugin_action_if[3] = {(void*)(wm_plugin_action), 0, 0};

static void wm_plugin_init (int view)		{plugin_SetInterface( view, 0x41435430 /*ACT0*/, wm_plugin_action_if);}
static int  wm_plugin_start(void * view)	{return SYS_PRX_START_OK;}
static int  wm_plugin_stop (void)			{return SYS_PRX_STOP_OK;}
static void wm_plugin_exit (void)			{return;}

static void *wm_plugin_functions[4] =
{
		(void*)(wm_plugin_init),
		(int* )(wm_plugin_start),
		(int* )(wm_plugin_stop),
		(void*)(wm_plugin_exit)
};

static void * getNIDfunc(const char * vsh_module, uint32_t fnid)
{
	uint32_t table = (*(uint32_t*)0x1008C) + 0x984; // vsh table address

	while(((uint32_t)*(uint32_t*)table) != 0)
	{
		uint32_t* export_stru_ptr = (uint32_t*)*(uint32_t*)table;

		const char* lib_name_ptr =  (const char*)*(uint32_t*)((char*)export_stru_ptr + 0x10);

		if(strncmp(vsh_module, lib_name_ptr, strlen(lib_name_ptr))==0)
		{
			uint32_t lib_fnid_ptr = *(uint32_t*)((char*)export_stru_ptr + 0x14);
			uint32_t lib_func_ptr = *(uint32_t*)((char*)export_stru_ptr + 0x18);
			uint16_t count = *(uint16_t*)((char*)export_stru_ptr + 6); // number of exports
			for(int i=0;i<count;i++)
			{
				if(fnid == *(uint32_t*)((char*)lib_fnid_ptr + i*4))
				{
					return (void**)*((uint32_t*)(lib_func_ptr) + i);
				}
			}
		}
		table=table+4;
	}
	return 0;
}

int setInterface(unsigned int view)
{
	plugin_SetInterface = (void*)((int*)getNIDfunc("paf",0xA1DC401));
	plugin_SetInterface2 = (void*)((int*)getNIDfunc("paf",0x3F7CB0BF));
	plugin_SetInterface2(view, 1, (void*)wm_plugin_functions);
	return 0;
}

int sys_sm_shutdown(uint16_t op)
{ 	
	system_call_3(379, (uint64_t)op, 0, 0);
	return_to_user_prog(int);
}
void xmb_reboot(uint16_t op)
{
	cellFsUnlink("/dev_hdd0/tmp/turnoff");
	sys_sm_shutdown(op);
}

void wm_plugin_action(const char * action)
{
	if(strcmp(action,"soft_reboot_action")==0)
	{
		xmb_reboot(SYS_SOFT_REBOOT);
	}
	else if(strcmp(action,"hard_reboot_action")==0)
	{
		xmb_reboot(SYS_HARD_REBOOT);
		// vshmain_87BB0001(2);
	}
	else if(strcmp(action,"cobra_stage")==0)
	{
		if(cobra_stage() == CELL_OK)
			xmb_reboot(SYS_SOFT_REBOOT);
	}
	else if(strcmp(action,"debugsettings_mode")==0)
	{
		debugsettings_mode();
	}
	else if(strcmp(action,"boot_mode")==0)
	{
		if(boot_mode() == true)
			xmb_reboot(SYS_SOFT_REBOOT);
	}
	else if(strcmp(action,"debug_mode")==0)
	{
		if(debug_mode() == true)
			xmb_reboot(SYS_SOFT_REBOOT);
	}
	else if(strcmp(action,"load_ckernel")==0)
	{
		if(load_ckernel() == true)
			xmb_reboot(SYS_SOFT_REBOOT);
	}
	else if(strcmp(action,"load_dkernel")==0)
	{
		if(load_dkernel() == true)
			xmb_reboot(SYS_SOFT_REBOOT);
	}
	else if(strcmp(action,"patch_dkernel")==0)
	{
		patch_dkernel();
	}
	else if(strcmp(action,"load_prx")==0)
	{
		load_prx();
	}
	else if(strcmp(action,"spe_mode")==0)
	{
		if(spe_mode() == true)
			xmb_reboot(SYS_HARD_REBOOT);
	}
	else if(strcmp(action,"cobra_mode")==0)
	{
		if(cobra_mode() == CELL_OK)
			xmb_reboot(SYS_SOFT_REBOOT);
	}
	else if(strcmp(action,"rebug_mode")==0)
	{
		if(rebug_mode() == CELL_OK)
			xmb_reboot(SYS_SOFT_REBOOT);
	}
	else if(strcmp(action,"normal_mode")==0)
	{
		if(normal_mode() == CELL_OK)
			xmb_reboot(SYS_SOFT_REBOOT);
	}
	else if(strcmp(action,"toggle_bootplugins")==0)
	{
		if(toggle_bootplugins() == CELL_OK)
			xmb_reboot(SYS_SOFT_REBOOT);
	}
	else if(strcmp(action,"toggle_sm")==0)
	{
		if(toggle_sm() == CELL_OK)
			xmb_reboot(SYS_SOFT_REBOOT);
	}
	else if(strcmp(action,"dump_eeprom")==0)
	{
		if(dump_eeprom() == CELL_OK)
			{system_call_3(392, 0x1004, 0x4, 0x6); } //1xBeep
	}
	else if(strcmp(action,"write_eeprom")==0)
	{
		// if(write_eeprom() == CELL_OK)
		write_eeprom();
		// xmb_reboot(SYS_HARD_REBOOT);
	}
	else if(strcmp(action,"bootmem_toggle")==0)
	{
		bootmem_toggle();
		xmb_reboot(SYS_SOFT_REBOOT);
	}
	else if(strcmp(action,"cu_toggle")==0)
	{
		cu_toggle();
	}
/* 	else if(strcmp(action,"write_eeprom()")==0)
	{
		uint64_t offset;
		uint8_t value;
		// if(write_eeprom() == CELL_OK)
		write_eeprom(offset,value);
			// xmb_reboot(SYS_HARD_REBOOT);
	} */
	// else show_msg((char*)"webMAN not ready!");
}


int prx_start(size_t args, void *argp)
{
	setInterface(*(unsigned int*)argp);
	// return SYS_PRX_NO_RESIDENT;
	return SYS_PRX_RESIDENT;
}

int prx_stop(void)
{
	return SYS_PRX_STOP_OK;
}
