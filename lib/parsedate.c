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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "parsedate.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "curlx/strcopy.h"

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

#यदि !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_FILE) || defined(USE_GNUTLS)
/* These names are also used by FTP and FILE code */
अचर वर्ण * अचर Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
अचर वर्ण * अचर Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#endif

#define PARSEDATE_OK     0
#define PARSEDATE_FAIL   (-1)

#ifndef CURL_DISABLE_PARSEDATE

#यदि SIZEOF_TIME_T < 5
#define PARSEDATE_LATER  1
#endif
#यदि SIZEOF_TIME_T < 5 || defined(HAVE_TIME_T_UNSIGNED)
#define PARSEDATE_SOONER 2
#endif

संरचना tzinfo {
  uint32_t tz;
  int16_t offset; /* +/- in minutes */
};

#define DST (-60)     /* offset for daylight savings time */

/* alpha-sorted list of single-letter time zones, A - Z */
स्थिर अचर int16_t tzone[] = {
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
स्थिर अचर संरचना tzinfo tzthree[] = {
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
स्थिर अचर संरचना tzinfo tzfoura[] = {
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
UNITTEST पूर्णाङ्क checkday(अचर वर्ण *check, माप_प्रकार len);
UNITTEST पूर्णाङ्क checkday(अचर वर्ण *check, माप_प्रकार len)
{
  पूर्णाङ्क day = -1;
  यदि(len < 3)
    फर्काऊ -1; /* too short */

  विकल्प(LOWERCASE(check[1])) {
  पक्ष 'o': /* monday */
    यदि((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    रोक;
  पक्ष 'u': /* tuesday or sunday */
    यदि((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    अन्यथा यदि((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    रोक;
  पक्ष 'e': /* wednesday */
    यदि((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    रोक;
  पक्ष 'h': /* thursday */
    यदि((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    रोक;
  पक्ष 'r': /* friday */
    यदि((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    रोक;
  पक्ष 'a': /* saturday */
    यदि((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    रोक;
  }
  यदि((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    स्थिर अचर वर्ण * अचर daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    माप_प्रकार wlen = म_माप(daysuffix[day]);
    यदि(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      फर्काऊ -1;
  }
  फर्काऊ day;
}

/* @unittest 4781 */

UNITTEST पूर्णाङ्क checkmonth(अचर वर्ण *check, माप_प्रकार len);
UNITTEST पूर्णाङ्क checkmonth(अचर वर्ण *check, माप_प्रकार len)
{
  यदि(len != 3)
    फर्काऊ -1; /* not a month */

  विकल्प(LOWERCASE(check[2])) {
  पक्ष 'n': /* jan, jun */
    यदि(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      यदि(c2 == 'a')
        फर्काऊ 0;
      अन्यथा यदि(c2 == 'u')
        फर्काऊ 5;
    }
    रोक;
  पक्ष 'b': /* feb */
    यदि((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      फर्काऊ 1;
    रोक;
  पक्ष 'r': /* mar, apr */
    यदि(LOWERCASE(check[0]) == 'm') {
      यदि(LOWERCASE(check[1]) == 'a')
        फर्काऊ 2;
    }
    अन्यथा यदि((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      फर्काऊ 3;
    रोक;
  पक्ष 'y': /* may */
    यदि((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      फर्काऊ 4;
    रोक;
  पक्ष 'l': /* jul */
    यदि((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      फर्काऊ 6;
    रोक;
  पक्ष 'g': /* aug */
    यदि((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      फर्काऊ 7;
    रोक;
  पक्ष 'p': /* sep */
    यदि((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      फर्काऊ 8;
    रोक;
  पक्ष 't': /* oct */
    यदि((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      फर्काऊ 9;
    रोक;
  पक्ष 'v': /* nov */
    यदि((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      फर्काऊ 10;
    रोक;
  पक्ष 'c': /* dec */
    यदि((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      फर्काऊ 11;
    रोक;
  }
  फर्काऊ -1; /* return the offset or -1, no real offset is -1 */
}

स्थिर पूर्णाङ्क tzcompare(अचर शून्य *m1, अचर शून्य *m2)
{
  अचर संरचना tzinfo *tz1 = m1;
  अचर संरचना tzinfo *tz2 = m2;
  फर्काऊ tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST पूर्णाङ्क checktz(अचर वर्ण *check, माप_प्रकार len);
UNITTEST पूर्णाङ्क checktz(अचर वर्ण *check, माप_प्रकार len)
{
  वर्ण first;
  यदि(len > 4)
    फर्काऊ -1;
  first = check[0];
  यदि(first < 'A')
    फर्काऊ -1;
  यदि(len == 1) {
    /* short-cut single letter names */
    यदि((first > 'Z') || (first == 'J'))
      फर्काऊ -1; /* no such tz */
    फर्काऊ tzone[first - 'A'] * 60;
  }
  अन्यथा यदि(len == 2) {
    /* two-letter names */
    यदि(check[1] == 'T') {
      यदि(first == 'N')
        फर्काऊ 11 * 60 * 60;
      अन्यथा यदि(first == 'U')
        फर्काऊ 0;
    }
    फर्काऊ -1; /* nope */
  }
  अन्यथा यदि(len == 3) {
    /* three-letter name */
    अचर संरचना tzinfo *what;
    संरचना tzinfo find;
    यदि(first > 'Y')
      फर्काऊ -1;
    find.tz = MKTZ(first, check[1], check[2], 0);
    what = द्वा_खोज(&find, tzthree, CURL_ARRAYSIZE(tzthree),
                   परिमाण(tzthree[0]), tzcompare);
    यदि(what)
      फर्काऊ what->offset * 60;
  }
  अन्यथा {
    /* four-letter name */
    अचर संरचना tzinfo *what;
    संरचना tzinfo find;
    यदि(first > 'W')
      फर्काऊ -1;
    find.tz = MKTZ(first, check[1], check[2], check[3]);
    what = द्वा_खोज(&find, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   परिमाण(tzfoura[0]), tzcompare);
    यदि(what)
      फर्काऊ what->offset * 60;
  }
  फर्काऊ -1;
}

स्थिर शून्य skip(अचर वर्ण **date)
{
  /* skip everything that are not letters or digits */
  जबसम्म(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
संरचना when {
  पूर्णाङ्क wday;  /* day of the week, 0-6 (mon-sun) */
  पूर्णाङ्क mon;   /* month of the year, 0-11 */
  पूर्णाङ्क mday;  /* day of month, 1 - 31 */
  पूर्णाङ्क dsecs; /* number of seconds into the day */
  पूर्णाङ्क year;  /* year, >= 1583 */
  पूर्णाङ्क tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#define LEAP_DAYS_BEFORE_1969 477
#define SECONDS_PER_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
स्थिर curl_off_t time2epoch(संरचना when *w)
{
  स्थिर अचर पूर्णाङ्क cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  पूर्णाङ्क y = w->year - (w->mon <= 1);
  पूर्णाङ्क leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  फर्काऊ (days * SECONDS_PER_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
स्थिर पूर्णाङ्क oneortwodigit(अचर वर्ण *date, अचर वर्ण **endp)
{
  पूर्णाङ्क अङ्क = date[0] - '0';
  यदि(ISDIGIT(date[1])) {
    *endp = &date[2];
    फर्काऊ (अङ्क * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  फर्काऊ अङ्क;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
स्थिर bool match_time(अचर वर्ण *date, संरचना when *w, वर्ण **endp)
{
  अचर वर्ण *p;
  पूर्णाङ्क hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  यदि((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    यदि(mm < 60) {
      यदि((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        यदि(ss <= 60) {
          /* valid HH:MM:SS */
          गच्छ match;
        }
      }
      अन्यथा {
        /* valid HH:MM */
        गच्छ match;
      }
    }
  }
  फर्काऊ FALSE; /* not a time string */
match:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (वर्ण *)CURL_UNCONST(p);
  फर्काऊ TRUE;
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

स्थिर शून्य initwhen(संरचना when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

स्थिर पूर्णाङ्क datestring(अचर वर्ण **datep, संरचना when *w)
{
  /* a name coming up */
  माप_प्रकार len = 0;
  अचर वर्ण *p = *datep;
  जबसम्म(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  यदि(len != NAME_LEN) {
    यदि(w->wday == -1) {
      w->wday = checkday(p, len);
      यदि(w->wday != -1)
        गच्छ found;
    }
    यदि(w->mon == -1) {
      w->mon = checkmonth(p, len);
      यदि(w->mon != -1)
        गच्छ found;
    }
    यदि(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      यदि(w->tzoff != -1)
        गच्छ found;
    }
  }
  फर्काऊ PARSEDATE_FAIL; /* bad string */
found:
  *datep += len;
  फर्काऊ PARSEDATE_OK;
}

स्थिर पूर्णाङ्क datenum(अचर वर्ण *indate, अचर वर्ण **datep, संरचना when *w)
{
  /* a digit */
  अचिन्हित पूर्णाङ्क val;
  वर्ण *end;
  अचर वर्ण *date = *datep;

  यदि((w->dsecs == -1) && match_time(date, w, &end)) {
    /* time stamp */
    date = end;
  }
  अन्यथा {
    bool found = FALSE;
    curl_off_t lval;
    पूर्णाङ्क अङ्क_digits = 0;
    अचर वर्ण *p = *datep;
    यदि(curlx_str_सङ्ख्या(&p, &lval, 99999999))
      फर्काऊ PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    अङ्क_digits = (पूर्णाङ्क)(p - *datep);
    val = (अचिन्हित पूर्णाङ्क)lval;

    यदि((w->tzoff == -1) &&
       (अङ्क_digits == 4) &&
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

    अन्यथा यदि((अङ्क_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      found = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      यदि(w->mon > 11)
        फर्काऊ PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    यदि(!found && (w->mday == -1)) {
      यदि((val > 0) && (val < 32)) {
        w->mday = val;
        found = TRUE;
      }
    }

    यदि(!found && (w->year == -1)) {
      w->year = val;
      found = TRUE;
      यदि(w->year < 100) {
        यदि(w->year > 70)
          w->year += 1900;
        अन्यथा
          w->year += 2000;
      }
    }

    यदि(!found)
      फर्काऊ PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  फर्काऊ PARSEDATE_OK;
}

स्थिर bool datecheck(संरचना when *w)
{
  यदि(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  यदि((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    फर्काऊ TRUE;

  फर्काऊ FALSE;
}

स्थिर शून्य tzadjust(curl_off_t *tp, संरचना when *w)
{
  यदि(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  यदि((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  अन्यथा
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

स्थिर पूर्णाङ्क mktimet(curl_off_t seconds, समय_प्रकार *output)
{
#यदि SIZEOF_TIME_T < 5
  यदि(seconds > TIME_T_MAX) {
    *output = TIME_T_MAX;
    फर्काऊ PARSEDATE_LATER;
  }
  अन्यथा यदि(seconds < TIME_T_MIN) {
    *output = TIME_T_MIN;
    फर्काऊ PARSEDATE_SOONER;
  }
#अथवायदि defined(HAVE_TIME_T_UNSIGNED)
  यदि(seconds < 0) {
    *output = 0;
    फर्काऊ PARSEDATE_SOONER;
  }
#endif
  *output = (समय_प्रकार)seconds;
  फर्काऊ PARSEDATE_OK;
}

स्थिर पूर्णाङ्क parsedate(अचर वर्ण *date, समय_प्रकार *output)
{
  curl_off_t seconds = 0;
  अचर वर्ण *indate = date; /* save the original pointer */
  पूर्णाङ्क part = 0; /* max 6 parts */
  पूर्णाङ्क rc = 0;
  संरचना when w;
  initwhen(&w);

  जबसम्म(*date && (part < 6)) {
    skip(&date);

    यदि(ISALPHA(*date))
      rc = datestring(&date, &w);
    अन्यथा यदि(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    यदि(rc)
      फर्काऊ rc;

    part++;
  }

  यदि(datecheck(&w))
    फर्काऊ PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  फर्काऊ rc;
}
#अन्यथा
/* disabled */
स्थिर पूर्णाङ्क parsedate(अचर वर्ण *date, समय_प्रकार *output)
{
  (शून्य)date;
  *output = 0;
  फर्काऊ PARSEDATE_OK; /* a lie */
}
#endif

समय_प्रकार curl_getdate(अचर वर्ण *p, अचर समय_प्रकार *unused)
{
  समय_प्रकार parsed = -1;
  पूर्णाङ्क rc;
  (शून्य)unused; /* legacy argument from the past that we ignore */

  यदि(!p)
    फर्काऊ -1;

  rc = parsedate(p, &parsed);
  यदि(rc == PARSEDATE_OK) {
    यदि(parsed == (समय_प्रकार)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    फर्काऊ parsed;
  }
  /* everything else is fail */
  फर्काऊ -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

पूर्णाङ्क Curl_getdate_capped(अचर वर्ण *p, समय_प्रकार *tp)
{
  पूर्णाङ्क rc = parsedate(p, tp);
  फर्काऊ (rc == PARSEDATE_FAIL);
}
