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
#include <pspkernel.h>
#include <stdarg.h>

// function
int itostring(char *buf, int value, int base, int code, int zero, int w_flag)
{
	char *ptr;
	int res, len, val, i, n;

	val = value;
	ptr = buf;

	if(base == 10 && value < 0)
	{
		*buf++ = '-';
		ptr = buf;
		val = !value;
	}

	do
	{
		res = val % base;
		val = val / base;

		if(res < 10)
		{
			res += '0';
		}
		else
		{
			res += 'a' - 10;

			if(code)
				res -= 0x20;
		}

		*ptr++ = (char)res;
	}
	while(val > 0);

	*ptr = '\0';
	len = ptr - buf;

	for(i = 0; i < len / 2; i++)
	{
		res = buf[i];
		buf[i] = buf[len-i-1];
		buf[len-i-1] = res;
	}

	res = len;

	for(i = 0; i < (zero - len); i++)
	{
		for(n = 0; n < (res + 1); n++)
		{
			buf[res-n+1] = buf[res-n];
		}

		buf[0] = '0';
		res++;
	}

	if(w_flag == 1)
	{
		for(i = 0; i < res; i++)
		{
			len = res - i;
			n = len << 1;

			buf[n] = buf[len];
			buf[n+1] = 0;
		}

		buf[1] = 0;
	}

	return res;
}

int vsnprintf(char *buf, int n, const char *format, va_list ap)
{
	char *str;
	u32 data, len, i, cnt, code, zero;
	char degit[32];

	if(!format || n <= 0)
	{
		buf[0] = '\0';
		return 0;
	}

	n -= 1;
	cnt = 0;

	while(*format != '\0')
	{
		code = 0;
		zero = 0;

		if(cnt >= n)
			goto VS_EXIT;

		if(*format == '%')
		{
			if(format[1] == '0')
			{
				if(format[2] >= '0' && format[2] <= '9')
				{
					zero = format[2] - '0';
					format += 2;
				}
			}

			switch(*++format)
			{
				case 'C':
					code = 1;
				case 'c':
					data = va_arg(ap, int);
					buf[cnt++] = (code) ? data : data - 0x20;
					format++;
					break;

				case 'D':
					code = 1;
				case 'd':
					data = va_arg(ap, u32);
					len = itostring(degit, data, 10, code, zero, 0);

					for(i = 0; i < len; i++)
					{
						if(cnt >= n)
							goto VS_EXIT;

						buf[cnt++] = degit[i];
					}

					format++;
					break;

				case 'P':
				case 'X':
					code = 1;
				case 'p':
				case 'x':
					data = va_arg(ap, u32);
					len = itostring(degit, data, 16, code, zero, 0);

					for(i = 0; i < len; i++)
					{
						if(cnt >= n)
							goto VS_EXIT;

						buf[cnt++] = degit[i];
					}

					format++;
					break;

				case 'S':
					code = 1;
				case 's':
					str = va_arg(ap, char *);

					if(!str)
						str = "(null)";

					for(; *str != '\0'; str++)
					{
						if(cnt >= n)
							goto VS_EXIT;

						if(code && *str >= 'a' && *str <= 'z')
						{
							buf[cnt++] = *str - 0x20;
						}
						else
						{
							buf[cnt++] = *str;
						}
					}

					format++;
					break;

				case '%':
					buf[cnt++] = '%';
					format++;
					break;

				default:
					buf[cnt++] = *format++;
					break;
			}
		}
		else
		{
			buf[cnt++] = *format++;
		}
	}

VS_EXIT:

	buf[cnt] = '\0';
	return cnt;
}

int snprintf(char *buf, int n, const char *format, ...)
{
	int len;
	va_list ap;

	va_start(ap, format);
	len = vsnprintf(buf, n, format, ap);
	va_end(ap);

	return len;
}

int vswprintf(wchar_t *buf, int n, const wchar_t *format, va_list ap)
{
	wchar_t *str;
	u32 data, len, i, cnt, code, zero;
	wchar_t degit[32];

	if(!format || n <= 0)
	{
		buf[0] = L'\0';
		return 0;
	}

	n -= 1;
	cnt = 0;

	while(*format != L'\0')
	{
		code = 0;
		zero = 0;

		if(cnt >= n)
			goto VW_EXIT;

		if(*format == L'%')
		{
			if(format[1] == L'0')
			{
				if(format[2] >= L'0' && format[2] <= L'9')
				{
					zero = format[2] - L'0';
					format += 2;
				}
			}

			switch(*++format)
			{
				case L'C':
					code = 1;
				case L'c':
					data = va_arg(ap, int);
					buf[cnt++] = (code) ? data : data - 0x20;
					format++;
					break;

				case L'D':
					code = 1;
				case L'd':
					data = va_arg(ap, u32);
					len = itostring((char *)degit, data, 10, code, zero, 1);

					for(i = 0; i < len; i++)
					{
						if(cnt >= n)
							goto VW_EXIT;

						buf[cnt++] = degit[i];
					}

					format++;
					break;

				case L'P':
				case L'X':
					code = 1;
				case L'p':
				case L'x':
					data = va_arg(ap, u32);
					len = itostring((char *)degit, data, 16, code, zero, 1);

					for(i = 0; i < len; i++)
					{
						if(cnt >= n)
							goto VW_EXIT;

						buf[cnt++] = degit[i];
					}

					format++;
					break;

				case L'S':
					code = 1;
				case L's':
					str = va_arg(ap, wchar_t *);

					if(!str)
						str = L"(null)";

					for(; *str != L'\0'; str++)
					{
						if(cnt >= n)
							goto VW_EXIT;

						if(code && *str >= L'a' && *str <= L'z')
						{
							buf[cnt++] = *str - 0x20;
						}
						else
						{
							buf[cnt++] = *str;
						}
					}

					format++;
					break;

				case L'%':
					buf[cnt++] = L'%';
					format++;
					break;

				default:
					buf[cnt++] = *format++;
					break;
			}
		}
		else
		{
			buf[cnt++] = *format++;
		}
	}

VW_EXIT:

	buf[cnt] = L'\0';
	return cnt;
}

int swprintf(wchar_t *buf, int n, const wchar_t *format, ...)
{
	int len;
	va_list ap;

	va_start(ap, format);
	len = vswprintf(buf, n, format, ap);
	va_end(ap);

	return len;
}

