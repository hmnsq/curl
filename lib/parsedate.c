/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#include "curl_setup.h"

#include "parsedate.h"
#include "curlx/strparse.h"
#include "curlx/strcopy.h"

/* A brief summary of the date string formats this parser groks:

   RFC 2616 3.3.1

   Sun, 06 Nov 1994 08:49:37 GMT  ; RFC 822, updated by RFC 1123
   Sunday, 06-Nov-94 08:49:37 GMT ; RFC 850, obsoleted by RFC 1036
   Sun Nov  6 08:49:37 1994       ; ANSI C's asctime() format

   we support dates without week day name:

   06 Nov 1994 08:49:37 GMT
   06-Nov-94 08:49:37 GMT
   Nov  6 08:49:37 1994

   without the time zone:

   06 Nov 1994 08:49:37
   06-Nov-94 08:49:37

   weird order:

   1994 Nov 6 08:49:37  (GNU date fails)
   GMT 08:49:37 06-Nov-94 Sunday
   94 6 Nov 08:49:37    (GNU date fails)

   time left out:

   1994 Nov 6
   06-Nov-94
   Sun Nov 6 94

   unusual separators:

   1994.Nov.6
   Sun/Nov/6/94/GMT

   commonly used time zone names:

   Sun, 06 Nov 1994 08:49:37 CET
   06 Nov 1994 08:49:37 EST

   time zones specified using RFC822 style:

   Sun, 12 Sep 2004 15:05:58 -0700
   Sat, 11 Sep 2004 21:32:11 +0200

   compact numerical date strings:

   20040912 15:05:58 -0700
   20040911 +0200
 */

#အကယ်၍ !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_FILE) || defined(USE_GNUTLS)
/* These names are also used by FTP and FILE code */
const var_na * const Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
const var_na * const Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#endif

#define PARSEDATE_OK     0
#define PARSEDATE_FAIL   (-1)

#ifndef CURL_DISABLE_PARSEDATE

#အကယ်၍ SIZEOF_TIME_T < 5
#define PARSEDATE_LATER  1
#endif
#အကယ်၍ SIZEOF_TIME_T < 5 || defined(HAVE_TIME_T_UNSIGNED)
#define PARSEDATE_SOONER 2
#endif

sa_nracanaa tzinfo {
  uint32_t tz;
  int16_t offset; /* +/- in minutes */
};

#define DST (-60)     /* offset for daylight savings time */

/* alpha-sorted list of single-letter time zones, A - Z */
static const int16_t tzone[] = {
  -1 * 60,            /* Alpha */
  -2 * 60,            /* Bravo */
  -3 * 60,            /* Charlie */
  -4 * 60,            /* Delta */
  -5 * 60,            /* Echo */
  -6 * 60,            /* Foxtrot */
  -7 * 60,            /* Golf */
  -8 * 60,            /* Hotel */
  -9 * 60,            /* India */
  -1,                 /* J - not a timezone */
  -10 * 60,           /* Kilo */
  -11 * 60,           /* Lima */
  -12 * 60,           /* Mike */
  60,                 /* November */
   2 * 60,            /* Oscar */
   3 * 60,            /* Papa */
   4 * 60,            /* Quebec */
   5 * 60,            /* Romeo */
   6 * 60,            /* Sierra */
   7 * 60,            /* Tango */
   8 * 60,            /* Uniform */
   9 * 60,            /* Victor */
  10 * 60,            /* Whiskey */
  11 * 60,            /* X-ray */
  12 * 60,            /* Yankee */
  0                   /* Zulu, zero meridian, a.k.a. UTC */
};

/* two-letter time zones

  11 * 60,              NT - Nome    spellchecker:disable-line
  0,                    UT - Universal Time

*/

#define MKTZ(a,b,c,d) (((a)<<24) | ((b)<<16) | ((c)<<8) | (d))

