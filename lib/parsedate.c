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
ثابت حرف * ثابت Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
ثابت حرف * ثابت Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#اگر_ختم

#تعریف_کرو PARSEDATE_OK     0
#تعریف_کرو PARSEDATE_FAIL   (-1)

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PARSEDATE

#اگر SIZEOF_وقت_T < 5
#تعریف_کرو PARSEDATE_LATER  1
#اگر_ختم
#اگر SIZEOF_وقت_T < 5 || defined(HAVE_وقت_T_UNSIGNED)
#تعریف_کرو PARSEDATE_SOONER 2
#اگر_ختم

ساخت tzinfo {
  uint32_t tz;
  int16_t انحراف; /* +/- in minutes */
};

#تعریف_کرو DST (-60)     /* offset for daylight savings time */

/* alpha-sorted list of single-letter time zones, A - Z */
ساکن ثابت int16_t tzone[] = {
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
   8 * 60,            /*یکساں*/
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

#تعریف_کرو MKTZ(a,b,c,d) (((a)<<24) | ((b)<<16) | ((c)<<8) | (d))

/* three-letter time zones */
ساکن ثابت ساخت tzinfo tzthree[] = {
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
ساکن ثابت ساخت tzinfo tzfoura[] = {
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

#تعریف_کرو LOWERCASE(x) ((x) | 0x20)

/* returns:
   -1 no day
   0 monday - 6 sunday

   @unittest 4781
*/
UNITTEST صحیح checkday(ثابت حرف *check, size_t len);
UNITTEST صحیح checkday(ثابت حرف *check, size_t len)
{
  صحیح day = -1;
  اگر(len < 3)
    واپس -1; /* too short */

  انتخاب(LOWERCASE(check[1])) {
  حالت 'o': /* monday */
    اگر((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    توڑو;
  حالت 'u': /* tuesday or sunday */
    اگر((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    ورنہ اگر((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    توڑو;
  حالت 'e': /* wednesday */
    اگر((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    توڑو;
  حالت 'h': /* thursday */
    اگر((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    توڑو;
  حالت 'r': /* friday */
    اگر((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    توڑو;
  حالت 'a': /* saturday */
    اگر((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    توڑو;
  }
  اگر((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    ساکن ثابت حرف * ثابت daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    size_t wlen = لڑی_لمبائی(daysuffix[day]);
    اگر(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      واپس -1;
  }
  واپس day;
}

/* @unittest 4781 */

UNITTEST صحیح checkmonth(ثابت حرف *check, size_t len);
UNITTEST صحیح checkmonth(ثابت حرف *check, size_t len)
{
  اگر(len != 3)
    واپس -1; /* not a month */

  انتخاب(LOWERCASE(check[2])) {
  حالت 'n': /* jan, jun */
    اگر(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      اگر(c2 == 'a')
        واپس 0;
      ورنہ اگر(c2 == 'u')
        واپس 5;
    }
    توڑو;
  حالت 'b': /* feb */
    اگر((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      واپس 1;
    توڑو;
  حالت 'r': /* mar, apr */
    اگر(LOWERCASE(check[0]) == 'm') {
      اگر(LOWERCASE(check[1]) == 'a')
        واپس 2;
    }
    ورنہ اگر((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      واپس 3;
    توڑو;
  حالت 'y': /* may */
    اگر((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      واپس 4;
    توڑو;
  حالت 'l': /* jul */
    اگر((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      واپس 6;
    توڑو;
  حالت 'g': /* aug */
    اگر((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      واپس 7;
    توڑو;
  حالت 'p': /* sep */
    اگر((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      واپس 8;
    توڑو;
  حالت 't': /* oct */
    اگر((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      واپس 9;
    توڑو;
  حالت 'v': /* nov */
    اگر((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      واپس 10;
    توڑو;
  حالت 'c': /* dec */
    اگر((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      واپس 11;
    توڑو;
  }
  واپس -1; /* return the offset or -1, no real offset is -1 */
}

ساکن صحیح tzcompare(ثابت خالی *m1, ثابت خالی *m2)
{
  ثابت ساخت tzinfo *tz1 = m1;
  ثابت ساخت tzinfo *tz2 = m2;
  واپس tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST صحیح checktz(ثابت حرف *check, size_t len);
UNITTEST صحیح checktz(ثابت حرف *check, size_t len)
{
  حرف پہلا;
  اگر(len > 4)
    واپس -1;
  پہلا = check[0];
  اگر(پہلا < 'A')
    واپس -1;
  اگر(len == 1) {
    /* short-cut single letter names */
    اگر((پہلا > 'Z') || (پہلا == 'J'))
      واپس -1; /* no such tz */
    واپس tzone[پہلا - 'A'] * 60;
  }
  ورنہ اگر(len == 2) {
    /* two-letter names */
    اگر(check[1] == 'T') {
      اگر(پہلا == 'N')
        واپس 11 * 60 * 60;
      ورنہ اگر(پہلا == 'U')
        واپس 0;
    }
    واپس -1; /* nope */
  }
  ورنہ اگر(len == 3) {
    /* three-letter name */
    ثابت ساخت tzinfo *what;
    ساخت tzinfo ڈھونڈو;
    اگر(پہلا > 'Y')
      واپس -1;
    ڈھونڈو.tz = MKTZ(پہلا, check[1], check[2], 0);
    what = bsearch(&ڈھونڈو, tzthree, CURL_ARRAYSIZE(tzthree),
                   حجم(tzthree[0]), tzcompare);
    اگر(what)
      واپس what->انحراف * 60;
  }
  ورنہ {
    /* four-letter name */
    ثابت ساخت tzinfo *what;
    ساخت tzinfo ڈھونڈو;
    اگر(پہلا > 'W')
      واپس -1;
    ڈھونڈو.tz = MKTZ(پہلا, check[1], check[2], check[3]);
    what = bsearch(&ڈھونڈو, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   حجم(tzfoura[0]), tzcompare);
    اگر(what)
      واپس what->انحراف * 60;
  }
  واپس -1;
}

ساکن خالی skip(ثابت حرف **date)
{
  /* skip everything that are not letters or digits */
  جب(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
ساخت when {
  صحیح wday;  /* day of the week, 0-6 (mon-sun) */
  صحیح mon;   /* month of the year, 0-11 */
  صحیح mday;  /* day of month, 1 - 31 */
  صحیح dsecs; /* number of seconds into the day */
  صحیح year;  /* year, >= 1583 */
  صحیح tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#تعریف_کرو LEAP_DAYS_BEFORE_1969 477
#تعریف_کرو SECONDS_فی_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
ساکن curl_off_t time2epoch(ساخت when *w)
{
  ساکن ثابت صحیح cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  صحیح y = w->year - (w->mon <= 1);
  صحیح leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  واپس (days * SECONDS_فی_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
ساکن صحیح oneortwodigit(ثابت حرف *date, ثابت حرف **endp)
{
  صحیح عدد = date[0] - '0';
  اگر(ISDIGIT(date[1])) {
    *endp = &date[2];
    واپس (عدد * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  واپس عدد;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
ساکن bool مطابقت_وقت(ثابت حرف *date, ساخت when *w, حرف **endp)
{
  ثابت حرف *p;
  صحیح hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  اگر((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    اگر(mm < 60) {
      اگر((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        اگر(ss <= 60) {
          /* valid HH:MM:SS */
          جاؤ مطابقت;
        }
      }
      ورنہ {
        /* valid HH:MM */
        جاؤ مطابقت;
      }
    }
  }
  واپس FALSE; /* not a time string */
مطابقت:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (حرف *)CURL_UNCONST(p);
  واپس TRUE;
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
#تعریف_کرو NAME_LEN 10

ساکن خالی initwhen(ساخت when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

ساکن صحیح datestring(ثابت حرف **datep, ساخت when *w)
{
  /* a name coming up */
  size_t len = 0;
  ثابت حرف *p = *datep;
  جب(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  اگر(len != NAME_LEN) {
    اگر(w->wday == -1) {
      w->wday = checkday(p, len);
      اگر(w->wday != -1)
        جاؤ ملا;
    }
    اگر(w->mon == -1) {
      w->mon = checkmonth(p, len);
      اگر(w->mon != -1)
        جاؤ ملا;
    }
    اگر(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      اگر(w->tzoff != -1)
        جاؤ ملا;
    }
  }
  واپس PARSEDATE_FAIL; /* bad string */
ملا:
  *datep += len;
  واپس PARSEDATE_OK;
}

ساکن صحیح datenum(ثابت حرف *indate, ثابت حرف **datep, ساخت when *w)
{
  /* a digit */
  غیرعلامتی صحیح val;
  حرف *انتہا;
  ثابت حرف *date = *datep;

  اگر((w->dsecs == -1) && مطابقت_وقت(date, w, &انتہا)) {
    /* time stamp */
    date = انتہا;
  }
  ورنہ {
    bool ملا = FALSE;
    curl_off_t lval;
    صحیح عدد_digits = 0;
    ثابت حرف *p = *datep;
    اگر(curlx_str_نمبر(&p, &lval, 99999999))
      واپس PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    عدد_digits = (صحیح)(p - *datep);
    val = (غیرعلامتی صحیح)lval;

    اگر((w->tzoff == -1) &&
       (عدد_digits == 4) &&
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
      ملا = TRUE;
      w->tzoff = ((val / 100 * 60) + (val % 100)) * 60;

      /* the + and - prefix indicates the local time compared to GMT,
         this we need their reversed math to get what we want */
      w->tzoff = date[-1] == '+' ? -w->tzoff : w->tzoff;
    }

    ورنہ اگر((عدد_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      ملا = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      اگر(w->mon > 11)
        واپس PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    اگر(!ملا && (w->mday == -1)) {
      اگر((val > 0) && (val < 32)) {
        w->mday = val;
        ملا = TRUE;
      }
    }

    اگر(!ملا && (w->year == -1)) {
      w->year = val;
      ملا = TRUE;
      اگر(w->year < 100) {
        اگر(w->year > 70)
          w->year += 1900;
        ورنہ
          w->year += 2000;
      }
    }

    اگر(!ملا)
      واپس PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  واپس PARSEDATE_OK;
}

ساکن bool datecheck(ساخت when *w)
{
  اگر(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  اگر((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    واپس TRUE;

  واپس FALSE;
}

ساکن خالی tzadjust(curl_off_t *tp, ساخت when *w)
{
  اگر(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  اگر((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  ورنہ
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

ساکن صحیح mktimet(curl_off_t seconds, time_t *output)
{
#اگر SIZEOF_وقت_T < 5
  اگر(seconds > وقت_T_MAX) {
    *output = وقت_T_MAX;
    واپس PARSEDATE_LATER;
  }
  ورنہ اگر(seconds < وقت_T_MIN) {
    *output = وقت_T_MIN;
    واپس PARSEDATE_SOONER;
  }
#وگرنہ_اگر defined(HAVE_وقت_T_UNSIGNED)
  اگر(seconds < 0) {
    *output = 0;
    واپس PARSEDATE_SOONER;
  }
#اگر_ختم
  *output = (time_t)seconds;
  واپس PARSEDATE_OK;
}

ساکن صحیح parsedate(ثابت حرف *date, time_t *output)
{
  curl_off_t seconds = 0;
  ثابت حرف *indate = date; /* save the original pointer */
  صحیح part = 0; /* max 6 parts */
  صحیح rc = 0;
  ساخت when w;
  initwhen(&w);

  جب(*date && (part < 6)) {
    skip(&date);

    اگر(ISALPHA(*date))
      rc = datestring(&date, &w);
    ورنہ اگر(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    اگر(rc)
      واپس rc;

    part++;
  }

  اگر(datecheck(&w))
    واپس PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  واپس rc;
}
#ورنہ
/* disabled */
ساکن صحیح parsedate(ثابت حرف *date, time_t *output)
{
  (خالی)date;
  *output = 0;
  واپس PARSEDATE_OK; /* a lie */
}
#اگر_ختم

time_t curl_getdate(ثابت حرف *p, ثابت time_t *غیر_مستعمل)
{
  time_t parsed = -1;
  صحیح rc;
  (خالی)غیر_مستعمل; /* legacy argument from the past that we ignore */

  اگر(!p)
    واپس -1;

  rc = parsedate(p, &parsed);
  اگر(rc == PARSEDATE_OK) {
    اگر(parsed == (time_t)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    واپس parsed;
  }
  /* everything else is fail */
  واپس -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

صحیح Curl_getdate_capped(ثابت حرف *p, time_t *tp)
{
  صحیح rc = parsedate(p, tp);
  واپس (rc == PARSEDATE_FAIL);
}
