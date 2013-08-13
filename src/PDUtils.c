/**
 * This code is derived from PDPCLIB, the public domain C runtime
 * library by Paul Edwards. http://pdos.sourceforge.net/
 * 
 * This code is released to the public domain.
 */

#include "pebble_os.h"

/**
 * Scalar date routines -- public domain by Ray Gardner
 * These will work over the range 1-01-01 thru 14699-12-31
 * The functions written by Ray are isleap, months_to_days,
 * years_to_days, ymd_to_scalar, scalar_to_ymd.
 * modified slightly by Paul Edwards
 */

static int isleap (unsigned yr) {
	return yr % 400 == 0 || (yr % 4 == 0 && yr % 100 != 0);
}

static unsigned months_to_days (unsigned month) {
	return (month * 3057 - 3007) / 100;
}

static unsigned years_to_days (unsigned yr) {
	return yr * 365L + yr / 4 - yr / 100 + yr / 400;
}

static long ymd_to_scalar (unsigned yr, unsigned mo, unsigned day) {
	long scalar;

	scalar = day + months_to_days(mo);
	if(mo > 2) /* adjust if past February */
		scalar -= isleap(yr) ? 1 : 2;
	yr--;
	scalar += years_to_days(yr);
	return scalar;
}

static void scalar_to_ymd (long scalar,
						unsigned *pyr,
						unsigned *pmo,
						unsigned *pday) {
	unsigned n; // compute inverse of years_to_days()

	n = (unsigned)((scalar * 400L) / 146097L);
	
	while((long)years_to_days(n) < scalar) {
		n++;
	}
	
	for(n = (unsigned)((scalar * 400L) / 146097L); (long)years_to_days(n) < scalar;)
		n++; // 146097 == years_to_days(400)

	*pyr = n;

	n = (unsigned)(scalar - years_to_days(n-1));

	if(n > 59) { // adjust if past February
		n += 2;

		if(isleap(*pyr))
			n -= n > 61 ? 1 : 2;
	}
	
	*pmo = (n * 100 + 3007) / 3057; // inverse of months_to_days()
	*pday = n - months_to_days(*pmo);
	
	return;
}

time_t pmktime (PblTm *timeptr) {
	time_t tt;

	if((timeptr->tm_year < 70) || (timeptr->tm_year > 120)) {
		tt = (time_t)-1;
	} else {
		tt = ymd_to_scalar(timeptr->tm_year + 1900,
					timeptr->tm_mon + 1,
					timeptr->tm_mday)
			- ymd_to_scalar(1970, 1, 1);
		tt = tt * 24 + timeptr->tm_hour;
		tt = tt * 60 + timeptr->tm_min;
		tt = tt * 60 + timeptr->tm_sec;
	}
	
	return tt;
}

/**
 * dow - written by Paul Edwards, 1993-01-31
 * Released to the Public Domain
 * This routine will work over the range 1-01-01 to 32767-12-31.
 * It assumes the current calendar system has always been in
 * place in that time.	If you pass 0 or negative years, then
 * it produces results on the assumption that there is a year
 * 0.	It should always produce a value in the range of 0..6
 * if a valid month and day have been passed, no matter what
 * the year is.	However, it has not been tested for negative
 * years, because the results are meaningless anyway.	It is
 * mainly to stop people playing silly buggers and causing
 * the macro to crash on negative years.
 */

#define dow(y,m,d) \
	((((((m)+9)%12+1)<<4)%27 + (d) + 1 + \
	((y)%400+400) + ((y)%400+400)/4 - ((y)%400+400)/100 + \
	(((m)<=2) ? ( \
	(((((y)%4)==0) && (((y)%100)!=0)) || (((y)%400)==0)) \
	? 5 : 6) : 0)) % 7)

/**
 * Note: Pebble doesn't think about timezones. So we pass a time_t to
 * gmtime that's already been offset for the desired TZ, and get a PblTm
 * object back in that TZ. I don't know how seedy actual developers will
 * think that is.
 */
static PblTm tms;

PblTm *pgmtime (const time_t *timer) {
	unsigned yr, mo, da;
	unsigned long secs, days;

	days = *timer / (60L*60*24);
	secs = *timer % (60L*60*24);
	scalar_to_ymd(days + ymd_to_scalar(1970, 1, 1), &yr, &mo, &da);
	tms.tm_year = yr - 1900;
	tms.tm_mon = mo - 1;
	tms.tm_mday = da;
	tms.tm_yday = (int)(ymd_to_scalar(tms.tm_year + 1900, mo, da)
					- ymd_to_scalar(tms.tm_year + 1900, 1, 1));
	tms.tm_wday = dow(tms.tm_year + 1900, mo, da);
	tms.tm_isdst = -1;
	tms.tm_sec = (int)(secs % 60);
	secs /= 60;
	tms.tm_min = (int)(secs % 60);
	secs /= 60;
	tms.tm_hour = (int)secs;
	return &tms;
}

char *pstrtok(char *s1, const char *s2) {
	static char *old = NULL;
	char *p;
	size_t len;
	size_t remain;

	if(s1 != NULL) old = s1;
	if(old == NULL) return (NULL);
	
	p = old;
	len = strspn(p, s2);
	remain = strlen(p);
	
	if(remain <= len) { old = NULL; return (NULL); }
	
	p += len;
	len = strcspn(p, s2);
	remain = strlen(p);
	
	if(remain <= len) { old = NULL; return (p); }
	
	*(p + len) = '\0';
	old = p + len + 1;
	
	return(p);
}