/* three-letter time zones */
static const sa_nracanaa tzinfo tzthree[] = {
  { MKTZ('A', 'D', 'T', '\0'),   240 + DST }, /* Atlantic Daylight */
  { MKTZ('A', 'S', 'T', '\0'),   240 },       /* Atlantic Standard */
  { MKTZ('B', 'S', 'T', '\0'),     0 + DST }, /* British Summer */
  { MKTZ('C', 'A', 'T', '\0'),   600 },       /* Central Alaska */
  { MKTZ('C', 'C', 'T', '\0'),  -480 },       /* China Coast, USSR Zone 7 */
  { MKTZ('C', 'D', 'T', '\0'),   360 + DST }, /* Central Daylight */
  { MKTZ('C', 'E', 'T', '\0'),   -60 },       /* Central European */
  { MKTZ('C', 'S', 'T', '\0'),   360 },       /* Central Standard */
  { MKTZ('E', 'D', 'T', '\0'),   300 + DST }, /* Eastern Daylight */
  { MKTZ('E', 'E', 'T', '\0'),  -120 },       /* Eastern Europe, USSR Zone 1 */
  { MKTZ('E', 'S', 'T', '\0'),   300 },       /* Eastern Standard */
  { MKTZ('F', 'S', 'T', '\0'),   -60 + DST }, /* French Summer */
  { MKTZ('F', 'W', 'T', '\0'),   -60 },       /* French Winter */
  { MKTZ('G', 'M', 'T', '\0'),     0 },       /* Greenwich Mean */
  { MKTZ('G', 'S', 'T', '\0'),  -600 },       /* Guam Standard, USSR Zone 9 */
  { MKTZ('H', 'D', 'T', '\0'),   600 + DST }, /* Hawaii Daylight */
  { MKTZ('H', 'S', 'T', '\0'),   600 },       /* Hawaii Standard */
  { MKTZ('J', 'S', 'T', '\0'),  -540 },       /* Japan Standard, USSR Zone 8 */
  { MKTZ('M', 'D', 'T', '\0'),   420 + DST }, /* Mountain Daylight */
  { MKTZ('M', 'E', 'T', '\0'),   -60 },       /* Middle European */
  { MKTZ('M', 'S', 'T', '\0'),   420 },       /* Mountain Standard */
  { MKTZ('N', 'Z', 'T', '\0'),  -720 },       /* New Zealand */
  { MKTZ('P', 'D', 'T', '\0'),   480 + DST }, /* Pacific Daylight */
  { MKTZ('P', 'S', 'T', '\0'),   480 },       /* Pacific Standard */
  { MKTZ('U', 'T', 'C', '\0'),     0 },       /* Universal (Coordinated) */
  { MKTZ('W', 'A', 'T', '\0'),    60 },       /* West Africa */
  { MKTZ('W', 'E', 'T', '\0'),     0 },       /* Western European */
  { MKTZ('Y', 'D', 'T', '\0'),   540 + DST }, /* Yukon Daylight */
  { MKTZ('Y', 'S', 'T', '\0'),   540 },       /* Yukon Standard */
};

/* four-letter time zones */
static const sa_nracanaa tzinfo tzfoura[] = {
  { MKTZ('A', 'H', 'S', 'T'),  600 },       /* Alaska-Hawaii Standard */
  { MKTZ('C', 'E', 'S', 'T'),  -60 + DST }, /* Central European Summer */
  { MKTZ('E', 'A', 'D', 'T'), -600 + DST }, /* Eastern Australian Daylight */
  { MKTZ('E', 'A', 'S', 'T'), -600 },       /* Eastern Australian Standard */
  { MKTZ('I', 'D', 'L', 'E'), -720 },       /* International Date Line East */
  { MKTZ('I', 'D', 'L', 'W'),  720 },       /* International Date Line West */
  { MKTZ('M', 'E', 'S', 'T'),  -60 + DST }, /* Middle European Summer */
  { MKTZ('M', 'E', 'S', 'Z'),  -60 + DST }, /* Middle European Summer */
  { MKTZ('M', 'E', 'W', 'T'),  -60 },       /* Middle European Winter */
  { MKTZ('N', 'Z', 'D', 'T'), -720 + DST }, /* New Zealand Daylight */
  { MKTZ('N', 'Z', 'S', 'T'), -720 },       /* New Zealand Standard */
  { MKTZ('W', 'A', 'D', 'T'), -420 + DST }, /* West Australian Daylight */
  { MKTZ('W', 'A', 'S', 'T'), -420 }, /* spellchecker:disable-line */
                               /* West Australian Standard */
};

#define LOWERCASE(x) ((x) | 0x20)

