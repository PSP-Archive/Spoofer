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

// define
#define MS_LOG_FILE_PATH "ms0:/log.txt"
#define EF_LOG_FILE_PATH "ef0:/log.txt"

// global
int first_flag = 0;

// extern
extern char g_path[256];
extern int go_model;

// function
void *copy_mem(void *dest, const void *src, int n)
{
	char *s1, *s2;

	for(s1 = (char *)dest, s2 = (char *)src; n > 0; s1++, s2++, n--)
	{
		*s1 = *s2;
	}

	return dest;
}

char *search_char(const char *dest, int src)
{
	char *ptr, *res;

	for(ptr = (char *)dest, res = NULL, src = (char)src; *ptr != '\0'; ptr++)
	{
		if(*ptr == src)
			res = ptr;
	}

	return res;
}

char *copy_char(char *dest, const char *src)
{
	char *ptr;

	for(ptr = dest;
		(*ptr = *src) != '\0';
			ptr++, src++);

	return dest;
}

wchar_t *copy_wchar(wchar_t *dest, const wchar_t *src)
{
	wchar_t *ptr;

	for(ptr = dest;
		(*ptr = *src) != L'\0';
			ptr++, src++);

	return dest;
}

int count_wchar(const wchar_t *dest)
{
	int count;
	wchar_t *ptr;

	for(ptr = (wchar_t *)dest, count = 0; *ptr != L'\0'; ptr++, count++);

	return count;
}

int cmp_char(const char *dest, const char *src)
{
	u8 *s1 = (u8 *)dest;
	u8 *s2 = (u8 *)src;

	for(; *s1 == *s2 && *s1 != '\0'; s1++, s2++);

	return (*s1 - *s2) == 0 ? 0 : 1;
}

int cmp_wchar(const wchar_t *dest, const wchar_t *src)
{
	wchar_t *s1 = (wchar_t *)dest;
	wchar_t *s2 = (wchar_t *)src;

	for(; *s1 == *s2 && *s1 != L'\0'; s1++, s2++);

	return (*s1 - *s2) == 0 ? 0 : 1;
}

int cmpn_case_wchar(const wchar_t *dest, const wchar_t *src, int n)
{
	int res;
	wchar_t c1, c2;

	if(!dest || !src || n <= 0)
		return 1;

	for(res = 0; res == 0 && n > 0; dest++, src++, n--)
	{
		c1 = *dest;
		c2 = *src;

		if(*dest >= L'A' && *dest <= L'Z')
			c1 += 0x20;

		if(*src >= L'A' && *src <= L'Z')
			c2 += 0x20;

		if(c1 != c2)
			res = 1;
	}

	return res;
}

void *malloc_p(SceSize size)
{
	SceUID block;
	void *ptr;

	block = sceKernelAllocPartitionMemory(2, "block", 1, size + 4, NULL);

	if(block < 0)
		return NULL;

	ptr = sceKernelGetBlockHeadAddr(block);

	copy_mem(ptr, &block, 4);
	return ptr + 4;
}

int free_p(void *ptr)
{
	SceUID block;

	if(!ptr)
		return -1;

	copy_mem(&block, ptr - 4, 4);
	return sceKernelFreePartitionMemory(block);
}

int Log_printf(const char *format, ...)
{
	SceUID fd;
	int len, res;
	char buffer[256], *str;
	va_list ap;

	if(g_path[0] == 'e' && go_model)
	{
		str = EF_LOG_FILE_PATH;
	}
	else
	{
		str = MS_LOG_FILE_PATH;
	}

	if(first_flag != 1)
	{
		first_flag = 1;
		fd = sceIoOpen(str, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	}
	else
	{
		fd = sceIoOpen(str, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_APPEND, 0777);
	}

	va_start(ap, format);
	len = vsnprintf(buffer, 256, format, ap);
	va_end(ap);

	res = sceIoWrite(fd, buffer, len);
	sceIoClose(fd);

	return res;
}

