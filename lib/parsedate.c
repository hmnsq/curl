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
#समावेश "curl_setup.h"

#समावेश "parsedate.h"
#समावेश "curlx/strparse.h"
#समावेश "curlx/strcopy.h"

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

#अगर !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_FILE) || defined(USE_GNUTLS)
/* These names are also used by FTP and FILE code */
स्थिर अक्षर * स्थिर Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
स्थिर अक्षर * स्थिर Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#पूर्ण_अगर

#घोषणा PARSEDATE_OK     0
#घोषणा PARSEDATE_FAIL   (-1)

#अगर_अघोषित CURL_DISABLE_PARSEDATE

#अगर SIZEOF_TIME_T < 5
#घोषणा PARSEDATE_LATER  1
#पूर्ण_अगर
#अगर SIZEOF_TIME_T < 5 || defined(HAVE_TIME_T_UNSIGNED)
#घोषणा PARSEDATE_SOONER 2
#पूर्ण_अगर

काष्ठा tzinfo {
  uint32_t tz;
  int16_t offset; /* +/- in minutes */
};

#घोषणा DST (-60)     /* offset for daylight savings time */

/* alpha-sorted list of single-letter time zones, A - Z */
अटल स्थिर int16_t tzone[] = {
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

#घोषणा MKTZ(a,b,c,d) (((a)<<24) | ((b)<<16) | ((c)<<8) | (d))

/* three-letter time zones */
अटल स्थिर काष्ठा tzinfo tzthree[] = {
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
अटल स्थिर काष्ठा tzinfo tzfoura[] = {
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

#घोषणा LOWERCASE(x) ((x) | 0x20)

/* returns:
   -1 no day
   0 monday - 6 sunday

   @unittest 4781
*/
UNITTEST पूर्णांक checkday(स्थिर अक्षर *check, माप_प्रकार len);
UNITTEST पूर्णांक checkday(स्थिर अक्षर *check, माप_प्रकार len)
{
  पूर्णांक day = -1;
  अगर(len < 3)
    वापस -1; /* too short */

  चयन(LOWERCASE(check[1])) {
  हाल 'o': /* monday */
    अगर((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    अवरोध;
  हाल 'u': /* tuesday or sunday */
    अगर((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    अन्यथा अगर((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    अवरोध;
  हाल 'e': /* wednesday */
    अगर((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    अवरोध;
  हाल 'h': /* thursday */
    अगर((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    अवरोध;
  हाल 'r': /* friday */
    अगर((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    अवरोध;
  हाल 'a': /* saturday */
    अगर((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    अवरोध;
  }
  अगर((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    अटल स्थिर अक्षर * स्थिर daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    माप_प्रकार wlen = म_माप(daysuffix[day]);
    अगर(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      वापस -1;
  }
  वापस day;
}

/* @unittest 4781 */

UNITTEST पूर्णांक checkmonth(स्थिर अक्षर *check, माप_प्रकार len);
UNITTEST पूर्णांक checkmonth(स्थिर अक्षर *check, माप_प्रकार len)
{
  अगर(len != 3)
    वापस -1; /* not a month */

  चयन(LOWERCASE(check[2])) {
  हाल 'n': /* jan, jun */
    अगर(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      अगर(c2 == 'a')
        वापस 0;
      अन्यथा अगर(c2 == 'u')
        वापस 5;
    }
    अवरोध;
  हाल 'b': /* feb */
    अगर((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      वापस 1;
    अवरोध;
  हाल 'r': /* mar, apr */
    अगर(LOWERCASE(check[0]) == 'm') {
      अगर(LOWERCASE(check[1]) == 'a')
        वापस 2;
    }
    अन्यथा अगर((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      वापस 3;
    अवरोध;
  हाल 'y': /* may */
    अगर((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      वापस 4;
    अवरोध;
  हाल 'l': /* jul */
    अगर((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      वापस 6;
    अवरोध;
  हाल 'g': /* aug */
    अगर((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      वापस 7;
    अवरोध;
  हाल 'p': /* sep */
    अगर((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      वापस 8;
    अवरोध;
  हाल 't': /* oct */
    अगर((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      वापस 9;
    अवरोध;
  हाल 'v': /* nov */
    अगर((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      वापस 10;
    अवरोध;
  हाल 'c': /* dec */
    अगर((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      वापस 11;
    अवरोध;
  }
  वापस -1; /* return the offset or -1, no real offset is -1 */
}

अटल पूर्णांक tzcompare(स्थिर व्योम *m1, स्थिर व्योम *m2)
{
  स्थिर काष्ठा tzinfo *tz1 = m1;
  स्थिर काष्ठा tzinfo *tz2 = m2;
  वापस tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST पूर्णांक checktz(स्थिर अक्षर *check, माप_प्रकार len);
UNITTEST पूर्णांक checktz(स्थिर अक्षर *check, माप_प्रकार len)
{
  अक्षर first;
  अगर(len > 4)
    वापस -1;
  first = check[0];
  अगर(first < 'A')
    वापस -1;
  अगर(len == 1) {
    /* short-cut single letter names */
    अगर((first > 'Z') || (first == 'J'))
      वापस -1; /* no such tz */
    वापस tzone[first - 'A'] * 60;
  }
  अन्यथा अगर(len == 2) {
    /* two-letter names */
    अगर(check[1] == 'T') {
      अगर(first == 'N')
        वापस 11 * 60 * 60;
      अन्यथा अगर(first == 'U')
        वापस 0;
    }
    वापस -1; /* nope */
  }
  अन्यथा अगर(len == 3) {
    /* three-letter name */
    स्थिर काष्ठा tzinfo *what;
    काष्ठा tzinfo find;
    अगर(first > 'Y')
      वापस -1;
    find.tz = MKTZ(first, check[1], check[2], 0);
    what = द्वा_खोज(&find, tzthree, CURL_ARRAYSIZE(tzthree),
                   माप(tzthree[0]), tzcompare);
    अगर(what)
      वापस what->offset * 60;
  }
  अन्यथा {
    /* four-letter name */
    स्थिर काष्ठा tzinfo *what;
    काष्ठा tzinfo find;
    अगर(first > 'W')
      वापस -1;
    find.tz = MKTZ(first, check[1], check[2], check[3]);
    what = द्वा_खोज(&find, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   माप(tzfoura[0]), tzcompare);
    अगर(what)
      वापस what->offset * 60;
  }
  वापस -1;
}

अटल व्योम skip(स्थिर अक्षर **date)
{
  /* skip everything that are not letters or digits */
  जबतक(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
काष्ठा when {
  पूर्णांक wday;  /* day of the week, 0-6 (mon-sun) */
  पूर्णांक mon;   /* month of the year, 0-11 */
  पूर्णांक mday;  /* day of month, 1 - 31 */
  पूर्णांक dsecs; /* number of seconds into the day */
  पूर्णांक year;  /* year, >= 1583 */
  पूर्णांक tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#घोषणा LEAP_DAYS_BEFORE_1969 477
#घोषणा SECONDS_PER_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
अटल curl_off_t time2epoch(काष्ठा when *w)
{
  अटल स्थिर पूर्णांक cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  पूर्णांक y = w->year - (w->mon <= 1);
  पूर्णांक leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  वापस (days * SECONDS_PER_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
अटल पूर्णांक oneortwodigit(स्थिर अक्षर *date, स्थिर अक्षर **endp)
{
  पूर्णांक अंक = date[0] - '0';
  अगर(ISDIGIT(date[1])) {
    *endp = &date[2];
    वापस (अंक * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  वापस अंक;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
अटल bool match_time(स्थिर अक्षर *date, काष्ठा when *w, अक्षर **endp)
{
  स्थिर अक्षर *p;
  पूर्णांक hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  अगर((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    अगर(mm < 60) {
      अगर((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        अगर(ss <= 60) {
          /* valid HH:MM:SS */
          जाओ match;
        }
      }
      अन्यथा {
        /* valid HH:MM */
        जाओ match;
      }
    }
  }
  वापस FALSE; /* not a time string */
match:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (अक्षर *)CURL_UNCONST(p);
  वापस TRUE;
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
#घोषणा NAME_LEN 10

अटल व्योम initwhen(काष्ठा when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

अटल पूर्णांक datestring(स्थिर अक्षर **datep, काष्ठा when *w)
{
  /* a name coming up */
  माप_प्रकार len = 0;
  स्थिर अक्षर *p = *datep;
  जबतक(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  अगर(len != NAME_LEN) {
    अगर(w->wday == -1) {
      w->wday = checkday(p, len);
      अगर(w->wday != -1)
        जाओ found;
    }
    अगर(w->mon == -1) {
      w->mon = checkmonth(p, len);
      अगर(w->mon != -1)
        जाओ found;
    }
    अगर(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      अगर(w->tzoff != -1)
        जाओ found;
    }
  }
  वापस PARSEDATE_FAIL; /* bad string */
found:
  *datep += len;
  वापस PARSEDATE_OK;
}

अटल पूर्णांक datenum(स्थिर अक्षर *indate, स्थिर अक्षर **datep, काष्ठा when *w)
{
  /* a digit */
  अचिन्हित पूर्णांक val;
  अक्षर *end;
  स्थिर अक्षर *date = *datep;

  अगर((w->dsecs == -1) && match_time(date, w, &end)) {
    /* time stamp */
    date = end;
  }
  अन्यथा {
    bool found = FALSE;
    curl_off_t lval;
    पूर्णांक अंक_digits = 0;
    स्थिर अक्षर *p = *datep;
    अगर(curlx_str_संख्या(&p, &lval, 99999999))
      वापस PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    अंक_digits = (पूर्णांक)(p - *datep);
    val = (अचिन्हित पूर्णांक)lval;

    अगर((w->tzoff == -1) &&
       (अंक_digits == 4) &&
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

    अन्यथा अगर((अंक_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      found = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      अगर(w->mon > 11)
        वापस PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    अगर(!found && (w->mday == -1)) {
      अगर((val > 0) && (val < 32)) {
        w->mday = val;
        found = TRUE;
      }
    }

    अगर(!found && (w->year == -1)) {
      w->year = val;
      found = TRUE;
      अगर(w->year < 100) {
        अगर(w->year > 70)
          w->year += 1900;
        अन्यथा
          w->year += 2000;
      }
    }

    अगर(!found)
      वापस PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  वापस PARSEDATE_OK;
}

अटल bool datecheck(काष्ठा when *w)
{
  अगर(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  अगर((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    वापस TRUE;

  वापस FALSE;
}

अटल व्योम tzadjust(curl_off_t *tp, काष्ठा when *w)
{
  अगर(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  अगर((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  अन्यथा
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

अटल पूर्णांक mktimet(curl_off_t seconds, समय_प्रकार *output)
{
#अगर SIZEOF_TIME_T < 5
  अगर(seconds > TIME_T_MAX) {
    *output = TIME_T_MAX;
    वापस PARSEDATE_LATER;
  }
  अन्यथा अगर(seconds < TIME_T_MIN) {
    *output = TIME_T_MIN;
    वापस PARSEDATE_SOONER;
  }
#या_अगर defined(HAVE_TIME_T_UNSIGNED)
  अगर(seconds < 0) {
    *output = 0;
    वापस PARSEDATE_SOONER;
  }
#पूर्ण_अगर
  *output = (समय_प्रकार)seconds;
  वापस PARSEDATE_OK;
}

अटल पूर्णांक parsedate(स्थिर अक्षर *date, समय_प्रकार *output)
{
  curl_off_t seconds = 0;
  स्थिर अक्षर *indate = date; /* save the original pointer */
  पूर्णांक part = 0; /* max 6 parts */
  पूर्णांक rc = 0;
  काष्ठा when w;
  initwhen(&w);

  जबतक(*date && (part < 6)) {
    skip(&date);

    अगर(ISALPHA(*date))
      rc = datestring(&date, &w);
    अन्यथा अगर(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    अगर(rc)
      वापस rc;

    part++;
  }

  अगर(datecheck(&w))
    वापस PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  वापस rc;
}
#अन्यथा
/* disabled */
अटल पूर्णांक parsedate(स्थिर अक्षर *date, समय_प्रकार *output)
{
  (व्योम)date;
  *output = 0;
  वापस PARSEDATE_OK; /* a lie */
}
#पूर्ण_अगर

समय_प्रकार curl_getdate(स्थिर अक्षर *p, स्थिर समय_प्रकार *unused)
{
  समय_प्रकार parsed = -1;
  पूर्णांक rc;
  (व्योम)unused; /* legacy argument from the past that we ignore */

  अगर(!p)
    वापस -1;

  rc = parsedate(p, &parsed);
  अगर(rc == PARSEDATE_OK) {
    अगर(parsed == (समय_प्रकार)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    वापस parsed;
  }
  /* everything else is fail */
  वापस -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

पूर्णांक Curl_getdate_capped(स्थिर अक्षर *p, समय_प्रकार *tp)
{
  पूर्णांक rc = parsedate(p, tp);
  वापस (rc == PARSEDATE_FAIL);
}