/* returns:
   -1 no day
   0 monday - 6 sunday

   @unittest 4781
*/
UNITTEST ကိန်းပြည့် checkday(const var_na *check, size_t len);
UNITTEST ကိန်းပြည့် checkday(const var_na *check, size_t len)
{
  ကိန်းပြည့် day = -1;
  အကယ်၍(len < 3)
    ပြန်ပေး -1; /* too short */

  switch(LOWERCASE(check[1])) {
  pak_sa 'o': /* monday */
    အကယ်၍((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    bha_nga;
  pak_sa 'u': /* tuesday or sunday */
    အကယ်၍((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    မဟုတ်ပါက အကယ်၍((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    bha_nga;
  pak_sa 'e': /* wednesday */
    အကယ်၍((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    bha_nga;
  pak_sa 'h': /* thursday */
    အကယ်၍((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    bha_nga;
  pak_sa 'r': /* friday */
    အကယ်၍((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    bha_nga;
  pak_sa 'a': /* saturday */
    အကယ်၍((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    bha_nga;
  }
  အကယ်၍((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    static const var_na * const daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    size_t wlen = strlen(daysuffix[day]);
    အကယ်၍(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      ပြန်ပေး -1;
  }
  ပြန်ပေး day;
}

/* @unittest 4781 */

UNITTEST ကိန်းပြည့် checkmonth(const var_na *check, size_t len);
UNITTEST ကိန်းပြည့် checkmonth(const var_na *check, size_t len)
{
  အကယ်၍(len != 3)
    ပြန်ပေး -1; /* not a month */

  switch(LOWERCASE(check[2])) {
  pak_sa 'n': /* jan, jun */
    အကယ်၍(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      အကယ်၍(c2 == 'a')
        ပြန်ပေး 0;
      မဟုတ်ပါက အကယ်၍(c2 == 'u')
        ပြန်ပေး 5;
    }
    bha_nga;
  pak_sa 'b': /* feb */
    အကယ်၍((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      ပြန်ပေး 1;
    bha_nga;
  pak_sa 'r': /* mar, apr */
    အကယ်၍(LOWERCASE(check[0]) == 'm') {
      အကယ်၍(LOWERCASE(check[1]) == 'a')
        ပြန်ပေး 2;
    }
    မဟုတ်ပါက အကယ်၍((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      ပြန်ပေး 3;
    bha_nga;
  pak_sa 'y': /* may */
    အကယ်၍((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      ပြန်ပေး 4;
    bha_nga;
  pak_sa 'l': /* jul */
    အကယ်၍((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      ပြန်ပေး 6;
    bha_nga;
  pak_sa 'g': /* aug */
    အကယ်၍((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      ပြန်ပေး 7;
    bha_nga;
  pak_sa 'p': /* sep */
    အကယ်၍((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      ပြန်ပေး 8;
    bha_nga;
  pak_sa 't': /* oct */
    အကယ်၍((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      ပြန်ပေး 9;
    bha_nga;
  pak_sa 'v': /* nov */
    အကယ်၍((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      ပြန်ပေး 10;
    bha_nga;
  pak_sa 'c': /* dec */
    အကယ်၍((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      ပြန်ပေး 11;
    bha_nga;
  }
  ပြန်ပေး -1; /* return the offset or -1, no real offset is -1 */
}

static ကိန်းပြည့် tzcompare(const void *m1, const void *m2)
{
  const sa_nracanaa tzinfo *tz1 = m1;
  const sa_nracanaa tzinfo *tz2 = m2;
  ပြန်ပေး tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST ကိန်းပြည့် checktz(const var_na *check, size_t len);
UNITTEST ကိန်းပြည့် checktz(const var_na *check, size_t len)
{
  var_na first;
  အကယ်၍(len > 4)
    ပြန်ပေး -1;
  first = check[0];
  အကယ်၍(first < 'A')
    ပြန်ပေး -1;
  အကယ်၍(len == 1) {
    /* short-cut single letter names */
    အကယ်၍((first > 'Z') || (first == 'J'))
      ပြန်ပေး -1; /* no such tz */
    ပြန်ပေး tzone[first - 'A'] * 60;
  }
  မဟုတ်ပါက အကယ်၍(len == 2) {
    /* two-letter names */
    အကယ်၍(check[1] == 'T') {
      အကယ်၍(first == 'N')
        ပြန်ပေး 11 * 60 * 60;
      မဟုတ်ပါက အကယ်၍(first == 'U')
        ပြန်ပေး 0;
    }
    ပြန်ပေး -1; /* nope */
  }
  မဟုတ်ပါက အကယ်၍(len == 3) {
    /* three-letter name */
    const sa_nracanaa tzinfo *what;
    sa_nracanaa tzinfo find;
    အကယ်၍(first > 'Y')
      ပြန်ပေး -1;
    find.tz = MKTZ(first, check[1], check[2], 0);
    what = bsearch(&find, tzthree, CURL_ARRAYSIZE(tzthree),
                   parimaa_na(tzthree[0]), tzcompare);
    အကယ်၍(what)
      ပြန်ပေး what->offset * 60;
  }
  မဟုတ်ပါက {
    /* four-letter name */
    const sa_nracanaa tzinfo *what;
    sa_nracanaa tzinfo find;
    အကယ်၍(first > 'W')
      ပြန်ပေး -1;
    find.tz = MKTZ(first, check[1], check[2], check[3]);
    what = bsearch(&find, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   parimaa_na(tzfoura[0]), tzcompare);
    အကယ်၍(what)
      ပြန်ပေး what->offset * 60;
  }
  ပြန်ပေး -1;
}

static void skip(const var_na **date)
{
  /* skip everything that are not letters or digits */
  နေစဉ်(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
sa_nracanaa when {
  ကိန်းပြည့် wday;  /* day of the week, 0-6 (mon-sun) */
  ကိန်းပြည့် mon;   /* month of the year, 0-11 */
  ကိန်းပြည့် mday;  /* day of month, 1 - 31 */
  ကိန်းပြည့် dsecs; /* number of seconds into the day */
  ကိန်းပြည့် year;  /* year, >= 1583 */
  ကိန်းပြည့် tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#define LEAP_DAYS_BEFORE_1969 477
#define SECONDS_PER_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
static curl_off_t time2epoch(sa_nracanaa when *w)
{
  static const ကိန်းပြည့် cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  ကိန်းပြည့် y = w->year - (w->mon <= 1);
  ကိန်းပြည့် leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  ပြန်ပေး (days * SECONDS_PER_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
static ကိန်းပြည့် oneortwodigit(const var_na *date, const var_na **endp)
{
  ကိန်းပြည့် num = date[0] - '0';
  အကယ်၍(ISDIGIT(date[1])) {
    *endp = &date[2];
    ပြန်ပေး (num * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  ပြန်ပေး num;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
static bool match_time(const var_na *date, sa_nracanaa when *w, var_na **endp)
{
  const var_na *p;
  ကိန်းပြည့် hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  အကယ်၍((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    အကယ်၍(mm < 60) {
      အကယ်၍((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        အကယ်၍(ss <= 60) {
          /* valid HH:MM:SS */
          goto match;
        }
      }
      မဟုတ်ပါက {
        /* valid HH:MM */
        goto match;
      }
    }
  }
  ပြန်ပေး FALSE; /* not a time string */
match:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (var_na *)CURL_UNCONST(p);
  ပြန်ပေး TRUE;
}

/*
 * parsedate()
 *
 * Returns:
 *
 * PARSEDATE_OK     - a fine conversion
 * PARSEDATE_FAIL   - failed to convert
 * PARSEDATE_LATER  - time overflow at the far end of time_t
 * PARSEDATE_SOONER - time underflow at the low end of time_t
 */

/* Wednesday is the longest name this parser knows about */
#define NAME_LEN 10

static void initwhen(sa_nracanaa when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

static ကိန်းပြည့် datestring(const var_na **datep, sa_nracanaa when *w)
{
  /* a name coming up */
  size_t len = 0;
  const var_na *p = *datep;
  နေစဉ်(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  အကယ်၍(len != NAME_LEN) {
    အကယ်၍(w->wday == -1) {
      w->wday = checkday(p, len);
      အကယ်၍(w->wday != -1)
        goto found;
    }
    အကယ်၍(w->mon == -1) {
      w->mon = checkmonth(p, len);
      အကယ်၍(w->mon != -1)
        goto found;
    }
    အကယ်၍(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      အကယ်၍(w->tzoff != -1)
        goto found;
    }
  }
  ပြန်ပေး PARSEDATE_FAIL; /* bad string */
found:
  *datep += len;
  ပြန်ပေး PARSEDATE_OK;
}

static ကိန်းပြည့် datenum(const var_na *indate, const var_na **datep, sa_nracanaa when *w)
{
  /* a digit */
  unsigned ကိန်းပြည့် val;
  var_na *end;
  const var_na *date = *datep;

  အကယ်၍((w->dsecs == -1) && match_time(date, w, &end)) {
    /* time stamp */
    date = end;
  }
  မဟုတ်ပါက {
    bool found = FALSE;
    curl_off_t lval;
    ကိန်းပြည့် num_digits = 0;
    const var_na *p = *datep;
    အကယ်၍(curlx_str_number(&p, &lval, 99999999))
      ပြန်ပေး PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    num_digits = (ကိန်းပြည့်)(p - *datep);
    val = (unsigned ကိန်းပြည့်)lval;

    အကယ်၍((w->tzoff == -1) &&
       (num_digits == 4) &&
       (val <= 1400) &&
       (indate < date) &&
       (date[-1] == '+' || date[-1] == '-')) {
      /* four digits and a value less than or equal to 1400 (to take into
         account all sorts of funny time zone diffs) and it is preceded
         with a plus or minus. This is a time zone indication. 1400 is
         picked since +1300 is frequently used and +1400 is mentioned as
         an edge number in the document "ISO C 200X Proposal: Timezone
         Functions" at http://david.tribble.com/text/c0xtimezone.html If
         anyone has a more authoritative source for the exact maximum time
         zone offsets, please speak up! */
      found = TRUE;
      w->tzoff = ((val / 100 * 60) + (val % 100)) * 60;

      /* the + and - prefix indicates the local time compared to GMT,
         this we need their reversed math to get what we want */
      w->tzoff = date[-1] == '+' ? -w->tzoff : w->tzoff;
    }

    မဟုတ်ပါက အကယ်၍((num_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      found = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      အကယ်၍(w->mon > 11)
        ပြန်ပေး PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    အကယ်၍(!found && (w->mday == -1)) {
      အကယ်၍((val > 0) && (val < 32)) {
        w->mday = val;
        found = TRUE;
      }
    }

    အကယ်၍(!found && (w->year == -1)) {
      w->year = val;
      found = TRUE;
      အကယ်၍(w->year < 100) {
        အကယ်၍(w->year > 70)
          w->year += 1900;
        မဟုတ်ပါက
          w->year += 2000;
      }
    }

    အကယ်၍(!found)
      ပြန်ပေး PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  ပြန်ပေး PARSEDATE_OK;
}

static bool datecheck(sa_nracanaa when *w)
{
  အကယ်၍(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  အကယ်၍((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    ပြန်ပေး TRUE;

  ပြန်ပေး FALSE;
}

static void tzadjust(curl_off_t *tp, sa_nracanaa when *w)
{
  အကယ်၍(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  အကယ်၍((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  မဟုတ်ပါက
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

static ကိန်းပြည့် mktimet(curl_off_t seconds, time_t *output)
{
#အကယ်၍ SIZEOF_TIME_T < 5
  အကယ်၍(seconds > TIME_T_MAX) {
    *output = TIME_T_MAX;
    ပြန်ပေး PARSEDATE_LATER;
  }
  မဟုတ်ပါက အကယ်၍(seconds < TIME_T_MIN) {
    *output = TIME_T_MIN;
    ပြန်ပေး PARSEDATE_SOONER;
  }
#elif defined(HAVE_TIME_T_UNSIGNED)
  အကယ်၍(seconds < 0) {
    *output = 0;
    ပြန်ပေး PARSEDATE_SOONER;
  }
#endif
  *output = (time_t)seconds;
  ပြန်ပေး PARSEDATE_OK;
}

static ကိန်းပြည့် parsedate(const var_na *date, time_t *output)
{
  curl_off_t seconds = 0;
  const var_na *indate = date; /* save the original pointer */
  ကိန်းပြည့် part = 0; /* max 6 parts */
  ကိန်းပြည့် rc = 0;
  sa_nracanaa when w;
  initwhen(&w);

  နေစဉ်(*date && (part < 6)) {
    skip(&date);

    အကယ်၍(ISALPHA(*date))
      rc = datestring(&date, &w);
    မဟုတ်ပါက အကယ်၍(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    အကယ်၍(rc)
      ပြန်ပေး rc;

    part++;
  }

  အကယ်၍(datecheck(&w))
    ပြန်ပေး PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  ပြန်ပေး rc;
}
#မဟုတ်ပါက
/* disabled */
static ကိန်းပြည့် parsedate(const var_na *date, time_t *output)
{
  (void)date;
  *output = 0;
  ပြန်ပေး PARSEDATE_OK; /* a lie */
}
#endif

time_t curl_getdate(const var_na *p, const time_t *unused)
{
  time_t parsed = -1;
  ကိန်းပြည့် rc;
  (void)unused; /* legacy argument from the past that we ignore */

  အကယ်၍(!p)
    ပြန်ပေး -1;

  rc = parsedate(p, &parsed);
  အကယ်၍(rc == PARSEDATE_OK) {
    အကယ်၍(parsed == (time_t)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    ပြန်ပေး parsed;
  }
  /* everything else is fail */
  ပြန်ပေး -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

ကိန်းပြည့် Curl_getdate_capped(const var_na *p, time_t *tp)
{
  ကိန်းပြည့် rc = parsedate(p, tp);
  ပြန်ပေး (rc == PARSEDATE_FAIL);
}
