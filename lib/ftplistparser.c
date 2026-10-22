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

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "fileinfo.h"
#अन्तर्भाव "llist.h"
#अन्तर्भाव "ftp.h"
#अन्तर्भाव "ftp-int.h"
#अन्तर्भाव "ftplistparser.h"
#अन्तर्भाव "curl_fnmatch.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "curlx/strparse.h"

प्रकारनिर्देश गणना {
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

प्रकारनिर्देश सङ्घ {
  गणना {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } total_dirsize;

  गणना {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_सङ्ख्या
  } hlinks;

  गणना {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  गणना {
    PL_UNIX_GROUP_PRESPACE = 0,
    PL_UNIX_GROUP_NAME
  } group;

  गणना {
    PL_UNIX_SIZE_PRESPACE = 0,
    PL_UNIX_SIZE_सङ्ख्या
  } size;

  गणना {
    PL_UNIX_TIME_PREPART1 = 0,
    PL_UNIX_TIME_PART1,
    PL_UNIX_TIME_PREPART2,
    PL_UNIX_TIME_PART2,
    PL_UNIX_TIME_PREPART3,
    PL_UNIX_TIME_PART3
  } time;

  गणना {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  गणना {
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

प्रकारनिर्देश गणना {
  PL_WINNT_DATE = 0,
  PL_WINNT_TIME,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

प्रकारनिर्देश सङ्घ {
  गणना {
    PL_WINNT_TIME_PRESPACE = 0,
    PL_WINNT_TIME_TIME
  } time;
  गणना {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  गणना {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
संरचना ftp_parselist_data {
  गणना {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  सङ्घ {
    संरचना {
      pl_unix_mainstate main;
      pl_unix_substate sub;
    } UNIX;

    संरचना {
      pl_winNT_mainstate main;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  संरचना fileinfo *file_data;
  unsigned पूर्णाङ्क item_length;
  size_t item_offset;
  संरचना {
    size_t filename;
    size_t user;
    size_t group;
    size_t time;
    size_t perm;
    size_t symlink_target;
  } offsets;
};

स्थिर शून्य fileinfo_dtor(शून्य *user, शून्य *element)
{
  (शून्य)user;
  Curl_fileinfo_cleanup(element);
}

शून्य Curl_wildcard_init(संरचना WildcardData *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

शून्य Curl_wildcard_dtor(संरचना WildcardData **wcp)
{
  संरचना WildcardData *wc = *wcp;
  जइ(!wc)
    प्रत्यागम;

  जइ(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = रिक्त;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, रिक्त);
  curlx_safefree(wc->path);
  curlx_safefree(wc->pattern);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = रिक्त;
}

संरचना ftp_parselist_data *Curl_ftp_parselist_data_alloc(शून्य)
{
  प्रत्यागम curlx_calloc(1, परिमाण(संरचना ftp_parselist_data));
}

शून्य Curl_ftp_parselist_data_free(संरचना ftp_parselist_data **parserp)
{
  संरचना ftp_parselist_data *parser = *parserp;
  जइ(parser)
    Curl_fileinfo_cleanup(parser->file_data);
  curlx_free(parser);
  *parserp = रिक्त;
}

CURLcode Curl_ftp_parselist_geterror(संरचना ftp_parselist_data *pl_data)
{
  प्रत्यागम pl_data->error;
}

#define FTP_LP_MALFORMATED_PERM 0x01000000

स्थिर unsigned पूर्णाङ्क ftp_pl_get_permission(ध्रुव वर्ण *str)
{
  unsigned पूर्णाङ्क permissions = 0;
  /* USER */
  जइ(str[0] == 'r')
    permissions |= 1 << 8;
  अण्णहा जइ(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  जइ(str[1] == 'w')
    permissions |= 1 << 7;
  अण्णहा जइ(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  जइ(str[2] == 'x')
    permissions |= 1 << 6;
  अण्णहा जइ(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  अण्णहा जइ(str[2] == 'S')
    permissions |= 1 << 11;
  अण्णहा जइ(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* GROUP */
  जइ(str[3] == 'r')
    permissions |= 1 << 5;
  अण्णहा जइ(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  जइ(str[4] == 'w')
    permissions |= 1 << 4;
  अण्णहा जइ(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  जइ(str[5] == 'x')
    permissions |= 1 << 3;
  अण्णहा जइ(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  अण्णहा जइ(str[5] == 'S')
    permissions |= 1 << 10;
  अण्णहा जइ(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  जइ(str[6] == 'r')
    permissions |= 1 << 2;
  अण्णहा जइ(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  जइ(str[7] == 'w')
    permissions |= 1 << 1;
  अण्णहा जइ(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  जइ(str[8] == 'x')
    permissions |= 1;
  अण्णहा जइ(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  अण्णहा जइ(str[8] == 'T')
    permissions |= 1 << 9;
  अण्णहा जइ(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  प्रत्यागम permissions;
}

स्थिर CURLcode ftp_pl_निवेशय_finfo(संरचना Curl_easy *data,
                                    संरचना fileinfo *infop)
{
  curl_fnmatch_callback compare;
  संरचना WildcardData *wc = data->wildcard;
  संरचना ftp_wc *ftpwc = wc->ftpwc;
  संरचना Curl_llist *llist = &wc->filelist;
  संरचना ftp_parselist_data *parser = ftpwc->parser;
  bool add = TRUE;
  संरचना curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  वर्ण *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->strings.group  = parser->offsets.group ?
                          str + parser->offsets.group : रिक्त;
  finfo->strings.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : रिक्त;
  finfo->strings.target = parser->offsets.symlink_target &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_target : रिक्त;
  finfo->strings.time   = str + parser->offsets.time;
  finfo->strings.user   = parser->offsets.user ?
                          str + parser->offsets.user : रिक्त;

  /* get correct fnmatch callback */
  compare = data->set.fnmatch;
  जइ(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  जइ(strchr(finfo->filename, '/'))
    add = FALSE;
  अण्णहा {
    /* a real file name cannot contain a slash */
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fnmatch_data);
    जइ(compare(data->set.fnmatch_data, wc->pattern, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      जइ((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->strings.target &&
         strstr(finfo->strings.target, " -> ")) {
        add = FALSE;
      }
    }
    अण्णहा {
      add = FALSE;
    }
    CURL_CBAPI_END(&guard);
  }

  जइ(add) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  अण्णहा {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_data = रिक्त;
  प्रत्यागम CURLE_OK;
}

#define MAX_FTPLIST_BUFFER 10000 /* arbitrarily set */

स्थिर CURLcode unix_filetype(ध्रुव वर्ण c, curlfiletype *t)
{
  विकल्प(c) {
  पक्ष '-':
    *t = CURLFILETYPE_FILE;
    भङ्ग;
  पक्ष 'd':
    *t = CURLFILETYPE_DIRECTORY;
    भङ्ग;
  पक्ष 'l':
    *t = CURLFILETYPE_SYMLINK;
    भङ्ग;
  पक्ष 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    भङ्ग;
  पक्ष 's':
    *t = CURLFILETYPE_SOCKET;
    भङ्ग;
  पक्ष 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    भङ्ग;
  पक्ष 'b':
    *t = CURLFILETYPE_DEVICE_BLOCK;
    भङ्ग;
  पक्ष 'D':
    *t = CURLFILETYPE_DOOR;
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_totalsize(संरचना ftp_parselist_data *parser,
                                     संरचना fileinfo *infop,
                                     ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  विकल्प(parser->state.UNIX.sub.total_dirsize) {
  पक्ष PL_UNIX_TOTALSIZE_INIT:
    जइ(c == 't') {
      parser->state.UNIX.sub.total_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_length++;
    }
    अण्णहा {
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    भङ्ग;
  पक्ष PL_UNIX_TOTALSIZE_READING:
    parser->item_length++;
    जइ(c == '\r') {
      parser->item_length--;
      जइ(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    अण्णहा जइ(c == '\n') {
      mem[parser->item_length - 1] = 0;
      जइ(!strncmp("total ", mem, 6)) {
        ध्रुव वर्ण *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        जाव(ISDIGIT(*endptr))
          endptr++;
        जइ(*endptr) {
          प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.main = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      अण्णहा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    }
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_permission(संरचना ftp_parselist_data *parser,
                                      संरचना fileinfo *infop,
                                      ध्रुव वर्ण c)
{
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_length++;
  जइ((parser->item_length <= 9) && !strchr("rwx-tTsS", c))
    प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

  अण्णहा जइ(parser->item_length == 10) {
    unsigned पूर्णाङ्क perm;
    जइ(c != ' ')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_offset);
    जइ(perm & FTP_LP_MALFORMATED_PERM)
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

    parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_PERM;
    parser->file_data->info.perm = perm;
    parser->offsets.perm = parser->item_offset;

    parser->item_length = 0;
    parser->state.UNIX.main = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_hlinks(संरचना ftp_parselist_data *parser,
                                  संरचना fileinfo *infop,
                                  ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);

  विकल्प(parser->state.UNIX.sub.hlinks) {
  पक्ष PL_UNIX_HLINKS_PRESPACE:
    जइ(c != ' ') {
      जइ(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_सङ्ख्या;
      }
      अण्णहा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    }
    भङ्ग;
  पक्ष PL_UNIX_HLINKS_सङ्ख्या:
    parser->item_length++;
    जइ(c == ' ') {
      ध्रुव वर्ण *p = &mem[parser->item_offset];
      curl_off_t hlinks;
      mem[parser->item_offset + parser->item_length - 1] = 0;

      जइ(!curlx_str_सङ्ख्या(&p, &hlinks, LONG_MAX)) {
        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_HLINKCOUNT;
        parser->file_data->info.hardlinks = (long)hlinks;
      }
      parser->item_length = 0;
      parser->item_offset = 0;
      parser->state.UNIX.main = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    अण्णहा जइ(!ISDIGIT(c))
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_user(संरचना ftp_parselist_data *parser,
                                संरचना fileinfo *infop,
                                ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  विकल्प(parser->state.UNIX.sub.user) {
  पक्ष PL_UNIX_USER_PRESPACE:
    जइ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    भङ्ग;
  पक्ष PL_UNIX_USER_PARSING:
    parser->item_length++;
    जइ(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.user = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_GROUP;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_group(संरचना ftp_parselist_data *parser,
                                 संरचना fileinfo *infop,
                                 ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  विकल्प(parser->state.UNIX.sub.group) {
  पक्ष PL_UNIX_GROUP_PRESPACE:
    जइ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_NAME;
    }
    भङ्ग;
  पक्ष PL_UNIX_GROUP_NAME:
    parser->item_length++;
    जइ(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.group = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_SIZE;
      parser->state.UNIX.sub.size = PL_UNIX_SIZE_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_size(संरचना ftp_parselist_data *parser,
                                संरचना fileinfo *infop,
                                ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  विकल्प(parser->state.UNIX.sub.size) {
  पक्ष PL_UNIX_SIZE_PRESPACE:
    जइ(c != ' ') {
      जइ(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.size = PL_UNIX_SIZE_सङ्ख्या;
      }
      अण्णहा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    }
    भङ्ग;
  पक्ष PL_UNIX_SIZE_सङ्ख्या:
    parser->item_length++;
    जइ(c == ' ') {
      ध्रुव वर्ण *p = mem + parser->item_offset;
      curl_off_t fsize;
      mem[parser->item_offset + parser->item_length - 1] = 0;
      जइ(!curlx_str_numblanks(&p, &fsize)) {
        जइ(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
          parser->file_data->info.size = fsize;
        }
        parser->item_length = 0;
        parser->item_offset = 0;
        parser->state.UNIX.main = PL_UNIX_TIME;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART1;
      }
    }
    अण्णहा जइ(!ISDIGIT(c))
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_time(संरचना ftp_parselist_data *parser,
                                संरचना fileinfo *infop,
                                ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  संरचना curl_fileinfo *finfo = &infop->info;

  विकल्प(parser->state.UNIX.sub.time) {
  पक्ष PL_UNIX_TIME_PREPART1:
    जइ(c != ' ') {
      जइ(ISALNUM(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART1;
      }
      अण्णहा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    }
    भङ्ग;
  पक्ष PL_UNIX_TIME_PART1:
    parser->item_length++;
    जइ(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART2;

    अण्णहा जइ(!ISALNUM(c) && c != '.')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

    भङ्ग;
  पक्ष PL_UNIX_TIME_PREPART2:
    parser->item_length++;
    जइ(c != ' ') {
      जइ(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART2;
      अण्णहा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    }
    भङ्ग;
  पक्ष PL_UNIX_TIME_PART2:
    parser->item_length++;
    जइ(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART3;
    अण्णहा जइ(!ISALNUM(c) && c != '.')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    भङ्ग;
  पक्ष PL_UNIX_TIME_PREPART3:
    parser->item_length++;
    जइ(c != ' ') {
      जइ(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART3;
      अण्णहा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    }
    भङ्ग;
  पक्ष PL_UNIX_TIME_PART3:
    parser->item_length++;
    जइ(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.time = parser->item_offset;
      जइ(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.main = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      अण्णहा {
        parser->state.UNIX.main = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    अण्णहा जइ(!ISALNUM(c) && c != '.' && c != ':')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_filename(संरचना Curl_easy *data,
                                    संरचना ftp_parselist_data *parser,
                                    संरचना fileinfo *infop,
                                    ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  विकल्प(parser->state.UNIX.sub.filename) {
  पक्ष PL_UNIX_FILENAME_PRESPACE:
    जइ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    भङ्ग;
  पक्ष PL_UNIX_FILENAME_NAME:
    parser->item_length++;
    जइ(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    अण्णहा जइ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      result = ftp_pl_निवेशय_finfo(data, infop);
    }
    भङ्ग;
  पक्ष PL_UNIX_FILENAME_WINDOWSEOL:
    जइ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      result = ftp_pl_निवेशय_finfo(data, infop);
    }
    अण्णहा
      result = CURLE_FTP_BAD_FILE_LIST;
    भङ्ग;
  }
  प्रत्यागम result;
}

स्थिर CURLcode parse_unix_symlink(संरचना Curl_easy *data,
                                   संरचना ftp_parselist_data *parser,
                                   संरचना fileinfo *infop,
                                   ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  विकल्प(parser->state.UNIX.sub.symlink) {
  पक्ष PL_UNIX_SYMLINK_PRESPACE:
    जइ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    भङ्ग;
  पक्ष PL_UNIX_SYMLINK_NAME:
    parser->item_length++;
    जइ(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    अण्णहा जइ(c == '\r' || c == '\n')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

    भङ्ग;
  पक्ष PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_length++;
    जइ(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    अण्णहा जइ(c == '\r' || c == '\n')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    अण्णहा
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    भङ्ग;
  पक्ष PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_length++;
    जइ(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    अण्णहा जइ(c == '\r' || c == '\n')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    अण्णहा
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    भङ्ग;
  पक्ष PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_length++;
    जइ(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_offset + parser->item_length - 4] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->item_length = 0;
      parser->item_offset = 0;
    }
    अण्णहा जइ(c == '\r' || c == '\n')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    अण्णहा
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    भङ्ग;
  पक्ष PL_UNIX_SYMLINK_PRETARGET4:
    जइ(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_offset = len - 1;
      parser->item_length = 1;
    }
    अण्णहा
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

    भङ्ग;
  पक्ष PL_UNIX_SYMLINK_TARGET:
    parser->item_length++;
    जइ(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    अण्णहा जइ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_निवेशय_finfo(data, infop);
      जइ(result)
        भङ्ग;

      parser->state.UNIX.main = PL_UNIX_FILETYPE;
    }
    भङ्ग;
  पक्ष PL_UNIX_SYMLINK_WINDOWSEOL:
    जइ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_निवेशय_finfo(data, infop);
      जइ(result)
        भङ्ग;

      parser->state.UNIX.main = PL_UNIX_FILETYPE;
    }
    अण्णहा
      result = CURLE_FTP_BAD_FILE_LIST;

    भङ्ग;
  }
  प्रत्यागम result;
}

स्थिर CURLcode parse_unix(संरचना Curl_easy *data,
                           संरचना ftp_parselist_data *parser,
                           संरचना fileinfo *infop,
                           ध्रुव वर्ण c)
{
  संरचना curl_fileinfo *finfo = &infop->info;
  CURLcode result = CURLE_OK;

  विकल्प(parser->state.UNIX.main) {
  पक्ष PL_UNIX_TOTALSIZE:
    result = parse_unix_totalsize(parser, infop, c);
    जइ(result)
      भङ्ग;
    जइ(parser->state.UNIX.main != PL_UNIX_FILETYPE)
      भङ्ग;
    FALLTHROUGH();
  पक्ष PL_UNIX_FILETYPE:
    result = unix_filetype(c, &finfo->filetype);
    जइ(!result) {
      parser->state.UNIX.main = PL_UNIX_PERMISSION;
      parser->item_length = 0;
      parser->item_offset = 1;
    }
    भङ्ग;
  पक्ष PL_UNIX_PERMISSION:
    result = parse_unix_permission(parser, infop, c);
    भङ्ग;
  पक्ष PL_UNIX_HLINKS:
    result = parse_unix_hlinks(parser, infop, c);
    भङ्ग;
  पक्ष PL_UNIX_USER:
    result = parse_unix_user(parser, infop, c);
    भङ्ग;
  पक्ष PL_UNIX_GROUP:
    result = parse_unix_group(parser, infop, c);
    भङ्ग;
  पक्ष PL_UNIX_SIZE:
    result = parse_unix_size(parser, infop, c);
    भङ्ग;
  पक्ष PL_UNIX_TIME:
    result = parse_unix_time(parser, infop, c);
    भङ्ग;
  पक्ष PL_UNIX_FILENAME:
    result = parse_unix_filename(data, parser, infop, c);
    भङ्ग;
  पक्ष PL_UNIX_SYMLINK:
    result = parse_unix_symlink(data, parser, infop, c);
    भङ्ग;
  }
  प्रत्यागम result;
}

स्थिर CURLcode parse_winnt(संरचना Curl_easy *data,
                            संरचना ftp_parselist_data *parser,
                            संरचना fileinfo *infop,
                            ध्रुव वर्ण c)
{
  संरचना curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  विकल्प(parser->state.NT.main) {
  पक्ष PL_WINNT_DATE:
    parser->item_length++;
    जइ(parser->item_length < 9) {
      जइ(!strchr("0123456789-", c)) { /* only simple control */
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
      }
    }
    अण्णहा जइ(parser->item_length == 9) {
      जइ(c == ' ') {
        parser->state.NT.main = PL_WINNT_TIME;
        parser->state.NT.sub.time = PL_WINNT_TIME_PRESPACE;
      }
      अण्णहा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    }
    अण्णहा
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    भङ्ग;
  पक्ष PL_WINNT_TIME:
    parser->item_length++;
    विकल्प(parser->state.NT.sub.time) {
    पक्ष PL_WINNT_TIME_PRESPACE:
      जइ(!ISBLANK(c))
        parser->state.NT.sub.time = PL_WINNT_TIME_TIME;
      भङ्ग;
    पक्ष PL_WINNT_TIME_TIME:
      जइ(c == ' ') {
        parser->offsets.time = parser->item_offset;
        mem[parser->item_offset + parser->item_length - 1] = 0;
        parser->state.NT.main = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_length = 0;
      }
      अण्णहा जइ(!strchr("APM0123456789:", c))
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
      भङ्ग;
    }
    भङ्ग;
  पक्ष PL_WINNT_DIRORSIZE:
    विकल्प(parser->state.NT.sub.dirorsize) {
    पक्ष PL_WINNT_DIRORSIZE_PRESPACE:
      जइ(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      भङ्ग;
    पक्ष PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_length++;
      जइ(c == ' ') {
        mem[parser->item_offset + parser->item_length - 1] = 0;
        जइ(!strcmp("<DIR>", mem + parser->item_offset)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->size = 0;
        }
        अण्णहा {
          ध्रुव वर्ण *p = mem + parser->item_offset;
          जइ(curlx_str_numblanks(&p, &finfo->size)) {
            प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_data->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
        parser->item_length = 0;
        parser->state.NT.main = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      भङ्ग;
    }
    भङ्ग;
  पक्ष PL_WINNT_FILENAME:
    विकल्प(parser->state.NT.sub.filename) {
    पक्ष PL_WINNT_FILENAME_PRESPACE:
      जइ(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      भङ्ग;
    पक्ष PL_WINNT_FILENAME_CONTENT:
      parser->item_length++;
      जइ(!len)
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
      जइ(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      अण्णहा जइ(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        mem[len - 1] = 0;
        result = ftp_pl_निवेशय_finfo(data, infop);
        जइ(result)
          प्रत्यागम result;

        parser->state.NT.main = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      भङ्ग;
    पक्ष PL_WINNT_FILENAME_WINEOL:
      जइ(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        result = ftp_pl_निवेशय_finfo(data, infop);
        जइ(result)
          प्रत्यागम result;

        parser->state.NT.main = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      अण्णहा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

      भङ्ग;
    }
    भङ्ग;
  }

  प्रत्यागम CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(संरचना Curl_easy *data)
{
  जइ(data->state.wildcardmatch) {
    संरचना WildcardData * ध्रुव wildcard = data->wildcard;
    प्रत्यागम wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  प्रत्यागम FALSE;
}

size_t Curl_ftp_parselist(वर्ण *buffer, size_t size, size_t nmemb,
                          शून्य *connptr)
{
  size_t bufflen = size * nmemb;
  संरचना Curl_easy *data = (संरचना Curl_easy *)connptr;
  संरचना ftp_wc *ftpwc = data->wildcard->ftpwc;
  संरचना ftp_parselist_data *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode result;
  size_t retsize = bufflen;

  जइ(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    गच्छ fail;
  }

  जइ(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(buffer[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  जाव(i < bufflen) { /* FSM */
    वर्ण c = buffer[i];
    संरचना fileinfo *infop;
    जइ(!parser->file_data) { /* tmp file data is not allocated yet */
      parser->file_data = Curl_fileinfo_alloc();
      जइ(!parser->file_data) {
        parser->error = CURLE_OUT_OF_MEMORY;
        गच्छ fail;
      }
      parser->item_offset = 0;
      parser->item_length = 0;
      curlx_dyn_init(&parser->file_data->buf, MAX_FTPLIST_BUFFER);
    }

    infop = parser->file_data;

    जइ(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_OUT_OF_MEMORY;
      गच्छ fail;
    }

    विकल्प(parser->os_type) {
    पक्ष OS_TYPE_UNIX:
      result = parse_unix(data, parser, infop, c);
      भङ्ग;
    पक्ष OS_TYPE_WIN_NT:
      result = parse_winnt(data, parser, infop, c);
      भङ्ग;
    स्वभाव:
      retsize = bufflen + 1;
      गच्छ fail;
    }
    जइ(result) {
      parser->error = result;
      गच्छ fail;
    }

    i++;
  }
  प्रत्यागम retsize;

fail:

  /* Clean up any allocated memory. */
  जइ(parser->file_data) {
    Curl_fileinfo_cleanup(parser->file_data);
    parser->file_data = रिक्त;
  }

  प्रत्यागम retsize;
}

#endif /* !CURL_DISABLE_FTP */
