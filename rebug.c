#include <sdk_version.h>
#include <cellstatus.h>
#include <cell/cell_fs.h>
#include <cell/rtc.h>
#include <cell/pad.h>
#include <cell/fs/cell_fs_file_api.h>
#include <sys/vm.h>
#include <sysutil/sysutil_common.h>
// #include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <sys/event.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/memory.h>
#include <sys/timer.h>
#include <sys/process.h>
/* #include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netex/net.h>
#include <netex/errno.h>
#include <netex/libnetctl.h>
#include <netex/sockinfo.h> */
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include "types.h"
#include "rebug.h"
#include "printf.h"
// #include "log.h"
// #include "cfw_settings.h"
#include "inc/exports.h"
#include "misc.h"

#define SYS_SOFT_REBOOT 				0x8201
#define SYS_HARD_REBOOT					0x1200
#define BOOT_MODE_FLAG_OFFSET 0x48C69
#define UPDATE_MGR_PACKET_ID_READ_EPROM		0x600B
#define UPDATE_MGR_PACKET_ID_WRITE_EPROM	0x600C
#define EPROM_BOOT_MODE_OFFSET				0x48C69
#define EPROM_SPE_OFFSET				0x48C30
#define CCAPI_PATH200	"/dev_hdd0/game/XMBM00001/USRDIR/ccapi.sprx200"
#define CCAPI_PATH250	"/dev_hdd0/game/XMBM00001/USRDIR/ccapi.sprx250"
#define SNMAPI_PATH		"/dev_hdd0/game/XMBM00001/USRDIR/SNMAPI.sprx"
// #define ART_VSH_PATH	"/dev_hdd0/game/ARTPS3001/USRDIR/artemis_ps3.sprx"
#define TRAINER_VSH_PATH	"/dev_hdd0/z/plugins/vsh/vsh.sprx"
#define TEST_VSH_PATH	"/dev_hdd0/z/plugins/vsh/test.sprx"
// #define LIBDAMINI		"/dev_flash/sys/external/libda-mini.sprx"
// #define SELF_PATH	"/dev_hdd0/game/XMBM00001/USRDIR/reload_xmb.self"
#define SYSCALL_TABLE_421D			0x800000000037A1B0ULL	// 4.21 DEX
#define SYSCALL_TABLE_421			0x800000000035BCA8ULL	// 4.21
// #define SYSCALL_TABLE_421D			0x800000000038A4E8ULL	// 4.21 DEX
// #define SYSCALL_TABLE_421			0x8000000000363BE0ULL	// 4.21
// redefinition of pad bit flags
#define	PAD_SELECT	 (1<<0)
#define	PAD_L3		   (1<<1)
#define	PAD_R3		   (1<<2)
#define	PAD_START	   (1<<3)
#define	PAD_UP		   (1<<4)
#define	PAD_RIGHT	   (1<<5)
#define	PAD_DOWN	   (1<<6)
#define	PAD_LEFT	   (1<<7)
#define	PAD_L2		   (1<<8)
#define	PAD_R2		   (1<<9)
#define	PAD_L1		   (1<<10)
#define	PAD_R1		   (1<<11)
#define	PAD_TRIANGLE (1<<12)
#define	PAD_CIRCLE   (1<<13)
#define	PAD_CROSS    (1<<14)
#define	PAD_SQUARE	 (1<<15)
#define SYSCALL11_OPCODE_PS3MAPI				0x7777
#define PS3MAPI_OPCODE_GET_VSH_PLUGIN_INFO		0x0047
#define SYSCALL11_OPCODE_LOAD_VSH_PLUGIN		0x1EE7
#define SYSCALL11_OPCODE_UNLOAD_VSH_PLUGIN		0x364F
/* #define SYSCALL11_OPCODE_TOGGLE_COBRA_STAGE		0x6A13
#define SYSCALL11_OPCODE_TOGGLE_SYSAUDIO		0x6CDF
#define SYSCALL11_OPCODE_TOGGLE_SMAN			0x6CE1 */

// static const char * devrebug = "/dev_rebug";
static const char * stage2cex = "/dev_rebug/rebug/cobra/stage2.cex";
static const char * stage2dex = "/dev_rebug/rebug/cobra/stage2.dex";
static const char * stage2cexret = "/dev_rebug/rebug/cobra/stage2.cexr";
static const char * stage2dexret = "/dev_rebug/rebug/cobra/stage2.dexr";
static const char * stage2cexdeb = "/dev_rebug/rebug/cobra/stage2.cexd";
static const char * stage2dexdeb = "/dev_rebug/rebug/cobra/stage2.dexd";
static const char * sysconfprx = "/dev_rebug/vsh/module/sysconf_plugin.sprx";
static const char * sysconfcex = "/dev_rebug/vsh/module/sysconf_plugin.sprx.cex";
static const char * sysconfdex = "/dev_rebug/vsh/module/sysconf_plugin.sprx.dex";
static const char * stage2cexbak = "/dev_rebug/rebug/cobra/stage2.cex.bak";
static const char * stage2dexbak = "/dev_rebug/rebug/cobra/stage2.dex.bak";
static const char * sysaudio = "/dev_rebug/sys/internal/sys_audio.sprx";
static const char * sysplug = "/dev_rebug/sys/internal/sys_plugin.sprx";
static const char * smplug = "/dev_rebug/sys/internal/sys_sm.sprx";
// static const char * smplug2 = "/dev_hdd0/z/plugins/vsh/sys_audio.sprx";
static const char * vshnrm = "/dev_rebug/vsh/module/vsh.self.nrm";
static const char * vshself= "/dev_rebug/vsh/module/vsh.self";
static const char * vshswp = "/dev_rebug/vsh/module/vsh.self.swp";
static const char * vshdsp = "/dev_rebug/vsh/module/vsh.self.dexsp";
static const char * vshcsp = "/dev_rebug/vsh/module/vsh.self.cexsp";
static const char * idxdat = "/dev_rebug/vsh/etc/index.dat";
static const char * idxswp = "/dev_rebug/vsh/etc/index.dat.swp";
static const char * idxnrm = "/dev_rebug/vsh/etc/index.dat.nrm";
static const char * vertxt = "/dev_rebug/vsh/etc/version.txt";
static const char * verswp = "/dev_rebug/vsh/etc/version.txt.swp";
static const char * vernrm = "/dev_rebug/vsh/etc/version.txt.nrm";
static const char * txt = "/dev_flash2/boot_plugins.txt";
static const char * txtbak = "/dev_flash2/boot_plugins.txt.bak";
static const char * txtsm = "/dev_flash2/boot_plugins.txtsm";
static const char * txtsmn = "/dev_flash2/boot_plugins.txtsmn";
static const char * edump = "/dev_hdd0/eeprom.log";
static float c_firmware = 0.0f;
static uint8_t dex_mode = 0, dex_flash = 0;
static CellFsStat s;
uint64_t SYSCALL_TABLE;
sys_prx_id_t load_start(const char *path);
sys_prx_id_t stop_unload(sys_prx_id_t id);

/* uint64_t lv1_peek(uint64_t addr);
void lv1_poke( uint64_t addr, uint64_t val);
uint64_t peekq(uint64_t addr);
void pokeq( uint64_t addr, uint64_t val); */
static uint64_t lv1_peek(uint64_t addr)
{
	system_call_1(8, addr);
	return_to_user_prog(uint64_t);
}
static void lv1_poke( uint64_t addr, uint64_t val) 
{
	system_call_2(9, addr, val);
}
static uint64_t peekq(uint64_t addr)
{
	system_call_1(6, addr);
	return_to_user_prog(uint64_t);
}
static void pokeq( uint64_t addr, uint64_t val)
{
	system_call_2(7, addr, val);
}
static int lv2_ss_update_mgr_if(uint32_t packet_id, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6)
{
	system_call_7(863, packet_id, arg1, arg2, arg3, arg4, arg5, arg6);
	return_to_user_prog(int);
}
static int save_file(const char *file, const char *mem, s64 size)
{
	int fd = 0; u32 flags = CELL_FS_O_CREAT | CELL_FS_O_TRUNC | CELL_FS_O_WRONLY;
	cellFsChmod(file, 0777);

	if( size < 0 )  {flags = CELL_FS_O_APPEND | CELL_FS_O_CREAT | CELL_FS_O_WRONLY; size = (size == (-0xADD0ADD0ADD000ALL)) ? 0 : -size;} //else
	// if(!extcmp(file, "/PARAM.SFO", 10)) flags = CELL_FS_O_CREAT | CELL_FS_O_WRONLY;

	if(cellFsOpen(file, flags, &fd, NULL, 0) == CELL_FS_SUCCEEDED)
	{
		if((size <= 0) && mem) size = strlen(mem);

		if(size) cellFsWrite(fd, (void *)mem, size, NULL);
		cellFsClose(fd);

		cellFsChmod(file, 0777);

		return CELL_FS_SUCCEEDED;
	}

	return -1;
}

