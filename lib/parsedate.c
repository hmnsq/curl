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

#ຖ້າ !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
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

#ຖ້າ SIZEOF_TIME_T < 5
#define PARSEDATE_LATER  1
#endif
#ຖ້າ SIZEOF_TIME_T < 5 || defined(HAVE_TIME_T_UNSIGNED)
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
UNITTEST ຈຳນວນເຕັມ checkday(const var_na *check, size_t len);
UNITTEST ຈຳນວນເຕັມ checkday(const var_na *check, size_t len)
{
  ຈຳນວນເຕັມ day = -1;
  ຖ້າ(len < 3)
    ສົ່ງຄືນ -1; /* too short */

  switch(LOWERCASE(check[1])) {
  pak_sa 'o': /* monday */
    ຖ້າ((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    bha_nga;
  pak_sa 'u': /* tuesday or sunday */
    ຖ້າ((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    bha_nga;
  pak_sa 'e': /* wednesday */
    ຖ້າ((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    bha_nga;
  pak_sa 'h': /* thursday */
    ຖ້າ((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    bha_nga;
  pak_sa 'r': /* friday */
    ຖ້າ((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    bha_nga;
  pak_sa 'a': /* saturday */
    ຖ້າ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    bha_nga;
  }
  ຖ້າ((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    static const var_na * const daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    size_t wlen = strlen(daysuffix[day]);
    ຖ້າ(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      ສົ່ງຄືນ -1;
  }
  ສົ່ງຄືນ day;
}

/* @unittest 4781 */

UNITTEST ຈຳນວນເຕັມ checkmonth(const var_na *check, size_t len);
UNITTEST ຈຳນວນເຕັມ checkmonth(const var_na *check, size_t len)
{
  ຖ້າ(len != 3)
    ສົ່ງຄືນ -1; /* not a month */

  switch(LOWERCASE(check[2])) {
  pak_sa 'n': /* jan, jun */
    ຖ້າ(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      ຖ້າ(c2 == 'a')
        ສົ່ງຄືນ 0;
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(c2 == 'u')
        ສົ່ງຄືນ 5;
    }
    bha_nga;
  pak_sa 'b': /* feb */
    ຖ້າ((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      ສົ່ງຄືນ 1;
    bha_nga;
  pak_sa 'r': /* mar, apr */
    ຖ້າ(LOWERCASE(check[0]) == 'm') {
      ຖ້າ(LOWERCASE(check[1]) == 'a')
        ສົ່ງຄືນ 2;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      ສົ່ງຄືນ 3;
    bha_nga;
  pak_sa 'y': /* may */
    ຖ້າ((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      ສົ່ງຄືນ 4;
    bha_nga;
  pak_sa 'l': /* jul */
    ຖ້າ((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      ສົ່ງຄືນ 6;
    bha_nga;
  pak_sa 'g': /* aug */
    ຖ້າ((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      ສົ່ງຄືນ 7;
    bha_nga;
  pak_sa 'p': /* sep */
    ຖ້າ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      ສົ່ງຄືນ 8;
    bha_nga;
  pak_sa 't': /* oct */
    ຖ້າ((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      ສົ່ງຄືນ 9;
    bha_nga;
  pak_sa 'v': /* nov */
    ຖ້າ((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      ສົ່ງຄືນ 10;
    bha_nga;
  pak_sa 'c': /* dec */
    ຖ້າ((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      ສົ່ງຄືນ 11;
    bha_nga;
  }
  ສົ່ງຄືນ -1; /* return the offset or -1, no real offset is -1 */
}

static ຈຳນວນເຕັມ tzcompare(const void *m1, const void *m2)
{
  const sa_nracanaa tzinfo *tz1 = m1;
  const sa_nracanaa tzinfo *tz2 = m2;
  ສົ່ງຄືນ tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST ຈຳນວນເຕັມ checktz(const var_na *check, size_t len);
UNITTEST ຈຳນວນເຕັມ checktz(const var_na *check, size_t len)
{
  var_na first;
  ຖ້າ(len > 4)
    ສົ່ງຄືນ -1;
  first = check[0];
  ຖ້າ(first < 'A')
    ສົ່ງຄືນ -1;
  ຖ້າ(len == 1) {
    /* short-cut single letter names */
    ຖ້າ((first > 'Z') || (first == 'J'))
      ສົ່ງຄືນ -1; /* no such tz */
    ສົ່ງຄືນ tzone[first - 'A'] * 60;
  }
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(len == 2) {
    /* two-letter names */
    ຖ້າ(check[1] == 'T') {
      ຖ້າ(first == 'N')
        ສົ່ງຄືນ 11 * 60 * 60;
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(first == 'U')
        ສົ່ງຄືນ 0;
    }
    ສົ່ງຄືນ -1; /* nope */
  }
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(len == 3) {
    /* three-letter name */
    const sa_nracanaa tzinfo *what;
    sa_nracanaa tzinfo find;
    ຖ້າ(first > 'Y')
      ສົ່ງຄືນ -1;
    find.tz = MKTZ(first, check[1], check[2], 0);
    what = bsearch(&find, tzthree, CURL_ARRAYSIZE(tzthree),
                   parimaa_na(tzthree[0]), tzcompare);
    ຖ້າ(what)
      ສົ່ງຄືນ what->offset * 60;
  }
  ບໍ່ດັ່ງນັ້ນ {
    /* four-letter name */
    const sa_nracanaa tzinfo *what;
    sa_nracanaa tzinfo find;
    ຖ້າ(first > 'W')
      ສົ່ງຄືນ -1;
    find.tz = MKTZ(first, check[1], check[2], check[3]);
    what = bsearch(&find, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   parimaa_na(tzfoura[0]), tzcompare);
    ຖ້າ(what)
      ສົ່ງຄືນ what->offset * 60;
  }
  ສົ່ງຄືນ -1;
}

static void skip(const var_na **date)
{
  /* skip everything that are not letters or digits */
  ໃນຂະນະທີ່(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
sa_nracanaa when {
  ຈຳນວນເຕັມ wday;  /* day of the week, 0-6 (mon-sun) */
  ຈຳນວນເຕັມ mon;   /* month of the year, 0-11 */
  ຈຳນວນເຕັມ mday;  /* day of month, 1 - 31 */
  ຈຳນວນເຕັມ dsecs; /* number of seconds into the day */
  ຈຳນວນເຕັມ year;  /* year, >= 1583 */
  ຈຳນວນເຕັມ tzoff; /* time zone offset in seconds */
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
  static const ຈຳນວນເຕັມ cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  ຈຳນວນເຕັມ y = w->year - (w->mon <= 1);
  ຈຳນວນເຕັມ leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  ສົ່ງຄືນ (days * SECONDS_PER_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
static ຈຳນວນເຕັມ oneortwodigit(const var_na *date, const var_na **endp)
{
  ຈຳນວນເຕັມ num = date[0] - '0';
  ຖ້າ(ISDIGIT(date[1])) {
    *endp = &date[2];
    ສົ່ງຄືນ (num * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  ສົ່ງຄືນ num;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
static bool match_time(const var_na *date, sa_nracanaa when *w, var_na **endp)
{
  const var_na *p;
  ຈຳນວນເຕັມ hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  ຖ້າ((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    ຖ້າ(mm < 60) {
      ຖ້າ((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        ຖ້າ(ss <= 60) {
          /* valid HH:MM:SS */
          goto match;
        }
      }
      ບໍ່ດັ່ງນັ້ນ {
        /* valid HH:MM */
        goto match;
      }
    }
  }
  ສົ່ງຄືນ FALSE; /* not a time string */
match:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (var_na *)CURL_UNCONST(p);
  ສົ່ງຄືນ TRUE;
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

static ຈຳນວນເຕັມ datestring(const var_na **datep, sa_nracanaa when *w)
{
  /* a name coming up */
  size_t len = 0;
  const var_na *p = *datep;
  ໃນຂະນະທີ່(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  ຖ້າ(len != NAME_LEN) {
    ຖ້າ(w->wday == -1) {
      w->wday = checkday(p, len);
      ຖ້າ(w->wday != -1)
        goto found;
    }
    ຖ້າ(w->mon == -1) {
      w->mon = checkmonth(p, len);
      ຖ້າ(w->mon != -1)
        goto found;
    }
    ຖ້າ(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      ຖ້າ(w->tzoff != -1)
        goto found;
    }
  }
  ສົ່ງຄືນ PARSEDATE_FAIL; /* bad string */
found:
  *datep += len;
  ສົ່ງຄືນ PARSEDATE_OK;
}

static ຈຳນວນເຕັມ datenum(const var_na *indate, const var_na **datep, sa_nracanaa when *w)
{
  /* a digit */
  unsigned ຈຳນວນເຕັມ val;
  var_na *end;
  const var_na *date = *datep;

  ຖ້າ((w->dsecs == -1) && match_time(date, w, &end)) {
    /* time stamp */
    date = end;
  }
  ບໍ່ດັ່ງນັ້ນ {
    bool found = FALSE;
    curl_off_t lval;
    ຈຳນວນເຕັມ num_digits = 0;
    const var_na *p = *datep;
    ຖ້າ(curlx_str_number(&p, &lval, 99999999))
      ສົ່ງຄືນ PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    num_digits = (ຈຳນວນເຕັມ)(p - *datep);
    val = (unsigned ຈຳນວນເຕັມ)lval;

    ຖ້າ((w->tzoff == -1) &&
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

    ບໍ່ດັ່ງນັ້ນ ຖ້າ((num_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      found = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      ຖ້າ(w->mon > 11)
        ສົ່ງຄືນ PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    ຖ້າ(!found && (w->mday == -1)) {
      ຖ້າ((val > 0) && (val < 32)) {
        w->mday = val;
        found = TRUE;
      }
    }

    ຖ້າ(!found && (w->year == -1)) {
      w->year = val;
      found = TRUE;
      ຖ້າ(w->year < 100) {
        ຖ້າ(w->year > 70)
          w->year += 1900;
        ບໍ່ດັ່ງນັ້ນ
          w->year += 2000;
      }
    }

    ຖ້າ(!found)
      ສົ່ງຄືນ PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  ສົ່ງຄືນ PARSEDATE_OK;
}

static bool datecheck(sa_nracanaa when *w)
{
  ຖ້າ(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  ຖ້າ((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    ສົ່ງຄືນ TRUE;

  ສົ່ງຄືນ FALSE;
}

static void tzadjust(curl_off_t *tp, sa_nracanaa when *w)
{
  ຖ້າ(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  ຖ້າ((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  ບໍ່ດັ່ງນັ້ນ
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

static ຈຳນວນເຕັມ mktimet(curl_off_t seconds, time_t *output)
{
#ຖ້າ SIZEOF_TIME_T < 5
  ຖ້າ(seconds > TIME_T_MAX) {
    *output = TIME_T_MAX;
    ສົ່ງຄືນ PARSEDATE_LATER;
  }
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(seconds < TIME_T_MIN) {
    *output = TIME_T_MIN;
    ສົ່ງຄືນ PARSEDATE_SOONER;
  }
#elif defined(HAVE_TIME_T_UNSIGNED)
  ຖ້າ(seconds < 0) {
    *output = 0;
    ສົ່ງຄືນ PARSEDATE_SOONER;
  }
#endif
  *output = (time_t)seconds;
  ສົ່ງຄືນ PARSEDATE_OK;
}

static ຈຳນວນເຕັມ parsedate(const var_na *date, time_t *output)
{
  curl_off_t seconds = 0;
  const var_na *indate = date; /* save the original pointer */
  ຈຳນວນເຕັມ part = 0; /* max 6 parts */
  ຈຳນວນເຕັມ rc = 0;
  sa_nracanaa when w;
  initwhen(&w);

  ໃນຂະນະທີ່(*date && (part < 6)) {
    skip(&date);

    ຖ້າ(ISALPHA(*date))
      rc = datestring(&date, &w);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    ຖ້າ(rc)
      ສົ່ງຄືນ rc;

    part++;
  }

  ຖ້າ(datecheck(&w))
    ສົ່ງຄືນ PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  ສົ່ງຄືນ rc;
}
#ບໍ່ດັ່ງນັ້ນ
/* disabled */
static ຈຳນວນເຕັມ parsedate(const var_na *date, time_t *output)
{
  (void)date;
  *output = 0;
  ສົ່ງຄືນ PARSEDATE_OK; /* a lie */
}
#endif

time_t curl_getdate(const var_na *p, const time_t *unused)
{
  time_t parsed = -1;
  ຈຳນວນເຕັມ rc;
  (void)unused; /* legacy argument from the past that we ignore */

  ຖ້າ(!p)
    ສົ່ງຄືນ -1;

  rc = parsedate(p, &parsed);
  ຖ້າ(rc == PARSEDATE_OK) {
    ຖ້າ(parsed == (time_t)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    ສົ່ງຄືນ parsed;
  }
  /* everything else is fail */
  ສົ່ງຄືນ -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

ຈຳນວນເຕັມ Curl_getdate_capped(const var_na *p, time_t *tp)
{
  ຈຳນວນເຕັມ rc = parsedate(p, tp);
  ສົ່ງຄືນ (rc == PARSEDATE_FAIL);
}
