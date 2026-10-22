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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "parsedate.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"

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

#ਜੇ !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_FILE) || defined(USE_GNUTLS)
/* These names are also used by FTP and FILE code */
ਅਚਲ ਅੱਖਰ * ਅਚਲ Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
ਅਚਲ ਅੱਖਰ * ਅਚਲ Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARSEDATE_OK     0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARSEDATE_FAIL   (-1)

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PARSEDATE

#ਜੇ SIZEOF_ਸਮਾਂ_T < 5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARSEDATE_LATER  1
#ਜੇ_ਅੰਤ
#ਜੇ SIZEOF_ਸਮਾਂ_T < 5 || defined(HAVE_ਸਮਾਂ_T_UNSIGNED)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARSEDATE_SOONER 2
#ਜੇ_ਅੰਤ

ਬਣਤਰ tzinfo {
  uint32_t tz;
  int16_t ਵਿੱਥ; /* +/- in minutes */
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DST (-60)     /* offset for daylight savings time */

/* alpha-sorted list of single-letter time zones, A - Z */
ਸਥਿਰ ਅਚਲ int16_t tzone[] = {
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
   8 * 60,            /*ਇਕਸਾਰ*/
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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MKTZ(a,b,c,d) (((a)<<24) | ((b)<<16) | ((c)<<8) | (d))

/* three-letter time zones */
ਸਥਿਰ ਅਚਲ ਬਣਤਰ tzinfo tzthree[] = {
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
ਸਥਿਰ ਅਚਲ ਬਣਤਰ tzinfo tzfoura[] = {
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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOWERCASE(x) ((x) | 0x20)

/* returns:
   -1 no day
   0 monday - 6 sunday

   @unittest 4781
*/
UNITTEST ਪੂਰਨ checkday(ਅਚਲ ਅੱਖਰ *check, size_t len);
UNITTEST ਪੂਰਨ checkday(ਅਚਲ ਅੱਖਰ *check, size_t len)
{
  ਪੂਰਨ day = -1;
  ਜੇ(len < 3)
    ਵਾਪਸ -1; /* too short */

  ਚੋਣ(LOWERCASE(check[1])) {
  ਹਾਲਤ 'o': /* monday */
    ਜੇ((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    ਰੋਕੋ;
  ਹਾਲਤ 'u': /* tuesday or sunday */
    ਜੇ((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    ਨਹੀਂਤਾਂ ਜੇ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    ਰੋਕੋ;
  ਹਾਲਤ 'e': /* wednesday */
    ਜੇ((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    ਰੋਕੋ;
  ਹਾਲਤ 'h': /* thursday */
    ਜੇ((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    ਰੋਕੋ;
  ਹਾਲਤ 'r': /* friday */
    ਜੇ((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    ਰੋਕੋ;
  ਹਾਲਤ 'a': /* saturday */
    ਜੇ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    ਰੋਕੋ;
  }
  ਜੇ((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    size_t wlen = ਲੜੀ_ਲੰਬਾਈ(daysuffix[day]);
    ਜੇ(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      ਵਾਪਸ -1;
  }
  ਵਾਪਸ day;
}

/* @unittest 4781 */

UNITTEST ਪੂਰਨ checkmonth(ਅਚਲ ਅੱਖਰ *check, size_t len);
UNITTEST ਪੂਰਨ checkmonth(ਅਚਲ ਅੱਖਰ *check, size_t len)
{
  ਜੇ(len != 3)
    ਵਾਪਸ -1; /* not a month */

  ਚੋਣ(LOWERCASE(check[2])) {
  ਹਾਲਤ 'n': /* jan, jun */
    ਜੇ(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      ਜੇ(c2 == 'a')
        ਵਾਪਸ 0;
      ਨਹੀਂਤਾਂ ਜੇ(c2 == 'u')
        ਵਾਪਸ 5;
    }
    ਰੋਕੋ;
  ਹਾਲਤ 'b': /* feb */
    ਜੇ((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      ਵਾਪਸ 1;
    ਰੋਕੋ;
  ਹਾਲਤ 'r': /* mar, apr */
    ਜੇ(LOWERCASE(check[0]) == 'm') {
      ਜੇ(LOWERCASE(check[1]) == 'a')
        ਵਾਪਸ 2;
    }
    ਨਹੀਂਤਾਂ ਜੇ((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      ਵਾਪਸ 3;
    ਰੋਕੋ;
  ਹਾਲਤ 'y': /* may */
    ਜੇ((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      ਵਾਪਸ 4;
    ਰੋਕੋ;
  ਹਾਲਤ 'l': /* jul */
    ਜੇ((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      ਵਾਪਸ 6;
    ਰੋਕੋ;
  ਹਾਲਤ 'g': /* aug */
    ਜੇ((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      ਵਾਪਸ 7;
    ਰੋਕੋ;
  ਹਾਲਤ 'p': /* sep */
    ਜੇ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      ਵਾਪਸ 8;
    ਰੋਕੋ;
  ਹਾਲਤ 't': /* oct */
    ਜੇ((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      ਵਾਪਸ 9;
    ਰੋਕੋ;
  ਹਾਲਤ 'v': /* nov */
    ਜੇ((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      ਵਾਪਸ 10;
    ਰੋਕੋ;
  ਹਾਲਤ 'c': /* dec */
    ਜੇ((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      ਵਾਪਸ 11;
    ਰੋਕੋ;
  }
  ਵਾਪਸ -1; /* return the offset or -1, no real offset is -1 */
}

ਸਥਿਰ ਪੂਰਨ tzcompare(ਅਚਲ ਖਾਲੀ *m1, ਅਚਲ ਖਾਲੀ *m2)
{
  ਅਚਲ ਬਣਤਰ tzinfo *tz1 = m1;
  ਅਚਲ ਬਣਤਰ tzinfo *tz2 = m2;
  ਵਾਪਸ tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST ਪੂਰਨ checktz(ਅਚਲ ਅੱਖਰ *check, size_t len);
UNITTEST ਪੂਰਨ checktz(ਅਚਲ ਅੱਖਰ *check, size_t len)
{
  ਅੱਖਰ ਪਹਿਲਾ;
  ਜੇ(len > 4)
    ਵਾਪਸ -1;
  ਪਹਿਲਾ = check[0];
  ਜੇ(ਪਹਿਲਾ < 'A')
    ਵਾਪਸ -1;
  ਜੇ(len == 1) {
    /* short-cut single letter names */
    ਜੇ((ਪਹਿਲਾ > 'Z') || (ਪਹਿਲਾ == 'J'))
      ਵਾਪਸ -1; /* no such tz */
    ਵਾਪਸ tzone[ਪਹਿਲਾ - 'A'] * 60;
  }
  ਨਹੀਂਤਾਂ ਜੇ(len == 2) {
    /* two-letter names */
    ਜੇ(check[1] == 'T') {
      ਜੇ(ਪਹਿਲਾ == 'N')
        ਵਾਪਸ 11 * 60 * 60;
      ਨਹੀਂਤਾਂ ਜੇ(ਪਹਿਲਾ == 'U')
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ -1; /* nope */
  }
  ਨਹੀਂਤਾਂ ਜੇ(len == 3) {
    /* three-letter name */
    ਅਚਲ ਬਣਤਰ tzinfo *what;
    ਬਣਤਰ tzinfo ਲੱਭੋ;
    ਜੇ(ਪਹਿਲਾ > 'Y')
      ਵਾਪਸ -1;
    ਲੱਭੋ.tz = MKTZ(ਪਹਿਲਾ, check[1], check[2], 0);
    what = bsearch(&ਲੱਭੋ, tzthree, CURL_ARRAYSIZE(tzthree),
                   ਆਕਾਰ(tzthree[0]), tzcompare);
    ਜੇ(what)
      ਵਾਪਸ what->ਵਿੱਥ * 60;
  }
  ਨਹੀਂਤਾਂ {
    /* four-letter name */
    ਅਚਲ ਬਣਤਰ tzinfo *what;
    ਬਣਤਰ tzinfo ਲੱਭੋ;
    ਜੇ(ਪਹਿਲਾ > 'W')
      ਵਾਪਸ -1;
    ਲੱਭੋ.tz = MKTZ(ਪਹਿਲਾ, check[1], check[2], check[3]);
    what = bsearch(&ਲੱਭੋ, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   ਆਕਾਰ(tzfoura[0]), tzcompare);
    ਜੇ(what)
      ਵਾਪਸ what->ਵਿੱਥ * 60;
  }
  ਵਾਪਸ -1;
}

ਸਥਿਰ ਖਾਲੀ skip(ਅਚਲ ਅੱਖਰ **date)
{
  /* skip everything that are not letters or digits */
  ਜਦੋਂ(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
ਬਣਤਰ when {
  ਪੂਰਨ wday;  /* day of the week, 0-6 (mon-sun) */
  ਪੂਰਨ mon;   /* month of the year, 0-11 */
  ਪੂਰਨ mday;  /* day of month, 1 - 31 */
  ਪੂਰਨ dsecs; /* number of seconds into the day */
  ਪੂਰਨ year;  /* year, >= 1583 */
  ਪੂਰਨ tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LEAP_DAYS_BEFORE_1969 477
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SECONDS_ਪ੍ਰਤੀ_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
ਸਥਿਰ curl_off_t time2epoch(ਬਣਤਰ when *w)
{
  ਸਥਿਰ ਅਚਲ ਪੂਰਨ cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  ਪੂਰਨ y = w->year - (w->mon <= 1);
  ਪੂਰਨ leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  ਵਾਪਸ (days * SECONDS_ਪ੍ਰਤੀ_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
ਸਥਿਰ ਪੂਰਨ oneortwodigit(ਅਚਲ ਅੱਖਰ *date, ਅਚਲ ਅੱਖਰ **endp)
{
  ਪੂਰਨ ਅੰਕ = date[0] - '0';
  ਜੇ(ISDIGIT(date[1])) {
    *endp = &date[2];
    ਵਾਪਸ (ਅੰਕ * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  ਵਾਪਸ ਅੰਕ;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
ਸਥਿਰ bool ਮੇਲ_ਸਮਾਂ(ਅਚਲ ਅੱਖਰ *date, ਬਣਤਰ when *w, ਅੱਖਰ **endp)
{
  ਅਚਲ ਅੱਖਰ *p;
  ਪੂਰਨ hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  ਜੇ((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    ਜੇ(mm < 60) {
      ਜੇ((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        ਜੇ(ss <= 60) {
          /* valid HH:MM:SS */
          ਜਾਓ ਮੇਲ;
        }
      }
      ਨਹੀਂਤਾਂ {
        /* valid HH:MM */
        ਜਾਓ ਮੇਲ;
      }
    }
  }
  ਵਾਪਸ FALSE; /* not a time string */
ਮੇਲ:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (ਅੱਖਰ *)CURL_UNCONST(p);
  ਵਾਪਸ TRUE;
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NAME_LEN 10

ਸਥਿਰ ਖਾਲੀ initwhen(ਬਣਤਰ when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

ਸਥਿਰ ਪੂਰਨ datestring(ਅਚਲ ਅੱਖਰ **datep, ਬਣਤਰ when *w)
{
  /* a name coming up */
  size_t len = 0;
  ਅਚਲ ਅੱਖਰ *p = *datep;
  ਜਦੋਂ(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  ਜੇ(len != NAME_LEN) {
    ਜੇ(w->wday == -1) {
      w->wday = checkday(p, len);
      ਜੇ(w->wday != -1)
        ਜਾਓ ਮਿਲਿਆ;
    }
    ਜੇ(w->mon == -1) {
      w->mon = checkmonth(p, len);
      ਜੇ(w->mon != -1)
        ਜਾਓ ਮਿਲਿਆ;
    }
    ਜੇ(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      ਜੇ(w->tzoff != -1)
        ਜਾਓ ਮਿਲਿਆ;
    }
  }
  ਵਾਪਸ PARSEDATE_FAIL; /* bad string */
ਮਿਲਿਆ:
  *datep += len;
  ਵਾਪਸ PARSEDATE_OK;
}

ਸਥਿਰ ਪੂਰਨ datenum(ਅਚਲ ਅੱਖਰ *indate, ਅਚਲ ਅੱਖਰ **datep, ਬਣਤਰ when *w)
{
  /* a digit */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ val;
  ਅੱਖਰ *ਸਿਰਾ;
  ਅਚਲ ਅੱਖਰ *date = *datep;

  ਜੇ((w->dsecs == -1) && ਮੇਲ_ਸਮਾਂ(date, w, &ਸਿਰਾ)) {
    /* time stamp */
    date = ਸਿਰਾ;
  }
  ਨਹੀਂਤਾਂ {
    bool ਮਿਲਿਆ = FALSE;
    curl_off_t lval;
    ਪੂਰਨ ਅੰਕ_digits = 0;
    ਅਚਲ ਅੱਖਰ *p = *datep;
    ਜੇ(curlx_str_ਸੰਖਿਆ(&p, &lval, 99999999))
      ਵਾਪਸ PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    ਅੰਕ_digits = (ਪੂਰਨ)(p - *datep);
    val = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)lval;

    ਜੇ((w->tzoff == -1) &&
       (ਅੰਕ_digits == 4) &&
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
      ਮਿਲਿਆ = TRUE;
      w->tzoff = ((val / 100 * 60) + (val % 100)) * 60;

      /* the + and - prefix indicates the local time compared to GMT,
         this we need their reversed math to get what we want */
      w->tzoff = date[-1] == '+' ? -w->tzoff : w->tzoff;
    }

    ਨਹੀਂਤਾਂ ਜੇ((ਅੰਕ_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      ਮਿਲਿਆ = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      ਜੇ(w->mon > 11)
        ਵਾਪਸ PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    ਜੇ(!ਮਿਲਿਆ && (w->mday == -1)) {
      ਜੇ((val > 0) && (val < 32)) {
        w->mday = val;
        ਮਿਲਿਆ = TRUE;
      }
    }

    ਜੇ(!ਮਿਲਿਆ && (w->year == -1)) {
      w->year = val;
      ਮਿਲਿਆ = TRUE;
      ਜੇ(w->year < 100) {
        ਜੇ(w->year > 70)
          w->year += 1900;
        ਨਹੀਂਤਾਂ
          w->year += 2000;
      }
    }

    ਜੇ(!ਮਿਲਿਆ)
      ਵਾਪਸ PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  ਵਾਪਸ PARSEDATE_OK;
}

ਸਥਿਰ bool datecheck(ਬਣਤਰ when *w)
{
  ਜੇ(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  ਜੇ((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    ਵਾਪਸ TRUE;

  ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਖਾਲੀ tzadjust(curl_off_t *tp, ਬਣਤਰ when *w)
{
  ਜੇ(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  ਜੇ((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  ਨਹੀਂਤਾਂ
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

ਸਥਿਰ ਪੂਰਨ mktimet(curl_off_t seconds, time_t *output)
{
#ਜੇ SIZEOF_ਸਮਾਂ_T < 5
  ਜੇ(seconds > ਸਮਾਂ_T_MAX) {
    *output = ਸਮਾਂ_T_MAX;
    ਵਾਪਸ PARSEDATE_LATER;
  }
  ਨਹੀਂਤਾਂ ਜੇ(seconds < ਸਮਾਂ_T_MIN) {
    *output = ਸਮਾਂ_T_MIN;
    ਵਾਪਸ PARSEDATE_SOONER;
  }
#ਜਾਂਜੇ defined(HAVE_ਸਮਾਂ_T_UNSIGNED)
  ਜੇ(seconds < 0) {
    *output = 0;
    ਵਾਪਸ PARSEDATE_SOONER;
  }
#ਜੇ_ਅੰਤ
  *output = (time_t)seconds;
  ਵਾਪਸ PARSEDATE_OK;
}

ਸਥਿਰ ਪੂਰਨ parsedate(ਅਚਲ ਅੱਖਰ *date, time_t *output)
{
  curl_off_t seconds = 0;
  ਅਚਲ ਅੱਖਰ *indate = date; /* save the original pointer */
  ਪੂਰਨ part = 0; /* max 6 parts */
  ਪੂਰਨ rc = 0;
  ਬਣਤਰ when w;
  initwhen(&w);

  ਜਦੋਂ(*date && (part < 6)) {
    skip(&date);

    ਜੇ(ISALPHA(*date))
      rc = datestring(&date, &w);
    ਨਹੀਂਤਾਂ ਜੇ(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    ਜੇ(rc)
      ਵਾਪਸ rc;

    part++;
  }

  ਜੇ(datecheck(&w))
    ਵਾਪਸ PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  ਵਾਪਸ rc;
}
#ਨਹੀਂਤਾਂ
/* disabled */
ਸਥਿਰ ਪੂਰਨ parsedate(ਅਚਲ ਅੱਖਰ *date, time_t *output)
{
  (ਖਾਲੀ)date;
  *output = 0;
  ਵਾਪਸ PARSEDATE_OK; /* a lie */
}
#ਜੇ_ਅੰਤ

time_t curl_getdate(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ time_t *ਅਣਵਰਤਿਆ)
{
  time_t parsed = -1;
  ਪੂਰਨ rc;
  (ਖਾਲੀ)ਅਣਵਰਤਿਆ; /* legacy argument from the past that we ignore */

  ਜੇ(!p)
    ਵਾਪਸ -1;

  rc = parsedate(p, &parsed);
  ਜੇ(rc == PARSEDATE_OK) {
    ਜੇ(parsed == (time_t)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    ਵਾਪਸ parsed;
  }
  /* everything else is fail */
  ਵਾਪਸ -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

ਪੂਰਨ Curl_getdate_capped(ਅਚਲ ਅੱਖਰ *p, time_t *tp)
{
  ਪੂਰਨ rc = parsedate(p, tp);
  ਵਾਪਸ (rc == PARSEDATE_FAIL);
}