static void detect_firmware(void)
{
	uint64_t CEX=0x4345580000000000ULL;
	uint64_t DEX=0x4445580000000000ULL;

	dex_mode=0;

	if(peekq(0x80000000002E8610ULL)==CEX) { SYSCALL_TABLE = SYSCALL_TABLE_421; dex_mode=0; c_firmware=4.21f; } else
	if(peekq(0x8000000000302D88ULL)==DEX) { SYSCALL_TABLE = SYSCALL_TABLE_421D; dex_mode=2; c_firmware=4.21f; }
}
static void detect_target(void)
{
	uint64_t type;
	{system_call_1(985, (uint64_t)(uint32_t) &type);};
	if(type==2) dex_flash=1; // DEX
}
/* static void patch_hv_checks(void)
{
	uint64_t auth_check=0x16F4BC; //auth_check poke-offset 3.55
	uint64_t write_eeprom=0xFDBB4; //eeprom_write_access poke-offset 3.55

	// notify("Überprüfe LV1 und deaktiviere Hypervisor Checks...");
	// notify((char*)"Überprüfe LV1 und deaktiviere Hypervisor Checks...");
	// vshtask_notify("Überprüfe LV1 und deaktiviere Hypervisor Checks...");
	// sys_timer_sleep(2);

	// patch whitelist for read eprom
	for(uint64_t offset = 0xE0000; offset < 0x1000000;offset = offset + 4)
	{	
		if(lv1_peek(offset) == 0x2B800003419D0054ULL)
		{
			// printf("Found LV1 Code @ 0x%x\n",(int)offset);
			lv1_poke(offset,0x2B8000032B800003ULL);
			break;			
		}
	}

	// patch whitelist for write eprom
	for(uint64_t offset = 0xE0000; offset < 0x1000000;offset = offset + 4)
	{	
		if(lv1_peek(offset) == 0x2B800003419D02B4ULL)
		{
			// printf("Found LV1 Code @ 0x%x\n",(int)offset);
			lv1_poke(offset,0x2B8000032B800003ULL);
			break;			
		}
	}
	
	if(lv1_peek(auth_check)!=0x2F800000409E0050ULL){ //(1st) patch pattern - if no match, search lv1
		auth_check=0;
		for(uint64_t addr=0xE0000; addr<0x1000000; addr+=4){ //search lv1
			auth_check=addr;
			if(lv1_peek(addr) == 0x4BFFFF8888010070ULL){ //static search pattern for auth_check function
				auth_check=addr+8;
				break;
			}
		}
	}
	if(auth_check && lv1_peek(auth_check)==0x2F800000409E0050ULL){
		// printf("Found LV1 Code @ 0x%x\n",(int)auth_check);
		lv1_poke(auth_check, 0x2f80000048000050ULL);
	}
	
	if(lv1_peek(write_eeprom)!=0xE81800082FA00000ULL){ //(1st) patch pattern - if no match, search lv1
		write_eeprom=0;
		for(uint64_t addr=0xE0000; addr<0x1000000; addr+=4){ //search lv1
			write_eeprom=addr;
			if(lv1_peek(addr) == 0x2F8000FF419E0088ULL){ //static search pattern for eeprom_write_access function
				write_eeprom=addr+24;
				break;
			}
		}
	}
	if(write_eeprom && lv1_peek(write_eeprom)==0xE81800082FA00000ULL){
		// printf("Found LV1 Code @ 0x%x\n",(int)write_eeprom);
		lv1_poke(write_eeprom, 0x380000002FA00000ULL);
	}
} */
bool boot_mode()
{
	detect_target();
	if(dex_flash)
	{
		detect_firmware();
		if(dex_mode)
		{
			uint8_t value, new;
			// patch_hv_checks();
			if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, EPROM_BOOT_MODE_OFFSET, (uint64_t)(uint32_t) &value, 0, 0, 0, 0)) == 0)
			{
				if(value == 0x07)
				{	// disable //
					new = 0x17;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Systemsoftware Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x17)
				{	// enable //
					new = 0x07;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Release Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x27)
				{
					new = 0x37;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Systemsoftware Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x37)
				{
					new = 0x27;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Release Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x47)
				{
					new = 0x57;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Release Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x57)
				{
					new = 0x47;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Systemsoftware Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x67)
				{
					new = 0x77;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Release Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x77)
				{
					new = 0x67;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Systemsoftware Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x87)
				{
					new = 0x97;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Systemsoftware Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x97)
				{
					new = 0x87;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Release Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else
				{
					// vshtask_notify("Unbekanntes EEPROM Flag");
					// sys_timer_sleep(2);
					return false;
				}
			}
		}
	// return true;
	}
	// wait(2);
exit:
	return true;
}
bool debug_mode()
{
	detect_target();
	if(dex_flash)
	{
		detect_firmware();
		if(dex_mode)
		{
			uint8_t value, new;
			// patch_hv_checks();
			if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, EPROM_BOOT_MODE_OFFSET, (uint64_t)(uint32_t) &value, 0, 0, 0, 0)) == 0)
			{
				if (value == 0x07)
				{
					new = 0x16;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Debug Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x17)
				{
					new = 0x16;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Debug Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x27)
				{
					new = 0x36;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Debug Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x37)
				{
					new = 0x36;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Debug Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x47)
				{
					new = 0x56;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Release Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x57)
				{
					new = 0x56;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Systemsoftware Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x67)
				{
					new = 0x76;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Release Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x77)
				{
					new = 0x76;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Systemsoftware Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x87)
				{
					new = 0x96;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Debug Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if(value == 0x97)
				{
					new = 0x96;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_BOOT_MODE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						// vshtask_notify("Aktiviere Debug Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else
				{
					// vshtask_notify("Unbekanntes EEPROM Flag");
					// sys_timer_sleep(2);
					return false;
				}
			}
		}
	// return true;
	}
	// wait(2);
exit:
	return true;
}
bool spe_mode()
{
	detect_target();
	if(dex_flash)
	{
		detect_firmware();
		if(dex_mode)
		{
			uint8_t value, new;
			// patch_hv_checks();
			if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, EPROM_SPE_OFFSET, (uint64_t)(uint32_t) &value, 0, 0, 0, 0)) == 0)
			{
				if (value == 0x06)
				{
					new = 0x07;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_SPE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						printf("Aktiviere 8. SPE Kern: 0x%x -> 0x%x\n",value,new);
						// vshtask_notify("Aktiviere Debug Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else if (value == 0x07)
				{
					new = 0x06;
					if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, EPROM_SPE_OFFSET, new, 0, 0, 0, 0)) == 0)
					{
						printf("Deaktiviere 8. SPE Kern: 0x%x -> 0x%x\n",value,new);
						// vshtask_notify("Aktiviere Debug Mode...");
						// sys_timer_sleep(2);
						goto exit;
					}
				}
				else
				{
					// vshtask_notify("Unbekanntes EEPROM Flag");
					// sys_timer_sleep(2);
					return false;
				}
			}
		}
	// return true;
	}
	// wait(2);
exit:
	{system_call_3(392, 0x1004, 0x4, 0x6);}
	return true;
}

void debugsettings_mode(void)
{
	int ret;
	ret = cellFsStat(sysconfcex, &s); // "/dev_rebug/vsh/module/sysconf_plugin.sprx.cex";
	if(ret == CELL_OK)
	{
		cellFsRename(sysconfprx,sysconfdex);
		cellFsRename(sysconfcex,sysconfprx);
		vshtask_notify("CEX QA Menü ist jetzt aktiv");
	}
	else
	{
		ret = cellFsStat(sysconfdex, &s); // "/dev_rebug/vsh/module/sysconf_plugin.sprx.dex";
		if(ret == CELL_OK)
		{
			cellFsRename(sysconfprx,sysconfcex);
			cellFsRename(sysconfdex,sysconfprx);
			vshtask_notify("DEBUG Menü ist jetzt aktiv");
		}
	}
}

