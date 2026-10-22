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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "parsedate.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "curlx/strcopy.h"

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

#எனில் !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_FILE) || defined(USE_GNUTLS)
/* These names are also used by FTP and FILE code */
மாறிலி எழுத்து * மாறிலி Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
மாறிலி எழுத்து * மாறிலி Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#endif

#define PARSEDATE_OK     0
#define PARSEDATE_FAIL   (-1)

#ifndef CURL_DISABLE_PARSEDATE

#எனில் SIZEOF_TIME_T < 5
#define PARSEDATE_LATER  1
#endif
#எனில் SIZEOF_TIME_T < 5 || defined(HAVE_TIME_T_UNSIGNED)
#define PARSEDATE_SOONER 2
#endif

கட்டமைப்பு tzinfo {
  uint32_t tz;
  int16_t offset; /* +/- in minutes */
};

#define DST (-60)     /* offset for daylight savings time */

/* alpha-sorted list of single-letter time zones, A - Z */
நிலையான மாறிலி int16_t tzone[] = {
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
நிலையான மாறிலி கட்டமைப்பு tzinfo tzthree[] = {
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
நிலையான மாறிலி கட்டமைப்பு tzinfo tzfoura[] = {
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
UNITTEST முழு checkday(மாறிலி எழுத்து *check, size_t len);
UNITTEST முழு checkday(மாறிலி எழுத்து *check, size_t len)
{
  முழு day = -1;
  எனில்(len < 3)
    திரும்பு -1; /* too short */

  தேர்வு(LOWERCASE(check[1])) {
  நிலை 'o': /* monday */
    எனில்((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    நிறுத்து;
  நிலை 'u': /* tuesday or sunday */
    எனில்((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    இல்லையெனில் எனில்((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    நிறுத்து;
  நிலை 'e': /* wednesday */
    எனில்((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    நிறுத்து;
  நிலை 'h': /* thursday */
    எனில்((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    நிறுத்து;
  நிலை 'r': /* friday */
    எனில்((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    நிறுத்து;
  நிலை 'a': /* saturday */
    எனில்((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    நிறுத்து;
  }
  எனில்((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    நிலையான மாறிலி எழுத்து * மாறிலி daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    size_t wlen = strlen(daysuffix[day]);
    எனில்(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      திரும்பு -1;
  }
  திரும்பு day;
}

/* @unittest 4781 */

UNITTEST முழு checkmonth(மாறிலி எழுத்து *check, size_t len);
UNITTEST முழு checkmonth(மாறிலி எழுத்து *check, size_t len)
{
  எனில்(len != 3)
    திரும்பு -1; /* not a month */

  தேர்வு(LOWERCASE(check[2])) {
  நிலை 'n': /* jan, jun */
    எனில்(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      எனில்(c2 == 'a')
        திரும்பு 0;
      இல்லையெனில் எனில்(c2 == 'u')
        திரும்பு 5;
    }
    நிறுத்து;
  நிலை 'b': /* feb */
    எனில்((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      திரும்பு 1;
    நிறுத்து;
  நிலை 'r': /* mar, apr */
    எனில்(LOWERCASE(check[0]) == 'm') {
      எனில்(LOWERCASE(check[1]) == 'a')
        திரும்பு 2;
    }
    இல்லையெனில் எனில்((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      திரும்பு 3;
    நிறுத்து;
  நிலை 'y': /* may */
    எனில்((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      திரும்பு 4;
    நிறுத்து;
  நிலை 'l': /* jul */
    எனில்((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      திரும்பு 6;
    நிறுத்து;
  நிலை 'g': /* aug */
    எனில்((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      திரும்பு 7;
    நிறுத்து;
  நிலை 'p': /* sep */
    எனில்((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      திரும்பு 8;
    நிறுத்து;
  நிலை 't': /* oct */
    எனில்((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      திரும்பு 9;
    நிறுத்து;
  நிலை 'v': /* nov */
    எனில்((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      திரும்பு 10;
    நிறுத்து;
  நிலை 'c': /* dec */
    எனில்((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      திரும்பு 11;
    நிறுத்து;
  }
  திரும்பு -1; /* return the offset or -1, no real offset is -1 */
}

நிலையான முழு tzcompare(மாறிலி வெறுமை *m1, மாறிலி வெறுமை *m2)
{
  மாறிலி கட்டமைப்பு tzinfo *tz1 = m1;
  மாறிலி கட்டமைப்பு tzinfo *tz2 = m2;
  திரும்பு tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST முழு checktz(மாறிலி எழுத்து *check, size_t len);
UNITTEST முழு checktz(மாறிலி எழுத்து *check, size_t len)
{
  எழுத்து first;
  எனில்(len > 4)
    திரும்பு -1;
  first = check[0];
  எனில்(first < 'A')
    திரும்பு -1;
  எனில்(len == 1) {
    /* short-cut single letter names */
    எனில்((first > 'Z') || (first == 'J'))
      திரும்பு -1; /* no such tz */
    திரும்பு tzone[first - 'A'] * 60;
  }
  இல்லையெனில் எனில்(len == 2) {
    /* two-letter names */
    எனில்(check[1] == 'T') {
      எனில்(first == 'N')
        திரும்பு 11 * 60 * 60;
      இல்லையெனில் எனில்(first == 'U')
        திரும்பு 0;
    }
    திரும்பு -1; /* nope */
  }
  இல்லையெனில் எனில்(len == 3) {
    /* three-letter name */
    மாறிலி கட்டமைப்பு tzinfo *what;
    கட்டமைப்பு tzinfo find;
    எனில்(first > 'Y')
      திரும்பு -1;
    find.tz = MKTZ(first, check[1], check[2], 0);
    what = bsearch(&find, tzthree, CURL_ARRAYSIZE(tzthree),
                   அளவு(tzthree[0]), tzcompare);
    எனில்(what)
      திரும்பு what->offset * 60;
  }
  இல்லையெனில் {
    /* four-letter name */
    மாறிலி கட்டமைப்பு tzinfo *what;
    கட்டமைப்பு tzinfo find;
    எனில்(first > 'W')
      திரும்பு -1;
    find.tz = MKTZ(first, check[1], check[2], check[3]);
    what = bsearch(&find, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   அளவு(tzfoura[0]), tzcompare);
    எனில்(what)
      திரும்பு what->offset * 60;
  }
  திரும்பு -1;
}

நிலையான வெறுமை skip(மாறிலி எழுத்து **date)
{
  /* skip everything that are not letters or digits */
  போது(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
கட்டமைப்பு when {
  முழு wday;  /* day of the week, 0-6 (mon-sun) */
  முழு mon;   /* month of the year, 0-11 */
  முழு mday;  /* day of month, 1 - 31 */
  முழு dsecs; /* number of seconds into the day */
  முழு year;  /* year, >= 1583 */
  முழு tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#define LEAP_DAYS_BEFORE_1969 477
#define SECONDS_PER_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
நிலையான curl_off_t time2epoch(கட்டமைப்பு when *w)
{
  நிலையான மாறிலி முழு cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  முழு y = w->year - (w->mon <= 1);
  முழு leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  திரும்பு (days * SECONDS_PER_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
நிலையான முழு oneortwodigit(மாறிலி எழுத்து *date, மாறிலி எழுத்து **endp)
{
  முழு இலக்கம் = date[0] - '0';
  எனில்(ISDIGIT(date[1])) {
    *endp = &date[2];
    திரும்பு (இலக்கம் * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  திரும்பு இலக்கம்;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
நிலையான bool match_time(மாறிலி எழுத்து *date, கட்டமைப்பு when *w, எழுத்து **endp)
{
  மாறிலி எழுத்து *p;
  முழு hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  எனில்((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    எனில்(mm < 60) {
      எனில்((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        எனில்(ss <= 60) {
          /* valid HH:MM:SS */
          செல் match;
        }
      }
      இல்லையெனில் {
        /* valid HH:MM */
        செல் match;
      }
    }
  }
  திரும்பு FALSE; /* not a time string */
match:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (எழுத்து *)CURL_UNCONST(p);
  திரும்பு TRUE;
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

நிலையான வெறுமை initwhen(கட்டமைப்பு when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

நிலையான முழு datestring(மாறிலி எழுத்து **datep, கட்டமைப்பு when *w)
{
  /* a name coming up */
  size_t len = 0;
  மாறிலி எழுத்து *p = *datep;
  போது(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  எனில்(len != NAME_LEN) {
    எனில்(w->wday == -1) {
      w->wday = checkday(p, len);
      எனில்(w->wday != -1)
        செல் found;
    }
    எனில்(w->mon == -1) {
      w->mon = checkmonth(p, len);
      எனில்(w->mon != -1)
        செல் found;
    }
    எனில்(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      எனில்(w->tzoff != -1)
        செல் found;
    }
  }
  திரும்பு PARSEDATE_FAIL; /* bad string */
found:
  *datep += len;
  திரும்பு PARSEDATE_OK;
}

நிலையான முழு datenum(மாறிலி எழுத்து *indate, மாறிலி எழுத்து **datep, கட்டமைப்பு when *w)
{
  /* a digit */
  unsigned முழு val;
  எழுத்து *end;
  மாறிலி எழுத்து *date = *datep;

  எனில்((w->dsecs == -1) && match_time(date, w, &end)) {
    /* time stamp */
    date = end;
  }
  இல்லையெனில் {
    bool found = FALSE;
    curl_off_t lval;
    முழு இலக்கம்_digits = 0;
    மாறிலி எழுத்து *p = *datep;
    எனில்(curlx_str_எண்(&p, &lval, 99999999))
      திரும்பு PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    இலக்கம்_digits = (முழு)(p - *datep);
    val = (unsigned முழு)lval;

    எனில்((w->tzoff == -1) &&
       (இலக்கம்_digits == 4) &&
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

    இல்லையெனில் எனில்((இலக்கம்_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      found = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      எனில்(w->mon > 11)
        திரும்பு PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    எனில்(!found && (w->mday == -1)) {
      எனில்((val > 0) && (val < 32)) {
        w->mday = val;
        found = TRUE;
      }
    }

    எனில்(!found && (w->year == -1)) {
      w->year = val;
      found = TRUE;
      எனில்(w->year < 100) {
        எனில்(w->year > 70)
          w->year += 1900;
        இல்லையெனில்
          w->year += 2000;
      }
    }

    எனில்(!found)
      திரும்பு PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  திரும்பு PARSEDATE_OK;
}

நிலையான bool datecheck(கட்டமைப்பு when *w)
{
  எனில்(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  எனில்((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    திரும்பு TRUE;

  திரும்பு FALSE;
}

நிலையான வெறுமை tzadjust(curl_off_t *tp, கட்டமைப்பு when *w)
{
  எனில்(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  எனில்((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  இல்லையெனில்
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

நிலையான முழு mktimet(curl_off_t seconds, time_t *output)
{
#எனில் SIZEOF_TIME_T < 5
  எனில்(seconds > TIME_T_MAX) {
    *output = TIME_T_MAX;
    திரும்பு PARSEDATE_LATER;
  }
  இல்லையெனில் எனில்(seconds < TIME_T_MIN) {
    *output = TIME_T_MIN;
    திரும்பு PARSEDATE_SOONER;
  }
#அல்லதெனில் defined(HAVE_TIME_T_UNSIGNED)
  எனில்(seconds < 0) {
    *output = 0;
    திரும்பு PARSEDATE_SOONER;
  }
#endif
  *output = (time_t)seconds;
  திரும்பு PARSEDATE_OK;
}

நிலையான முழு parsedate(மாறிலி எழுத்து *date, time_t *output)
{
  curl_off_t seconds = 0;
  மாறிலி எழுத்து *indate = date; /* save the original pointer */
  முழு part = 0; /* max 6 parts */
  முழு rc = 0;
  கட்டமைப்பு when w;
  initwhen(&w);

  போது(*date && (part < 6)) {
    skip(&date);

    எனில்(ISALPHA(*date))
      rc = datestring(&date, &w);
    இல்லையெனில் எனில்(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    எனில்(rc)
      திரும்பு rc;

    part++;
  }

  எனில்(datecheck(&w))
    திரும்பு PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  திரும்பு rc;
}
#இல்லையெனில்
/* disabled */
நிலையான முழு parsedate(மாறிலி எழுத்து *date, time_t *output)
{
  (வெறுமை)date;
  *output = 0;
  திரும்பு PARSEDATE_OK; /* a lie */
}
#endif

time_t curl_getdate(மாறிலி எழுத்து *p, மாறிலி time_t *unused)
{
  time_t parsed = -1;
  முழு rc;
  (வெறுமை)unused; /* legacy argument from the past that we ignore */

  எனில்(!p)
    திரும்பு -1;

  rc = parsedate(p, &parsed);
  எனில்(rc == PARSEDATE_OK) {
    எனில்(parsed == (time_t)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    திரும்பு parsed;
  }
  /* everything else is fail */
  திரும்பு -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

முழு Curl_getdate_capped(மாறிலி எழுத்து *p, time_t *tp)
{
  முழு rc = parsedate(p, tp);
  திரும்பு (rc == PARSEDATE_FAIL);
}
