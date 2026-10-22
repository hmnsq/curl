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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "parsedate.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

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

#ಆದರೆ !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_FILE) || defined(USE_GNUTLS)
/* These names are also used by FTP and FILE code */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು PARSEDATE_OK     0
#ವ್ಯಾಖ್ಯಾನಿಸು PARSEDATE_FAIL   (-1)

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PARSEDATE

#ಆದರೆ SIZEOF_ಸಮಯ_T < 5
#ವ್ಯಾಖ್ಯಾನಿಸು PARSEDATE_LATER  1
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ SIZEOF_ಸಮಯ_T < 5 || defined(HAVE_ಸಮಯ_T_UNSIGNED)
#ವ್ಯಾಖ್ಯಾನಿಸು PARSEDATE_SOONER 2
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ tzinfo {
  uint32_t tz;
  int16_t ಸ್ಥಾನಾಂತರ; /* +/- in minutes */
};

#ವ್ಯಾಖ್ಯಾನಿಸು DST (-60)     /* offset for daylight savings time */

/* alpha-sorted list of single-letter time zones, A - Z */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int16_t tzone[] = {
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
   8 * 60,            /*ಏಕರೂಪ*/
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

#ವ್ಯಾಖ್ಯಾನಿಸು MKTZ(a,b,c,d) (((a)<<24) | ((b)<<16) | ((c)<<8) | (d))

/* three-letter time zones */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ tzinfo tzthree[] = {
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
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ tzinfo tzfoura[] = {
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

#ವ್ಯಾಖ್ಯಾನಿಸು LOWERCASE(x) ((x) | 0x20)

/* returns:
   -1 no day
   0 monday - 6 sunday

   @unittest 4781
*/
UNITTEST ಪೂರ್ಣಾಂಕ checkday(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *check, size_t len);
UNITTEST ಪೂರ್ಣಾಂಕ checkday(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *check, size_t len)
{
  ಪೂರ್ಣಾಂಕ day = -1;
  ಆದರೆ(len < 3)
    ಹಿಂತಿರುಗಿ -1; /* too short */

  ಆಯ್ಕೆ(LOWERCASE(check[1])) {
  ಸ್ಥಿತಿ 'o': /* monday */
    ಆದರೆ((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'u': /* tuesday or sunday */
    ಆದರೆ((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'e': /* wednesday */
    ಆದರೆ((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'h': /* thursday */
    ಆದರೆ((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'r': /* friday */
    ಆದರೆ((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'a': /* saturday */
    ಆದರೆ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    ನಿಲ್ಲಿಸು;
  }
  ಆದರೆ((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    size_t wlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(daysuffix[day]);
    ಆದರೆ(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      ಹಿಂತಿರುಗಿ -1;
  }
  ಹಿಂತಿರುಗಿ day;
}

/* @unittest 4781 */

UNITTEST ಪೂರ್ಣಾಂಕ checkmonth(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *check, size_t len);
UNITTEST ಪೂರ್ಣಾಂಕ checkmonth(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *check, size_t len)
{
  ಆದರೆ(len != 3)
    ಹಿಂತಿರುಗಿ -1; /* not a month */

  ಆಯ್ಕೆ(LOWERCASE(check[2])) {
  ಸ್ಥಿತಿ 'n': /* jan, jun */
    ಆದರೆ(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      ಆದರೆ(c2 == 'a')
        ಹಿಂತಿರುಗಿ 0;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c2 == 'u')
        ಹಿಂತಿರುಗಿ 5;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'b': /* feb */
    ಆದರೆ((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      ಹಿಂತಿರುಗಿ 1;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'r': /* mar, apr */
    ಆದರೆ(LOWERCASE(check[0]) == 'm') {
      ಆದರೆ(LOWERCASE(check[1]) == 'a')
        ಹಿಂತಿರುಗಿ 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      ಹಿಂತಿರುಗಿ 3;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'y': /* may */
    ಆದರೆ((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      ಹಿಂತಿರುಗಿ 4;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'l': /* jul */
    ಆದರೆ((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      ಹಿಂತಿರುಗಿ 6;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'g': /* aug */
    ಆದರೆ((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      ಹಿಂತಿರುಗಿ 7;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'p': /* sep */
    ಆದರೆ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      ಹಿಂತಿರುಗಿ 8;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 't': /* oct */
    ಆದರೆ((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      ಹಿಂತಿರುಗಿ 9;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'v': /* nov */
    ಆದರೆ((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      ಹಿಂತಿರುಗಿ 10;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'c': /* dec */
    ಆದರೆ((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      ಹಿಂತಿರುಗಿ 11;
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ -1; /* return the offset or -1, no real offset is -1 */
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tzcompare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *m1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *m2)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ tzinfo *tz1 = m1;
  ಸ್ಥಿರಾಂಕ ರಚನೆ tzinfo *tz2 = m2;
  ಹಿಂತಿರುಗಿ tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST ಪೂರ್ಣಾಂಕ checktz(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *check, size_t len);
UNITTEST ಪೂರ್ಣಾಂಕ checktz(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *check, size_t len)
{
  ಅಕ್ಷರ ಮೊದಲ;
  ಆದರೆ(len > 4)
    ಹಿಂತಿರುಗಿ -1;
  ಮೊದಲ = check[0];
  ಆದರೆ(ಮೊದಲ < 'A')
    ಹಿಂತಿರುಗಿ -1;
  ಆದರೆ(len == 1) {
    /* short-cut single letter names */
    ಆದರೆ((ಮೊದಲ > 'Z') || (ಮೊದಲ == 'J'))
      ಹಿಂತಿರುಗಿ -1; /* no such tz */
    ಹಿಂತಿರುಗಿ tzone[ಮೊದಲ - 'A'] * 60;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len == 2) {
    /* two-letter names */
    ಆದರೆ(check[1] == 'T') {
      ಆದರೆ(ಮೊದಲ == 'N')
        ಹಿಂತಿರುಗಿ 11 * 60 * 60;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಮೊದಲ == 'U')
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ -1; /* nope */
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len == 3) {
    /* three-letter name */
    ಸ್ಥಿರಾಂಕ ರಚನೆ tzinfo *what;
    ರಚನೆ tzinfo ಕಂಡುಹಿಡಿ;
    ಆದರೆ(ಮೊದಲ > 'Y')
      ಹಿಂತಿರುಗಿ -1;
    ಕಂಡುಹಿಡಿ.tz = MKTZ(ಮೊದಲ, check[1], check[2], 0);
    what = bsearch(&ಕಂಡುಹಿಡಿ, tzthree, CURL_ARRAYSIZE(tzthree),
                   ಗಾತ್ರ(tzthree[0]), tzcompare);
    ಆದರೆ(what)
      ಹಿಂತಿರುಗಿ what->ಸ್ಥಾನಾಂತರ * 60;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* four-letter name */
    ಸ್ಥಿರಾಂಕ ರಚನೆ tzinfo *what;
    ರಚನೆ tzinfo ಕಂಡುಹಿಡಿ;
    ಆದರೆ(ಮೊದಲ > 'W')
      ಹಿಂತಿರುಗಿ -1;
    ಕಂಡುಹಿಡಿ.tz = MKTZ(ಮೊದಲ, check[1], check[2], check[3]);
    what = bsearch(&ಕಂಡುಹಿಡಿ, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   ಗಾತ್ರ(tzfoura[0]), tzcompare);
    ಆದರೆ(what)
      ಹಿಂತಿರುಗಿ what->ಸ್ಥಾನಾಂತರ * 60;
  }
  ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ ಶೂನ್ಯ skip(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **date)
{
  /* skip everything that are not letters or digits */
  ಆಗ(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
ರಚನೆ when {
  ಪೂರ್ಣಾಂಕ wday;  /* day of the week, 0-6 (mon-sun) */
  ಪೂರ್ಣಾಂಕ mon;   /* month of the year, 0-11 */
  ಪೂರ್ಣಾಂಕ mday;  /* day of month, 1 - 31 */
  ಪೂರ್ಣಾಂಕ dsecs; /* number of seconds into the day */
  ಪೂರ್ಣಾಂಕ year;  /* year, >= 1583 */
  ಪೂರ್ಣಾಂಕ tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#ವ್ಯಾಖ್ಯಾನಿಸು LEAP_DAYS_BEFORE_1969 477
#ವ್ಯಾಖ್ಯಾನಿಸು SECONDS_ಪ್ರತಿ_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
ಸ್ಥಿರ curl_off_t time2epoch(ರಚನೆ when *w)
{
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  ಪೂರ್ಣಾಂಕ y = w->year - (w->mon <= 1);
  ಪೂರ್ಣಾಂಕ leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  ಹಿಂತಿರುಗಿ (days * SECONDS_ಪ್ರತಿ_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ oneortwodigit(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *date, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **endp)
{
  ಪೂರ್ಣಾಂಕ ಅಂಕೆ = date[0] - '0';
  ಆದರೆ(ISDIGIT(date[1])) {
    *endp = &date[2];
    ಹಿಂತಿರುಗಿ (ಅಂಕೆ * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  ಹಿಂತಿರುಗಿ ಅಂಕೆ;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
ಸ್ಥಿರ bool ಹೊಂದಿಕೆ_ಸಮಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *date, ರಚನೆ when *w, ಅಕ್ಷರ **endp)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
  ಪೂರ್ಣಾಂಕ hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  ಆದರೆ((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    ಆದರೆ(mm < 60) {
      ಆದರೆ((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        ಆದರೆ(ss <= 60) {
          /* valid HH:MM:SS */
          ಹೋಗು ಹೊಂದಿಕೆ;
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* valid HH:MM */
        ಹೋಗು ಹೊಂದಿಕೆ;
      }
    }
  }
  ಹಿಂತಿರುಗಿ FALSE; /* not a time string */
ಹೊಂದಿಕೆ:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (ಅಕ್ಷರ *)CURL_UNCONST(p);
  ಹಿಂತಿರುಗಿ TRUE;
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
#ವ್ಯಾಖ್ಯಾನಿಸು NAME_LEN 10

ಸ್ಥಿರ ಶೂನ್ಯ initwhen(ರಚನೆ when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ datestring(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **datep, ರಚನೆ when *w)
{
  /* a name coming up */
  size_t len = 0;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = *datep;
  ಆಗ(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  ಆದರೆ(len != NAME_LEN) {
    ಆದರೆ(w->wday == -1) {
      w->wday = checkday(p, len);
      ಆದರೆ(w->wday != -1)
        ಹೋಗು ಸಿಕ್ಕಿತು;
    }
    ಆದರೆ(w->mon == -1) {
      w->mon = checkmonth(p, len);
      ಆದರೆ(w->mon != -1)
        ಹೋಗು ಸಿಕ್ಕಿತು;
    }
    ಆದರೆ(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      ಆದರೆ(w->tzoff != -1)
        ಹೋಗು ಸಿಕ್ಕಿತು;
    }
  }
  ಹಿಂತಿರುಗಿ PARSEDATE_FAIL; /* bad string */
ಸಿಕ್ಕಿತು:
  *datep += len;
  ಹಿಂತಿರುಗಿ PARSEDATE_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ datenum(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *indate, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **datep, ರಚನೆ when *w)
{
  /* a digit */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ val;
  ಅಕ್ಷರ *ಕೊನೆ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *date = *datep;

  ಆದರೆ((w->dsecs == -1) && ಹೊಂದಿಕೆ_ಸಮಯ(date, w, &ಕೊನೆ)) {
    /* time stamp */
    date = ಕೊನೆ;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    bool ಸಿಕ್ಕಿತು = FALSE;
    curl_off_t lval;
    ಪೂರ್ಣಾಂಕ ಅಂಕೆ_digits = 0;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = *datep;
    ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&p, &lval, 99999999))
      ಹಿಂತಿರುಗಿ PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    ಅಂಕೆ_digits = (ಪೂರ್ಣಾಂಕ)(p - *datep);
    val = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)lval;

    ಆದರೆ((w->tzoff == -1) &&
       (ಅಂಕೆ_digits == 4) &&
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
      ಸಿಕ್ಕಿತು = TRUE;
      w->tzoff = ((val / 100 * 60) + (val % 100)) * 60;

      /* the + and - prefix indicates the local time compared to GMT,
         this we need their reversed math to get what we want */
      w->tzoff = date[-1] == '+' ? -w->tzoff : w->tzoff;
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಅಂಕೆ_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      ಸಿಕ್ಕಿತು = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      ಆದರೆ(w->mon > 11)
        ಹಿಂತಿರುಗಿ PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    ಆದರೆ(!ಸಿಕ್ಕಿತು && (w->mday == -1)) {
      ಆದರೆ((val > 0) && (val < 32)) {
        w->mday = val;
        ಸಿಕ್ಕಿತು = TRUE;
      }
    }

    ಆದರೆ(!ಸಿಕ್ಕಿತು && (w->year == -1)) {
      w->year = val;
      ಸಿಕ್ಕಿತು = TRUE;
      ಆದರೆ(w->year < 100) {
        ಆದರೆ(w->year > 70)
          w->year += 1900;
        ಇಲ್ಲದಿದ್ದರೆ
          w->year += 2000;
      }
    }

    ಆದರೆ(!ಸಿಕ್ಕಿತು)
      ಹಿಂತಿರುಗಿ PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  ಹಿಂತಿರುಗಿ PARSEDATE_OK;
}

ಸ್ಥಿರ bool datecheck(ರಚನೆ when *w)
{
  ಆದರೆ(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  ಆದರೆ((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    ಹಿಂತಿರುಗಿ TRUE;

  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ tzadjust(curl_off_t *tp, ರಚನೆ when *w)
{
  ಆದರೆ(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  ಆದರೆ((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  ಇಲ್ಲದಿದ್ದರೆ
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mktimet(curl_off_t seconds, time_t *output)
{
#ಆದರೆ SIZEOF_ಸಮಯ_T < 5
  ಆದರೆ(seconds > ಸಮಯ_T_MAX) {
    *output = ಸಮಯ_T_MAX;
    ಹಿಂತಿರುಗಿ PARSEDATE_LATER;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(seconds < ಸಮಯ_T_MIN) {
    *output = ಸಮಯ_T_MIN;
    ಹಿಂತಿರುಗಿ PARSEDATE_SOONER;
  }
#ಅಥವಾಆದರೆ defined(HAVE_ಸಮಯ_T_UNSIGNED)
  ಆದರೆ(seconds < 0) {
    *output = 0;
    ಹಿಂತಿರುಗಿ PARSEDATE_SOONER;
  }
#ಆದರೆ_ಅಂತ್ಯ
  *output = (time_t)seconds;
  ಹಿಂತಿರುಗಿ PARSEDATE_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parsedate(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *date, time_t *output)
{
  curl_off_t seconds = 0;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *indate = date; /* save the original pointer */
  ಪೂರ್ಣಾಂಕ part = 0; /* max 6 parts */
  ಪೂರ್ಣಾಂಕ rc = 0;
  ರಚನೆ when w;
  initwhen(&w);

  ಆಗ(*date && (part < 6)) {
    skip(&date);

    ಆದರೆ(ISALPHA(*date))
      rc = datestring(&date, &w);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    ಆದರೆ(rc)
      ಹಿಂತಿರುಗಿ rc;

    part++;
  }

  ಆದರೆ(datecheck(&w))
    ಹಿಂತಿರುಗಿ PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  ಹಿಂತಿರುಗಿ rc;
}
#ಇಲ್ಲದಿದ್ದರೆ
/* disabled */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parsedate(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *date, time_t *output)
{
  (ಶೂನ್ಯ)date;
  *output = 0;
  ಹಿಂತಿರುಗಿ PARSEDATE_OK; /* a lie */
}
#ಆದರೆ_ಅಂತ್ಯ

time_t curl_getdate(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ time_t *ಬಳಸದ)
{
  time_t parsed = -1;
  ಪೂರ್ಣಾಂಕ rc;
  (ಶೂನ್ಯ)ಬಳಸದ; /* legacy argument from the past that we ignore */

  ಆದರೆ(!p)
    ಹಿಂತಿರುಗಿ -1;

  rc = parsedate(p, &parsed);
  ಆದರೆ(rc == PARSEDATE_OK) {
    ಆದರೆ(parsed == (time_t)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    ಹಿಂತಿರುಗಿ parsed;
  }
  /* everything else is fail */
  ಹಿಂತಿರುಗಿ -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

ಪೂರ್ಣಾಂಕ Curl_getdate_capped(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, time_t *tp)
{
  ಪೂರ್ಣಾಂಕ rc = parsedate(p, tp);
  ಹಿಂತಿರುಗಿ (rc == PARSEDATE_FAIL);
}