bool copy_aborted = false;
static uint64_t get_free_drive_space(char *_path)
{
	if(strlen(_path)<6) return 0;
	char tmp_path[1024];
	if(_path[0]!='/')
		sprintf(tmp_path, "/%s/", _path);
	else
		sprintf(tmp_path, "%s/", _path);
	tmp_path[strchr((tmp_path+1), '/')-tmp_path]=0;
	uint32_t blockSize;
	uint64_t freeSize;
	cellFsGetFreeSize(tmp_path, &blockSize, &freeSize);
	return (uint64_t)( ((uint64_t)blockSize * freeSize));
}
static uint64_t is_size(char *path)
{
	if(cellFsStat(path, &s)==CELL_FS_SUCCEEDED)
		return s.st_size;
	else
		return 0;
}
static int sysLv2FsLink(const char oldpath, const char newpath)
{
	system_call_2(810, oldpath, newpath);
	return_to_user_prog(int);
}
static int filecopy(char *file1, char *file2, uint64_t maxbytes)
{
    struct CellFsStat buf;
    int fd1, fd2;
    int ret=-1;
    uint64_t chunk_size=64*1024; //64K
	if(cellFsStat(file1, &buf)!=CELL_FS_SUCCEEDED) return ret;
	if(!memcmp(file1, "/dev_hdd0/", 10) && !memcmp(file2, "/dev_hdd0/", 10))
	{
		if(strcmp(file1, file2)==0) return ret;
		cellFsUnlink(file2);
		// return sysLv2FsLink(file1, file2);
		return sysLv2FsLink(*file1, *file2);
	}
	if(cellFsOpen((char*)file1, CELL_FS_O_RDONLY, &fd1, 0, 0)==CELL_FS_SUCCEEDED)
	{
		uint64_t size = buf.st_size;
		if(maxbytes>0 && size>maxbytes) size=maxbytes;
		sys_addr_t buf1=0;
		if(sys_memory_allocate(chunk_size, SYS_MEMORY_PAGE_SIZE_64K, &buf1)==0)
		{	// copy_file
			if(cellFsOpen(file2, CELL_FS_O_CREAT | CELL_FS_O_TRUNC | CELL_FS_O_WRONLY, &fd2, 0,0)==CELL_FS_SUCCEEDED)
			{
				char *chunk=(char*)buf1;
				uint64_t msiz1 = 0, msiz2 = 0, pos=0;
				copy_aborted = false;
				while(size>0ULL)
        	    {
					if(copy_aborted) break;
					cellFsLseek(fd1, pos, CELL_FS_SEEK_SET, &msiz1);
					cellFsRead(fd1, chunk, chunk_size, &msiz1);
					cellFsWrite(fd2, chunk, msiz1, &msiz2);
					if(!msiz2) {break;}
					pos+=msiz2;
					size-=msiz2;
					if(chunk_size>size) chunk_size=(int) size;
					sys_timer_usleep(1);
				}
				cellFsClose(fd2);
				if(copy_aborted)
					cellFsUnlink(file2); //remove incomplete file
				else
					cellFsChmod(file2, 0777);
				ret=size;
			}
			sys_memory_free(buf1);
		}
		cellFsClose(fd1);
	}
	return ret;
}
bool load_ckernel()
{
	// CellFsStat s;
	// int ret;
	char path[512];
	detect_target();
	detect_firmware();
	uint64_t lv2_offset;
	// if(!dex_flash) lv2_offset=0x15DE78;
	// else lv2_offset=0x379E78;
	if(!dex_flash) lv2_offset=0x15DE78;
	else lv2_offset=0x379E78;
	// vshtask_notify("Externer Kernel Loader aktiviert!");
	// sys_timer_sleep(2);
	if(cellFsStat((char*)"/dev_rebug/lv2Ckernel.self", &s)==CELL_FS_SUCCEEDED)
	{
		cellFsUnlink((char*)"/dev_rebug/lv2Ckernel.self");
		goto cex_kernel;
	}
	else
	if(cellFsStat((char*)"/dev_rebug/lv2_kernel.self", &s)==CELL_FS_SUCCEEDED)
	{
		cellFsUnlink((char*)"/dev_rebug/lv2_kernel.self");
		goto cex_kernel;
	}
cex_kernel:
	if(((cellFsStat((char*)"/dev_rebug/lv2Ckernel.self", &s)!=CELL_FS_SUCCEEDED) 
	|| (cellFsStat((char*)"/dev_rebug/lv2_kernel.self", &s)!=CELL_FS_SUCCEEDED)) 
	&& (cellFsStat((char*)"/dev_hdd0/z/lv2/lv2Ckernel.self", &s)==CELL_FS_SUCCEEDED))
	{
		if(get_free_drive_space((char*)"/dev_flash")>(is_size(path)+2048))
		// ret = get_free_space_flash();
		// ret = get_free_drive_space((char*)"/dev_flash")>(is_size(path)+2048);
		// if(ret == CELL_OK)
		{
			filecopy((char*)"/dev_hdd0/z/lv2/lv2Ckernel.self", (char*)"/dev_rebug/lv2Ckernel.self", 0);
			if(cellFsStat((char*)"/dev_flash/lv2Ckernel.self", &s)==CELL_FS_SUCCEEDED)
			{
				// vshtask_notify("CEX Kernel wird geladen!\r\nÜberprüfe LV1 und starte neu...");
				// sys_timer_sleep(2);
				if(lv1_peek(lv2_offset)!=0x2F666C682F6F732FULL)  // /flh/os/
				{
					lv2_offset=0;
					for(uint64_t addr=0xA000; addr<0x800000ULL; addr+=4)//16MB
					{
						lv2_offset=addr;
						if(lv1_peek(addr) == 0x2F6F732F6C76325FULL) // /os/lv2_
						{
							lv2_offset=addr-4; // /os/lv2_ - 4 = /flh/os/
							break;
						}
					}
				}
				if(lv2_offset && lv1_peek(lv2_offset)==0x2F666C682F6F732FULL)  // /flh/os/
				{
					lv1_poke(lv2_offset +  0, 0x2F6C6F63616C5F73ULL); // /local_s
					lv1_poke(lv2_offset +  8, 0x7973302F6C763243ULL); // ys0/lv2C
					lv1_poke(lv2_offset + 16, 0x6B65726E656C2E73ULL); // kernel.s
					lv1_poke(lv2_offset + 24, 0x656C660000000000ULL); // elf.....
					{system_call_3(392, 0x1004, 0x4, 0x6);}
					goto exit;
				}
				else if(lv1_peek(lv2_offset)!=0x2F6C6F63616C5F73ULL) // /local_s
				{
					lv2_offset=0;
					for(uint64_t addr=0xA000; addr<0x800000ULL; addr+=4)//16MB
					{
						lv2_offset=addr;
						if(lv1_peek(addr) == 0x7973302F6C76325FULL) // ys0/lv2_
						{
							lv2_offset=addr-8; // ys0/lv2_ - 8 = /local_s
							break;
						}
					}
				}
				if(lv2_offset && lv1_peek(lv2_offset)==0x2F6C6F63616C5F73ULL) // /local_s
				{
					lv1_poke(lv2_offset +  0, 0x2F6C6F63616C5F73ULL); // /local_s
					lv1_poke(lv2_offset +  8, 0x7973302F6C763243ULL); // ys0/lv2_ -> ys0/lv2C
					{system_call_3(392, 0x1004, 0x4, 0x6);}
					goto exit;
				}
				else if(lv1_peek(lv2_offset)!=0x2F6C6F63616C5F73ULL) // /local_s
				{
					lv2_offset=0;
					for(uint64_t addr=0xA000; addr<0x800000ULL; addr+=4)//16MB
					{
						lv2_offset=addr;
						if(lv1_peek(addr) == 0x7973302F6C763243ULL) // ys0/lv2C
						{
							break;
						}
					}
					{system_call_3(392, 0x1004, 0x4, 0x6);}
					goto exit;
				}
			}
		}
		else
		{
			// vshtask_notify("Nicht genügend freier Speicher in /dev_flash!");
			// sys_timer_sleep(2);
			// break;
			return false;
		}
	}
exit:
	return true;
}
bool load_dkernel()
{
	// CellFsStat s;
	// int ret;
	char path[512];
	detect_target();
	uint64_t lv2_offset;
	if(!dex_flash) lv2_offset=0x15DE78;
	else lv2_offset=0x379E78;
	// vshtask_notify("Externer Kernel Loader aktiviert!");
	// sys_timer_sleep(2);
	if(cellFsStat((char*)"/dev_rebug/lv2_kernel.self", &s)==CELL_OK)
	{
		cellFsUnlink((char*)"/dev_rebug/lv2_kernel.self");
		goto dex_kernel;
	}
	else
	if(cellFsStat((char*)"/dev_rebug/lv2Ckernel.self", &s)==CELL_FS_SUCCEEDED)
	{
		cellFsUnlink((char*)"/dev_rebug/lv2Ckernel.self");
		goto dex_kernel;
	}
dex_kernel:
	if(((cellFsStat((char*)"/dev_rebug/lv2_kernel.self", &s)!=CELL_FS_SUCCEEDED) 
	|| (cellFsStat((char*)"/dev_rebug/lv2Ckernel.self", &s)!=CELL_FS_SUCCEEDED)) 
	&& (cellFsStat((char*)"/dev_hdd0/z/lv2/lv2_kernel.self", &s)==CELL_FS_SUCCEEDED))
	{
		if(get_free_drive_space((char*)"/dev_flash")>(is_size(path)+2048))
		// ret = get_free_drive_space((char*)"/dev_flash")>(is_size(path)+2048);
		// ret = get_free_space_flash();
		// if(ret == CELL_OK)
		{
			filecopy((char*)"/dev_hdd0/z/lv2/lv2_kernel.self", (char*)"/dev_rebug/lv2_kernel.self", 0);
			if(cellFsStat((char*)"/dev_flash/lv2_kernel.self", &s)==CELL_FS_SUCCEEDED)
			{
				// vshtask_notify("DEX Kernel wird geladen!\r\nÜberprüfe LV1 und starte neu...");
				// sys_timer_sleep(2);
				if(lv1_peek(lv2_offset)!=0x2F666C682F6F732FULL)  // /flh/os/
				{
					lv2_offset=0;
					for(uint64_t addr=0xA000; addr<0x800000ULL; addr+=4)//16MB
					{
						lv2_offset=addr;
						if(lv1_peek(addr) == 0x2F6F732F6C76325FULL) // /os/lv2_
						{
							lv2_offset=addr-4; // /os/lv2_ - 4 = /flh/os/
							break;
						}
					}
				}
				if(lv2_offset && lv1_peek(lv2_offset)==0x2F666C682F6F732FULL) // /flh/os/
				{
					lv1_poke(lv2_offset +  0, 0x2F6C6F63616C5F73ULL); // /local_s
					lv1_poke(lv2_offset +  8, 0x7973302F6C76325FULL); // ys0/lv2_
					lv1_poke(lv2_offset + 16, 0x6B65726E656C2E73ULL); // kernel.s
					lv1_poke(lv2_offset + 24, 0x656C660000000000ULL); // elf.....
					{system_call_3(392, 0x1004, 0x4, 0x6);}
					goto exit;
				}
				else if(lv1_peek(lv2_offset)!=0x2F6C6F63616C5F73ULL) // /local_s
				{
					lv2_offset=0;
					for(uint64_t addr=0xA000; addr<0x800000ULL; addr+=4)//16MB
					{
						lv2_offset=addr;
						if(lv1_peek(addr) == 0x7973302F6C763243ULL) // ys0/lv2C
						{
							lv2_offset=addr-8; // ys0/lv2C - 8 = /local_s
							break;
						}
					}
				}
				if(lv2_offset && lv1_peek(lv2_offset)==0x2F6C6F63616C5F73ULL) // /local_s
				{
					lv1_poke(lv2_offset +  0, 0x2F6C6F63616C5F73ULL); // /local_s
					lv1_poke(lv2_offset +  8, 0x7973302F6C76325FULL); // ys0/lv2_
					{system_call_3(392, 0x1004, 0x4, 0x6);}
					goto exit;
				}
				else if(lv1_peek(lv2_offset)!=0x2F6C6F63616C5F73ULL) // /local_s
				{
					lv2_offset=0;
					for(uint64_t addr=0xA000; addr<0x800000ULL; addr+=4)//16MB
					{
						lv2_offset=addr;
						if(lv1_peek(addr) == 0x7973302F6C76325FULL) // ys0/lv2_
						{
							break;
						}
					}
					{system_call_3(392, 0x1004, 0x4, 0x6);}
					goto exit;
				}
			}
		}
		else
		{
			// vshtask_notify("Nicht genügend freier Speicher im /dev_flash!");
			// sys_timer_sleep(2);
			// break;
			return false;
		}
	}
exit:
	return true;
}

