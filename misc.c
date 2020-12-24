#include "misc.h"
#include "inc/exports.h"

// #ifdef VSH_MENU
static void *vsh_pdata_addr = NULL;
static void *vsh_pad_obj_addr = NULL;
// #endif

/***********************************************************************
* search and return vsh_process toc
* Not the best way, but it work, it's generic and it is fast enough...
***********************************************************************/
// #ifdef VSH_MENU
static uint32_t get_vsh_toc(void)
{
    uint32_t pm_start  = 0x10000UL;
    uint32_t v0 = 0, v1 = 0, v2 = 0;

    while(pm_start < 0x700000UL)
    {
        v0 = *(uint32_t*)(pm_start +0x00);
        v1 = *(uint32_t*)(pm_start +0x04);
        v2 = *(uint32_t*)(pm_start +0x0C);

        if((v0 == 0x10200UL/* .init_proc() */) && (v1 == v2)) break;

        pm_start += 4;
    }

    return v1;
}
// #endif

/***********************************************************************
* get vsh io_pad_object
***********************************************************************/
// #ifdef VSH_MENU
static uint32_t get_vsh_pad_obj(void)
{
    uint32_t (*base)(uint32_t) = (void *)sys_io_3733EA3C;               // get pointer to cellPadGetData()
    int16_t idx = *(uint32_t*)(*(uint32_t*)base) & 0x0000FFFF;  // get got_entry idx from first instruction,  
    uint32_t got_entry = (idx + get_vsh_toc());                 // get got_entry of io_pad_object
    vsh_pad_obj_addr = (void *)got_entry;
    return (*(uint32_t*)got_entry);                             // return io_pad_object address
}
// #endif

/***********************************************************************
* get pad data struct of vsh process
* 
* CellPadData *data = data struct for holding pad_data
***********************************************************************/
// #ifdef VSH_MENU
void VSHPadGetData(CellPadData *data)
{
    uint32_t pm_start = 0x10000UL;
    uint64_t pat[2]   = {0x380000077D3F4B78ULL, 0x7D6C5B787C0903A6ULL};

    if(!vsh_pdata_addr)        // first time, get address
    {
        while(pm_start < 0x700000UL)
        {
            if((*(uint64_t*) pm_start     == pat[0])
            && (*(uint64_t*)(pm_start +8) == pat[1]))
            {
                vsh_pdata_addr = (void*)(uint32_t)(
                    (int32_t)((*(uint32_t*)(pm_start +0x234) & 0x0000FFFF) <<16) +
                    (int16_t)( *(uint32_t*)(pm_start +0x244) & 0x0000FFFF));

                break;
            }

            pm_start += 4;
        }
    }

    memcpy(data, vsh_pdata_addr, sizeof(CellPadData));
}
// #endif

/***********************************************************************
* set/unset io_pad_library init flag
* 
* uint8_t flag = 0(unset) or 1(set)
* 
* To prevent vsh pad events during vsh-menu, we set this flag to 0
* (pad_library not init). Each try of vsh to get pad_data leads intern
* to error 0x80121104(CELL_PAD_ERROR_UNINITIALIZED) and nothing will
* happen. The lib is of course not deinit, there are pad(s) mapped and we
* can get pad_data direct over syscall sys_hid_manager_read() in our
* own function MyPadGetData().
***********************************************************************/
// #ifdef VSH_MENU
void start_stop_vsh_pad(uint8_t flag)
{
    if (!vsh_pad_obj_addr) {
      get_vsh_pad_obj();
    }
    uint32_t lib_init_flag = *(uint32_t *)vsh_pad_obj_addr;
    *(uint8_t*)lib_init_flag = flag;
}
// #endif

/***********************************************************************
* get pad data direct over syscall 
* (very simple, no error-handling, no synchronization...)
* 
* int32_t port_no   = pad port number (0 - 7)
* CellPadData *data = data struct for holding pad_data
***********************************************************************/
// #ifdef VSH_MENU
void MyPadGetData(int32_t port_no, CellPadData *data)
{
    if (!vsh_pad_obj_addr) {
      get_vsh_pad_obj();
    }
    uint32_t vsh_pad_obj = (uint32_t)(*(uint32_t*)vsh_pad_obj_addr);
    uint32_t port = *(uint32_t*)(*(uint32_t*)(vsh_pad_obj +4) + 0x104 + port_no * 0xE8);

    // sys_hid_manager_read()
    system_call_4(0x1F6, (uint64_t)port, /*0x02*//*0x82*/0xFF, (uint64_t)(uint32_t)data+4, 0x80);

    data->len = (int32_t)p1;
}
// #endif

