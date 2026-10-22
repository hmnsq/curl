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

#ако !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_FILE) || defined(USE_GNUTLS)
/* These names are also used by FTP and FILE code */
const char * const Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
const char * const Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#endif

#define PARSEDATE_OK     0
#define PARSEDATE_FAIL   (-1)

#ifndef CURL_DISABLE_PARSEDATE

#ако SIZEOF_TIME_T < 5
#define PARSEDATE_LATER  1
#endif
#ако SIZEOF_TIME_T < 5 || defined(HAVE_TIME_T_UNSIGNED)
#define PARSEDATE_SOONER 2
#endif

struct tzinfo {
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
static const struct tzinfo tzthree[] = {
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
static const struct tzinfo tzfoura[] = {
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
UNITTEST цяло checkday(const char *check, size_t len);
UNITTEST цяло checkday(const char *check, size_t len)
{
  цяло day = -1;
  ако(len < 3)
    върни -1; /* too short */

  switch(LOWERCASE(check[1])) {
  case 'o': /* monday */
    ако((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    break;
  case 'u': /* tuesday or sunday */
    ако((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    иначе ако((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    break;
  case 'e': /* wednesday */
    ако((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    break;
  case 'h': /* thursday */
    ако((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    break;
  case 'r': /* friday */
    ако((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    break;
  case 'a': /* saturday */
    ако((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    break;
  }
  ако((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    static const char * const daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    size_t wlen = strlen(daysuffix[day]);
    ако(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      върни -1;
  }
  върни day;
}

/* @unittest 4781 */

UNITTEST цяло checkmonth(const char *check, size_t len);
UNITTEST цяло checkmonth(const char *check, size_t len)
{
  ако(len != 3)
    върни -1; /* not a month */

  switch(LOWERCASE(check[2])) {
  case 'n': /* jan, jun */
    ако(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      ако(c2 == 'a')
        върни 0;
      иначе ако(c2 == 'u')
        върни 5;
    }
    break;
  case 'b': /* feb */
    ако((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      върни 1;
    break;
  case 'r': /* mar, apr */
    ако(LOWERCASE(check[0]) == 'm') {
      ако(LOWERCASE(check[1]) == 'a')
        върни 2;
    }
    иначе ако((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      върни 3;
    break;
  case 'y': /* may */
    ако((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      върни 4;
    break;
  case 'l': /* jul */
    ако((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      върни 6;
    break;
  case 'g': /* aug */
    ако((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      върни 7;
    break;
  case 'p': /* sep */
    ако((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      върни 8;
    break;
  case 't': /* oct */
    ако((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      върни 9;
    break;
  case 'v': /* nov */
    ако((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      върни 10;
    break;
  case 'c': /* dec */
    ако((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      върни 11;
    break;
  }
  върни -1; /* return the offset or -1, no real offset is -1 */
}

static цяло tzcompare(const void *m1, const void *m2)
{
  const struct tzinfo *tz1 = m1;
  const struct tzinfo *tz2 = m2;
  върни tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST цяло checktz(const char *check, size_t len);
UNITTEST цяло checktz(const char *check, size_t len)
{
  char first;
  ако(len > 4)
    върни -1;
  first = check[0];
  ако(first < 'A')
    върни -1;
  ако(len == 1) {
    /* short-cut single letter names */
    ако((first > 'Z') || (first == 'J'))
      върни -1; /* no such tz */
    върни tzone[first - 'A'] * 60;
  }
  иначе ако(len == 2) {
    /* two-letter names */
    ако(check[1] == 'T') {
      ако(first == 'N')
        върни 11 * 60 * 60;
      иначе ако(first == 'U')
        върни 0;
    }
    върни -1; /* nope */
  }
  иначе ако(len == 3) {
    /* three-letter name */
    const struct tzinfo *what;
    struct tzinfo find;
    ако(first > 'Y')
      върни -1;
    find.tz = MKTZ(first, check[1], check[2], 0);
    what = bsearch(&find, tzthree, CURL_ARRAYSIZE(tzthree),
                   sizeof(tzthree[0]), tzcompare);
    ако(what)
      върни what->offset * 60;
  }
  иначе {
    /* four-letter name */
    const struct tzinfo *what;
    struct tzinfo find;
    ако(first > 'W')
      върни -1;
    find.tz = MKTZ(first, check[1], check[2], check[3]);
    what = bsearch(&find, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   sizeof(tzfoura[0]), tzcompare);
    ако(what)
      върни what->offset * 60;
  }
  върни -1;
}

static void skip(const char **date)
{
  /* skip everything that are not letters or digits */
  докато(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
struct when {
  цяло wday;  /* day of the week, 0-6 (mon-sun) */
  цяло mon;   /* month of the year, 0-11 */
  цяло mday;  /* day of month, 1 - 31 */
  цяло dsecs; /* number of seconds into the day */
  цяло year;  /* year, >= 1583 */
  цяло tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#define LEAP_DAYS_BEFORE_1969 477
#define SECONDS_PER_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
static curl_off_t time2epoch(struct when *w)
{
  static const цяло cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  цяло y = w->year - (w->mon <= 1);
  цяло leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  върни (days * SECONDS_PER_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
static цяло oneortwodigit(const char *date, const char **endp)
{
  цяло num = date[0] - '0';
  ако(ISDIGIT(date[1])) {
    *endp = &date[2];
    върни (num * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  върни num;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
static bool match_time(const char *date, struct when *w, char **endp)
{
  const char *p;
  цяло hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  ако((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    ако(mm < 60) {
      ако((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        ако(ss <= 60) {
          /* valid HH:MM:SS */
          goto match;
        }
      }
      иначе {
        /* valid HH:MM */
        goto match;
      }
    }
  }
  върни FALSE; /* not a time string */
match:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (char *)CURL_UNCONST(p);
  върни TRUE;
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

static void initwhen(struct when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

static цяло datestring(const char **datep, struct when *w)
{
  /* a name coming up */
  size_t len = 0;
  const char *p = *datep;
  докато(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  ако(len != NAME_LEN) {
    ако(w->wday == -1) {
      w->wday = checkday(p, len);
      ако(w->wday != -1)
        goto found;
    }
    ако(w->mon == -1) {
      w->mon = checkmonth(p, len);
      ако(w->mon != -1)
        goto found;
    }
    ако(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      ако(w->tzoff != -1)
        goto found;
    }
  }
  върни PARSEDATE_FAIL; /* bad string */
found:
  *datep += len;
  върни PARSEDATE_OK;
}

static цяло datenum(const char *indate, const char **datep, struct when *w)
{
  /* a digit */
  unsigned цяло val;
  char *end;
  const char *date = *datep;

  ако((w->dsecs == -1) && match_time(date, w, &end)) {
    /* time stamp */
    date = end;
  }
  иначе {
    bool found = FALSE;
    curl_off_t lval;
    цяло num_digits = 0;
    const char *p = *datep;
    ако(curlx_str_number(&p, &lval, 99999999))
      върни PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    num_digits = (цяло)(p - *datep);
    val = (unsigned цяло)lval;

    ако((w->tzoff == -1) &&
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

    иначе ако((num_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      found = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      ако(w->mon > 11)
        върни PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    ако(!found && (w->mday == -1)) {
      ако((val > 0) && (val < 32)) {
        w->mday = val;
        found = TRUE;
      }
    }

    ако(!found && (w->year == -1)) {
      w->year = val;
      found = TRUE;
      ако(w->year < 100) {
        ако(w->year > 70)
          w->year += 1900;
        иначе
          w->year += 2000;
      }
    }

    ако(!found)
      върни PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  върни PARSEDATE_OK;
}

static bool datecheck(struct when *w)
{
  ако(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  ако((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    върни TRUE;

  върни FALSE;
}

static void tzadjust(curl_off_t *tp, struct when *w)
{
  ако(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  ако((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  иначе
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

static цяло mktimet(curl_off_t seconds, time_t *output)
{
#ако SIZEOF_TIME_T < 5
  ако(seconds > TIME_T_MAX) {
    *output = TIME_T_MAX;
    върни PARSEDATE_LATER;
  }
  иначе ако(seconds < TIME_T_MIN) {
    *output = TIME_T_MIN;
    върни PARSEDATE_SOONER;
  }
#elif defined(HAVE_TIME_T_UNSIGNED)
  ако(seconds < 0) {
    *output = 0;
    върни PARSEDATE_SOONER;
  }
#endif
  *output = (time_t)seconds;
  върни PARSEDATE_OK;
}

static цяло parsedate(const char *date, time_t *output)
{
  curl_off_t seconds = 0;
  const char *indate = date; /* save the original pointer */
  цяло part = 0; /* max 6 parts */
  цяло rc = 0;
  struct when w;
  initwhen(&w);

  докато(*date && (part < 6)) {
    skip(&date);

    ако(ISALPHA(*date))
      rc = datestring(&date, &w);
    иначе ако(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    ако(rc)
      върни rc;

    part++;
  }

  ако(datecheck(&w))
    върни PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  върни rc;
}
#иначе
/* disabled */
static цяло parsedate(const char *date, time_t *output)
{
  (void)date;
  *output = 0;
  върни PARSEDATE_OK; /* a lie */
}
#endif

time_t curl_getdate(const char *p, const time_t *unused)
{
  time_t parsed = -1;
  цяло rc;
  (void)unused; /* legacy argument from the past that we ignore */

  ако(!p)
    върни -1;

  rc = parsedate(p, &parsed);
  ако(rc == PARSEDATE_OK) {
    ако(parsed == (time_t)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    върни parsed;
  }
  /* everything else is fail */
  върни -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

цяло Curl_getdate_capped(const char *p, time_t *tp)
{
  цяло rc = parsedate(p, tp);
  върни (rc == PARSEDATE_FAIL);
}
