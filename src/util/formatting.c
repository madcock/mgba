/* Copyright (c) 2013-2015 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include <mgba-util/formatting.h>

#include <float.h>

int ftostr_l(char* restrict str, size_t size, float f, locale_t locale) {
#ifdef HAVE_SNPRINTF_L
	return snprintf_l(str, size, locale, "%.*g", FLT_DIG, f);
#elif defined(HAVE_LOCALE)
	locale_t old = uselocale(locale);
	int res = snprintf(str, size, "%.*g", FLT_DIG, f);
	uselocale(old);
	return res;
#elif defined(HAVE_SETLOCALE)
	char* old = setlocale(LC_NUMERIC, locale);
	int res = snprintf(str, size, "%.*g", FLT_DIG, f);
	setlocale(LC_NUMERIC, old);
	return res;
#else
	UNUSED(locale);
	return snprintf(str, size, "%.*g", FLT_DIG, f);
#endif
}

#ifndef HAVE_STRTOF_L
float strtof_l(const char* restrict str, char** restrict end, locale_t locale) {
#ifdef HAVE_LOCALE
	locale_t old = uselocale(locale);
	float res = strtof(str, end);
	uselocale(old);
	return res;
#elif defined(HAVE_SETLOCALE)
	char* old = setlocale(LC_NUMERIC, locale);
	float res = strtof(str, end);
	setlocale(LC_NUMERIC, old);
	return res;
#else
	UNUSED(locale);
	return strtof(str, end);
#endif
}
#endif

int ftostr_u(char* restrict str, size_t size, float f) {
#if HAVE_LOCALE
	locale_t l = newlocale(LC_NUMERIC_MASK, "C", 0);
#else
	locale_t l = "C";
#endif
	int res = ftostr_l(str, size, f, l);
#if HAVE_LOCALE
	freelocale(l);
#endif
	return res;
}

float strtof_u(const char* restrict str, char** restrict end) {
#if HAVE_LOCALE
	locale_t l = newlocale(LC_NUMERIC_MASK, "C", 0);
#else
	locale_t l = "C";
#endif
	float res = strtof_l(str, end, l);
#if HAVE_LOCALE
	freelocale(l);
#endif
	return res;
}

#ifndef HAVE_LOCALTIME_R
struct tm* localtime_r(const time_t* t, struct tm* date) {
#ifdef _WIN32
	localtime_s(date, t);
	return date;
#elif defined(PSP2)
	return sceKernelLibcLocaltime_r(t, date);
#elif defined(__CELLOS_LV2__)
	*date = *localtime(t);
	return date;
#else
#warning localtime_r not emulated on this platform
	return 0;
#endif
}
#endif

#ifdef __CELLOS_LV2__
#include <sys/sys_time.h>
int gettimeofday(struct timeval* tv, void* unused) {
	int64_t time = sys_time_get_system_time();

	tv->tv_sec = time / 1000000;
	tv->tv_usec = time - (tv->tv_sec * 1000000); // implicit rounding will take care of this for us
	return 0;
}
#endif