void patch_dkernel(void)
{
	detect_target();
	if(dex_flash)
	{
		detect_firmware();
		if(dex_mode)
		{
			uint64_t addr=0x8000000000003B38ULL;
			if(peekq(addr)==0xE92280087C0802A6ULL)
			{
				pokeq(addr, 0x386000014E800020ULL);
				system_call_3(392, 0x1004, 0x4, 0x6); //1 Beep
			}
			else
			{
				pokeq(addr, 0xE92280087C0802A6ULL);
				system_call_3(392, 0x1004, 0xa, 0x1b6); //3 beep
			}
		}
	}
}

void patch_ccapi(void)
{
	detect_firmware();
	if(dex_mode){
		if( (cellFsStat((char*)"/dev_hdd0/game/XMBM00001/USRDIR/DEX", &s)==CELL_FS_SUCCEEDED)){
			cellFsRename("/dev_hdd0/game/XMBM00001/USRDIR/CFG", "/dev_hdd0/game/XMBM00001/USRDIR/CEX");
			cellFsRename("/dev_hdd0/game/XMBM00001/USRDIR/DEX", "/dev_hdd0/game/XMBM00001/USRDIR/CFG");
		}
		pokeq(0x8000000000002030ULL, 0x7CDD3378480711ADULL);
		pokeq(0x8000000000002080ULL, 0x38A100704808E861ULL);
		pokeq(0x80000000000020A0ULL, 0x38A000004806574DULL);
		pokeq(0x80000000000020C0ULL, 0x4800E0012F830000ULL);
		pokeq(0x80000000000020E0ULL, 0x38E000014829B5CDULL);
		pokeq(0x8000000000002100ULL, 0x3880000D48065B29ULL);
		pokeq(0x8000000000002108ULL, 0xE922AA80E8810070ULL);
		pokeq(0x8000000000002110ULL, 0xE86900004800FE35ULL);
		pokeq(0x8000000000002118ULL, 0x4BFFFF30E922AA80ULL);
		pokeq(0x8000000000002130ULL, 0x4800FE194BFFFF14ULL);
		pokeq(0x8000000000002230ULL, 0x7C9C237848070FADULL);
		pokeq(0x8000000000002280ULL, 0x419AFFC84808E661ULL);
		pokeq(0x80000000000022A0ULL, 0x409EFFAC4806554DULL);
		pokeq(0x80000000000022D0ULL, 0x4829B56D63DE000DULL);
		pokeq(0x80000000000022E8ULL, 0xE922AA80E8810070ULL);
		pokeq(0x80000000000022F0ULL, 0xE86900004800FC55ULL);
		pokeq(0x8000000000002300ULL, 0x7FA5EB784800DBA1ULL);
		pokeq(0x8000000000002310ULL, 0x7FE3FB7848065919ULL);
		pokeq(0x8000000000002318ULL, 0x4BFFFF3448065911ULL);
		pokeq(0x8000000000002320ULL, 0xE922AA80E8810070ULL);
		pokeq(0x8000000000002328ULL, 0xE86900004800FC1DULL);
		pokeq(0x8000000000002420ULL, 0xE922AA80EBA90000ULL);
		pokeq(0x8000000000002430ULL, 0x4800F615F87F0000ULL);
		pokeq(0x8000000000002448ULL, 0x4800F675E9210100ULL);
	}
	else{
		if( (cellFsStat((char*)"/dev_hdd0/game/XMBM00001/USRDIR/CEX", &s)==CELL_FS_SUCCEEDED)){
			cellFsRename("/dev_hdd0/game/XMBM00001/USRDIR/CFG", "/dev_hdd0/game/XMBM00001/USRDIR/DEX");
			cellFsRename("/dev_hdd0/game/XMBM00001/USRDIR/CEX", "/dev_hdd0/game/XMBM00001/USRDIR/CFG");
		}
		pokeq(0x8000000000002030ULL, 0x7CDD33784806D715ULL);
		pokeq(0x8000000000002080ULL, 0x38A100704808A215ULL);
		pokeq(0x80000000000020A0ULL, 0x38A0000048061E35ULL);
		pokeq(0x80000000000020C0ULL, 0x4800D9B92F830000ULL);
		pokeq(0x80000000000020E0ULL, 0x38E0000148294D69ULL);
		pokeq(0x8000000000002100ULL, 0x3880000D48062211ULL);
		pokeq(0x8000000000002108ULL, 0xE922A9A0E8810070ULL);
		pokeq(0x8000000000002110ULL, 0xE86900004800F7EDULL);
		pokeq(0x8000000000002118ULL, 0x4BFFFF30E922A9A0ULL);
		pokeq(0x8000000000002130ULL, 0x4800F7D14BFFFF14ULL);
		pokeq(0x8000000000002230ULL, 0x7C9C23784806D515ULL);
		pokeq(0x8000000000002280ULL, 0x419AFFC84808A015ULL);
		pokeq(0x80000000000022A0ULL, 0x409EFFAC48061C35ULL);
		pokeq(0x80000000000022D0ULL, 0x48294D0963DE000DULL);
		pokeq(0x80000000000022E8ULL, 0xE922A9A0E8810070ULL);
		pokeq(0x80000000000022F0ULL, 0xE86900004800F60DULL);
		pokeq(0x8000000000002300ULL, 0x7FA5EB784800D559ULL);
		pokeq(0x8000000000002310ULL, 0x7FE3FB7848062001ULL);
		pokeq(0x8000000000002318ULL, 0x4BFFFF3448061FF9ULL);
		pokeq(0x8000000000002320ULL, 0xE922A9A0E8810070ULL);
		pokeq(0x8000000000002328ULL, 0xE86900004800F5D5ULL);
		pokeq(0x8000000000002420ULL, 0xE922A9A0EBA90000ULL);
		pokeq(0x8000000000002430ULL, 0x4800EFCDF87F0000ULL);
		pokeq(0x8000000000002448ULL, 0x4800F02DE9210100ULL);
	}
	pokeq(0x8000000000001FF8ULL, 0x8000000000002000ULL);
	pokeq(0x8000000000002000ULL, 0xF821FF517C0802A6ULL);
	pokeq(0x8000000000002008ULL, 0xFBC100A0FBE100A8ULL);
	pokeq(0x8000000000002010ULL, 0x7CBE2B787C7F1B78ULL);
	pokeq(0x8000000000002018ULL, 0x38600000FB810090ULL);
	pokeq(0x8000000000002020ULL, 0xFBA10098FB610088ULL);
	pokeq(0x8000000000002028ULL, 0xF80100C07C9C2378ULL);
	pokeq(0x8000000000002038ULL, 0x2F9E0000409E0034ULL);
	pokeq(0x8000000000002040ULL, 0x3FE0800163FF0409ULL);
	pokeq(0x8000000000002048ULL, 0xE80100C07FE307B4ULL);
	pokeq(0x8000000000002050ULL, 0xEB610088EB810090ULL);
	pokeq(0x8000000000002058ULL, 0x7C0803A6EBA10098ULL);
	pokeq(0x8000000000002060ULL, 0xEBC100A0EBE100A8ULL);
	pokeq(0x8000000000002068ULL, 0x382100B04E800020ULL);
	pokeq(0x8000000000002070ULL, 0x2FBD0000419EFFCCULL);
	pokeq(0x8000000000002078ULL, 0x7BE3002038810078ULL);
	pokeq(0x8000000000002088ULL, 0x2F8300007C7F1B78ULL);
	pokeq(0x8000000000002090ULL, 0x409EFFB87BDE0020ULL);
	pokeq(0x8000000000002098ULL, 0x3880000D7FC3F378ULL);
	pokeq(0x80000000000020A8ULL, 0x2C2300007C7B1B78ULL);
	pokeq(0x80000000000020B0ULL, 0x41C2006C7FA3EB78ULL);
	pokeq(0x80000000000020B8ULL, 0x7F64DB787FC5F378ULL);
	pokeq(0x80000000000020C8ULL, 0x7C7F1B78409E0030ULL);
	pokeq(0x80000000000020D0ULL, 0x7F84E378E8610078ULL);
	pokeq(0x80000000000020D8ULL, 0x7FC6F3787F65DB78ULL);
	pokeq(0x80000000000020E8ULL, 0x2F8300007C7F1B78ULL);
	pokeq(0x80000000000020F0ULL, 0x419E000C3FE08001ULL);
	pokeq(0x80000000000020F8ULL, 0x63FF000D7F63DB78ULL);
	pokeq(0x8000000000002120ULL, 0x3FE08001E8810070ULL);
	pokeq(0x8000000000002128ULL, 0x63FF0408E8690000ULL);
	pokeq(0x80000000000021F8ULL, 0x8000000000002200ULL);
	pokeq(0x8000000000002200ULL, 0xF821FF517C0802A6ULL);
	pokeq(0x8000000000002208ULL, 0xFBA10098FBE100A8ULL);
	pokeq(0x8000000000002210ULL, 0x7C7D1B787CBF2B78ULL);
	pokeq(0x8000000000002218ULL, 0x38600000FB610088ULL);
	pokeq(0x8000000000002220ULL, 0xFB810090FBC100A0ULL);
	pokeq(0x8000000000002228ULL, 0x7CDB3378F80100C0ULL);
	pokeq(0x8000000000002238ULL, 0x2F9F00002F3B0000ULL);
	pokeq(0x8000000000002240ULL, 0x3FC08001409E0030ULL);
	pokeq(0x8000000000002248ULL, 0x63DE0409E80100C0ULL);
	pokeq(0x8000000000002250ULL, 0x7FC307B4EB610088ULL);
	pokeq(0x8000000000002258ULL, 0xEB8100907C0803A6ULL);
	pokeq(0x8000000000002260ULL, 0xEBA10098EBC100A0ULL);
	pokeq(0x8000000000002268ULL, 0xEBE100A8382100B0ULL);
	pokeq(0x8000000000002270ULL, 0x4E8000207BA30020ULL);
	pokeq(0x8000000000002278ULL, 0x3881007838A10070ULL);
	pokeq(0x8000000000002288ULL, 0x7BFD00207C7E1B78ULL);
	pokeq(0x8000000000002290ULL, 0x38A000002F9E0000ULL);
	pokeq(0x8000000000002298ULL, 0x7FA3EB783880000DULL);
	pokeq(0x80000000000022A8ULL, 0x3FC080012C230000ULL);
	pokeq(0x80000000000022B0ULL, 0x7F84E3787FA6EB78ULL);
	pokeq(0x80000000000022B8ULL, 0x7C7F1B787C651B78ULL);
	pokeq(0x80000000000022C0ULL, 0x63DE040841C2005CULL);
	pokeq(0x80000000000022C8ULL, 0xE86100783FC08001ULL);
	pokeq(0x80000000000022D8ULL, 0x2F8300003880000DULL);
	pokeq(0x80000000000022E0ULL, 0x7FE3FB78409E0038ULL);
	pokeq(0x80000000000022F8ULL, 0x7F64DB787FE3FB78ULL);
	pokeq(0x8000000000002308ULL, 0x3880000D7C7E1B78ULL);
	pokeq(0x8000000000002330ULL, 0x4BFFFF1C00000000ULL);
	pokeq(0x80000000000023F8ULL, 0x8000000000002400ULL);
	pokeq(0x8000000000002400ULL, 0xF821FE017D2802A6ULL);
	pokeq(0x8000000000002408ULL, 0xF9210100FBE10110ULL);
	pokeq(0x8000000000002410ULL, 0xFBC10120FBA10130ULL);
	pokeq(0x8000000000002418ULL, 0x7C7F1B787C9E2378ULL);
	pokeq(0x8000000000002428ULL, 0x7FA3EB7838800001ULL);
	pokeq(0x8000000000002438ULL, 0x7C661B787FA3EB78ULL);
	pokeq(0x8000000000002440ULL, 0x388000017FC5F378ULL);
	pokeq(0x8000000000002450ULL, 0x7D2803A6EBE10110ULL);
	pokeq(0x8000000000002458ULL, 0xEBC10120EBA10130ULL);
	pokeq(0x8000000000002460ULL, 0x382102004E800020ULL);
	pokeq(SYSCALL_TABLE + 1600, 0x80000000000021F8ULL);
	pokeq(SYSCALL_TABLE + 1608, 0x8000000000001FF8ULL);
	pokeq(SYSCALL_TABLE + 1616, 0x80000000000023F8ULL);
	pokeq(SYSCALL_TABLE + 1624, 0x8000000000001778ULL);
	pokeq(SYSCALL_TABLE + 1632, 0x8000000000001780ULL);
	pokeq(SYSCALL_TABLE + 1640, 0x8000000000001788ULL);
	pokeq(SYSCALL_TABLE + 1648, 0x8000000000001790ULL);
}
int load_prx(void)
{
	CellPadData pdata;
	uint32_t oldpad = 0, curpad = 0;

	VSHPadGetData(&pdata);
	if(pdata.len > 0)
	{
		curpad = (pdata.button[2] | (pdata.button[3] << 8));
 		if (curpad != oldpad)
		{
			if(curpad & PAD_L1 && curpad & PAD_START)
			{
				int			res;
				int			modres;
				sys_prx_id_t	id=0;
				patch_ccapi();
				id = sys_prx_load_module(CCAPI_PATH200, 0, NULL);
				if (id < CELL_OK)
				{
					{system_call_3(392, 0x1004, 0xA, 0x1B6); } //3xBeep
				}
				res = sys_prx_start_module(id, 0, NULL, &modres, 0, NULL);
				if (res < CELL_OK)
				{
					{system_call_3(392, 0x1004, 0xA, 0x1B6); } //3xBeep
				}
				else
					vshtask_notify("CCAPI 2.00 geladen");
					{system_call_3(392, 0x1004, 0x4, 0x6); } //1xBeep
			}
			else
			if(curpad & PAD_L2 && curpad & PAD_START)
			{
				int			res;
				int			modres;
				sys_prx_id_t	id=0;
				patch_ccapi();
				id = sys_prx_load_module(CCAPI_PATH250, 0, NULL);
				if (id < CELL_OK)
				{
					{system_call_3(392, 0x1004, 0xA, 0x1B6); } //3xBeep
				}
				res = sys_prx_start_module(id, 0, NULL, &modres, 0, NULL);
				if (res < CELL_OK)
				{
					{system_call_3(392, 0x1004, 0xA, 0x1B6); } //3xBeep
				}
				else
					vshtask_notify("CCAPI 2.50 geladen");
					{system_call_3(392, 0x1004, 0x4, 0x6); } //1xBeep
			}
			else
			if(curpad & PAD_R1 && curpad & PAD_START)
			{
				int			res;
				int			modres;
				sys_prx_id_t	id=0;
				id = sys_prx_load_module(SNMAPI_PATH, 0, NULL);
				if (id < CELL_OK)
				{
					{system_call_3(392, 0x1004, 0xA, 0x1B6); } //3xBeep
				}
				res = sys_prx_start_module(id, 0, NULL, &modres, 0, NULL);
				if (res < CELL_OK)
				{
					{system_call_3(392, 0x1004, 0xA, 0x1B6); } //3xBeep
				}
				else
					vshtask_notify("SNMAPI geladen");
					{system_call_3(392, 0x1004, 0x4, 0x6); } //1xBeep
			}
			else
			if(curpad & PAD_SELECT && curpad & PAD_CROSS)
			{
				char plugin_name[30];
				char plugin_filename[256];
				memset(plugin_name, 0, sizeof(plugin_name));
				memset(plugin_filename, 0, sizeof(plugin_filename));
				{system_call_5(11, SYSCALL11_OPCODE_PS3MAPI, PS3MAPI_OPCODE_GET_VSH_PLUGIN_INFO, 2, (uint64_t)((uint32_t)plugin_name), (uint64_t)((uint32_t)plugin_filename));}
				if (strlen(plugin_filename) <= 0 && strcmp(plugin_filename, (char *)TRAINER_VSH_PATH) != 0)
				{
					{system_call_5(11, SYSCALL11_OPCODE_LOAD_VSH_PLUGIN, 2, (uint64_t)TRAINER_VSH_PATH, 0, 0);}
					{system_call_3(392, 0x1004, 0x4, 0x6);}//1xBeep
					vshtask_notify("VSH Plugin geladen");
				}
				else
				{
					{system_call_2(11, SYSCALL11_OPCODE_UNLOAD_VSH_PLUGIN, 2);}
					{system_call_3(392, 0x1004, 0x7, 0x36);}//2xBeep
					vshtask_notify("VSH Plugin entfernt");
				}
/* 				int			res;
				int			modres;
				sys_prx_id_t	id=0;
				// id = sys_prx_load_module(TRAINER_VSH_PATH, 0, NULL);
				id = sys_prx_load_module_on_memcontainer(TRAINER_VSH_PATH, 3f000006, 0, NULL);
				if (id < CELL_OK)
				{
					{system_call_3(392, 0x1004, 0xA, 0x1B6); } //3xBeep
				}
				res = sys_prx_start_module(id, 0, NULL, &modres, 0, NULL);
				if (res < CELL_OK)
				{
					{system_call_3(392, 0x1004, 0xA, 0x1B6); } //3xBeep
				}
				else
					vshtask_notify("TrainerLoader geladen");
					{system_call_3(392, 0x1004, 0x4, 0x6); } //1xBeep */
			}
			else
			if(curpad & PAD_R2 && curpad & PAD_START)
			{
				char plugin_name[30];
				char plugin_filename[256];
				memset(plugin_name, 0, sizeof(plugin_name));
				memset(plugin_filename, 0, sizeof(plugin_filename));
				{system_call_5(11, SYSCALL11_OPCODE_PS3MAPI, PS3MAPI_OPCODE_GET_VSH_PLUGIN_INFO, 5, (uint64_t)((uint32_t)plugin_name), (uint64_t)((uint32_t)plugin_filename));}
				if (strlen(plugin_filename) <= 0 && strcmp(plugin_filename, (char *)TEST_VSH_PATH) != 0)
				{
					{system_call_5(11, SYSCALL11_OPCODE_LOAD_VSH_PLUGIN, 5, (uint64_t)TEST_VSH_PATH, 0, 0);}
					{system_call_3(392, 0x1004, 0x4, 0x6);}//1xBeep
					vshtask_notify("VSH Test-Plugin geladen");
				}
				else
				{
					{system_call_2(11, SYSCALL11_OPCODE_UNLOAD_VSH_PLUGIN, 5);}
					{system_call_3(392, 0x1004, 0x7, 0x36);}//2xBeep
					vshtask_notify("VSH Test-Plugin entfernt");
				}
			}
		}
		oldpad = curpad;
	}
	else
	{
		oldpad = 0;
	}
	return CELL_OK;
}

