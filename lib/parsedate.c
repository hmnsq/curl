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
#ضمن "curl_setup.h"

#ضمن "parsedate.h"
#ضمن "curlx/strparse.h"
#ضمن "curlx/strcopy.h"

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

#إذا !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_FILE) || defined(USE_GNUTLS)
/* These names are also used by FTP and FILE code */
ثابت حرف * ثابت Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
ثابت حرف * ثابت Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#جيڪڏهن_پورو

#وصف_ڪريو PARSEDATE_OK     0
#وصف_ڪريو PARSEDATE_FAIL   (-1)

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PARSEDATE

#إذا SIZEOF_وقت_T < 5
#وصف_ڪريو PARSEDATE_LATER  1
#جيڪڏهن_پورو
#إذا SIZEOF_وقت_T < 5 || defined(HAVE_وقت_T_UNSIGNED)
#وصف_ڪريو PARSEDATE_SOONER 2
#جيڪڏهن_پورو

بنية tzinfo {
  uint32_t tz;
  int16_t وٿي; /* +/- in minutes */
};

#وصف_ڪريو DST (-60)     /* offset for daylight savings time */

/* alpha-sorted list of single-letter time zones, A - Z */
ساكن ثابت int16_t tzone[] = {
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
   8 * 60,            /*هڪجهڙو*/
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

#وصف_ڪريو MKTZ(a,b,c,d) (((a)<<24) | ((b)<<16) | ((c)<<8) | (d))

/* three-letter time zones */
ساكن ثابت بنية tzinfo tzthree[] = {
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
ساكن ثابت بنية tzinfo tzfoura[] = {
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

#وصف_ڪريو LOWERCASE(x) ((x) | 0x20)

/* returns:
   -1 no day
   0 monday - 6 sunday

   @unittest 4781
*/
UNITTEST صحيح checkday(ثابت حرف *check, size_t len);
UNITTEST صحيح checkday(ثابت حرف *check, size_t len)
{
  صحيح day = -1;
  إذا(len < 3)
    أرجع -1; /* too short */

  بدل(LOWERCASE(check[1])) {
  حالة 'o': /* monday */
    إذا((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    اكسر;
  حالة 'u': /* tuesday or sunday */
    إذا((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    وإلا إذا((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    اكسر;
  حالة 'e': /* wednesday */
    إذا((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    اكسر;
  حالة 'h': /* thursday */
    إذا((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    اكسر;
  حالة 'r': /* friday */
    إذا((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    اكسر;
  حالة 'a': /* saturday */
    إذا((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    اكسر;
  }
  إذا((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    ساكن ثابت حرف * ثابت daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    size_t wlen = لڙي_ڊيگهه(daysuffix[day]);
    إذا(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      أرجع -1;
  }
  أرجع day;
}

/* @unittest 4781 */

UNITTEST صحيح checkmonth(ثابت حرف *check, size_t len);
UNITTEST صحيح checkmonth(ثابت حرف *check, size_t len)
{
  إذا(len != 3)
    أرجع -1; /* not a month */

  بدل(LOWERCASE(check[2])) {
  حالة 'n': /* jan, jun */
    إذا(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      إذا(c2 == 'a')
        أرجع 0;
      وإلا إذا(c2 == 'u')
        أرجع 5;
    }
    اكسر;
  حالة 'b': /* feb */
    إذا((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      أرجع 1;
    اكسر;
  حالة 'r': /* mar, apr */
    إذا(LOWERCASE(check[0]) == 'm') {
      إذا(LOWERCASE(check[1]) == 'a')
        أرجع 2;
    }
    وإلا إذا((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      أرجع 3;
    اكسر;
  حالة 'y': /* may */
    إذا((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      أرجع 4;
    اكسر;
  حالة 'l': /* jul */
    إذا((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      أرجع 6;
    اكسر;
  حالة 'g': /* aug */
    إذا((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      أرجع 7;
    اكسر;
  حالة 'p': /* sep */
    إذا((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      أرجع 8;
    اكسر;
  حالة 't': /* oct */
    إذا((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      أرجع 9;
    اكسر;
  حالة 'v': /* nov */
    إذا((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      أرجع 10;
    اكسر;
  حالة 'c': /* dec */
    إذا((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      أرجع 11;
    اكسر;
  }
  أرجع -1; /* return the offset or -1, no real offset is -1 */
}

ساكن صحيح tzcompare(ثابت فراغ *m1, ثابت فراغ *m2)
{
  ثابت بنية tzinfo *tz1 = m1;
  ثابت بنية tzinfo *tz2 = m2;
  أرجع tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST صحيح checktz(ثابت حرف *check, size_t len);
UNITTEST صحيح checktz(ثابت حرف *check, size_t len)
{
  حرف پهريون;
  إذا(len > 4)
    أرجع -1;
  پهريون = check[0];
  إذا(پهريون < 'A')
    أرجع -1;
  إذا(len == 1) {
    /* short-cut single letter names */
    إذا((پهريون > 'Z') || (پهريون == 'J'))
      أرجع -1; /* no such tz */
    أرجع tzone[پهريون - 'A'] * 60;
  }
  وإلا إذا(len == 2) {
    /* two-letter names */
    إذا(check[1] == 'T') {
      إذا(پهريون == 'N')
        أرجع 11 * 60 * 60;
      وإلا إذا(پهريون == 'U')
        أرجع 0;
    }
    أرجع -1; /* nope */
  }
  وإلا إذا(len == 3) {
    /* three-letter name */
    ثابت بنية tzinfo *what;
    بنية tzinfo ڳوليو;
    إذا(پهريون > 'Y')
      أرجع -1;
    ڳوليو.tz = MKTZ(پهريون, check[1], check[2], 0);
    what = bsearch(&ڳوليو, tzthree, CURL_ARRAYSIZE(tzthree),
                   حجم(tzthree[0]), tzcompare);
    إذا(what)
      أرجع what->وٿي * 60;
  }
  وإلا {
    /* four-letter name */
    ثابت بنية tzinfo *what;
    بنية tzinfo ڳوليو;
    إذا(پهريون > 'W')
      أرجع -1;
    ڳوليو.tz = MKTZ(پهريون, check[1], check[2], check[3]);
    what = bsearch(&ڳوليو, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   حجم(tzfoura[0]), tzcompare);
    إذا(what)
      أرجع what->وٿي * 60;
  }
  أرجع -1;
}

ساكن فراغ skip(ثابت حرف **date)
{
  /* skip everything that are not letters or digits */
  طالما(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
بنية when {
  صحيح wday;  /* day of the week, 0-6 (mon-sun) */
  صحيح mon;   /* month of the year, 0-11 */
  صحيح mday;  /* day of month, 1 - 31 */
  صحيح dsecs; /* number of seconds into the day */
  صحيح year;  /* year, >= 1583 */
  صحيح tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#وصف_ڪريو LEAP_DAYS_BEFORE_1969 477
#وصف_ڪريو SECONDS_في_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
ساكن curl_off_t time2epoch(بنية when *w)
{
  ساكن ثابت صحيح cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  صحيح y = w->year - (w->mon <= 1);
  صحيح leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  أرجع (days * SECONDS_في_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
ساكن صحيح oneortwodigit(ثابت حرف *date, ثابت حرف **endp)
{
  صحيح انگ = date[0] - '0';
  إذا(ISDIGIT(date[1])) {
    *endp = &date[2];
    أرجع (انگ * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  أرجع انگ;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
ساكن bool ميلاپ_وقت(ثابت حرف *date, بنية when *w, حرف **endp)
{
  ثابت حرف *p;
  صحيح hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  إذا((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    إذا(mm < 60) {
      إذا((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        إذا(ss <= 60) {
          /* valid HH:MM:SS */
          اذهب ميلاپ;
        }
      }
      وإلا {
        /* valid HH:MM */
        اذهب ميلاپ;
      }
    }
  }
  أرجع FALSE; /* not a time string */
ميلاپ:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (حرف *)CURL_UNCONST(p);
  أرجع TRUE;
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
#وصف_ڪريو NAME_LEN 10

ساكن فراغ initwhen(بنية when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

ساكن صحيح datestring(ثابت حرف **datep, بنية when *w)
{
  /* a name coming up */
  size_t len = 0;
  ثابت حرف *p = *datep;
  طالما(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  إذا(len != NAME_LEN) {
    إذا(w->wday == -1) {
      w->wday = checkday(p, len);
      إذا(w->wday != -1)
        اذهب مليو;
    }
    إذا(w->mon == -1) {
      w->mon = checkmonth(p, len);
      إذا(w->mon != -1)
        اذهب مليو;
    }
    إذا(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      إذا(w->tzoff != -1)
        اذهب مليو;
    }
  }
  أرجع PARSEDATE_FAIL; /* bad string */
مليو:
  *datep += len;
  أرجع PARSEDATE_OK;
}

ساكن صحيح datenum(ثابت حرف *indate, ثابت حرف **datep, بنية when *w)
{
  /* a digit */
  بنانشان صحيح val;
  حرف *پڇاڙي;
  ثابت حرف *date = *datep;

  إذا((w->dsecs == -1) && ميلاپ_وقت(date, w, &پڇاڙي)) {
    /* time stamp */
    date = پڇاڙي;
  }
  وإلا {
    bool مليو = FALSE;
    curl_off_t lval;
    صحيح انگ_digits = 0;
    ثابت حرف *p = *datep;
    إذا(curlx_str_عدد(&p, &lval, 99999999))
      أرجع PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    انگ_digits = (صحيح)(p - *datep);
    val = (بنانشان صحيح)lval;

    إذا((w->tzoff == -1) &&
       (انگ_digits == 4) &&
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
      مليو = TRUE;
      w->tzoff = ((val / 100 * 60) + (val % 100)) * 60;

      /* the + and - prefix indicates the local time compared to GMT,
         this we need their reversed math to get what we want */
      w->tzoff = date[-1] == '+' ? -w->tzoff : w->tzoff;
    }

    وإلا إذا((انگ_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      مليو = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      إذا(w->mon > 11)
        أرجع PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    إذا(!مليو && (w->mday == -1)) {
      إذا((val > 0) && (val < 32)) {
        w->mday = val;
        مليو = TRUE;
      }
    }

    إذا(!مليو && (w->year == -1)) {
      w->year = val;
      مليو = TRUE;
      إذا(w->year < 100) {
        إذا(w->year > 70)
          w->year += 1900;
        وإلا
          w->year += 2000;
      }
    }

    إذا(!مليو)
      أرجع PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  أرجع PARSEDATE_OK;
}

ساكن bool datecheck(بنية when *w)
{
  إذا(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  إذا((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    أرجع TRUE;

  أرجع FALSE;
}

ساكن فراغ tzadjust(curl_off_t *tp, بنية when *w)
{
  إذا(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  إذا((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  وإلا
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

ساكن صحيح mktimet(curl_off_t seconds, time_t *output)
{
#إذا SIZEOF_وقت_T < 5
  إذا(seconds > وقت_T_MAX) {
    *output = وقت_T_MAX;
    أرجع PARSEDATE_LATER;
  }
  وإلا إذا(seconds < وقت_T_MIN) {
    *output = وقت_T_MIN;
    أرجع PARSEDATE_SOONER;
  }
#وإلاإذا defined(HAVE_وقت_T_UNSIGNED)
  إذا(seconds < 0) {
    *output = 0;
    أرجع PARSEDATE_SOONER;
  }
#جيڪڏهن_پورو
  *output = (time_t)seconds;
  أرجع PARSEDATE_OK;
}

ساكن صحيح parsedate(ثابت حرف *date, time_t *output)
{
  curl_off_t seconds = 0;
  ثابت حرف *indate = date; /* save the original pointer */
  صحيح part = 0; /* max 6 parts */
  صحيح rc = 0;
  بنية when w;
  initwhen(&w);

  طالما(*date && (part < 6)) {
    skip(&date);

    إذا(ISALPHA(*date))
      rc = datestring(&date, &w);
    وإلا إذا(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    إذا(rc)
      أرجع rc;

    part++;
  }

  إذا(datecheck(&w))
    أرجع PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  أرجع rc;
}
#وإلا
/* disabled */
ساكن صحيح parsedate(ثابت حرف *date, time_t *output)
{
  (فراغ)date;
  *output = 0;
  أرجع PARSEDATE_OK; /* a lie */
}
#جيڪڏهن_پورو

time_t curl_getdate(ثابت حرف *p, ثابت time_t *اڻ_استعمال_ٿيل)
{
  time_t parsed = -1;
  صحيح rc;
  (فراغ)اڻ_استعمال_ٿيل; /* legacy argument from the past that we ignore */

  إذا(!p)
    أرجع -1;

  rc = parsedate(p, &parsed);
  إذا(rc == PARSEDATE_OK) {
    إذا(parsed == (time_t)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    أرجع parsed;
  }
  /* everything else is fail */
  أرجع -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

صحيح Curl_getdate_capped(ثابت حرف *p, time_t *tp)
{
  صحيح rc = parsedate(p, tp);
  أرجع (rc == PARSEDATE_FAIL);
}
