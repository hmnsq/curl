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
#অন্তর্ভাৱ "curl_setup.h"

#ifndef CURL_DISABLE_FTP

/**
 * Now implemented:
 *
 * 1) Unix version 1
 * drwxr-xr-x 1 user01 ftp  512 Jan 29 23:32 prog
 * 2) Unix version 2
 * drwxr-xr-x 1 user01 ftp  512 Jan 29 1997  prog
 * 3) Unix version 3
 * drwxr-xr-x 1      1   1  512 Jan 29 23:32 prog
 * 4) Unix symlink
 * lrwxr-xr-x 1 user01 ftp  512 Jan 29 23:32 prog -> prog2000
 * 5) DOS style
 * 01-29-97 11:32PM <DIR> prog
 */

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "fileinfo.h"
#অন্তর্ভাৱ "llist.h"
#অন্তর্ভাৱ "ftp.h"
#অন্তর্ভাৱ "ftp-int.h"
#অন্তর্ভাৱ "ftplistparser.h"
#অন্তর্ভাৱ "curl_fnmatch.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "curlx/strparse.h"

প্রকারনির্দেশ গণনা {
  PL_UNIX_TOTALSIZE = 0,
  PL_UNIX_FILETYPE,
  PL_UNIX_PERMISSION,
  PL_UNIX_HLINKS,
  PL_UNIX_USER,
  PL_UNIX_GROUP,
  PL_UNIX_SIZE,
  PL_UNIX_TIME,
  PL_UNIX_FILENAME,
  PL_UNIX_SYMLINK
} pl_unix_mainstate;