int cobra_mode(void)
{
	toggle_cobra_mode();
	// return 0;
	return CELL_OK;
}
void toggle_cobra_mode(void)
{	
	{system_call_8(837, (u64)(char*)"CELL_FS_IOS:BUILTIN_FLSH1", (u64)(char*)"CELL_FS_FAT", (u64)(char*)"/dev_rebug", 0, 0, 0, 0, 0);}
	if(cellFsStat(stage2dexbak, &s) == CELL_OK)
	{
		if(cellFsStat(stage2cexbak, &s) == CELL_OK)
		{
			cellFsRename(stage2cexbak, stage2cex);
			cellFsRename(stage2dexbak, stage2dex);
		}
		else
		{
			cellFsRename(stage2dexbak, stage2dex);
		}
		cellFsRename(sysaudio, smplug);
		cellFsRename(sysplug, sysaudio);
	}
	else
	if(cellFsStat(stage2cexbak, &s) == CELL_OK)
	{
		if(cellFsStat(stage2dexbak, &s) == CELL_OK)
		{
			cellFsRename(stage2cexbak, stage2cex);
			cellFsRename(stage2dexbak, stage2dex);
		}
		else
		{
			cellFsRename(stage2cexbak, stage2cex);
		}
		cellFsRename(sysaudio, smplug);
		cellFsRename(sysplug, sysaudio);
	}
	else
	{
/* 		{system_call_1(11, SYSCALL11_OPCODE_TOGGLE_COBRA_STAGE);}
		{system_call_1(11, SYSCALL11_OPCODE_TOGGLE_SYSAUDIO);}
		{system_call_1(11, SYSCALL11_OPCODE_TOGGLE_SMAN);} */
		cellFsRename(stage2cex, stage2cexbak);
		cellFsRename(stage2dex, stage2dexbak);
		cellFsRename(sysaudio, sysplug);
		cellFsRename(smplug, sysaudio);
	}
}

