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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

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

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "fileinfo.h"
#ଅନ୍ତର୍ଭାଵ "llist.h"
#ଅନ୍ତର୍ଭାଵ "ftp.h"
#ଅନ୍ତର୍ଭାଵ "ftp-int.h"
#ଅନ୍ତର୍ଭାଵ "ftplistparser.h"
#ଅନ୍ତର୍ଭାଵ "curl_fnmatch.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂଘ {
  ଗଣନା {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } total_dirsize;

  ଗଣନା {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_ସଂଖ୍ୟା
  } hlinks;

  ଗଣନା {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  ଗଣନା {
    PL_UNIX_GROUP_PRESPACE = 0,
    PL_UNIX_GROUP_NAME
  } group;

  ଗଣନା {
    PL_UNIX_SIZE_PRESPACE = 0,
    PL_UNIX_SIZE_ସଂଖ୍ୟା
  } size;

  ଗଣନା {
    PL_UNIX_TIME_PREPART1 = 0,
    PL_UNIX_TIME_PART1,
    PL_UNIX_TIME_PREPART2,
    PL_UNIX_TIME_PART2,
    PL_UNIX_TIME_PREPART3,
    PL_UNIX_TIME_PART3
  } time;

  ଗଣନା {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  ଗଣନା {
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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  PL_WINNT_DATE = 0,
  PL_WINNT_TIME,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂଘ {
  ଗଣନା {
    PL_WINNT_TIME_PRESPACE = 0,
    PL_WINNT_TIME_TIME
  } time;
  ଗଣନା {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  ଗଣନା {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
ସଂରଚନା ftp_parselist_data {
  ଗଣନା {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  ସଂଘ {
    ସଂରଚନା {
      pl_unix_mainstate ମୁଖ୍ୟ;
      pl_unix_substate sub;
    } UNIX;

    ସଂରଚନା {
      pl_winNT_mainstate ମୁଖ୍ୟ;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  ସଂରଚନା fileinfo *file_data;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା item_length;
  size_t item_offset;
  ସଂରଚନା {
    size_t filename;
    size_t user;
    size_t group;
    size_t time;
    size_t perm;
    size_t symlink_target;
  } offsets;
};

ସ୍ଥିର ଶୂନ୍ୟ fileinfo_dtor(ଶୂନ୍ୟ *user, ଶୂନ୍ୟ *element)
{
  (ଶୂନ୍ୟ)user;
  Curl_fileinfo_cleanup(element);
}

ଶୂନ୍ୟ Curl_wildcard_init(ସଂରଚନା WildcardData *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

ଶୂନ୍ୟ Curl_wildcard_dtor(ସଂରଚନା WildcardData **wcp)
{
  ସଂରଚନା WildcardData *wc = *wcp;
  ଯଦି(!wc)
    ଫେରନ୍ତୁ;

  ଯଦି(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = ଖାଲି;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, ଖାଲି);
  curlx_safefree(wc->path);
  curlx_safefree(wc->pattern);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = ଖାଲି;
}

ସଂରଚନା ftp_parselist_data *Curl_ftp_parselist_data_alloc(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ curlx_calloc(1, ଆକାର(ସଂରଚନା ftp_parselist_data));
}

ଶୂନ୍ୟ Curl_ftp_parselist_data_free(ସଂରଚନା ftp_parselist_data **parserp)
{
  ସଂରଚନା ftp_parselist_data *parser = *parserp;
  ଯଦି(parser)
    Curl_fileinfo_cleanup(parser->file_data);
  curlx_free(parser);
  *parserp = ଖାଲି;
}

CURLcode Curl_ftp_parselist_geterror(ସଂରଚନା ftp_parselist_data *pl_data)
{
  ଫେରନ୍ତୁ pl_data->error;
}

#define FTP_LP_MALFORMATED_PERM 0x01000000

ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ftp_pl_get_permission(ଅଚଳ ଅକ୍ଷର *str)
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା permissions = 0;
  /* USER */
  ଯଦି(str[0] == 'r')
    permissions |= 1 << 8;
  ଅନ୍ୟଥା ଯଦି(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ଯଦି(str[1] == 'w')
    permissions |= 1 << 7;
  ଅନ୍ୟଥା ଯଦି(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  ଯଦି(str[2] == 'x')
    permissions |= 1 << 6;
  ଅନ୍ୟଥା ଯଦି(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  ଅନ୍ୟଥା ଯଦି(str[2] == 'S')
    permissions |= 1 << 11;
  ଅନ୍ୟଥା ଯଦି(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* GROUP */
  ଯଦି(str[3] == 'r')
    permissions |= 1 << 5;
  ଅନ୍ୟଥା ଯଦି(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ଯଦି(str[4] == 'w')
    permissions |= 1 << 4;
  ଅନ୍ୟଥା ଯଦି(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ଯଦି(str[5] == 'x')
    permissions |= 1 << 3;
  ଅନ୍ୟଥା ଯଦି(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  ଅନ୍ୟଥା ଯଦି(str[5] == 'S')
    permissions |= 1 << 10;
  ଅନ୍ୟଥା ଯଦି(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  ଯଦି(str[6] == 'r')
    permissions |= 1 << 2;
  ଅନ୍ୟଥା ଯଦି(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ଯଦି(str[7] == 'w')
    permissions |= 1 << 1;
  ଅନ୍ୟଥା ଯଦି(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ଯଦି(str[8] == 'x')
    permissions |= 1;
  ଅନ୍ୟଥା ଯଦି(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  ଅନ୍ୟଥା ଯଦି(str[8] == 'T')
    permissions |= 1 << 9;
  ଅନ୍ୟଥା ଯଦି(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  ଫେରନ୍ତୁ permissions;
}

ସ୍ଥିର CURLcode ftp_pl_ଭର୍ତ୍ତି_finfo(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା fileinfo *infop)
{
  curl_fnmatch_callback compare;
  ସଂରଚନା WildcardData *wc = data->wildcard;
  ସଂରଚନା ftp_wc *ftpwc = wc->ftpwc;
  ସଂରଚନା Curl_llist *llist = &wc->filelist;
  ସଂରଚନା ftp_parselist_data *parser = ftpwc->parser;
  bool add = TRUE;
  ସଂରଚନା curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  ଅକ୍ଷର *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->strings.group  = parser->offsets.group ?
                          str + parser->offsets.group : ଖାଲି;
  finfo->strings.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : ଖାଲି;
  finfo->strings.target = parser->offsets.symlink_target &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_target : ଖାଲି;
  finfo->strings.time   = str + parser->offsets.time;
  finfo->strings.user   = parser->offsets.user ?
                          str + parser->offsets.user : ଖାଲି;

  /* get correct fnmatch callback */
  compare = data->set.fnmatch;
  ଯଦି(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  ଯଦି(strchr(finfo->filename, '/'))
    add = FALSE;
  ଅନ୍ୟଥା {
    /* a real file name cannot contain a slash */
    ସଂରଚନା Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fnmatch_data);
    ଯଦି(compare(data->set.fnmatch_data, wc->pattern, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      ଯଦି((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->strings.target &&
         strstr(finfo->strings.target, " -> ")) {
        add = FALSE;
      }
    }
    ଅନ୍ୟଥା {
      add = FALSE;
    }
    CURL_CBAPI_END(&guard);
  }

  ଯଦି(add) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  ଅନ୍ୟଥା {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_data = ଖାଲି;
  ଫେରନ୍ତୁ CURLE_OK;
}

#define MAX_FTPLIST_BUFFER 10000 /* arbitrarily set */

ସ୍ଥିର CURLcode unix_filetype(ଅଚଳ ଅକ୍ଷର c, curlfiletype *t)
{
  ବାଛ(c) {
  ସ୍ଥିତି '-':
    *t = CURLFILETYPE_FILE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'd':
    *t = CURLFILETYPE_DIRECTORY;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'l':
    *t = CURLFILETYPE_SYMLINK;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 's':
    *t = CURLFILETYPE_SOCKET;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'b':
    *t = CURLFILETYPE_DEVICE_BLOCK;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'D':
    *t = CURLFILETYPE_DOOR;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode parse_unix_totalsize(ସଂରଚନା ftp_parselist_data *parser,
                                     ସଂରଚନା fileinfo *infop,
                                     ଅଚଳ ଅକ୍ଷର c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ଅକ୍ଷର *mem = curlx_dyn_ptr(&infop->buf);
  ବାଛ(parser->state.UNIX.sub.total_dirsize) {
  ସ୍ଥିତି PL_UNIX_TOTALSIZE_INIT:
    ଯଦି(c == 't') {
      parser->state.UNIX.sub.total_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_length++;
    }
    ଅନ୍ୟଥା {
      parser->state.UNIX.ମୁଖ୍ୟ = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_TOTALSIZE_READING:
    parser->item_length++;
    ଯଦି(c == '\r') {
      parser->item_length--;
      ଯଦି(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    ଅନ୍ୟଥା ଯଦି(c == '\n') {
      mem[parser->item_length - 1] = 0;
      ଯଦି(!strncmp("total ", mem, 6)) {
        ଅଚଳ ଅକ୍ଷର *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        ଯେତେବେଳେ(ISDIGIT(*endptr))
          endptr++;
        ଯଦି(*endptr) {
          ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.ମୁଖ୍ୟ = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      ଅନ୍ୟଥା
        ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
    }
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode parse_unix_permission(ସଂରଚନା ftp_parselist_data *parser,
                                      ସଂରଚନା fileinfo *infop,
                                      ଅଚଳ ଅକ୍ଷର c)
{
  ଅକ୍ଷର *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_length++;
  ଯଦି((parser->item_length <= 9) && !strchr("rwx-tTsS", c))
    ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;

  ଅନ୍ୟଥା ଯଦି(parser->item_length == 10) {
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା perm;
    ଯଦି(c != ' ')
      ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_offset);
    ଯଦି(perm & FTP_LP_MALFORMATED_PERM)
      ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;

    parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_PERM;
    parser->file_data->info.perm = perm;
    parser->offsets.perm = parser->item_offset;

    parser->item_length = 0;
    parser->state.UNIX.ମୁଖ୍ୟ = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode parse_unix_hlinks(ସଂରଚନା ftp_parselist_data *parser,
                                  ସଂରଚନା fileinfo *infop,
                                  ଅଚଳ ଅକ୍ଷର c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ଅକ୍ଷର *mem = curlx_dyn_ptr(&infop->buf);

  ବାଛ(parser->state.UNIX.sub.hlinks) {
  ସ୍ଥିତି PL_UNIX_HLINKS_PRESPACE:
    ଯଦି(c != ' ') {
      ଯଦି(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_ସଂଖ୍ୟା;
      }
      ଅନ୍ୟଥା
        ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_HLINKS_ସଂଖ୍ୟା:
    parser->item_length++;
    ଯଦି(c == ' ') {
      ଅଚଳ ଅକ୍ଷର *p = &mem[parser->item_offset];
      curl_off_t hlinks;
      mem[parser->item_offset + parser->item_length - 1] = 0;

      ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &hlinks, LONG_MAX)) {
        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_HLINKCOUNT;
        parser->file_data->info.hardlinks = (long)hlinks;
      }
      parser->item_length = 0;
      parser->item_offset = 0;
      parser->state.UNIX.ମୁଖ୍ୟ = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    ଅନ୍ୟଥା ଯଦି(!ISDIGIT(c))
      ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;

    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode parse_unix_user(ସଂରଚନା ftp_parselist_data *parser,
                                ସଂରଚନା fileinfo *infop,
                                ଅଚଳ ଅକ୍ଷର c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ଅକ୍ଷର *mem = curlx_dyn_ptr(&infop->buf);
  ବାଛ(parser->state.UNIX.sub.user) {
  ସ୍ଥିତି PL_UNIX_USER_PRESPACE:
    ଯଦି(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_USER_PARSING:
    parser->item_length++;
    ଯଦି(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.user = parser->item_offset;
      parser->state.UNIX.ମୁଖ୍ୟ = PL_UNIX_GROUP;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode parse_unix_group(ସଂରଚନା ftp_parselist_data *parser,
                                 ସଂରଚନା fileinfo *infop,
                                 ଅଚଳ ଅକ୍ଷର c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ଅକ୍ଷର *mem = curlx_dyn_ptr(&infop->buf);
  ବାଛ(parser->state.UNIX.sub.group) {
  ସ୍ଥିତି PL_UNIX_GROUP_PRESPACE:
    ଯଦି(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_NAME;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_GROUP_NAME:
    parser->item_length++;
    ଯଦି(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.group = parser->item_offset;
      parser->state.UNIX.ମୁଖ୍ୟ = PL_UNIX_SIZE;
      parser->state.UNIX.sub.size = PL_UNIX_SIZE_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode parse_unix_size(ସଂରଚନା ftp_parselist_data *parser,
                                ସଂରଚନା fileinfo *infop,
                                ଅଚଳ ଅକ୍ଷର c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ଅକ୍ଷର *mem = curlx_dyn_ptr(&infop->buf);
  ବାଛ(parser->state.UNIX.sub.size) {
  ସ୍ଥିତି PL_UNIX_SIZE_PRESPACE:
    ଯଦି(c != ' ') {
      ଯଦି(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.size = PL_UNIX_SIZE_ସଂଖ୍ୟା;
      }
      ଅନ୍ୟଥା
        ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_SIZE_ସଂଖ୍ୟା:
    parser->item_length++;
    ଯଦି(c == ' ') {
      ଅଚଳ ଅକ୍ଷର *p = mem + parser->item_offset;
      curl_off_t fsize;
      mem[parser->item_offset + parser->item_length - 1] = 0;
      ଯଦି(!curlx_str_numblanks(&p, &fsize)) {
        ଯଦି(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
          parser->file_data->info.size = fsize;
        }
        parser->item_length = 0;
        parser->item_offset = 0;
        parser->state.UNIX.ମୁଖ୍ୟ = PL_UNIX_TIME;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART1;
      }
    }
    ଅନ୍ୟଥା ଯଦି(!ISDIGIT(c))
      ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;

    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode parse_unix_time(ସଂରଚନା ftp_parselist_data *parser,
                                ସଂରଚନା fileinfo *infop,
                                ଅଚଳ ଅକ୍ଷର c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ଅକ୍ଷର *mem = curlx_dyn_ptr(&infop->buf);
  ସଂରଚନା curl_fileinfo *finfo = &infop->info;

  ବାଛ(parser->state.UNIX.sub.time) {
  ସ୍ଥିତି PL_UNIX_TIME_PREPART1:
    ଯଦି(c != ' ') {
      ଯଦି(ISALNUM(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART1;
      }
      ଅନ୍ୟଥା
        ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_TIME_PART1:
    parser->item_length++;
    ଯଦି(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART2;

    ଅନ୍ୟଥା ଯଦି(!ISALNUM(c) && c != '.')
      ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;

    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_TIME_PREPART2:
    parser->item_length++;
    ଯଦି(c != ' ') {
      ଯଦି(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART2;
      ଅନ୍ୟଥା
        ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_TIME_PART2:
    parser->item_length++;
    ଯଦି(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART3;
    ଅନ୍ୟଥା ଯଦି(!ISALNUM(c) && c != '.')
      ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_TIME_PREPART3:
    parser->item_length++;
    ଯଦି(c != ' ') {
      ଯଦି(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART3;
      ଅନ୍ୟଥା
        ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_TIME_PART3:
    parser->item_length++;
    ଯଦି(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.time = parser->item_offset;
      ଯଦି(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.ମୁଖ୍ୟ = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      ଅନ୍ୟଥା {
        parser->state.UNIX.ମୁଖ୍ୟ = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    ଅନ୍ୟଥା ଯଦି(!ISALNUM(c) && c != '.' && c != ':')
      ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode parse_unix_filename(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା ftp_parselist_data *parser,
                                    ସଂରଚନା fileinfo *infop,
                                    ଅଚଳ ଅକ୍ଷର c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ଅକ୍ଷର *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  ବାଛ(parser->state.UNIX.sub.filename) {
  ସ୍ଥିତି PL_UNIX_FILENAME_PRESPACE:
    ଯଦି(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_FILENAME_NAME:
    parser->item_length++;
    ଯଦି(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    ଅନ୍ୟଥା ଯଦି(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.ମୁଖ୍ୟ = PL_UNIX_FILETYPE;
      result = ftp_pl_ଭର୍ତ୍ତି_finfo(data, infop);
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_FILENAME_WINDOWSEOL:
    ଯଦି(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.ମୁଖ୍ୟ = PL_UNIX_FILETYPE;
      result = ftp_pl_ଭର୍ତ୍ତି_finfo(data, infop);
    }
    ଅନ୍ୟଥା
      result = CURLE_FTP_BAD_FILE_LIST;
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode parse_unix_symlink(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ftp_parselist_data *parser,
                                   ସଂରଚନା fileinfo *infop,
                                   ଅଚଳ ଅକ୍ଷର c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ଅକ୍ଷର *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  ବାଛ(parser->state.UNIX.sub.symlink) {
  ସ୍ଥିତି PL_UNIX_SYMLINK_PRESPACE:
    ଯଦି(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_SYMLINK_NAME:
    parser->item_length++;
    ଯଦି(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    ଅନ୍ୟଥା ଯଦି(c == '\r' || c == '\n')
      ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;

    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_length++;
    ଯଦି(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    ଅନ୍ୟଥା ଯଦି(c == '\r' || c == '\n')
      ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
    ଅନ୍ୟଥା
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_length++;
    ଯଦି(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    ଅନ୍ୟଥା ଯଦି(c == '\r' || c == '\n')
      ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
    ଅନ୍ୟଥା
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_length++;
    ଯଦି(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_offset + parser->item_length - 4] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->item_length = 0;
      parser->item_offset = 0;
    }
    ଅନ୍ୟଥା ଯଦି(c == '\r' || c == '\n')
      ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
    ଅନ୍ୟଥା
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_SYMLINK_PRETARGET4:
    ଯଦି(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_offset = len - 1;
      parser->item_length = 1;
    }
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;

    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_SYMLINK_TARGET:
    parser->item_length++;
    ଯଦି(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    ଅନ୍ୟଥା ଯଦି(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_ଭର୍ତ୍ତି_finfo(data, infop);
      ଯଦି(result)
        ଭାଙ୍ଗ;

      parser->state.UNIX.ମୁଖ୍ୟ = PL_UNIX_FILETYPE;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_SYMLINK_WINDOWSEOL:
    ଯଦି(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_ଭର୍ତ୍ତି_finfo(data, infop);
      ଯଦି(result)
        ଭାଙ୍ଗ;

      parser->state.UNIX.ମୁଖ୍ୟ = PL_UNIX_FILETYPE;
    }
    ଅନ୍ୟଥା
      result = CURLE_FTP_BAD_FILE_LIST;

    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode parse_unix(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା ftp_parselist_data *parser,
                           ସଂରଚନା fileinfo *infop,
                           ଅଚଳ ଅକ୍ଷର c)
{
  ସଂରଚନା curl_fileinfo *finfo = &infop->info;
  CURLcode result = CURLE_OK;

  ବାଛ(parser->state.UNIX.ମୁଖ୍ୟ) {
  ସ୍ଥିତି PL_UNIX_TOTALSIZE:
    result = parse_unix_totalsize(parser, infop, c);
    ଯଦି(result)
      ଭାଙ୍ଗ;
    ଯଦି(parser->state.UNIX.ମୁଖ୍ୟ != PL_UNIX_FILETYPE)
      ଭାଙ୍ଗ;
    FALLTHROUGH();
  ସ୍ଥିତି PL_UNIX_FILETYPE:
    result = unix_filetype(c, &finfo->filetype);
    ଯଦି(!result) {
      parser->state.UNIX.ମୁଖ୍ୟ = PL_UNIX_PERMISSION;
      parser->item_length = 0;
      parser->item_offset = 1;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_PERMISSION:
    result = parse_unix_permission(parser, infop, c);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_HLINKS:
    result = parse_unix_hlinks(parser, infop, c);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_USER:
    result = parse_unix_user(parser, infop, c);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_GROUP:
    result = parse_unix_group(parser, infop, c);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_SIZE:
    result = parse_unix_size(parser, infop, c);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_TIME:
    result = parse_unix_time(parser, infop, c);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_FILENAME:
    result = parse_unix_filename(data, parser, infop, c);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_UNIX_SYMLINK:
    result = parse_unix_symlink(data, parser, infop, c);
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode parse_winnt(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା ftp_parselist_data *parser,
                            ସଂରଚନା fileinfo *infop,
                            ଅଚଳ ଅକ୍ଷର c)
{
  ସଂରଚନା curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  ଅକ୍ଷର *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  ବାଛ(parser->state.NT.ମୁଖ୍ୟ) {
  ସ୍ଥିତି PL_WINNT_DATE:
    parser->item_length++;
    ଯଦି(parser->item_length < 9) {
      ଯଦି(!strchr("0123456789-", c)) { /* only simple control */
        ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
      }
    }
    ଅନ୍ୟଥା ଯଦି(parser->item_length == 9) {
      ଯଦି(c == ' ') {
        parser->state.NT.ମୁଖ୍ୟ = PL_WINNT_TIME;
        parser->state.NT.sub.time = PL_WINNT_TIME_PRESPACE;
      }
      ଅନ୍ୟଥା
        ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
    }
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_WINNT_TIME:
    parser->item_length++;
    ବାଛ(parser->state.NT.sub.time) {
    ସ୍ଥିତି PL_WINNT_TIME_PRESPACE:
      ଯଦି(!ISBLANK(c))
        parser->state.NT.sub.time = PL_WINNT_TIME_TIME;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି PL_WINNT_TIME_TIME:
      ଯଦି(c == ' ') {
        parser->offsets.time = parser->item_offset;
        mem[parser->item_offset + parser->item_length - 1] = 0;
        parser->state.NT.ମୁଖ୍ୟ = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_length = 0;
      }
      ଅନ୍ୟଥା ଯଦି(!strchr("APM0123456789:", c))
        ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
      ଭାଙ୍ଗ;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_WINNT_DIRORSIZE:
    ବାଛ(parser->state.NT.sub.dirorsize) {
    ସ୍ଥିତି PL_WINNT_DIRORSIZE_PRESPACE:
      ଯଦି(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_length++;
      ଯଦି(c == ' ') {
        mem[parser->item_offset + parser->item_length - 1] = 0;
        ଯଦି(!strcmp("<DIR>", mem + parser->item_offset)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->size = 0;
        }
        ଅନ୍ୟଥା {
          ଅଚଳ ଅକ୍ଷର *p = mem + parser->item_offset;
          ଯଦି(curlx_str_numblanks(&p, &finfo->size)) {
            ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_data->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
        parser->item_length = 0;
        parser->state.NT.ମୁଖ୍ୟ = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ଭାଙ୍ଗ;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି PL_WINNT_FILENAME:
    ବାଛ(parser->state.NT.sub.filename) {
    ସ୍ଥିତି PL_WINNT_FILENAME_PRESPACE:
      ଯଦି(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି PL_WINNT_FILENAME_CONTENT:
      parser->item_length++;
      ଯଦି(!len)
        ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;
      ଯଦି(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      ଅନ୍ୟଥା ଯଦି(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        mem[len - 1] = 0;
        result = ftp_pl_ଭର୍ତ୍ତି_finfo(data, infop);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;

        parser->state.NT.ମୁଖ୍ୟ = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି PL_WINNT_FILENAME_WINEOL:
      ଯଦି(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        result = ftp_pl_ଭର୍ତ୍ତି_finfo(data, infop);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;

        parser->state.NT.ମୁଖ୍ୟ = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ଅନ୍ୟଥା
        ଫେରନ୍ତୁ CURLE_FTP_BAD_FILE_LIST;

      ଭାଙ୍ଗ;
    }
    ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(ସଂରଚନା Curl_easy *data)
{
  ଯଦି(data->state.wildcardmatch) {
    ସଂରଚନା WildcardData * ଅଚଳ wildcard = data->wildcard;
    ଫେରନ୍ତୁ wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  ଫେରନ୍ତୁ FALSE;
}

size_t Curl_ftp_parselist(ଅକ୍ଷର *buffer, size_t size, size_t nmemb,
                          ଶୂନ୍ୟ *connptr)
{
  size_t bufflen = size * nmemb;
  ସଂରଚନା Curl_easy *data = (ସଂରଚନା Curl_easy *)connptr;
  ସଂରଚନା ftp_wc *ftpwc = data->wildcard->ftpwc;
  ସଂରଚନା ftp_parselist_data *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode result;
  size_t retsize = bufflen;

  ଯଦି(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    ଯାଅ fail;
  }

  ଯଦି(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(buffer[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  ଯେତେବେଳେ(i < bufflen) { /* FSM */
    ଅକ୍ଷର c = buffer[i];
    ସଂରଚନା fileinfo *infop;
    ଯଦି(!parser->file_data) { /* tmp file data is not allocated yet */
      parser->file_data = Curl_fileinfo_alloc();
      ଯଦି(!parser->file_data) {
        parser->error = CURLE_OUT_OF_MEMORY;
        ଯାଅ fail;
      }
      parser->item_offset = 0;
      parser->item_length = 0;
      curlx_dyn_init(&parser->file_data->buf, MAX_FTPLIST_BUFFER);
    }

    infop = parser->file_data;

    ଯଦି(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_OUT_OF_MEMORY;
      ଯାଅ fail;
    }

    ବାଛ(parser->os_type) {
    ସ୍ଥିତି OS_TYPE_UNIX:
      result = parse_unix(data, parser, infop, c);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି OS_TYPE_WIN_NT:
      result = parse_winnt(data, parser, infop, c);
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      retsize = bufflen + 1;
      ଯାଅ fail;
    }
    ଯଦି(result) {
      parser->error = result;
      ଯାଅ fail;
    }

    i++;
  }
  ଫେରନ୍ତୁ retsize;

fail:

  /* Clean up any allocated memory. */
  ଯଦି(parser->file_data) {
    Curl_fileinfo_cleanup(parser->file_data);
    parser->file_data = ଖାଲି;
  }

  ଫେରନ୍ତୁ retsize;
}

#endif /* !CURL_DISABLE_FTP */
