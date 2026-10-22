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
#شامل "curl_setup.h"

#شامل "parsedate.h"
#شامل "curlx/strparse.h"
#شامل "curlx/strcopy.h"

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

#اگر !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_FILE) || defined(USE_GNUTLS)
/* These names are also used by FTP and FILE code */
ثابت نویسه * ثابت Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
ثابت نویسه * ثابت Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#endif

#define PARSEDATE_OK     0
#define PARSEDATE_FAIL   (-1)

#ifndef CURL_DISABLE_PARSEDATE

#اگر SIZEOF_TIME_T < 5
#define PARSEDATE_LATER  1
#endif
#اگر SIZEOF_TIME_T < 5 || defined(HAVE_TIME_T_UNSIGNED)
#define PARSEDATE_SOONER 2
#endif

ساختار tzinfo {
  uint32_t tz;
  int16_t offset; /* +/- in minutes */
};

#define DST (-60)     /* offset for daylight savings time */

/* alpha-sorted list of single-letter time zones, A - Z */
ایستا ثابت int16_t tzone[] = {
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
ایستا ثابت ساختار tzinfo tzthree[] = {
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
ایستا ثابت ساختار tzinfo tzfoura[] = {
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
UNITTEST صحیح checkday(ثابت نویسه *check, size_t len);
UNITTEST صحیح checkday(ثابت نویسه *check, size_t len)
{
  صحیح day = -1;
  اگر(len < 3)
    بازگشت -1; /* too short */

  انتخاب(LOWERCASE(check[1])) {
  حالت 'o': /* monday */
    اگر((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    بشکن;
  حالت 'u': /* tuesday or sunday */
    اگر((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    وگرنه اگر((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    بشکن;
  حالت 'e': /* wednesday */
    اگر((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    بشکن;
  حالت 'h': /* thursday */
    اگر((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    بشکن;
  حالت 'r': /* friday */
    اگر((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    بشکن;
  حالت 'a': /* saturday */
    اگر((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    بشکن;
  }
  اگر((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    ایستا ثابت نویسه * ثابت daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    size_t wlen = strlen(daysuffix[day]);
    اگر(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      بازگشت -1;
  }
  بازگشت day;
}

/* @unittest 4781 */

UNITTEST صحیح checkmonth(ثابت نویسه *check, size_t len);
UNITTEST صحیح checkmonth(ثابت نویسه *check, size_t len)
{
  اگر(len != 3)
    بازگشت -1; /* not a month */

  انتخاب(LOWERCASE(check[2])) {
  حالت 'n': /* jan, jun */
    اگر(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      اگر(c2 == 'a')
        بازگشت 0;
      وگرنه اگر(c2 == 'u')
        بازگشت 5;
    }
    بشکن;
  حالت 'b': /* feb */
    اگر((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      بازگشت 1;
    بشکن;
  حالت 'r': /* mar, apr */
    اگر(LOWERCASE(check[0]) == 'm') {
      اگر(LOWERCASE(check[1]) == 'a')
        بازگشت 2;
    }
    وگرنه اگر((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      بازگشت 3;
    بشکن;
  حالت 'y': /* may */
    اگر((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      بازگشت 4;
    بشکن;
  حالت 'l': /* jul */
    اگر((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      بازگشت 6;
    بشکن;
  حالت 'g': /* aug */
    اگر((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      بازگشت 7;
    بشکن;
  حالت 'p': /* sep */
    اگر((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      بازگشت 8;
    بشکن;
  حالت 't': /* oct */
    اگر((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      بازگشت 9;
    بشکن;
  حالت 'v': /* nov */
    اگر((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      بازگشت 10;
    بشکن;
  حالت 'c': /* dec */
    اگر((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      بازگشت 11;
    بشکن;
  }
  بازگشت -1; /* return the offset or -1, no real offset is -1 */
}

ایستا صحیح tzcompare(ثابت تهی *m1, ثابت تهی *m2)
{
  ثابت ساختار tzinfo *tz1 = m1;
  ثابت ساختار tzinfo *tz2 = m2;
  بازگشت tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST صحیح checktz(ثابت نویسه *check, size_t len);
UNITTEST صحیح checktz(ثابت نویسه *check, size_t len)
{
  نویسه first;
  اگر(len > 4)
    بازگشت -1;
  first = check[0];
  اگر(first < 'A')
    بازگشت -1;
  اگر(len == 1) {
    /* short-cut single letter names */
    اگر((first > 'Z') || (first == 'J'))
      بازگشت -1; /* no such tz */
    بازگشت tzone[first - 'A'] * 60;
  }
  وگرنه اگر(len == 2) {
    /* two-letter names */
    اگر(check[1] == 'T') {
      اگر(first == 'N')
        بازگشت 11 * 60 * 60;
      وگرنه اگر(first == 'U')
        بازگشت 0;
    }
    بازگشت -1; /* nope */
  }
  وگرنه اگر(len == 3) {
    /* three-letter name */
    ثابت ساختار tzinfo *what;
    ساختار tzinfo find;
    اگر(first > 'Y')
      بازگشت -1;
    find.tz = MKTZ(first, check[1], check[2], 0);
    what = bsearch(&find, tzthree, CURL_ARRAYSIZE(tzthree),
                   اندازه(tzthree[0]), tzcompare);
    اگر(what)
      بازگشت what->offset * 60;
  }
  وگرنه {
    /* four-letter name */
    ثابت ساختار tzinfo *what;
    ساختار tzinfo find;
    اگر(first > 'W')
      بازگشت -1;
    find.tz = MKTZ(first, check[1], check[2], check[3]);
    what = bsearch(&find, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   اندازه(tzfoura[0]), tzcompare);
    اگر(what)
      بازگشت what->offset * 60;
  }
  بازگشت -1;
}

ایستا تهی skip(ثابت نویسه **date)
{
  /* skip everything that are not letters or digits */
  وقتی(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
ساختار when {
  صحیح wday;  /* day of the week, 0-6 (mon-sun) */
  صحیح mon;   /* month of the year, 0-11 */
  صحیح mday;  /* day of month, 1 - 31 */
  صحیح dsecs; /* number of seconds into the day */
  صحیح year;  /* year, >= 1583 */
  صحیح tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#define LEAP_DAYS_BEFORE_1969 477
#define SECONDS_PER_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
ایستا curl_off_t time2epoch(ساختار when *w)
{
  ایستا ثابت صحیح cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  صحیح y = w->year - (w->mon <= 1);
  صحیح leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  بازگشت (days * SECONDS_PER_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
ایستا صحیح oneortwodigit(ثابت نویسه *date, ثابت نویسه **endp)
{
  صحیح رقم = date[0] - '0';
  اگر(ISDIGIT(date[1])) {
    *endp = &date[2];
    بازگشت (رقم * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  بازگشت رقم;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
ایستا bool match_time(ثابت نویسه *date, ساختار when *w, نویسه **endp)
{
  ثابت نویسه *p;
  صحیح hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  اگر((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    اگر(mm < 60) {
      اگر((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        اگر(ss <= 60) {
          /* valid HH:MM:SS */
          برو match;
        }
      }
      وگرنه {
        /* valid HH:MM */
        برو match;
      }
    }
  }
  بازگشت FALSE; /* not a time string */
match:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (نویسه *)CURL_UNCONST(p);
  بازگشت TRUE;
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

ایستا تهی initwhen(ساختار when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

ایستا صحیح datestring(ثابت نویسه **datep, ساختار when *w)
{
  /* a name coming up */
  size_t len = 0;
  ثابت نویسه *p = *datep;
  وقتی(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  اگر(len != NAME_LEN) {
    اگر(w->wday == -1) {
      w->wday = checkday(p, len);
      اگر(w->wday != -1)
        برو found;
    }
    اگر(w->mon == -1) {
      w->mon = checkmonth(p, len);
      اگر(w->mon != -1)
        برو found;
    }
    اگر(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      اگر(w->tzoff != -1)
        برو found;
    }
  }
  بازگشت PARSEDATE_FAIL; /* bad string */
found:
  *datep += len;
  بازگشت PARSEDATE_OK;
}

ایستا صحیح datenum(ثابت نویسه *indate, ثابت نویسه **datep, ساختار when *w)
{
  /* a digit */
  unsigned صحیح val;
  نویسه *end;
  ثابت نویسه *date = *datep;

  اگر((w->dsecs == -1) && match_time(date, w, &end)) {
    /* time stamp */
    date = end;
  }
  وگرنه {
    bool found = FALSE;
    curl_off_t lval;
    صحیح رقم_digits = 0;
    ثابت نویسه *p = *datep;
    اگر(curlx_str_عدد(&p, &lval, 99999999))
      بازگشت PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    رقم_digits = (صحیح)(p - *datep);
    val = (unsigned صحیح)lval;

    اگر((w->tzoff == -1) &&
       (رقم_digits == 4) &&
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

    وگرنه اگر((رقم_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      found = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      اگر(w->mon > 11)
        بازگشت PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    اگر(!found && (w->mday == -1)) {
      اگر((val > 0) && (val < 32)) {
        w->mday = val;
        found = TRUE;
      }
    }

    اگر(!found && (w->year == -1)) {
      w->year = val;
      found = TRUE;
      اگر(w->year < 100) {
        اگر(w->year > 70)
          w->year += 1900;
        وگرنه
          w->year += 2000;
      }
    }

    اگر(!found)
      بازگشت PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  بازگشت PARSEDATE_OK;
}

ایستا bool datecheck(ساختار when *w)
{
  اگر(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  اگر((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    بازگشت TRUE;

  بازگشت FALSE;
}

ایستا تهی tzadjust(curl_off_t *tp, ساختار when *w)
{
  اگر(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  اگر((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  وگرنه
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

ایستا صحیح mktimet(curl_off_t seconds, time_t *output)
{
#اگر SIZEOF_TIME_T < 5
  اگر(seconds > TIME_T_MAX) {
    *output = TIME_T_MAX;
    بازگشت PARSEDATE_LATER;
  }
  وگرنه اگر(seconds < TIME_T_MIN) {
    *output = TIME_T_MIN;
    بازگشت PARSEDATE_SOONER;
  }
#وگرنهاگر defined(HAVE_TIME_T_UNSIGNED)
  اگر(seconds < 0) {
    *output = 0;
    بازگشت PARSEDATE_SOONER;
  }
#endif
  *output = (time_t)seconds;
  بازگشت PARSEDATE_OK;
}

ایستا صحیح parsedate(ثابت نویسه *date, time_t *output)
{
  curl_off_t seconds = 0;
  ثابت نویسه *indate = date; /* save the original pointer */
  صحیح part = 0; /* max 6 parts */
  صحیح rc = 0;
  ساختار when w;
  initwhen(&w);

  وقتی(*date && (part < 6)) {
    skip(&date);

    اگر(ISALPHA(*date))
      rc = datestring(&date, &w);
    وگرنه اگر(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    اگر(rc)
      بازگشت rc;

    part++;
  }

  اگر(datecheck(&w))
    بازگشت PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  بازگشت rc;
}
#وگرنه
/* disabled */
ایستا صحیح parsedate(ثابت نویسه *date, time_t *output)
{
  (تهی)date;
  *output = 0;
  بازگشت PARSEDATE_OK; /* a lie */
}
#endif

time_t curl_getdate(ثابت نویسه *p, ثابت time_t *unused)
{
  time_t parsed = -1;
  صحیح rc;
  (تهی)unused; /* legacy argument from the past that we ignore */

  اگر(!p)
    بازگشت -1;

  rc = parsedate(p, &parsed);
  اگر(rc == PARSEDATE_OK) {
    اگر(parsed == (time_t)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    بازگشت parsed;
  }
  /* everything else is fail */
  بازگشت -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

صحیح Curl_getdate_capped(ثابت نویسه *p, time_t *tp)
{
  صحیح rc = parsedate(p, tp);
  بازگشت (rc == PARSEDATE_FAIL);
}