int cobra_stage(void)
{
	toggle_cobra_stage();
	return CELL_OK;
}
void toggle_cobra_stage(void)
{
	// ret = cellFsStat(stage2dexret, &s);
	// if(ret == CELL_OK)
	if(cellFsStat(stage2dexret, &s) == CELL_OK)
	{
		cellFsRename(stage2cex, stage2cexdeb);
		cellFsRename(stage2cexret, stage2cex);
		cellFsRename(stage2dex, stage2dexdeb);
		cellFsRename(stage2dexret, stage2dex);
	}
	else
	{
		cellFsRename(stage2cex, stage2cexret);
		cellFsRename(stage2cexdeb, stage2cex);
		cellFsRename(stage2dex, stage2dexret);
		cellFsRename(stage2dexdeb, stage2dex);
	}
}

int rebug_mode(void)
{	
	toggle_rebug_mode();
	// return 0;
	return CELL_OK;
}
void toggle_rebug_mode(void)
{
	if(((cellFsStat(vshnrm, &s))==CELL_OK) && ((cellFsStat(vshcsp, &s))==CELL_OK))
	{
		cellFsRename(vshself,vshdsp);
		cellFsRename(vshcsp,vshself);
	}
	else
	if((cellFsStat(vshdsp, &s))==CELL_OK)
	{
		cellFsRename(vshself,vshcsp);
		cellFsRename(vshdsp,vshself);
	}
	else
	if((cellFsStat(vshswp, &s))==CELL_OK)
	{
		cellFsRename(vshself,vshnrm);
		cellFsRename(vshswp,vshself);

		cellFsRename(idxdat,idxnrm);
		cellFsRename(idxswp,idxdat);

		cellFsRename(vertxt,vernrm);
		cellFsRename(verswp,vertxt);
	}
}
int normal_mode(void)
{	
	toggle_normal_mode();
	// return 0;
	return CELL_OK;
}
void toggle_normal_mode(void)
{
	if(((cellFsStat(vshnrm, &s))==CELL_OK) && ((cellFsStat(vshcsp, &s))==CELL_OK))
	{
		cellFsRename(vshself,vshswp);
		cellFsRename(vshnrm,vshself);
		
		cellFsRename(idxdat,idxswp);
		cellFsRename(idxnrm,idxdat);
		
		cellFsRename(vertxt,verswp);
		cellFsRename(vernrm,vertxt);
	}
	else
	if(((cellFsStat(vshdsp, &s))==CELL_OK))
	{
		cellFsRename(vshself,vshcsp);
		cellFsRename(vshnrm,vshself);
		
		cellFsRename(idxdat,idxswp);
		cellFsRename(idxnrm,idxdat);
		
		cellFsRename(vertxt,verswp);
		cellFsRename(vernrm,vertxt);
	}
}

