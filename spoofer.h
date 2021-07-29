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

#ifndef __SPOOFER_H__
#define __SPOOFER_H__

// include
#include <pspkernel.h>
#include <systemctrl.h>
#include <psputilsforkernel.h>
#include <psprtc.h>
#include <psppower.h>
#include <stdarg.h>

// define
#define MAKE_CALL(a, f) _sw(0x0C000000 | (((u32)(f) >> 2) & 0x03FFFFFF), a);
#define REDIRECT_FUNCTION(a, f) _sw(0x08000000 | (((u32)(f) & 0x0FFFFFFC) >> 2), a); _sw(0, a + 4);
#define U_EXTRACT_CALL(x) ((((u32)_lw((u32)x)) & ~0x0C000000) << 2)

// struct
typedef struct
{
	wchar_t str[13];
	int len;
	wchar_t conv[5];
	int number;
} FormatType;

// vshpatch.c
int SpooferThread(SceSize args, void *argp);
void string_memory_allocate_patched(void *ptr, void *str);
void scePafUpdateIconStatusPatched(void *arg0, int size, void (*update_func)(), void *arg3);
void scePafConvertTextPatched(void *arg0, wchar_t *arg1);
int sce_paf_private_swprintf_patched(wchar_t *buf, int n, const wchar_t *format, void *t0);
int sceKernelWaitThreadEndPatched(SceUID thid, SceUInt *timeout);
SceUID sceKernelCreateThreadPatched(const char *name, SceKernelThreadEntry entry, int pri,
										int stack_size, SceUInt attr, SceKernelThreadOptParam *opt);

// conf.c
int SpooferLoadConfig(const char *path);

// utils.c
void *clear_mem(void *dest, int src, int n);
void *copy_mem(void *dest, const void *src, int n);
char *search_char(const char *dest, int src);
char *copy_char(char *dest, const char *src);
wchar_t *copy_wchar(wchar_t *dest, const wchar_t *src);
int count_wchar(const wchar_t *dest);
int cmp_char(const char *dest, const char *src);
int cmp_wchar(const wchar_t *dest, const wchar_t *src);
int cmpn_case_wchar(const wchar_t *dest, const wchar_t *src, int n);
void *malloc_p(SceSize size);
int free_p(void *ptr);
int Log_printf(const char *format, ...);

// printf.c
int itostring(char *buf, int value, int base, int code, int zero, int w_flag);
int vsnprintf(char *buf, int n, const char *format, va_list ap);
int snprintf(char *buf, int n, const char *format, ...);
int vswprintf(wchar_t *buf, int n, const wchar_t *format, va_list ap);
int swprintf(wchar_t *buf, int n, const wchar_t *format, ...);

#endif