প্রকারনির্দেশ সঙ্ঘ {
  গণনা {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } total_dirsize;

  গণনা {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_সংখ্যা
  } hlinks;

  গণনা {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  গণনা {
    PL_UNIX_GROUP_PRESPACE = 0,
    PL_UNIX_GROUP_NAME
  } group;

  গণনা {
    PL_UNIX_SIZE_PRESPACE = 0,
    PL_UNIX_SIZE_সংখ্যা
  } size;

  গণনা {
    PL_UNIX_TIME_PREPART1 = 0,
    PL_UNIX_TIME_PART1,
    PL_UNIX_TIME_PREPART2,
    PL_UNIX_TIME_PART2,
    PL_UNIX_TIME_PREPART3,
    PL_UNIX_TIME_PART3
  } time;

  গণনা {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  গণনা {
    PL_UNIX_SYMLINK_PRESPACE = 0,
    PL_UNIX_SYMLINK_NAME,
    PL_UNIX_SYMLINK_PRETARGET1,
    PL_UNIX_SYMLINK_PRETARGET2,
    PL_UNIX_SYMLINK_PRETARGET3,
    PL_UNIX_SYMLINK_PRETARGET4,
    PL_UNIX_SYMLINK_TARGET,
    PL_UNIX_SYMLINK_WINDOWSEOL
  } symlink;
} pl_unix_substate;

প্রকারনির্দেশ গণনা {
  PL_WINNT_DATE = 0,
  PL_WINNT_TIME,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

প্রকারনির্দেশ সঙ্ঘ {
  গণনা {
    PL_WINNT_TIME_PRESPACE = 0,
    PL_WINNT_TIME_TIME
  } time;
  গণনা {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  গণনা {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
সংরচনা ftp_parselist_data {
  গণনা {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  সঙ্ঘ {
    সংরচনা {
      pl_unix_mainstate main;
      pl_unix_substate sub;
    } UNIX;

    সংরচনা {
      pl_winNT_mainstate main;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  সংরচনা fileinfo *file_data;
  unsigned পূর্ণাঙ্ক item_length;
  size_t item_offset;
  সংরচনা {
    size_t filename;
    size_t user;
    size_t group;
    size_t time;
    size_t perm;
    size_t symlink_target;
  } offsets;
};

স্থির শূন্য fileinfo_dtor(শূন্য *user, শূন্য *element)
{
  (শূন্য)user;
  Curl_fileinfo_cleanup(element);
}

শূন্য Curl_wildcard_init(সংরচনা WildcardData *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

শূন্য Curl_wildcard_dtor(সংরচনা WildcardData **wcp)
{
  সংরচনা WildcardData *wc = *wcp;
  যদি(!wc)
    ঘূৰাই;

  যদি(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = রিক্ত;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, রিক্ত);
  curlx_safefree(wc->path);
  curlx_safefree(wc->pattern);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = রিক্ত;
}

সংরচনা ftp_parselist_data *Curl_ftp_parselist_data_alloc(শূন্য)
{
  ঘূৰাই curlx_calloc(1, পরিমাণ(সংরচনা ftp_parselist_data));
}

শূন্য Curl_ftp_parselist_data_free(সংরচনা ftp_parselist_data **parserp)
{
  সংরচনা ftp_parselist_data *parser = *parserp;
  যদি(parser)
    Curl_fileinfo_cleanup(parser->file_data);
  curlx_free(parser);
  *parserp = রিক্ত;
}

CURLcode Curl_ftp_parselist_geterror(সংরচনা ftp_parselist_data *pl_data)
{
  ঘূৰাই pl_data->error;
}

#define FTP_LP_MALFORMATED_PERM 0x01000000

স্থির unsigned পূর্ণাঙ্ক ftp_pl_get_permission(ধ্রুৱ ৱর্ণ *str)
{
  unsigned পূর্ণাঙ্ক permissions = 0;
  /* USER */
  যদি(str[0] == 'r')
    permissions |= 1 << 8;
  অন্যথা যদি(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  যদি(str[1] == 'w')
    permissions |= 1 << 7;
  অন্যথা যদি(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  যদি(str[2] == 'x')
    permissions |= 1 << 6;
  অন্যথা যদি(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  অন্যথা যদি(str[2] == 'S')
    permissions |= 1 << 11;
  অন্যথা যদি(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* GROUP */
  যদি(str[3] == 'r')
    permissions |= 1 << 5;
  অন্যথা যদি(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  যদি(str[4] == 'w')
    permissions |= 1 << 4;
  অন্যথা যদি(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  যদি(str[5] == 'x')
    permissions |= 1 << 3;
  অন্যথা যদি(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  অন্যথা যদি(str[5] == 'S')
    permissions |= 1 << 10;
  অন্যথা যদি(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  যদি(str[6] == 'r')
    permissions |= 1 << 2;
  অন্যথা যদি(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  যদি(str[7] == 'w')
    permissions |= 1 << 1;
  অন্যথা যদি(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  যদি(str[8] == 'x')
    permissions |= 1;
  অন্যথা যদি(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  অন্যথা যদি(str[8] == 'T')
    permissions |= 1 << 9;
  অন্যথা যদি(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  ঘূৰাই permissions;
}

স্থির CURLcode ftp_pl_ঢোকাও_finfo(সংরচনা Curl_easy *data,
                                    সংরচনা fileinfo *infop)
{
  curl_fnmatch_callback compare;
  সংরচনা WildcardData *wc = data->wildcard;
  সংরচনা ftp_wc *ftpwc = wc->ftpwc;
  সংরচনা Curl_llist *llist = &wc->filelist;
  সংরচনা ftp_parselist_data *parser = ftpwc->parser;
  bool add = TRUE;
  সংরচনা curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  ৱর্ণ *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->strings.group  = parser->offsets.group ?
                          str + parser->offsets.group : রিক্ত;
  finfo->strings.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : রিক্ত;
  finfo->strings.target = parser->offsets.symlink_target &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_target : রিক্ত;
  finfo->strings.time   = str + parser->offsets.time;
  finfo->strings.user   = parser->offsets.user ?
                          str + parser->offsets.user : রিক্ত;

  /* get correct fnmatch callback */
  compare = data->set.fnmatch;
  যদি(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  যদি(strchr(finfo->filename, '/'))
    add = FALSE;
  অন্যথা {
    /* a real file name cannot contain a slash */
    সংরচনা Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fnmatch_data);
    যদি(compare(data->set.fnmatch_data, wc->pattern, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      যদি((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->strings.target &&
         strstr(finfo->strings.target, " -> ")) {
        add = FALSE;
      }
    }
    অন্যথা {
      add = FALSE;
    }
    CURL_CBAPI_END(&guard);
  }

  যদি(add) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  অন্যথা {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_data = রিক্ত;
  ঘূৰাই CURLE_OK;
}

#define MAX_FTPLIST_BUFFER 10000 /* arbitrarily set */

স্থির CURLcode unix_filetype(ধ্রুৱ ৱর্ণ c, curlfiletype *t)
{
  ৱিকল্প(c) {
  পক্ষ '-':
    *t = CURLFILETYPE_FILE;
    ভঙ্গ;
  পক্ষ 'd':
    *t = CURLFILETYPE_DIRECTORY;
    ভঙ্গ;
  পক্ষ 'l':
    *t = CURLFILETYPE_SYMLINK;
    ভঙ্গ;
  পক্ষ 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    ভঙ্গ;
  পক্ষ 's':
    *t = CURLFILETYPE_SOCKET;
    ভঙ্গ;
  পক্ষ 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    ভঙ্গ;
  পক্ষ 'b':
    *t = CURLFILETYPE_DEVICE_BLOCK;
    ভঙ্গ;
  পক্ষ 'D':
    *t = CURLFILETYPE_DOOR;
    ভঙ্গ;
  স্ৱভাৱ:
    ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode parse_unix_totalsize(সংরচনা ftp_parselist_data *parser,
                                     সংরচনা fileinfo *infop,
                                     ধ্রুৱ ৱর্ণ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ৱর্ণ *mem = curlx_dyn_ptr(&infop->buf);
  ৱিকল্প(parser->state.UNIX.sub.total_dirsize) {
  পক্ষ PL_UNIX_TOTALSIZE_INIT:
    যদি(c == 't') {
      parser->state.UNIX.sub.total_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_length++;
    }
    অন্যথা {
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    ভঙ্গ;
  পক্ষ PL_UNIX_TOTALSIZE_READING:
    parser->item_length++;
    যদি(c == '\r') {
      parser->item_length--;
      যদি(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    অন্যথা যদি(c == '\n') {
      mem[parser->item_length - 1] = 0;
      যদি(!strncmp("total ", mem, 6)) {
        ধ্রুৱ ৱর্ণ *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        যেতিয়ালৈ(ISDIGIT(*endptr))
          endptr++;
        যদি(*endptr) {
          ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.main = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      অন্যথা
        ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
    }
    ভঙ্গ;
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode parse_unix_permission(সংরচনা ftp_parselist_data *parser,
                                      সংরচনা fileinfo *infop,
                                      ধ্রুৱ ৱর্ণ c)
{
  ৱর্ণ *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_length++;
  যদি((parser->item_length <= 9) && !strchr("rwx-tTsS", c))
    ঘূৰাই CURLE_FTP_BAD_FILE_LIST;

  অন্যথা যদি(parser->item_length == 10) {
    unsigned পূর্ণাঙ্ক perm;
    যদি(c != ' ')
      ঘূৰাই CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_offset);
    যদি(perm & FTP_LP_MALFORMATED_PERM)
      ঘূৰাই CURLE_FTP_BAD_FILE_LIST;

    parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_PERM;
    parser->file_data->info.perm = perm;
    parser->offsets.perm = parser->item_offset;

    parser->item_length = 0;
    parser->state.UNIX.main = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode parse_unix_hlinks(সংরচনা ftp_parselist_data *parser,
                                  সংরচনা fileinfo *infop,
                                  ধ্রুৱ ৱর্ণ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ৱর্ণ *mem = curlx_dyn_ptr(&infop->buf);

  ৱিকল্প(parser->state.UNIX.sub.hlinks) {
  পক্ষ PL_UNIX_HLINKS_PRESPACE:
    যদি(c != ' ') {
      যদি(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_সংখ্যা;
      }
      অন্যথা
        ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
    }
    ভঙ্গ;
  পক্ষ PL_UNIX_HLINKS_সংখ্যা:
    parser->item_length++;
    যদি(c == ' ') {
      ধ্রুৱ ৱর্ণ *p = &mem[parser->item_offset];
      curl_off_t hlinks;
      mem[parser->item_offset + parser->item_length - 1] = 0;

      যদি(!curlx_str_সংখ্যা(&p, &hlinks, LONG_MAX)) {
        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_HLINKCOUNT;
        parser->file_data->info.hardlinks = (long)hlinks;
      }
      parser->item_length = 0;
      parser->item_offset = 0;
      parser->state.UNIX.main = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    অন্যথা যদি(!ISDIGIT(c))
      ঘূৰাই CURLE_FTP_BAD_FILE_LIST;

    ভঙ্গ;
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode parse_unix_user(সংরচনা ftp_parselist_data *parser,
                                সংরচনা fileinfo *infop,
                                ধ্রুৱ ৱর্ণ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ৱর্ণ *mem = curlx_dyn_ptr(&infop->buf);
  ৱিকল্প(parser->state.UNIX.sub.user) {
  পক্ষ PL_UNIX_USER_PRESPACE:
    যদি(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    ভঙ্গ;
  পক্ষ PL_UNIX_USER_PARSING:
    parser->item_length++;
    যদি(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.user = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_GROUP;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    ভঙ্গ;
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode parse_unix_group(সংরচনা ftp_parselist_data *parser,
                                 সংরচনা fileinfo *infop,
                                 ধ্রুৱ ৱর্ণ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ৱর্ণ *mem = curlx_dyn_ptr(&infop->buf);
  ৱিকল্প(parser->state.UNIX.sub.group) {
  পক্ষ PL_UNIX_GROUP_PRESPACE:
    যদি(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_NAME;
    }
    ভঙ্গ;
  পক্ষ PL_UNIX_GROUP_NAME:
    parser->item_length++;
    যদি(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.group = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_SIZE;
      parser->state.UNIX.sub.size = PL_UNIX_SIZE_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    ভঙ্গ;
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode parse_unix_size(সংরচনা ftp_parselist_data *parser,
                                সংরচনা fileinfo *infop,
                                ধ্রুৱ ৱর্ণ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ৱর্ণ *mem = curlx_dyn_ptr(&infop->buf);
  ৱিকল্প(parser->state.UNIX.sub.size) {
  পক্ষ PL_UNIX_SIZE_PRESPACE:
    যদি(c != ' ') {
      যদি(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.size = PL_UNIX_SIZE_সংখ্যা;
      }
      অন্যথা
        ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
    }
    ভঙ্গ;
  পক্ষ PL_UNIX_SIZE_সংখ্যা:
    parser->item_length++;
    যদি(c == ' ') {
      ধ্রুৱ ৱর্ণ *p = mem + parser->item_offset;
      curl_off_t fsize;
      mem[parser->item_offset + parser->item_length - 1] = 0;
      যদি(!curlx_str_numblanks(&p, &fsize)) {
        যদি(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
          parser->file_data->info.size = fsize;
        }
        parser->item_length = 0;
        parser->item_offset = 0;
        parser->state.UNIX.main = PL_UNIX_TIME;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART1;
      }
    }
    অন্যথা যদি(!ISDIGIT(c))
      ঘূৰাই CURLE_FTP_BAD_FILE_LIST;

    ভঙ্গ;
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode parse_unix_time(সংরচনা ftp_parselist_data *parser,
                                সংরচনা fileinfo *infop,
                                ধ্রুৱ ৱর্ণ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ৱর্ণ *mem = curlx_dyn_ptr(&infop->buf);
  সংরচনা curl_fileinfo *finfo = &infop->info;

  ৱিকল্প(parser->state.UNIX.sub.time) {
  পক্ষ PL_UNIX_TIME_PREPART1:
    যদি(c != ' ') {
      যদি(ISALNUM(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART1;
      }
      অন্যথা
        ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
    }
    ভঙ্গ;
  পক্ষ PL_UNIX_TIME_PART1:
    parser->item_length++;
    যদি(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART2;

    অন্যথা যদি(!ISALNUM(c) && c != '.')
      ঘূৰাই CURLE_FTP_BAD_FILE_LIST;

    ভঙ্গ;
  পক্ষ PL_UNIX_TIME_PREPART2:
    parser->item_length++;
    যদি(c != ' ') {
      যদি(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART2;
      অন্যথা
        ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
    }
    ভঙ্গ;
  পক্ষ PL_UNIX_TIME_PART2:
    parser->item_length++;
    যদি(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART3;
    অন্যথা যদি(!ISALNUM(c) && c != '.')
      ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
    ভঙ্গ;
  পক্ষ PL_UNIX_TIME_PREPART3:
    parser->item_length++;
    যদি(c != ' ') {
      যদি(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART3;
      অন্যথা
        ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
    }
    ভঙ্গ;
  পক্ষ PL_UNIX_TIME_PART3:
    parser->item_length++;
    যদি(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.time = parser->item_offset;
      যদি(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.main = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      অন্যথা {
        parser->state.UNIX.main = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    অন্যথা যদি(!ISALNUM(c) && c != '.' && c != ':')
      ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
    ভঙ্গ;
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode parse_unix_filename(সংরচনা Curl_easy *data,
                                    সংরচনা ftp_parselist_data *parser,
                                    সংরচনা fileinfo *infop,
                                    ধ্রুৱ ৱর্ণ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ৱর্ণ *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  ৱিকল্প(parser->state.UNIX.sub.filename) {
  পক্ষ PL_UNIX_FILENAME_PRESPACE:
    যদি(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    ভঙ্গ;
  পক্ষ PL_UNIX_FILENAME_NAME:
    parser->item_length++;
    যদি(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    অন্যথা যদি(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      result = ftp_pl_ঢোকাও_finfo(data, infop);
    }
    ভঙ্গ;
  পক্ষ PL_UNIX_FILENAME_WINDOWSEOL:
    যদি(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      result = ftp_pl_ঢোকাও_finfo(data, infop);
    }
    অন্যথা
      result = CURLE_FTP_BAD_FILE_LIST;
    ভঙ্গ;
  }
  ঘূৰাই result;
}

স্থির CURLcode parse_unix_symlink(সংরচনা Curl_easy *data,
                                   সংরচনা ftp_parselist_data *parser,
                                   সংরচনা fileinfo *infop,
                                   ধ্রুৱ ৱর্ণ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ৱর্ণ *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  ৱিকল্প(parser->state.UNIX.sub.symlink) {
  পক্ষ PL_UNIX_SYMLINK_PRESPACE:
    যদি(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    ভঙ্গ;
  পক্ষ PL_UNIX_SYMLINK_NAME:
    parser->item_length++;
    যদি(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    অন্যথা যদি(c == '\r' || c == '\n')
      ঘূৰাই CURLE_FTP_BAD_FILE_LIST;

    ভঙ্গ;
  পক্ষ PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_length++;
    যদি(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    অন্যথা যদি(c == '\r' || c == '\n')
      ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
    অন্যথা
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    ভঙ্গ;
  পক্ষ PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_length++;
    যদি(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    অন্যথা যদি(c == '\r' || c == '\n')
      ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
    অন্যথা
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    ভঙ্গ;
  পক্ষ PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_length++;
    যদি(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_offset + parser->item_length - 4] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->item_length = 0;
      parser->item_offset = 0;
    }
    অন্যথা যদি(c == '\r' || c == '\n')
      ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
    অন্যথা
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    ভঙ্গ;
  পক্ষ PL_UNIX_SYMLINK_PRETARGET4:
    যদি(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_offset = len - 1;
      parser->item_length = 1;
    }
    অন্যথা
      ঘূৰাই CURLE_FTP_BAD_FILE_LIST;

    ভঙ্গ;
  পক্ষ PL_UNIX_SYMLINK_TARGET:
    parser->item_length++;
    যদি(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    অন্যথা যদি(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_ঢোকাও_finfo(data, infop);
      যদি(result)
        ভঙ্গ;

      parser->state.UNIX.main = PL_UNIX_FILETYPE;
    }
    ভঙ্গ;
  পক্ষ PL_UNIX_SYMLINK_WINDOWSEOL:
    যদি(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_ঢোকাও_finfo(data, infop);
      যদি(result)
        ভঙ্গ;

      parser->state.UNIX.main = PL_UNIX_FILETYPE;
    }
    অন্যথা
      result = CURLE_FTP_BAD_FILE_LIST;

    ভঙ্গ;
  }
  ঘূৰাই result;
}

স্থির CURLcode parse_unix(সংরচনা Curl_easy *data,
                           সংরচনা ftp_parselist_data *parser,
                           সংরচনা fileinfo *infop,
                           ধ্রুৱ ৱর্ণ c)
{
  সংরচনা curl_fileinfo *finfo = &infop->info;
  CURLcode result = CURLE_OK;

  ৱিকল্প(parser->state.UNIX.main) {
  পক্ষ PL_UNIX_TOTALSIZE:
    result = parse_unix_totalsize(parser, infop, c);
    যদি(result)
      ভঙ্গ;
    যদি(parser->state.UNIX.main != PL_UNIX_FILETYPE)
      ভঙ্গ;
    FALLTHROUGH();
  পক্ষ PL_UNIX_FILETYPE:
    result = unix_filetype(c, &finfo->filetype);
    যদি(!result) {
      parser->state.UNIX.main = PL_UNIX_PERMISSION;
      parser->item_length = 0;
      parser->item_offset = 1;
    }
    ভঙ্গ;
  পক্ষ PL_UNIX_PERMISSION:
    result = parse_unix_permission(parser, infop, c);
    ভঙ্গ;
  পক্ষ PL_UNIX_HLINKS:
    result = parse_unix_hlinks(parser, infop, c);
    ভঙ্গ;
  পক্ষ PL_UNIX_USER:
    result = parse_unix_user(parser, infop, c);
    ভঙ্গ;
  পক্ষ PL_UNIX_GROUP:
    result = parse_unix_group(parser, infop, c);
    ভঙ্গ;
  পক্ষ PL_UNIX_SIZE:
    result = parse_unix_size(parser, infop, c);
    ভঙ্গ;
  পক্ষ PL_UNIX_TIME:
    result = parse_unix_time(parser, infop, c);
    ভঙ্গ;
  পক্ষ PL_UNIX_FILENAME:
    result = parse_unix_filename(data, parser, infop, c);
    ভঙ্গ;
  পক্ষ PL_UNIX_SYMLINK:
    result = parse_unix_symlink(data, parser, infop, c);
    ভঙ্গ;
  }
  ঘূৰাই result;
}

স্থির CURLcode parse_winnt(সংরচনা Curl_easy *data,
                            সংরচনা ftp_parselist_data *parser,
                            সংরচনা fileinfo *infop,
                            ধ্রুৱ ৱর্ণ c)
{
  সংরচনা curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  ৱর্ণ *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  ৱিকল্প(parser->state.NT.main) {
  পক্ষ PL_WINNT_DATE:
    parser->item_length++;
    যদি(parser->item_length < 9) {
      যদি(!strchr("0123456789-", c)) { /* only simple control */
        ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
      }
    }
    অন্যথা যদি(parser->item_length == 9) {
      যদি(c == ' ') {
        parser->state.NT.main = PL_WINNT_TIME;
        parser->state.NT.sub.time = PL_WINNT_TIME_PRESPACE;
      }
      অন্যথা
        ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
    }
    অন্যথা
      ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
    ভঙ্গ;
  পক্ষ PL_WINNT_TIME:
    parser->item_length++;
    ৱিকল্প(parser->state.NT.sub.time) {
    পক্ষ PL_WINNT_TIME_PRESPACE:
      যদি(!ISBLANK(c))
        parser->state.NT.sub.time = PL_WINNT_TIME_TIME;
      ভঙ্গ;
    পক্ষ PL_WINNT_TIME_TIME:
      যদি(c == ' ') {
        parser->offsets.time = parser->item_offset;
        mem[parser->item_offset + parser->item_length - 1] = 0;
        parser->state.NT.main = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_length = 0;
      }
      অন্যথা যদি(!strchr("APM0123456789:", c))
        ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
      ভঙ্গ;
    }
    ভঙ্গ;
  পক্ষ PL_WINNT_DIRORSIZE:
    ৱিকল্প(parser->state.NT.sub.dirorsize) {
    পক্ষ PL_WINNT_DIRORSIZE_PRESPACE:
      যদি(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      ভঙ্গ;
    পক্ষ PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_length++;
      যদি(c == ' ') {
        mem[parser->item_offset + parser->item_length - 1] = 0;
        যদি(!strcmp("<DIR>", mem + parser->item_offset)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->size = 0;
        }
        অন্যথা {
          ধ্রুৱ ৱর্ণ *p = mem + parser->item_offset;
          যদি(curlx_str_numblanks(&p, &finfo->size)) {
            ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_data->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
        parser->item_length = 0;
        parser->state.NT.main = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ভঙ্গ;
    }
    ভঙ্গ;
  পক্ষ PL_WINNT_FILENAME:
    ৱিকল্প(parser->state.NT.sub.filename) {
    পক্ষ PL_WINNT_FILENAME_PRESPACE:
      যদি(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      ভঙ্গ;
    পক্ষ PL_WINNT_FILENAME_CONTENT:
      parser->item_length++;
      যদি(!len)
        ঘূৰাই CURLE_FTP_BAD_FILE_LIST;
      যদি(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      অন্যথা যদি(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        mem[len - 1] = 0;
        result = ftp_pl_ঢোকাও_finfo(data, infop);
        যদি(result)
          ঘূৰাই result;

        parser->state.NT.main = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ভঙ্গ;
    পক্ষ PL_WINNT_FILENAME_WINEOL:
      যদি(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        result = ftp_pl_ঢোকাও_finfo(data, infop);
        যদি(result)
          ঘূৰাই result;

        parser->state.NT.main = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      অন্যথা
        ঘূৰাই CURLE_FTP_BAD_FILE_LIST;

      ভঙ্গ;
    }
    ভঙ্গ;
  }

  ঘূৰাই CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(সংরচনা Curl_easy *data)
{
  যদি(data->state.wildcardmatch) {
    সংরচনা WildcardData * ধ্রুৱ wildcard = data->wildcard;
    ঘূৰাই wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  ঘূৰাই FALSE;
}

size_t Curl_ftp_parselist(ৱর্ণ *buffer, size_t size, size_t nmemb,
                          শূন্য *connptr)
{
  size_t bufflen = size * nmemb;
  সংরচনা Curl_easy *data = (সংরচনা Curl_easy *)connptr;
  সংরচনা ftp_wc *ftpwc = data->wildcard->ftpwc;
  সংরচনা ftp_parselist_data *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode result;
  size_t retsize = bufflen;

  যদি(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    গচ্ছ fail;
  }

  যদি(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(buffer[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  যেতিয়ালৈ(i < bufflen) { /* FSM */
    ৱর্ণ c = buffer[i];
    সংরচনা fileinfo *infop;
    যদি(!parser->file_data) { /* tmp file data is not allocated yet */
      parser->file_data = Curl_fileinfo_alloc();
      যদি(!parser->file_data) {
        parser->error = CURLE_OUT_OF_MEMORY;
        গচ্ছ fail;
      }
      parser->item_offset = 0;
      parser->item_length = 0;
      curlx_dyn_init(&parser->file_data->buf, MAX_FTPLIST_BUFFER);
    }

    infop = parser->file_data;

    যদি(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_OUT_OF_MEMORY;
      গচ্ছ fail;
    }

    ৱিকল্প(parser->os_type) {
    পক্ষ OS_TYPE_UNIX:
      result = parse_unix(data, parser, infop, c);
      ভঙ্গ;
    পক্ষ OS_TYPE_WIN_NT:
      result = parse_winnt(data, parser, infop, c);
      ভঙ্গ;
    স্ৱভাৱ:
      retsize = bufflen + 1;
      গচ্ছ fail;
    }
    যদি(result) {
      parser->error = result;
      গচ্ছ fail;
    }

    i++;
  }
  ঘূৰাই retsize;

fail:

  /* Clean up any allocated memory. */
  যদি(parser->file_data) {
    Curl_fileinfo_cleanup(parser->file_data);
    parser->file_data = রিক্ত;
  }

  ঘূৰাই retsize;
}

#endif /* !CURL_DISABLE_FTP */