int toggle_bootplugins(void)
{	
	if(cellFsStat(txtbak, &s) == CELL_OK)
		cellFsRename(txtbak, txt);
	else
		cellFsRename(txt, txtbak);
	return CELL_OK;
}
int toggle_sm(void)
{	
	if(cellFsStat(txtsm, &s) == CELL_OK)
	{
		cellFsRename(txt, txtsmn);
		cellFsRename(txtsm, txt);
	}
	else
	{
		cellFsRename(txt, txtsm);
		cellFsRename(txtsmn, txt);
	}
	return CELL_OK;
}

#define RECOVER_FLAG_OFFSET				0x48C61
#define QA_FLAG_OFFSET					0x48C0A
#define QA_TOKEN_OFFSET					0x48D3E
int dump_eeprom(void)
{	
	uint8_t value=0;
	int result;
	char buffer[4096];
	if(cellFsStat(edump, &s) == CELL_OK)
		cellFsUnlink(edump);
	sprintf(buffer, "%s:%d: Start\n", __func__, __LINE__);
	save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
/* 	// dump
	FILE *dump_fd = fopen(edump, "wb");
	fwrite(buf, 1, buf_size, dump_fd);
	fclose(dump_fd); */
	// patch eprom checks
/* 	for(uint64_t offset = 0xE0000; offset < 0x1000000;offset = offset + 4)
	{	
		if(lv1_peek(offset) == 0x2B800003419D0054ULL)
		{
			// printf("Found LV1 Code @ 0x%x\n",(int)offset);
			lv1_poke(offset,0x2B8000032B800003ULL);
			break;			
		}
	} */
	// patch_hv_checks();
	for(int i = 0x2F00; i < 0x03100; i++)
	{
		result = lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, i, (uint64_t)(uint32_t) &value, 0, 0, 0, 0);
		if (result != 0)
		{
			sprintf(buffer, "%s:%d: lv2_ss_update_mgr_if(READ_EPROM) failed (0x%08x)\n", __func__, __LINE__, result);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
		else
		{
/* 			if(i%16==0)
			{
				sprintf(buffer, "\n%02x", value);
				save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
			} */
			sprintf(buffer, "%02x", value);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
	}
	for(int i = 0x48000; i < 0x48100; i++)
	{
		result = lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, i, (uint64_t)(uint32_t) &value, 0, 0, 0, 0);
		if (result != 0)
		{
			sprintf(buffer, "%s:%d: lv2_ss_update_mgr_if(READ_EPROM) failed (0x%08x)\n", __func__, __LINE__, result);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
		else
		{
/* 			if(i%16==0)
			{
				sprintf(buffer, "\n%02x", value);
				save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
			} */
			sprintf(buffer, "%02x", value);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
	}
	for(int i = 0x48800; i < 0x48900; i++)
	{
		result = lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, i, (uint64_t)(uint32_t) &value, 0, 0, 0, 0);
		if (result != 0)
		{
			sprintf(buffer, "%s:%d: lv2_ss_update_mgr_if(READ_EPROM) failed (0x%08x)\n", __func__, __LINE__, result);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
		else
		{
/* 			if(i%16==0)
			{
				sprintf(buffer, "\n%02x", value);
				save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
			} */
			sprintf(buffer, "%02x", value);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
	}
	for(int i = 0x48C00; i < 0x48E00; i++)
	{
		result = lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, i, (uint64_t)(uint32_t) &value, 0, 0, 0, 0);
		if (result != 0)
		{
			sprintf(buffer, "%s:%d: lv2_ss_update_mgr_if(READ_EPROM) failed (0x%08x)\n", __func__, __LINE__, result);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
		else
		{
/* 			if(i%16==0)
			{
				sprintf(buffer, "\n%02x", value);
				save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
			} */
			sprintf(buffer, "%02x", value);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
	}
/* 	for(int i = 0x49000; i < 0x49C00; i++)
	{
		result = lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, i, (uint64_t)(uint32_t) &value, 0, 0, 0, 0);
		if (result != 0)
		{
			sprintf(buffer, "%s:%d: lv2_ss_update_mgr_if(READ_EPROM) failed (0x%08x)\n", __func__, __LINE__, result);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
		else
		{
			if(i%16==0)
			{
				sprintf(buffer, "\n%02x", value);
				save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
			}
			sprintf(buffer, "%02x", value);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
	} */
/* 	result = lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, RECOVER_FLAG_OFFSET, (uint64_t)(uint32_t) &value, 0, 0, 0, 0);
	if (result != 0)
	{
		// printf("%s:%d: lv2_ss_update_mgr_if(READ_EPROM) failed (0x%08x)\n", __func__, __LINE__, result);
		sprintf(buffer, "%s:%d: lv2_ss_update_mgr_if(READ_EPROM) failed (0x%08x)\n", __func__, __LINE__, result);
		save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
	}
	else
	{
		// printf("\n%02x", value);
		sprintf(buffer, "\n\n%02x", value);
		save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
	} */
/* 	for(int i = 0x48C0A; i < 0x48C0C; i++)
	{
		result = lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, i, (uint64_t)(uint32_t) &value, 0, 0, 0, 0);
		if (result != 0)
		{
			sprintf(buffer, "%s:%d: lv2_ss_update_mgr_if(READ_EPROM) failed (0x%08x)\n", __func__, __LINE__, result);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
		else
		{
			if(i%16==0)
			{
				sprintf(buffer, "\n%02x", value);
				save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
			}
			sprintf(buffer, "%02x", value);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
	} */
/* 	for(int i = 0x48D3E; i < 0x48E00; i++)
	{
		result = lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, i, (uint64_t)(uint32_t) &value, 0, 0, 0, 0);
		if (result != 0)
		{
			sprintf(buffer, "%s:%d: lv2_ss_update_mgr_if(READ_EPROM) failed (0x%08x)\n", __func__, __LINE__, result);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
			return -1;
		}
		else
		{
			if(i%16==0)
			{
				sprintf(buffer, "%02x", value);
				save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
			}
			sprintf(buffer, "%02x", value);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
	} */
	return CELL_OK;
}
/* int read_eeprom(void)
{	
	uint8_t value=0;
	int result;
	char buffer[4096];
	if(cellFsStat(edump, &s) == CELL_OK)
		cellFsUnlink(edump);
	sprintf(buffer, "%s:%d: Start\n", __func__, __LINE__);
	save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
	for(int i = 0x48C0A;)
	{
		result = lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, i, (uint64_t)(uint32_t) &value, 0, 0, 0, 0);
		if (result != 0)
		{
			sprintf(buffer, "%s:%d: lv2_ss_update_mgr_if(READ_EPROM) failed (0x%08x)\n", __func__, __LINE__, result);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
		else
		{
			if(i%16==0)
			{
				sprintf(buffer, "\n%02x", value);
				save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
			}
			sprintf(buffer, "%02x", value);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
	}
	for(int i = 0x48D3E; i < 0x48E00; i++)
	{
		result = lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, i, (uint64_t)(uint32_t) &value, 0, 0, 0, 0);
		if (result != 0)
		{
			sprintf(buffer, "%s:%d: lv2_ss_update_mgr_if(READ_EPROM) failed (0x%08x)\n", __func__, __LINE__, result);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
		else
		{
			if(i%16==0)
			{
				sprintf(buffer, "\n%02x", value);
				save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
			}
			sprintf(buffer, "%02x", value);
			save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
		}
	}
	return CELL_OK;
} */
// int write_eeprom(uint64_t offset, uint8_t value)
int write_eeprom(void)
{	
	uint8_t new=0, value;
	// uint64_t offset=0;
	int result;
	char buffer[4096];
	// patch eprom checks
	// patch_hv_checks();
/* 	if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, RECOVER_FLAG_OFFSET, (uint64_t)(uint32_t) &value, 0, 0, 0, 0)) == 0)
	{
		printf("\n%02x\n", value);
		if (value == 0xFF)
		{	// enable
			new = 0x0;
			if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, RECOVER_FLAG_OFFSET, new, 0, 0, 0, 0)) == 0)
			{
				// sprintf(buffer, "%s:%d: lv2_ss_update_mgr_if(WRITE_EPROM) failed (0x%08x)\n", __func__, __LINE__);
				// printf(buffer);
				goto exit;
			}
		}
		else if (value == 0x0)
		{// disable
			new = 0xFF;
			if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, (uint64_t)(uint32_t)offset, new, 0, 0, 0, 0)) == 0)
			{
				// sprintf(buffer, "%s:%d: lv2_ss_update_mgr_if(WRITE_EPROM) failed (0x%08x)\n", __func__, __LINE__);
				// printf(buffer);
				goto exit;
			}
		}
	} else goto exit; */
	if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, QA_FLAG_OFFSET, (uint64_t)(uint32_t) &value, 0, 0, 0, 0)) == 0)
	{
		printf("\n%02x\n", value);
		if (value == 0xFF)
		{	// enable
			new = 0xFF;
			for(int i = 0x48D3E; i < 0x48E00; i++)
			{
				result = lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, i, new, 0, 0, 0, 0);
				if (result != 0)
				{
					sprintf(buffer, "%s:%d: lv2_ss_update_mgr_if(READ_EPROM) failed (0x%08x)\n", __func__, __LINE__, result);
					save_file(edump, buffer, (-0xADD0ADD0ADD000ALL));
					goto exit;
				}
			}
		}
	} else goto exit;
/* 	if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_READ_EPROM, RECOVER_FLAG_OFFSET, (uint64_t)(uint32_t) &value, 0, 0, 0, 0)) == 0)
	{
		printf("\n%02x", value);
	} */
/* 	if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, (uint64_t)(uint32_t)offset, (uint64_t)(uint32_t)value, 0, 0, 0, 0)) != 0)
		goto exit; */
/* 	for (offset = 0x2F00; offset < 0x03100; offset ++)
	{
		if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, offset, value, 0, 0, 0, 0)) != 0)
			goto exit;
	}
	for (offset = 0x48000; offset < 0x48100; offset ++)
	{
		if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, offset, value, 0, 0, 0, 0)) != 0)
			goto exit;
	}
	for (offset = 0x48800; offset < 0x48900; offset ++)
	{
		if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, offset, value, 0, 0, 0, 0)) != 0)
			goto exit;
	}
	for (offset = 0x48C00; offset < 0x48E00; offset ++)
	{
		if((lv2_ss_update_mgr_if(UPDATE_MGR_PACKET_ID_WRITE_EPROM, offset, value, 0, 0, 0, 0)) != 0)
			goto exit;
	} */
	{system_call_3(392, 0x1004, 0x4, 0x6); } //1xBeep
	return CELL_OK;
exit:
	return -1;
}

