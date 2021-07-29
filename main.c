/*
	Spoofer v0.7.1
	Copyright (C) 2011, plum

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// header
#include "spoofer.h"

// info
PSP_MODULE_INFO("Spoofer", 0x0007, 0, 7);
PSP_NO_CREATE_MAIN_THREAD();

// function_ptr
void (*string_memory_allocate)(void *ptr, void *str);
void (*scePafConvertText)(void *arg0, wchar_t *arg1);
int (*scePafGetCurrentClockLocalTime)(pspTime *time);

// global
STMOD_HANDLER previous;
SceUID sema_ctrl;
SceUID second_sema;
u32 memalloc_buf[2];
u32 conv_buf[2];
u32 clock_buf[2];
int go_model;
char g_path[256];

// extern
extern int ver_flag;
extern int mac_flag;
extern wchar_t str_buf[256];

// function
void kuKernelIcacheInvalidateAll(void)
{
	int k1 = pspSdkSetK1(0);
	sceKernelIcacheInvalidateAll();
	pspSdkSetK1(k1);
}

void ClearCaches(void)
{
	sceKernelDcacheWritebackAll();
	kuKernelIcacheInvalidateAll();
}

int CheckGoModel(void)
{
	SceUID dfd = sceIoDopen("ef0:/");
	sceIoDclose(dfd);
	return dfd > 0 ? 1 : 0;
}

void *FindModuleStubAddr(SceModule2 *mod, const char *szLib, u32 nid)
{
	int i;
	u32 stub_top, stub_end;
	SceLibraryStubTable *stub;

	stub_top = (u32)mod->stub_top;
	stub_end = (u32)((u32)mod->stub_top + mod->stub_size);

	while(stub_top < stub_end)
	{
		stub = (SceLibraryStubTable *)stub_top;

		if(cmp_char(stub->libname, szLib) == 0)
		{
			for(i = 0; i < stub->stubcount; i++)
			{
				if(stub->nidtable[i] == nid)
				{
					return (void *)((u32)(stub->stubtable) + i * 8);
				}
			}
		}

		stub_top += (stub->len * 4);
	}

	return NULL;
}

int OnModuleStart(SceModule2 *mod)
{
	u32 text_addr, text_end;

	if(cmp_char(mod->modname, "vsh_module") == 0)
	{
		text_addr = mod->text_addr;
		text_end = mod->text_addr + mod->text_size;

		for(; text_addr < text_end; text_addr += 4)
		{
			if(_lw(text_addr) == 0x24060064)
			{
				REDIRECT_FUNCTION((u32)memalloc_buf, U_EXTRACT_CALL(text_addr + 0x24));
				string_memory_allocate = (void *)memalloc_buf;

				REDIRECT_FUNCTION((u32)clock_buf, U_EXTRACT_CALL(text_addr - 0x50));
				scePafGetCurrentClockLocalTime = (void *)clock_buf;

				_sh(0, text_addr - 0x48);
				MAKE_CALL(text_addr + 0x24, string_memory_allocate_patched);

				ClearCaches();
				break;
			}
		}
	}
	else if(cmp_char(mod->modname, "sysconf_plugin_module") == 0)
	{
		text_addr = mod->text_addr;
		text_end = mod->text_addr + mod->text_size;

		for(; text_addr < text_end; text_addr += 4)
		{
			if(_lw(text_addr) == 0x34C600C9 && _lw(text_addr + 8) == 0x00000000)
			{
				if(ver_flag == 1)
				{
					text_addr += 0x48;

					REDIRECT_FUNCTION((u32)conv_buf, U_EXTRACT_CALL(text_addr));
					scePafConvertText = (void *)conv_buf;

					MAKE_CALL(text_addr, scePafConvertTextPatched);
					ClearCaches();
				}
			}

			else if(_lw(text_addr) == 0x24070006)
			{
				if(mac_flag == 1)
				{
					MAKE_CALL(text_addr + 0x30, sce_paf_private_swprintf_patched);
					ClearCaches();
				}
			}
		}
	}

	if(go_model)
	{
		if(cmp_char(mod->modname, "VshCtrlSatelite") == 0)
		{
			void *create_stub = FindModuleStubAddr(mod, "ThreadManForUser", 0x446D8DE6);
			void *end_stub = FindModuleStubAddr(mod, "ThreadManForUser", 0x278C0DF5);

			if(create_stub != NULL && end_stub != NULL)
			{
				REDIRECT_FUNCTION((u32)create_stub, sceKernelCreateThreadPatched);
				REDIRECT_FUNCTION((u32)end_stub, sceKernelWaitThreadEndPatched);
				ClearCaches();
			}
		}
	}

	return previous ? previous(mod) : 0;
}

int module_start(SceSize args, void *argp)
{
	SceUID thid;
	char *ptr;

	thid = sceKernelCreateThread("SpooferThread", SpooferThread, 16, 0x1000, 0, NULL);

	if(thid < 0)
		return -1;

	sema_ctrl = sceKernelCreateSema("SemaController", 0, 0, 1, NULL);
	second_sema = sceKernelCreateSema("SecondarySema", 0, 0, 1, NULL);

	if(sema_ctrl < 0 || second_sema < 0)
		return -1;

	sceKernelStartThread(thid, 0, NULL);

	// check go_model
	go_model = CheckGoModel();

	// make path
	copy_char(g_path, (char *)argp);
	ptr = search_char(g_path, '/');
	copy_char(ptr + 1, "spoofer_config.txt");

	// load config
	SpooferLoadConfig(g_path);

	// init
	copy_wchar(str_buf, L"Please wait...");

	// set start_module_handler
	previous = sctrlHENSetStartModuleHandler(OnModuleStart);
	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}

