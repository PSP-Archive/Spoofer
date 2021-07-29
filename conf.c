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
#define BUFFER_SIZE 512

// global
wchar_t *format_buf;
wchar_t *week_buf[7];
wchar_t *month_buf[12];
wchar_t *ver_buf;
wchar_t *mac_buf;
u32 spoofer_param[13];
u32 ver_param[13];
u32 mac_param[13];
int ver_flag;
int mac_flag;

// struct
FormatType types[13] =
{
	{ L"%weekday", 8, L"%s", 0 },
	{ L"%day", 4, L"%02d", 1 },
	{ L"%monthname", 10, L"%s", 2 },
	{ L"%month", 6, L"%02d", 3 },
	{ L"%year", 5, L"%04d", 4 },
	{ L"%hour12", 7, L"%02d", 5 },
	{ L"%hour", 5, L"%02d", 6 },
	{ L"%min", 4, L"%02d", 7 },
	{ L"%sec", 4, L"%02d", 8 },
	{ L"%ampm", 5, L"%s", 9 },
	{ L"%batpercent", 11, L"%02d", 10 },
	{ L"%batlifehour", 12, L"%s", 11 },
	{ L"%batlifemin", 11, L"%s", 12 },
};

// function
void SetDefault(void)
{
	int i;

	// default week_text
	wchar_t week_text[7][10] =
	{
		L"Monday",
		L"Tuesday",
		L"Wednesday",
		L"Thursday",
		L"Friday",
		L"Saturday",
		L"Sunday",
	};

	// default month_text
	wchar_t month_text[12][10] =
	{
		L"January",
		L"February",
		L"March",
		L"April",
		L"May",
		L"June",
		L"July",
		L"August",
		L"September",
		L"October",
		L"November",
		L"December",
	};

	format_buf = malloc_p(256);

	if(!format_buf)
		return;

	copy_wchar(format_buf,
		L"\n%s, %02d. %s %02d %02d:%02d:%02d %02d%% (%02d:%02d)\nSpoofer, plum");

	spoofer_param[0] = 0;
	spoofer_param[1] = 1;
	spoofer_param[2] = 2;
	spoofer_param[3] = 4;
	spoofer_param[4] = 6;
	spoofer_param[5] = 7;
	spoofer_param[6] = 8;
	spoofer_param[7] = 10;
	spoofer_param[8] = 11;
	spoofer_param[9] = 12;
	spoofer_param[10] = 13;
	spoofer_param[11] = 13;
	spoofer_param[12] = 13;

	for(i = 0; i < 7; i++)
	{
		week_buf[i] = malloc_p(20);

		if(!week_buf[i])
			return;

		copy_wchar(week_buf[i], week_text[i]);
	}

	for(i = 0; i < 12; i++)
	{
		month_buf[i] = malloc_p(20);

		if(!month_buf[i])
			return;

		copy_wchar(month_buf[i], month_text[i]);
	}

	return;
}

int ConvertFormat(wchar_t *buf, wchar_t *str, u32 *param)
{
	wchar_t *ptr;
	int i, n, match, len;

	for(i = 0; i < 13; i++)
		param[i] = 13;

	n = 0;
	ptr = buf;

	while(*str != L'\0')
	{
		if(*str == L'%')
		{
			if(str[1] == L'%')
			{
				*ptr++ = L'%';
				*ptr++ = L'%';
				str += 2;
			}
			else
			{
				for(i = 0, match = 0; i < 13; i++)
				{
					if(cmpn_case_wchar(str, types[i].str, types[i].len) == 0)
					{
						len = count_wchar(types[i].conv);
						copy_wchar(ptr, types[i].conv);

						ptr += len;
						str += types[i].len;

						param[n++] = types[i].number;
						match = 1;
						break;
					}
				}

				if(!match)
					str++;
			}
		}
		else if(*str == L'\\')
		{
			*ptr++ = L'\n';
			str++;
		}
		else
		{
			*ptr++ = *str++;
		}
	}

	*ptr = L'\0';
	return ptr - buf;
}