void bootmem_toggle(void)
{
	uint64_t lv1_offset=0;
	detect_firmware();
	if(!dex_mode)
		lv1_offset=0x8000000000379F8AULL;
	else
		lv1_offset=0x800000000015DF9AULL;
	if(lv1_peek(lv1_offset)!=0x00000000001B0000ULL)//
	{
		lv1_offset=0;
		for(uint64_t addr=0xA000; addr<0x800000ULL; addr+=4)//16MB
		{
			lv1_offset=addr;
			if(lv1_peek(addr) == 0x4D47520000000000ULL)//MGR.....
			{
				lv1_offset=addr+16;//
					break;
			}
		}
	}
	if(lv1_offset && lv1_peek(lv1_offset)==0x00000000001B0000ULL)
	{
		lv1_poke(lv1_offset +  0, 0x00000000001E0000ULL);
		{system_call_3(392, 0x1004, 0x4, 0x6);}
	}
	else
	if(lv1_offset && lv1_peek(lv1_offset)==0x00000000001E0000ULL)
	{
		lv1_poke(lv1_offset +  0, 0x0000000000140000ULL);
		{system_call_3(392, 0x1004, 0x7, 0x36);}
	}
	else
	{
		// lv1_poke(lv1_offset +  0, 0x00000000001B0000ULL);
		system_call_3(392, 0x1004, 0xa, 0x1b6); //3 beep
	}
}

void cu_toggle(void)
{
	uint64_t dex=0x80000000002EA490ULL;
	uint64_t cex=0x80000000002CFD10ULL;
	detect_firmware();
	if(dex_mode) {
		if(peekq(dex)==0x4455435400000000ULL) {
			uint64_t addr=0x80000000002EB7E0ULL;
			pokeq(addr +   0, 0x2564257325303136ULL);
			pokeq(addr +   8, 0x6C78253031366C6CULL);
			pokeq(addr +  16, 0x78253031366C6C78ULL);
			pokeq(addr +  24, 0x2573257325303878ULL);
			pokeq(addr +  32, 0x2564253164253164ULL);
			pokeq(addr +  40, 0x2531644141410A00ULL);
							
			uint64_t addr2=0x800000000006149cULL;
			pokeq(addr2 +   0, 0x396001be38600000ULL);
			pokeq(addr2 +  24, 0x6386000239600001ULL);
			system_call_3(392, 0x1004, 0x4, 0x6); //1xBeep
		}
		else {
			pokeq(dex, 0x4455435400000000ULL);
			system_call_3(392, 0x1004, 0x4, 0x6); //1xBeep
		}
	}
	else {
		if(peekq(cex)==0x4455435400000000ULL) {
			uint64_t addr=0x80000000002D1060ULL;
			pokeq(addr +   0, 0x2564257325303136ULL);
			pokeq(addr +   8, 0x6C78253031366C6CULL);
			pokeq(addr +  16, 0x78253031366C6C78ULL);
			pokeq(addr +  24, 0x2573257325303878ULL);
			pokeq(addr +  32, 0x2564253164253164ULL);
			pokeq(addr +  40, 0x2531644141410A00ULL);
			system_call_3(392, 0x1004, 0x4, 0x6); //1xBeep
		}
		else {
			pokeq(cex, 0x4455435400000000ULL);
			system_call_3(392, 0x1004, 0x4, 0x6); //1xBeep
		}
	}
}


