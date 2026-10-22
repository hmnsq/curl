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
#അന്തര്ഭാവ "curl_setup.h"

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

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "fileinfo.h"
#അന്തര്ഭാവ "llist.h"
#അന്തര്ഭാവ "ftp.h"
#അന്തര്ഭാവ "ftp-int.h"
#അന്തര്ഭാവ "ftplistparser.h"
#അന്തര്ഭാവ "curl_fnmatch.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "curlx/strparse.h"

തരനിർവചനം എണ്ണൽ {
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

തരനിർവചനം സംഘം {
  എണ്ണൽ {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } total_dirsize;

  എണ്ണൽ {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_സംഖ്യ
  } hlinks;

  എണ്ണൽ {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  എണ്ണൽ {
    PL_UNIX_GROUP_PRESPACE = 0,
    PL_UNIX_GROUP_NAME
  } group;

  എണ്ണൽ {
    PL_UNIX_SIZE_PRESPACE = 0,
    PL_UNIX_SIZE_സംഖ്യ
  } size;

  എണ്ണൽ {
    PL_UNIX_TIME_PREPART1 = 0,
    PL_UNIX_TIME_PART1,
    PL_UNIX_TIME_PREPART2,
    PL_UNIX_TIME_PART2,
    PL_UNIX_TIME_PREPART3,
    PL_UNIX_TIME_PART3
  } time;

  എണ്ണൽ {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  എണ്ണൽ {
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

തരനിർവചനം എണ്ണൽ {
  PL_WINNT_DATE = 0,
  PL_WINNT_TIME,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

തരനിർവചനം സംഘം {
  എണ്ണൽ {
    PL_WINNT_TIME_PRESPACE = 0,
    PL_WINNT_TIME_TIME
  } time;
  എണ്ണൽ {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  എണ്ണൽ {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
ഘടന ftp_parselist_data {
  എണ്ണൽ {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  സംഘം {
    ഘടന {
      pl_unix_mainstate പ്രധാന;
      pl_unix_substate sub;
    } UNIX;

    ഘടന {
      pl_winNT_mainstate പ്രധാന;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  ഘടന fileinfo *file_data;
  unsigned പൂർണ്ണസംഖ്യ item_length;
  size_t item_offset;
  ഘടന {
    size_t filename;
    size_t user;
    size_t group;
    size_t time;
    size_t perm;
    size_t symlink_target;
  } offsets;
};

സ്ഥിതം ശൂന്യം fileinfo_dtor(ശൂന്യം *user, ശൂന്യം *element)
{
  (ശൂന്യം)user;
  Curl_fileinfo_cleanup(element);
}

ശൂന്യം Curl_wildcard_init(ഘടന WildcardData *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

ശൂന്യം Curl_wildcard_dtor(ഘടന WildcardData **wcp)
{
  ഘടന WildcardData *wc = *wcp;
  എങ്കിൽ(!wc)
    തിരികെ;

  എങ്കിൽ(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = ശൂന്യ;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, ശൂന്യ);
  curlx_safefree(wc->path);
  curlx_safefree(wc->pattern);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = ശൂന്യ;
}

ഘടന ftp_parselist_data *Curl_ftp_parselist_data_alloc(ശൂന്യം)
{
  തിരികെ curlx_calloc(1, വലുപ്പം(ഘടന ftp_parselist_data));
}

ശൂന്യം Curl_ftp_parselist_data_free(ഘടന ftp_parselist_data **parserp)
{
  ഘടന ftp_parselist_data *parser = *parserp;
  എങ്കിൽ(parser)
    Curl_fileinfo_cleanup(parser->file_data);
  curlx_free(parser);
  *parserp = ശൂന്യ;
}

CURLcode Curl_ftp_parselist_geterror(ഘടന ftp_parselist_data *pl_data)
{
  തിരികെ pl_data->error;
}

#define FTP_LP_MALFORMATED_PERM 0x01000000

സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ ftp_pl_get_permission(സ്ഥിരം അക്ഷരം *str)
{
  unsigned പൂർണ്ണസംഖ്യ permissions = 0;
  /* USER */
  എങ്കിൽ(str[0] == 'r')
    permissions |= 1 << 8;
  അല്ലെങ്കിൽ എങ്കിൽ(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  എങ്കിൽ(str[1] == 'w')
    permissions |= 1 << 7;
  അല്ലെങ്കിൽ എങ്കിൽ(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  എങ്കിൽ(str[2] == 'x')
    permissions |= 1 << 6;
  അല്ലെങ്കിൽ എങ്കിൽ(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(str[2] == 'S')
    permissions |= 1 << 11;
  അല്ലെങ്കിൽ എങ്കിൽ(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* GROUP */
  എങ്കിൽ(str[3] == 'r')
    permissions |= 1 << 5;
  അല്ലെങ്കിൽ എങ്കിൽ(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  എങ്കിൽ(str[4] == 'w')
    permissions |= 1 << 4;
  അല്ലെങ്കിൽ എങ്കിൽ(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  എങ്കിൽ(str[5] == 'x')
    permissions |= 1 << 3;
  അല്ലെങ്കിൽ എങ്കിൽ(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(str[5] == 'S')
    permissions |= 1 << 10;
  അല്ലെങ്കിൽ എങ്കിൽ(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  എങ്കിൽ(str[6] == 'r')
    permissions |= 1 << 2;
  അല്ലെങ്കിൽ എങ്കിൽ(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  എങ്കിൽ(str[7] == 'w')
    permissions |= 1 << 1;
  അല്ലെങ്കിൽ എങ്കിൽ(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  എങ്കിൽ(str[8] == 'x')
    permissions |= 1;
  അല്ലെങ്കിൽ എങ്കിൽ(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(str[8] == 'T')
    permissions |= 1 << 9;
  അല്ലെങ്കിൽ എങ്കിൽ(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  തിരികെ permissions;
}

സ്ഥിതം CURLcode ftp_pl_തിരുകുക_finfo(ഘടന Curl_easy *data,
                                    ഘടന fileinfo *infop)
{
  curl_fnmatch_callback compare;
  ഘടന WildcardData *wc = data->wildcard;
  ഘടന ftp_wc *ftpwc = wc->ftpwc;
  ഘടന Curl_llist *llist = &wc->filelist;
  ഘടന ftp_parselist_data *parser = ftpwc->parser;
  bool add = TRUE;
  ഘടന curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  അക്ഷരം *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->strings.group  = parser->offsets.group ?
                          str + parser->offsets.group : ശൂന്യ;
  finfo->strings.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : ശൂന്യ;
  finfo->strings.target = parser->offsets.symlink_target &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_target : ശൂന്യ;
  finfo->strings.time   = str + parser->offsets.time;
  finfo->strings.user   = parser->offsets.user ?
                          str + parser->offsets.user : ശൂന്യ;

  /* get correct fnmatch callback */
  compare = data->set.fnmatch;
  എങ്കിൽ(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  എങ്കിൽ(strchr(finfo->filename, '/'))
    add = FALSE;
  അല്ലെങ്കിൽ {
    /* a real file name cannot contain a slash */
    ഘടന Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fnmatch_data);
    എങ്കിൽ(compare(data->set.fnmatch_data, wc->pattern, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      എങ്കിൽ((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->strings.target &&
         strstr(finfo->strings.target, " -> ")) {
        add = FALSE;
      }
    }
    അല്ലെങ്കിൽ {
      add = FALSE;
    }
    CURL_CBAPI_END(&guard);
  }

  എങ്കിൽ(add) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  അല്ലെങ്കിൽ {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_data = ശൂന്യ;
  തിരികെ CURLE_OK;
}

#define MAX_FTPLIST_BUFFER 10000 /* arbitrarily set */

സ്ഥിതം CURLcode unix_filetype(സ്ഥിരം അക്ഷരം c, curlfiletype *t)
{
  തിരഞ്ഞെടുപ്പ്(c) {
  അവസ്ഥ '-':
    *t = CURLFILETYPE_FILE;
    നിർത്തുക;
  അവസ്ഥ 'd':
    *t = CURLFILETYPE_DIRECTORY;
    നിർത്തുക;
  അവസ്ഥ 'l':
    *t = CURLFILETYPE_SYMLINK;
    നിർത്തുക;
  അവസ്ഥ 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    നിർത്തുക;
  അവസ്ഥ 's':
    *t = CURLFILETYPE_SOCKET;
    നിർത്തുക;
  അവസ്ഥ 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    നിർത്തുക;
  അവസ്ഥ 'b':
    *t = CURLFILETYPE_DEVICE_BLOCK;
    നിർത്തുക;
  അവസ്ഥ 'D':
    *t = CURLFILETYPE_DOOR;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_FTP_BAD_FILE_LIST;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode parse_unix_totalsize(ഘടന ftp_parselist_data *parser,
                                     ഘടന fileinfo *infop,
                                     സ്ഥിരം അക്ഷരം c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  അക്ഷരം *mem = curlx_dyn_ptr(&infop->buf);
  തിരഞ്ഞെടുപ്പ്(parser->state.UNIX.sub.total_dirsize) {
  അവസ്ഥ PL_UNIX_TOTALSIZE_INIT:
    എങ്കിൽ(c == 't') {
      parser->state.UNIX.sub.total_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_length++;
    }
    അല്ലെങ്കിൽ {
      parser->state.UNIX.പ്രധാന = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_TOTALSIZE_READING:
    parser->item_length++;
    എങ്കിൽ(c == '\r') {
      parser->item_length--;
      എങ്കിൽ(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(c == '\n') {
      mem[parser->item_length - 1] = 0;
      എങ്കിൽ(!strncmp("total ", mem, 6)) {
        സ്ഥിരം അക്ഷരം *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        ആകുമ്പോൾ(ISDIGIT(*endptr))
          endptr++;
        എങ്കിൽ(*endptr) {
          തിരികെ CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.പ്രധാന = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      അല്ലെങ്കിൽ
        തിരികെ CURLE_FTP_BAD_FILE_LIST;
    }
    നിർത്തുക;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode parse_unix_permission(ഘടന ftp_parselist_data *parser,
                                      ഘടന fileinfo *infop,
                                      സ്ഥിരം അക്ഷരം c)
{
  അക്ഷരം *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_length++;
  എങ്കിൽ((parser->item_length <= 9) && !strchr("rwx-tTsS", c))
    തിരികെ CURLE_FTP_BAD_FILE_LIST;

  അല്ലെങ്കിൽ എങ്കിൽ(parser->item_length == 10) {
    unsigned പൂർണ്ണസംഖ്യ perm;
    എങ്കിൽ(c != ' ')
      തിരികെ CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_offset);
    എങ്കിൽ(perm & FTP_LP_MALFORMATED_PERM)
      തിരികെ CURLE_FTP_BAD_FILE_LIST;

    parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_PERM;
    parser->file_data->info.perm = perm;
    parser->offsets.perm = parser->item_offset;

    parser->item_length = 0;
    parser->state.UNIX.പ്രധാന = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode parse_unix_hlinks(ഘടന ftp_parselist_data *parser,
                                  ഘടന fileinfo *infop,
                                  സ്ഥിരം അക്ഷരം c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  അക്ഷരം *mem = curlx_dyn_ptr(&infop->buf);

  തിരഞ്ഞെടുപ്പ്(parser->state.UNIX.sub.hlinks) {
  അവസ്ഥ PL_UNIX_HLINKS_PRESPACE:
    എങ്കിൽ(c != ' ') {
      എങ്കിൽ(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_സംഖ്യ;
      }
      അല്ലെങ്കിൽ
        തിരികെ CURLE_FTP_BAD_FILE_LIST;
    }
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_HLINKS_സംഖ്യ:
    parser->item_length++;
    എങ്കിൽ(c == ' ') {
      സ്ഥിരം അക്ഷരം *p = &mem[parser->item_offset];
      curl_off_t hlinks;
      mem[parser->item_offset + parser->item_length - 1] = 0;

      എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &hlinks, LONG_MAX)) {
        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_HLINKCOUNT;
        parser->file_data->info.hardlinks = (long)hlinks;
      }
      parser->item_length = 0;
      parser->item_offset = 0;
      parser->state.UNIX.പ്രധാന = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!ISDIGIT(c))
      തിരികെ CURLE_FTP_BAD_FILE_LIST;

    നിർത്തുക;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode parse_unix_user(ഘടന ftp_parselist_data *parser,
                                ഘടന fileinfo *infop,
                                സ്ഥിരം അക്ഷരം c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  അക്ഷരം *mem = curlx_dyn_ptr(&infop->buf);
  തിരഞ്ഞെടുപ്പ്(parser->state.UNIX.sub.user) {
  അവസ്ഥ PL_UNIX_USER_PRESPACE:
    എങ്കിൽ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_USER_PARSING:
    parser->item_length++;
    എങ്കിൽ(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.user = parser->item_offset;
      parser->state.UNIX.പ്രധാന = PL_UNIX_GROUP;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    നിർത്തുക;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode parse_unix_group(ഘടന ftp_parselist_data *parser,
                                 ഘടന fileinfo *infop,
                                 സ്ഥിരം അക്ഷരം c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  അക്ഷരം *mem = curlx_dyn_ptr(&infop->buf);
  തിരഞ്ഞെടുപ്പ്(parser->state.UNIX.sub.group) {
  അവസ്ഥ PL_UNIX_GROUP_PRESPACE:
    എങ്കിൽ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_NAME;
    }
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_GROUP_NAME:
    parser->item_length++;
    എങ്കിൽ(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.group = parser->item_offset;
      parser->state.UNIX.പ്രധാന = PL_UNIX_SIZE;
      parser->state.UNIX.sub.size = PL_UNIX_SIZE_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    നിർത്തുക;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode parse_unix_size(ഘടന ftp_parselist_data *parser,
                                ഘടന fileinfo *infop,
                                സ്ഥിരം അക്ഷരം c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  അക്ഷരം *mem = curlx_dyn_ptr(&infop->buf);
  തിരഞ്ഞെടുപ്പ്(parser->state.UNIX.sub.size) {
  അവസ്ഥ PL_UNIX_SIZE_PRESPACE:
    എങ്കിൽ(c != ' ') {
      എങ്കിൽ(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.size = PL_UNIX_SIZE_സംഖ്യ;
      }
      അല്ലെങ്കിൽ
        തിരികെ CURLE_FTP_BAD_FILE_LIST;
    }
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_SIZE_സംഖ്യ:
    parser->item_length++;
    എങ്കിൽ(c == ' ') {
      സ്ഥിരം അക്ഷരം *p = mem + parser->item_offset;
      curl_off_t fsize;
      mem[parser->item_offset + parser->item_length - 1] = 0;
      എങ്കിൽ(!curlx_str_numblanks(&p, &fsize)) {
        എങ്കിൽ(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
          parser->file_data->info.size = fsize;
        }
        parser->item_length = 0;
        parser->item_offset = 0;
        parser->state.UNIX.പ്രധാന = PL_UNIX_TIME;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART1;
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!ISDIGIT(c))
      തിരികെ CURLE_FTP_BAD_FILE_LIST;

    നിർത്തുക;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode parse_unix_time(ഘടന ftp_parselist_data *parser,
                                ഘടന fileinfo *infop,
                                സ്ഥിരം അക്ഷരം c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  അക്ഷരം *mem = curlx_dyn_ptr(&infop->buf);
  ഘടന curl_fileinfo *finfo = &infop->info;

  തിരഞ്ഞെടുപ്പ്(parser->state.UNIX.sub.time) {
  അവസ്ഥ PL_UNIX_TIME_PREPART1:
    എങ്കിൽ(c != ' ') {
      എങ്കിൽ(ISALNUM(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART1;
      }
      അല്ലെങ്കിൽ
        തിരികെ CURLE_FTP_BAD_FILE_LIST;
    }
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_TIME_PART1:
    parser->item_length++;
    എങ്കിൽ(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART2;

    അല്ലെങ്കിൽ എങ്കിൽ(!ISALNUM(c) && c != '.')
      തിരികെ CURLE_FTP_BAD_FILE_LIST;

    നിർത്തുക;
  അവസ്ഥ PL_UNIX_TIME_PREPART2:
    parser->item_length++;
    എങ്കിൽ(c != ' ') {
      എങ്കിൽ(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART2;
      അല്ലെങ്കിൽ
        തിരികെ CURLE_FTP_BAD_FILE_LIST;
    }
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_TIME_PART2:
    parser->item_length++;
    എങ്കിൽ(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART3;
    അല്ലെങ്കിൽ എങ്കിൽ(!ISALNUM(c) && c != '.')
      തിരികെ CURLE_FTP_BAD_FILE_LIST;
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_TIME_PREPART3:
    parser->item_length++;
    എങ്കിൽ(c != ' ') {
      എങ്കിൽ(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART3;
      അല്ലെങ്കിൽ
        തിരികെ CURLE_FTP_BAD_FILE_LIST;
    }
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_TIME_PART3:
    parser->item_length++;
    എങ്കിൽ(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.time = parser->item_offset;
      എങ്കിൽ(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.പ്രധാന = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      അല്ലെങ്കിൽ {
        parser->state.UNIX.പ്രധാന = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!ISALNUM(c) && c != '.' && c != ':')
      തിരികെ CURLE_FTP_BAD_FILE_LIST;
    നിർത്തുക;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode parse_unix_filename(ഘടന Curl_easy *data,
                                    ഘടന ftp_parselist_data *parser,
                                    ഘടന fileinfo *infop,
                                    സ്ഥിരം അക്ഷരം c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  അക്ഷരം *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  തിരഞ്ഞെടുപ്പ്(parser->state.UNIX.sub.filename) {
  അവസ്ഥ PL_UNIX_FILENAME_PRESPACE:
    എങ്കിൽ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_FILENAME_NAME:
    parser->item_length++;
    എങ്കിൽ(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    അല്ലെങ്കിൽ എങ്കിൽ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.പ്രധാന = PL_UNIX_FILETYPE;
      result = ftp_pl_തിരുകുക_finfo(data, infop);
    }
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_FILENAME_WINDOWSEOL:
    എങ്കിൽ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.പ്രധാന = PL_UNIX_FILETYPE;
      result = ftp_pl_തിരുകുക_finfo(data, infop);
    }
    അല്ലെങ്കിൽ
      result = CURLE_FTP_BAD_FILE_LIST;
    നിർത്തുക;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode parse_unix_symlink(ഘടന Curl_easy *data,
                                   ഘടന ftp_parselist_data *parser,
                                   ഘടന fileinfo *infop,
                                   സ്ഥിരം അക്ഷരം c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  അക്ഷരം *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  തിരഞ്ഞെടുപ്പ്(parser->state.UNIX.sub.symlink) {
  അവസ്ഥ PL_UNIX_SYMLINK_PRESPACE:
    എങ്കിൽ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_SYMLINK_NAME:
    parser->item_length++;
    എങ്കിൽ(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    അല്ലെങ്കിൽ എങ്കിൽ(c == '\r' || c == '\n')
      തിരികെ CURLE_FTP_BAD_FILE_LIST;

    നിർത്തുക;
  അവസ്ഥ PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_length++;
    എങ്കിൽ(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    അല്ലെങ്കിൽ എങ്കിൽ(c == '\r' || c == '\n')
      തിരികെ CURLE_FTP_BAD_FILE_LIST;
    അല്ലെങ്കിൽ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_length++;
    എങ്കിൽ(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    അല്ലെങ്കിൽ എങ്കിൽ(c == '\r' || c == '\n')
      തിരികെ CURLE_FTP_BAD_FILE_LIST;
    അല്ലെങ്കിൽ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    നിർത്തുക;
  അവസ്ഥ PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_length++;
    എങ്കിൽ(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_offset + parser->item_length - 4] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->item_length = 0;
      parser->item_offset = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(c == '\r' || c == '\n')
      തിരികെ CURLE_FTP_BAD_FILE_LIST;
    അല്ലെങ്കിൽ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_SYMLINK_PRETARGET4:
    എങ്കിൽ(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_offset = len - 1;
      parser->item_length = 1;
    }
    അല്ലെങ്കിൽ
      തിരികെ CURLE_FTP_BAD_FILE_LIST;

    നിർത്തുക;
  അവസ്ഥ PL_UNIX_SYMLINK_TARGET:
    parser->item_length++;
    എങ്കിൽ(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    അല്ലെങ്കിൽ എങ്കിൽ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_തിരുകുക_finfo(data, infop);
      എങ്കിൽ(result)
        നിർത്തുക;

      parser->state.UNIX.പ്രധാന = PL_UNIX_FILETYPE;
    }
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_SYMLINK_WINDOWSEOL:
    എങ്കിൽ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_തിരുകുക_finfo(data, infop);
      എങ്കിൽ(result)
        നിർത്തുക;

      parser->state.UNIX.പ്രധാന = PL_UNIX_FILETYPE;
    }
    അല്ലെങ്കിൽ
      result = CURLE_FTP_BAD_FILE_LIST;

    നിർത്തുക;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode parse_unix(ഘടന Curl_easy *data,
                           ഘടന ftp_parselist_data *parser,
                           ഘടന fileinfo *infop,
                           സ്ഥിരം അക്ഷരം c)
{
  ഘടന curl_fileinfo *finfo = &infop->info;
  CURLcode result = CURLE_OK;

  തിരഞ്ഞെടുപ്പ്(parser->state.UNIX.പ്രധാന) {
  അവസ്ഥ PL_UNIX_TOTALSIZE:
    result = parse_unix_totalsize(parser, infop, c);
    എങ്കിൽ(result)
      നിർത്തുക;
    എങ്കിൽ(parser->state.UNIX.പ്രധാന != PL_UNIX_FILETYPE)
      നിർത്തുക;
    FALLTHROUGH();
  അവസ്ഥ PL_UNIX_FILETYPE:
    result = unix_filetype(c, &finfo->filetype);
    എങ്കിൽ(!result) {
      parser->state.UNIX.പ്രധാന = PL_UNIX_PERMISSION;
      parser->item_length = 0;
      parser->item_offset = 1;
    }
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_PERMISSION:
    result = parse_unix_permission(parser, infop, c);
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_HLINKS:
    result = parse_unix_hlinks(parser, infop, c);
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_USER:
    result = parse_unix_user(parser, infop, c);
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_GROUP:
    result = parse_unix_group(parser, infop, c);
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_SIZE:
    result = parse_unix_size(parser, infop, c);
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_TIME:
    result = parse_unix_time(parser, infop, c);
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_FILENAME:
    result = parse_unix_filename(data, parser, infop, c);
    നിർത്തുക;
  അവസ്ഥ PL_UNIX_SYMLINK:
    result = parse_unix_symlink(data, parser, infop, c);
    നിർത്തുക;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode parse_winnt(ഘടന Curl_easy *data,
                            ഘടന ftp_parselist_data *parser,
                            ഘടന fileinfo *infop,
                            സ്ഥിരം അക്ഷരം c)
{
  ഘടന curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  അക്ഷരം *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  തിരഞ്ഞെടുപ്പ്(parser->state.NT.പ്രധാന) {
  അവസ്ഥ PL_WINNT_DATE:
    parser->item_length++;
    എങ്കിൽ(parser->item_length < 9) {
      എങ്കിൽ(!strchr("0123456789-", c)) { /* only simple control */
        തിരികെ CURLE_FTP_BAD_FILE_LIST;
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(parser->item_length == 9) {
      എങ്കിൽ(c == ' ') {
        parser->state.NT.പ്രധാന = PL_WINNT_TIME;
        parser->state.NT.sub.time = PL_WINNT_TIME_PRESPACE;
      }
      അല്ലെങ്കിൽ
        തിരികെ CURLE_FTP_BAD_FILE_LIST;
    }
    അല്ലെങ്കിൽ
      തിരികെ CURLE_FTP_BAD_FILE_LIST;
    നിർത്തുക;
  അവസ്ഥ PL_WINNT_TIME:
    parser->item_length++;
    തിരഞ്ഞെടുപ്പ്(parser->state.NT.sub.time) {
    അവസ്ഥ PL_WINNT_TIME_PRESPACE:
      എങ്കിൽ(!ISBLANK(c))
        parser->state.NT.sub.time = PL_WINNT_TIME_TIME;
      നിർത്തുക;
    അവസ്ഥ PL_WINNT_TIME_TIME:
      എങ്കിൽ(c == ' ') {
        parser->offsets.time = parser->item_offset;
        mem[parser->item_offset + parser->item_length - 1] = 0;
        parser->state.NT.പ്രധാന = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_length = 0;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(!strchr("APM0123456789:", c))
        തിരികെ CURLE_FTP_BAD_FILE_LIST;
      നിർത്തുക;
    }
    നിർത്തുക;
  അവസ്ഥ PL_WINNT_DIRORSIZE:
    തിരഞ്ഞെടുപ്പ്(parser->state.NT.sub.dirorsize) {
    അവസ്ഥ PL_WINNT_DIRORSIZE_PRESPACE:
      എങ്കിൽ(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      നിർത്തുക;
    അവസ്ഥ PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_length++;
      എങ്കിൽ(c == ' ') {
        mem[parser->item_offset + parser->item_length - 1] = 0;
        എങ്കിൽ(!strcmp("<DIR>", mem + parser->item_offset)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->size = 0;
        }
        അല്ലെങ്കിൽ {
          സ്ഥിരം അക്ഷരം *p = mem + parser->item_offset;
          എങ്കിൽ(curlx_str_numblanks(&p, &finfo->size)) {
            തിരികെ CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_data->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
        parser->item_length = 0;
        parser->state.NT.പ്രധാന = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      നിർത്തുക;
    }
    നിർത്തുക;
  അവസ്ഥ PL_WINNT_FILENAME:
    തിരഞ്ഞെടുപ്പ്(parser->state.NT.sub.filename) {
    അവസ്ഥ PL_WINNT_FILENAME_PRESPACE:
      എങ്കിൽ(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      നിർത്തുക;
    അവസ്ഥ PL_WINNT_FILENAME_CONTENT:
      parser->item_length++;
      എങ്കിൽ(!len)
        തിരികെ CURLE_FTP_BAD_FILE_LIST;
      എങ്കിൽ(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        mem[len - 1] = 0;
        result = ftp_pl_തിരുകുക_finfo(data, infop);
        എങ്കിൽ(result)
          തിരികെ result;

        parser->state.NT.പ്രധാന = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      നിർത്തുക;
    അവസ്ഥ PL_WINNT_FILENAME_WINEOL:
      എങ്കിൽ(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        result = ftp_pl_തിരുകുക_finfo(data, infop);
        എങ്കിൽ(result)
          തിരികെ result;

        parser->state.NT.പ്രധാന = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      അല്ലെങ്കിൽ
        തിരികെ CURLE_FTP_BAD_FILE_LIST;

      നിർത്തുക;
    }
    നിർത്തുക;
  }

  തിരികെ CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(ഘടന Curl_easy *data)
{
  എങ്കിൽ(data->state.wildcardmatch) {
    ഘടന WildcardData * സ്ഥിരം wildcard = data->wildcard;
    തിരികെ wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  തിരികെ FALSE;
}

size_t Curl_ftp_parselist(അക്ഷരം *buffer, size_t size, size_t nmemb,
                          ശൂന്യം *connptr)
{
  size_t bufflen = size * nmemb;
  ഘടന Curl_easy *data = (ഘടന Curl_easy *)connptr;
  ഘടന ftp_wc *ftpwc = data->wildcard->ftpwc;
  ഘടന ftp_parselist_data *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode result;
  size_t retsize = bufflen;

  എങ്കിൽ(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    പോകുക fail;
  }

  എങ്കിൽ(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(buffer[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  ആകുമ്പോൾ(i < bufflen) { /* FSM */
    അക്ഷരം c = buffer[i];
    ഘടന fileinfo *infop;
    എങ്കിൽ(!parser->file_data) { /* tmp file data is not allocated yet */
      parser->file_data = Curl_fileinfo_alloc();
      എങ്കിൽ(!parser->file_data) {
        parser->error = CURLE_OUT_OF_MEMORY;
        പോകുക fail;
      }
      parser->item_offset = 0;
      parser->item_length = 0;
      curlx_dyn_init(&parser->file_data->buf, MAX_FTPLIST_BUFFER);
    }

    infop = parser->file_data;

    എങ്കിൽ(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_OUT_OF_MEMORY;
      പോകുക fail;
    }

    തിരഞ്ഞെടുപ്പ്(parser->os_type) {
    അവസ്ഥ OS_TYPE_UNIX:
      result = parse_unix(data, parser, infop, c);
      നിർത്തുക;
    അവസ്ഥ OS_TYPE_WIN_NT:
      result = parse_winnt(data, parser, infop, c);
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      retsize = bufflen + 1;
      പോകുക fail;
    }
    എങ്കിൽ(result) {
      parser->error = result;
      പോകുക fail;
    }

    i++;
  }
  തിരികെ retsize;

fail:

  /* Clean up any allocated memory. */
  എങ്കിൽ(parser->file_data) {
    Curl_fileinfo_cleanup(parser->file_data);
    parser->file_data = ശൂന്യ;
  }

  തിരികെ retsize;
}

#endif /* !CURL_DISABLE_FTP */
