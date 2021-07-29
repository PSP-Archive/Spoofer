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

// function_ptr
extern void (*string_memory_allocate)(void *ptr, void *str);
extern void (*scePafConvertText)(void *arg0, wchar_t *arg1);
extern int (*scePafGetCurrentClockLocalTime)(pspTime *time);

// global
wchar_t str_buf[256];
int sleep_flag = 0;

// extern
extern SceUID sema_ctrl;
extern SceUID second_sema;
extern wchar_t *format_buf;
extern wchar_t *week_buf[7];
extern wchar_t *month_buf[12];
extern wchar_t *ver_buf;
extern wchar_t *mac_buf;
extern u32 spoofer_param[13];
extern u32 ver_param[13];
extern u32 mac_param[13];

// function
void division(int m, int n, int *q, int *r)
{
	*q = 0;
	*r = 0;

	if(m < n)
		return;

	while(m >= n)
	{
		m -= n;
		*q += 1;
	}

	*r = m;
	return;
}

void ArgumentControl(wchar_t *buf, int len, const wchar_t *format, u32 *param)
{
	pspTime time;
	char bath_degit[64], batm_degit[64];
	int i, week_pos, last_year, last_month, last_day, hour_num, min_num, sec_num, life, per, bath, batm;
	int *arg[13];

	// init
	week_pos = 0;
	last_year = 0;
	last_month = 0;
	last_day = 0;
	hour_num = 0;

	// get info
	scePafGetCurrentClockLocalTime(&time);

	per = scePowerGetBatteryLifePercent();
	life = scePowerGetBatteryLifeTime();

	hour_num = time.hour;
	min_num = time.minutes;
	sec_num = time.seconds;

	if(time.year != last_year || time.month != last_month || time.day != last_day)
	{
		last_year = time.year;
		last_month = time.month;
		last_day = time.day;

		week_pos = sceRtcGetDayOfWeek(time.year, time.month, time.day);

		week_pos--;
		if(week_pos < 0) week_pos = 6;
	}

	division(life, 60, &bath, &batm);

	if(bath < 0 || bath > 59)
		bath = 60;

	if(batm < 0 || batm > 59)
		batm = 60;

	for(i = 0; i < 13; i++)
		arg[i] = NULL;

	for(i = 0; i < 13; i++)
	{
		if(param[i] == 13)
			break;

		switch(param[i])
		{
			case 0:
				arg[i] = (int *)week_buf[week_pos];
				break;
			case 1:
				arg[i] = (int *)last_day;
				break;
			case 2:
				arg[i] = (int *)month_buf[time.month-1];
				break;
			case 3:
				arg[i] = (int *)last_month;
				break;
			case 4:
				arg[i] = (int *)last_year;
				break;
			case 5:
				arg[i] = (int *)(hour_num > 11 ? hour_num - 12 : hour_num);
				break;
			case 6:
				arg[i] = (int *)hour_num;
				break;
			case 7:
				arg[i] = (int *)min_num;
				break;
			case 8:
				arg[i] = (int *)sec_num;
				break;
			case 9:
				arg[i] = (int *)(hour_num < 12 ? L"AM" : L"PM");
				break;
			case 10:
				arg[i] = (int *)per;
				break;
			case 11:
				if(bath != 60)
				{
					itostring(bath_degit, bath, 10, 0, 2, 1);
					arg[i] = (int *)bath_degit;
				}
				else
				{
					arg[i] = (int *)L"--";
				}
				break;
			case 12:
				if(batm != 60)
				{
					itostring(batm_degit, batm, 10, 0, 2, 1);
					arg[i] = (int *)batm_degit;
				}
				else
				{
					arg[i] = (int *)L"--";
				}
				break;
		}
	}

	swprintf(buf, len, format, arg[0], arg[1], arg[2],
		arg[3], arg[4], arg[5], arg[6], arg[7],
		arg[8], arg[9], arg[10], arg[11], arg[12]);

	return;
}

int SpooferThread(SceSize args, void *argp)
{
	while(1)
	{
		sceKernelWaitSema(sema_ctrl, 1, NULL);
		ArgumentControl(str_buf, 256, format_buf, spoofer_param);
		sceKernelSignalSema(second_sema, 1);
	}

	return 0;
}

void string_memory_allocate_patched(void *ptr, void *str)
{
	if(!sleep_flag)
	{
		sceKernelSignalSema(sema_ctrl, 1);
		sceKernelWaitSema(second_sema, 1, NULL);
	}

	string_memory_allocate(ptr, str_buf);
	return;
}

void scePafConvertTextPatched(void *arg0, wchar_t *arg1)
{
	wchar_t buf[256];
	ArgumentControl(buf, 256, ver_buf, ver_param);
	scePafConvertText(arg0, buf);
	return;
}

int sce_paf_private_swprintf_patched(wchar_t *buf, int n, const wchar_t *format, void *t0)
{
	ArgumentControl(buf, 256, mac_buf, mac_param);
	return 0;
}

int sceKernelWaitThreadEndPatched(SceUID thid, SceUInt *timeout)
{
	sleep_flag = 0;
	return sceKernelWaitThreadEnd(thid, timeout);
}

SceUID sceKernelCreateThreadPatched(const char *name, SceKernelThreadEntry entry, int pri,
										int stack_size, SceUInt attr, SceKernelThreadOptParam *opt)
{
	sleep_flag = 1;
	return sceKernelCreateThread(name, entry, pri, stack_size, attr, opt);
}

