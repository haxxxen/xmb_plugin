CELL_MK_DIR = $(CELL_SDK)/samples/mk
include $(CELL_MK_DIR)/sdk.makedef.mk

BUILD_TYPE     	= release

LIBSTUB_DIR		= ./lib
PRX_DIR			= .
INSTALL			= cp
PEXPORTPICKUP		= ppu-lv2-prx-exportpickup
PRX_LDFLAGS_EXTRA	= -L ./lib -Wl,--strip-unused-data

CRT_HEAD                += $(shell ppu-lv2-gcc -print-file-name'='ecrti.o)
CRT_HEAD                += $(shell ppu-lv2-gcc -print-file-name'='crtbegin.o)
CRT_TAIL                += $(shell ppu-lv2-gcc -print-file-name'='crtend.o)
CRT_HEAD                += $(shell ppu-lv2-gcc -print-file-name'='ecrtn.o)

PPU_PRX_EXLIBS =	-lstdc_export_stub \
          		 	-lsysPrxForUser_export_stub \
             	 	-lvsh_export_stub \
               		-lpaf_export_stub \
               		-lvshmain_export_stub \
               		-lvshtask_export_stub \
              		-lallocator_export_stub \
               		-lsdk_export_stub \
               		-lxsetting_export_stub \
               		-lsys_io_export_stub \
               		# -lpngdec_ppuonly_export_stub \
               		# -lsysutil_np_trophy_stub \
               		# -llv2_stub \
               		-lsysutil_stub

PPU_SRCS = printf.c libc.c misc.c rebug.c main.c
PPU_PRX_TARGET = edy_plugin.prx
PPU_PRX_LDFLAGS += $(PRX_LDFLAGS_EXTRA)
PPU_PRX_STRIP_FLAGS = -s
PPU_PRX_LDLIBS 	= $(PPU_PRX_EXLIBS) -lfs_stub -lnet_stub -lrtc_stub -lio_stub -llv2_stub
# PPU_PRX_LDLIBS 	= -lfs_stub -lnet_stub -lrtc_stub -lio_stub -lgcm_sys_stub -lc_stub

PPU_CFLAGS += -Os -fno-builtin-printf -nodefaultlibs -fmerge-all-constants -std=gnu99 -Wno-shadow -Wno-unused-parameter
#-ffunction-sections -fdata-sections -lstdc_export_stub -DTEST

CLEANFILES = $(PRX_DIR)/$(PPU_SPRX_TARGET) *.elf

all:
	$(MAKE) $(PPU_OBJS_DEPENDS)
	$(PPU_PRX_STRIP) --strip-debug --strip-section-header $(PPU_PRX_TARGET)
	F:/BKP/Tools/make_fself $(PPU_PRX_TARGET) $(PPU_SPRX_TARGET)
	# rm -fr objs *.sym *.prx *.elf
	# @sfk.exe partcopy stdc -allfrom 0x0 regcam_plugin.prx 0x30F0 -yes
	F:/BKP/Tools/scetool --v -p F:/BKP/Tools/data --sce-type=SELF --compress-data=TRUE --template="edy_plugin.sprx.ori" --encrypt edy_plugin.prx edy_plugin.sprx
	rm -fr objs *.sym *.prx *.elf

include $(CELL_MK_DIR)/sdk.target.mk