int ReadLine(SceUID fd, void *data, int read_size, int len)
{
	char ch, *cbuf;
	wchar_t wh, *wbuf;
	int count, res, flag;

	flag = 0;
	count = 0;
	len -= 1;

	if(read_size == 1)
	{
		cbuf = (char *)data;

		do
		{
			res = sceIoRead(fd, &ch, 1);

			if(res == 1)
			{
				if(ch == '\n')
				{
					flag = 1;
					break;
				}

				else if(ch == '\r')
					continue;

				cbuf[count] = ch;
				count += 2;
			}
		}
		while(res == 1);

		cbuf[count] = '\0';
	}
	else
	{
		wbuf = (wchar_t *)data;

		do
		{
			res = sceIoRead(fd, &wh, 2);

			if(res == 2)
			{
				if(wh == L'\n')
				{
					flag = 1;
					break;
				}

				else if(wh == L'\r')
					continue;

				wbuf[count++] = wh;
			}
		}
		while(res == 2);

		wbuf[count] = L'\0';
	}

	if(count == 0 && flag == 1)
		count++;

	return count;
}

int SpooferLoadConfig(const char *path)
{
	SceUID fd;
	int read_size, len, res, i;
	wchar_t *data;
	u16 isunicode;

	// init
	data = NULL;
	read_size = 2;
	isunicode = 0;
	res = 0;

	// set default data
	SetDefault();

	// open
	fd = sceIoOpen(path, PSP_O_RDONLY, 0);

	if(fd < 0)
		return -1;

	// Check BOM
	len = sceIoRead(fd, &isunicode, sizeof(u16));

	if(len != sizeof(u16))
	{
		Log_printf("read_error_check_bom\n");
		res = -1;
		goto EXIT;
	}

	// UTF-8
	if(isunicode != 0xFEFF)
	{
		read_size = 1;
		sceIoLseek32(fd, 0, PSP_SEEK_SET);
	}

	data = malloc_p(BUFFER_SIZE);

	if(!data)
	{
		Log_printf("buffer_memory_allocate_error\n");
		res = -1;
		goto EXIT;
	}

	// Reading ...
	do
	{
		len = ReadLine(fd, data, read_size, 256);

		if(len != 0 && data[0] != L'#' && data[0] != L'\0')
		{
			if(cmp_wchar(data, L"[FormatString]") == 0)
			{
				len = ReadLine(fd, data, read_size, 256);

				if(len != 0 && data[0] != L'#' && data[0] != L'\0')
				{
					free_p(format_buf);

					format_buf = malloc_p((len + 1) * 2);

					if(format_buf != NULL)
						ConvertFormat(format_buf, data, spoofer_param);
				}
			}

			else if(cmp_wchar(data, L"[WeekNames]") == 0)
			{
				for(i = 0; i < 7; i++)
				{
					len = ReadLine(fd, data, read_size, 256);

					if(len != 0 && data[0] != L'#' && data[0] != L'\0')
					{
						free_p(week_buf[i]);

						week_buf[i] = malloc_p((len + 1) * 2);

						if(week_buf[i] != NULL)
							copy_wchar(week_buf[i], data);
					}
				}
			}

			else if(cmp_wchar(data, L"[MonthNames]") == 0)
			{
				for(i = 0; i < 12; i++)
				{
					len = ReadLine(fd, data, read_size, 256);

					if(len != 0 && data[0] != L'#' && data[0] != L'\0')
					{
						free_p(month_buf[i]);

						month_buf[i] = malloc_p((len + 1) * 2);

						if(month_buf[i] != NULL)
							copy_wchar(month_buf[i], data);
					}
				}
			}

			else if(cmp_wchar(data, L"[VersionSpoof]") == 0)
			{
				len = ReadLine(fd, data, read_size, 256);

				if(cmp_wchar(data, L"(null)") != 0)
				{
					if(len != 0 && data[0] != L'#' && data[0] != L'\0')
					{
						ver_buf = malloc_p((len + 1) * 2);

						if(ver_buf != NULL)
						{
							ver_flag = 1;
							ConvertFormat(ver_buf, data, ver_param);
						}
					}
				}
			}

			else if(cmp_wchar(data, L"[MacAddressSpoof]") == 0)
			{
				len = ReadLine(fd, data, read_size, 256);

				if(cmp_wchar(data, L"(null)") != 0)
				{
					if(len != 0 && data[0] != L'#' && data[0] != L'\0')
					{
						mac_buf = malloc_p((len + 1) * 2);

						if(mac_buf != NULL)
						{
							mac_flag = 1;
							ConvertFormat(mac_buf, data, mac_param);
						}
					}
				}
			}
		}
	}
	while(len != 0);

EXIT:

	free_p(data);
	sceIoClose(fd);

	return res;
}

