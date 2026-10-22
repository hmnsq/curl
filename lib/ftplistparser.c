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

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "fileinfo.h"
#அந்தர்பாவ "llist.h"
#அந்தர்பாவ "ftp.h"
#அந்தர்பாவ "ftp-int.h"
#அந்தர்பாவ "ftplistparser.h"
#அந்தர்பாவ "curl_fnmatch.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "curlx/strparse.h"

வகைவரை எண்ணிக்கை {
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

வகைவரை ஒன்றிப்பு {
  எண்ணிக்கை {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } total_dirsize;

  எண்ணிக்கை {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_எண்
  } hlinks;

  எண்ணிக்கை {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  எண்ணிக்கை {
    PL_UNIX_GROUP_PRESPACE = 0,
    PL_UNIX_GROUP_NAME
  } group;

  எண்ணிக்கை {
    PL_UNIX_SIZE_PRESPACE = 0,
    PL_UNIX_SIZE_எண்
  } size;

  எண்ணிக்கை {
    PL_UNIX_TIME_PREPART1 = 0,
    PL_UNIX_TIME_PART1,
    PL_UNIX_TIME_PREPART2,
    PL_UNIX_TIME_PART2,
    PL_UNIX_TIME_PREPART3,
    PL_UNIX_TIME_PART3
  } time;

  எண்ணிக்கை {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  எண்ணிக்கை {
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

வகைவரை எண்ணிக்கை {
  PL_WINNT_DATE = 0,
  PL_WINNT_TIME,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

வகைவரை ஒன்றிப்பு {
  எண்ணிக்கை {
    PL_WINNT_TIME_PRESPACE = 0,
    PL_WINNT_TIME_TIME
  } time;
  எண்ணிக்கை {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  எண்ணிக்கை {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
கட்டமைப்பு ftp_parselist_data {
  எண்ணிக்கை {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  ஒன்றிப்பு {
    கட்டமைப்பு {
      pl_unix_mainstate முதன்மை;
      pl_unix_substate sub;
    } UNIX;

    கட்டமைப்பு {
      pl_winNT_mainstate முதன்மை;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  கட்டமைப்பு fileinfo *file_data;
  unsigned முழு item_length;
  size_t item_offset;
  கட்டமைப்பு {
    size_t filename;
    size_t user;
    size_t group;
    size_t time;
    size_t perm;
    size_t symlink_target;
  } offsets;
};

நிலையான வெறுமை fileinfo_dtor(வெறுமை *user, வெறுமை *element)
{
  (வெறுமை)user;
  Curl_fileinfo_cleanup(element);
}

வெறுமை Curl_wildcard_init(கட்டமைப்பு WildcardData *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

வெறுமை Curl_wildcard_dtor(கட்டமைப்பு WildcardData **wcp)
{
  கட்டமைப்பு WildcardData *wc = *wcp;
  எனில்(!wc)
    திரும்பு;

  எனில்(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = வெற்று;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, வெற்று);
  curlx_safefree(wc->path);
  curlx_safefree(wc->pattern);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = வெற்று;
}

கட்டமைப்பு ftp_parselist_data *Curl_ftp_parselist_data_alloc(வெறுமை)
{
  திரும்பு curlx_calloc(1, அளவு(கட்டமைப்பு ftp_parselist_data));
}

வெறுமை Curl_ftp_parselist_data_free(கட்டமைப்பு ftp_parselist_data **parserp)
{
  கட்டமைப்பு ftp_parselist_data *parser = *parserp;
  எனில்(parser)
    Curl_fileinfo_cleanup(parser->file_data);
  curlx_free(parser);
  *parserp = வெற்று;
}

CURLcode Curl_ftp_parselist_geterror(கட்டமைப்பு ftp_parselist_data *pl_data)
{
  திரும்பு pl_data->error;
}

#define FTP_LP_MALFORMATED_PERM 0x01000000

நிலையான unsigned முழு ftp_pl_get_permission(மாறிலி எழுத்து *str)
{
  unsigned முழு permissions = 0;
  /* USER */
  எனில்(str[0] == 'r')
    permissions |= 1 << 8;
  இல்லையெனில் எனில்(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  எனில்(str[1] == 'w')
    permissions |= 1 << 7;
  இல்லையெனில் எனில்(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  எனில்(str[2] == 'x')
    permissions |= 1 << 6;
  இல்லையெனில் எனில்(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  இல்லையெனில் எனில்(str[2] == 'S')
    permissions |= 1 << 11;
  இல்லையெனில் எனில்(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* GROUP */
  எனில்(str[3] == 'r')
    permissions |= 1 << 5;
  இல்லையெனில் எனில்(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  எனில்(str[4] == 'w')
    permissions |= 1 << 4;
  இல்லையெனில் எனில்(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  எனில்(str[5] == 'x')
    permissions |= 1 << 3;
  இல்லையெனில் எனில்(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  இல்லையெனில் எனில்(str[5] == 'S')
    permissions |= 1 << 10;
  இல்லையெனில் எனில்(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  எனில்(str[6] == 'r')
    permissions |= 1 << 2;
  இல்லையெனில் எனில்(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  எனில்(str[7] == 'w')
    permissions |= 1 << 1;
  இல்லையெனில் எனில்(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  எனில்(str[8] == 'x')
    permissions |= 1;
  இல்லையெனில் எனில்(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  இல்லையெனில் எனில்(str[8] == 'T')
    permissions |= 1 << 9;
  இல்லையெனில் எனில்(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  திரும்பு permissions;
}

நிலையான CURLcode ftp_pl_செருகு_finfo(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு fileinfo *infop)
{
  curl_fnmatch_callback compare;
  கட்டமைப்பு WildcardData *wc = data->wildcard;
  கட்டமைப்பு ftp_wc *ftpwc = wc->ftpwc;
  கட்டமைப்பு Curl_llist *llist = &wc->filelist;
  கட்டமைப்பு ftp_parselist_data *parser = ftpwc->parser;
  bool add = TRUE;
  கட்டமைப்பு curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  எழுத்து *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->strings.group  = parser->offsets.group ?
                          str + parser->offsets.group : வெற்று;
  finfo->strings.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : வெற்று;
  finfo->strings.target = parser->offsets.symlink_target &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_target : வெற்று;
  finfo->strings.time   = str + parser->offsets.time;
  finfo->strings.user   = parser->offsets.user ?
                          str + parser->offsets.user : வெற்று;

  /* get correct fnmatch callback */
  compare = data->set.fnmatch;
  எனில்(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  எனில்(strchr(finfo->filename, '/'))
    add = FALSE;
  இல்லையெனில் {
    /* a real file name cannot contain a slash */
    கட்டமைப்பு Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fnmatch_data);
    எனில்(compare(data->set.fnmatch_data, wc->pattern, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      எனில்((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->strings.target &&
         strstr(finfo->strings.target, " -> ")) {
        add = FALSE;
      }
    }
    இல்லையெனில் {
      add = FALSE;
    }
    CURL_CBAPI_END(&guard);
  }

  எனில்(add) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  இல்லையெனில் {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_data = வெற்று;
  திரும்பு CURLE_OK;
}

#define MAX_FTPLIST_BUFFER 10000 /* arbitrarily set */

நிலையான CURLcode unix_filetype(மாறிலி எழுத்து c, curlfiletype *t)
{
  தேர்வு(c) {
  நிலை '-':
    *t = CURLFILETYPE_FILE;
    நிறுத்து;
  நிலை 'd':
    *t = CURLFILETYPE_DIRECTORY;
    நிறுத்து;
  நிலை 'l':
    *t = CURLFILETYPE_SYMLINK;
    நிறுத்து;
  நிலை 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    நிறுத்து;
  நிலை 's':
    *t = CURLFILETYPE_SOCKET;
    நிறுத்து;
  நிலை 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    நிறுத்து;
  நிலை 'b':
    *t = CURLFILETYPE_DEVICE_BLOCK;
    நிறுத்து;
  நிலை 'D':
    *t = CURLFILETYPE_DOOR;
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLE_FTP_BAD_FILE_LIST;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode parse_unix_totalsize(கட்டமைப்பு ftp_parselist_data *parser,
                                     கட்டமைப்பு fileinfo *infop,
                                     மாறிலி எழுத்து c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  எழுத்து *mem = curlx_dyn_ptr(&infop->buf);
  தேர்வு(parser->state.UNIX.sub.total_dirsize) {
  நிலை PL_UNIX_TOTALSIZE_INIT:
    எனில்(c == 't') {
      parser->state.UNIX.sub.total_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_length++;
    }
    இல்லையெனில் {
      parser->state.UNIX.முதன்மை = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    நிறுத்து;
  நிலை PL_UNIX_TOTALSIZE_READING:
    parser->item_length++;
    எனில்(c == '\r') {
      parser->item_length--;
      எனில்(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    இல்லையெனில் எனில்(c == '\n') {
      mem[parser->item_length - 1] = 0;
      எனில்(!strncmp("total ", mem, 6)) {
        மாறிலி எழுத்து *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        போது(ISDIGIT(*endptr))
          endptr++;
        எனில்(*endptr) {
          திரும்பு CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.முதன்மை = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      இல்லையெனில்
        திரும்பு CURLE_FTP_BAD_FILE_LIST;
    }
    நிறுத்து;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode parse_unix_permission(கட்டமைப்பு ftp_parselist_data *parser,
                                      கட்டமைப்பு fileinfo *infop,
                                      மாறிலி எழுத்து c)
{
  எழுத்து *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_length++;
  எனில்((parser->item_length <= 9) && !strchr("rwx-tTsS", c))
    திரும்பு CURLE_FTP_BAD_FILE_LIST;

  இல்லையெனில் எனில்(parser->item_length == 10) {
    unsigned முழு perm;
    எனில்(c != ' ')
      திரும்பு CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_offset);
    எனில்(perm & FTP_LP_MALFORMATED_PERM)
      திரும்பு CURLE_FTP_BAD_FILE_LIST;

    parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_PERM;
    parser->file_data->info.perm = perm;
    parser->offsets.perm = parser->item_offset;

    parser->item_length = 0;
    parser->state.UNIX.முதன்மை = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode parse_unix_hlinks(கட்டமைப்பு ftp_parselist_data *parser,
                                  கட்டமைப்பு fileinfo *infop,
                                  மாறிலி எழுத்து c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  எழுத்து *mem = curlx_dyn_ptr(&infop->buf);

  தேர்வு(parser->state.UNIX.sub.hlinks) {
  நிலை PL_UNIX_HLINKS_PRESPACE:
    எனில்(c != ' ') {
      எனில்(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_எண்;
      }
      இல்லையெனில்
        திரும்பு CURLE_FTP_BAD_FILE_LIST;
    }
    நிறுத்து;
  நிலை PL_UNIX_HLINKS_எண்:
    parser->item_length++;
    எனில்(c == ' ') {
      மாறிலி எழுத்து *p = &mem[parser->item_offset];
      curl_off_t hlinks;
      mem[parser->item_offset + parser->item_length - 1] = 0;

      எனில்(!curlx_str_எண்(&p, &hlinks, LONG_MAX)) {
        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_HLINKCOUNT;
        parser->file_data->info.hardlinks = (long)hlinks;
      }
      parser->item_length = 0;
      parser->item_offset = 0;
      parser->state.UNIX.முதன்மை = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    இல்லையெனில் எனில்(!ISDIGIT(c))
      திரும்பு CURLE_FTP_BAD_FILE_LIST;

    நிறுத்து;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode parse_unix_user(கட்டமைப்பு ftp_parselist_data *parser,
                                கட்டமைப்பு fileinfo *infop,
                                மாறிலி எழுத்து c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  எழுத்து *mem = curlx_dyn_ptr(&infop->buf);
  தேர்வு(parser->state.UNIX.sub.user) {
  நிலை PL_UNIX_USER_PRESPACE:
    எனில்(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    நிறுத்து;
  நிலை PL_UNIX_USER_PARSING:
    parser->item_length++;
    எனில்(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.user = parser->item_offset;
      parser->state.UNIX.முதன்மை = PL_UNIX_GROUP;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    நிறுத்து;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode parse_unix_group(கட்டமைப்பு ftp_parselist_data *parser,
                                 கட்டமைப்பு fileinfo *infop,
                                 மாறிலி எழுத்து c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  எழுத்து *mem = curlx_dyn_ptr(&infop->buf);
  தேர்வு(parser->state.UNIX.sub.group) {
  நிலை PL_UNIX_GROUP_PRESPACE:
    எனில்(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_NAME;
    }
    நிறுத்து;
  நிலை PL_UNIX_GROUP_NAME:
    parser->item_length++;
    எனில்(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.group = parser->item_offset;
      parser->state.UNIX.முதன்மை = PL_UNIX_SIZE;
      parser->state.UNIX.sub.size = PL_UNIX_SIZE_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    நிறுத்து;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode parse_unix_size(கட்டமைப்பு ftp_parselist_data *parser,
                                கட்டமைப்பு fileinfo *infop,
                                மாறிலி எழுத்து c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  எழுத்து *mem = curlx_dyn_ptr(&infop->buf);
  தேர்வு(parser->state.UNIX.sub.size) {
  நிலை PL_UNIX_SIZE_PRESPACE:
    எனில்(c != ' ') {
      எனில்(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.size = PL_UNIX_SIZE_எண்;
      }
      இல்லையெனில்
        திரும்பு CURLE_FTP_BAD_FILE_LIST;
    }
    நிறுத்து;
  நிலை PL_UNIX_SIZE_எண்:
    parser->item_length++;
    எனில்(c == ' ') {
      மாறிலி எழுத்து *p = mem + parser->item_offset;
      curl_off_t fsize;
      mem[parser->item_offset + parser->item_length - 1] = 0;
      எனில்(!curlx_str_numblanks(&p, &fsize)) {
        எனில்(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
          parser->file_data->info.size = fsize;
        }
        parser->item_length = 0;
        parser->item_offset = 0;
        parser->state.UNIX.முதன்மை = PL_UNIX_TIME;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART1;
      }
    }
    இல்லையெனில் எனில்(!ISDIGIT(c))
      திரும்பு CURLE_FTP_BAD_FILE_LIST;

    நிறுத்து;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode parse_unix_time(கட்டமைப்பு ftp_parselist_data *parser,
                                கட்டமைப்பு fileinfo *infop,
                                மாறிலி எழுத்து c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  எழுத்து *mem = curlx_dyn_ptr(&infop->buf);
  கட்டமைப்பு curl_fileinfo *finfo = &infop->info;

  தேர்வு(parser->state.UNIX.sub.time) {
  நிலை PL_UNIX_TIME_PREPART1:
    எனில்(c != ' ') {
      எனில்(ISALNUM(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART1;
      }
      இல்லையெனில்
        திரும்பு CURLE_FTP_BAD_FILE_LIST;
    }
    நிறுத்து;
  நிலை PL_UNIX_TIME_PART1:
    parser->item_length++;
    எனில்(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART2;

    இல்லையெனில் எனில்(!ISALNUM(c) && c != '.')
      திரும்பு CURLE_FTP_BAD_FILE_LIST;

    நிறுத்து;
  நிலை PL_UNIX_TIME_PREPART2:
    parser->item_length++;
    எனில்(c != ' ') {
      எனில்(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART2;
      இல்லையெனில்
        திரும்பு CURLE_FTP_BAD_FILE_LIST;
    }
    நிறுத்து;
  நிலை PL_UNIX_TIME_PART2:
    parser->item_length++;
    எனில்(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART3;
    இல்லையெனில் எனில்(!ISALNUM(c) && c != '.')
      திரும்பு CURLE_FTP_BAD_FILE_LIST;
    நிறுத்து;
  நிலை PL_UNIX_TIME_PREPART3:
    parser->item_length++;
    எனில்(c != ' ') {
      எனில்(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART3;
      இல்லையெனில்
        திரும்பு CURLE_FTP_BAD_FILE_LIST;
    }
    நிறுத்து;
  நிலை PL_UNIX_TIME_PART3:
    parser->item_length++;
    எனில்(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.time = parser->item_offset;
      எனில்(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.முதன்மை = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      இல்லையெனில் {
        parser->state.UNIX.முதன்மை = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    இல்லையெனில் எனில்(!ISALNUM(c) && c != '.' && c != ':')
      திரும்பு CURLE_FTP_BAD_FILE_LIST;
    நிறுத்து;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode parse_unix_filename(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு ftp_parselist_data *parser,
                                    கட்டமைப்பு fileinfo *infop,
                                    மாறிலி எழுத்து c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  எழுத்து *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  தேர்வு(parser->state.UNIX.sub.filename) {
  நிலை PL_UNIX_FILENAME_PRESPACE:
    எனில்(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    நிறுத்து;
  நிலை PL_UNIX_FILENAME_NAME:
    parser->item_length++;
    எனில்(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    இல்லையெனில் எனில்(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.முதன்மை = PL_UNIX_FILETYPE;
      result = ftp_pl_செருகு_finfo(data, infop);
    }
    நிறுத்து;
  நிலை PL_UNIX_FILENAME_WINDOWSEOL:
    எனில்(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.முதன்மை = PL_UNIX_FILETYPE;
      result = ftp_pl_செருகு_finfo(data, infop);
    }
    இல்லையெனில்
      result = CURLE_FTP_BAD_FILE_LIST;
    நிறுத்து;
  }
  திரும்பு result;
}

நிலையான CURLcode parse_unix_symlink(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு ftp_parselist_data *parser,
                                   கட்டமைப்பு fileinfo *infop,
                                   மாறிலி எழுத்து c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  எழுத்து *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  தேர்வு(parser->state.UNIX.sub.symlink) {
  நிலை PL_UNIX_SYMLINK_PRESPACE:
    எனில்(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    நிறுத்து;
  நிலை PL_UNIX_SYMLINK_NAME:
    parser->item_length++;
    எனில்(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    இல்லையெனில் எனில்(c == '\r' || c == '\n')
      திரும்பு CURLE_FTP_BAD_FILE_LIST;

    நிறுத்து;
  நிலை PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_length++;
    எனில்(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    இல்லையெனில் எனில்(c == '\r' || c == '\n')
      திரும்பு CURLE_FTP_BAD_FILE_LIST;
    இல்லையெனில்
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    நிறுத்து;
  நிலை PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_length++;
    எனில்(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    இல்லையெனில் எனில்(c == '\r' || c == '\n')
      திரும்பு CURLE_FTP_BAD_FILE_LIST;
    இல்லையெனில்
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    நிறுத்து;
  நிலை PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_length++;
    எனில்(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_offset + parser->item_length - 4] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->item_length = 0;
      parser->item_offset = 0;
    }
    இல்லையெனில் எனில்(c == '\r' || c == '\n')
      திரும்பு CURLE_FTP_BAD_FILE_LIST;
    இல்லையெனில்
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    நிறுத்து;
  நிலை PL_UNIX_SYMLINK_PRETARGET4:
    எனில்(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_offset = len - 1;
      parser->item_length = 1;
    }
    இல்லையெனில்
      திரும்பு CURLE_FTP_BAD_FILE_LIST;

    நிறுத்து;
  நிலை PL_UNIX_SYMLINK_TARGET:
    parser->item_length++;
    எனில்(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    இல்லையெனில் எனில்(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_செருகு_finfo(data, infop);
      எனில்(result)
        நிறுத்து;

      parser->state.UNIX.முதன்மை = PL_UNIX_FILETYPE;
    }
    நிறுத்து;
  நிலை PL_UNIX_SYMLINK_WINDOWSEOL:
    எனில்(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_செருகு_finfo(data, infop);
      எனில்(result)
        நிறுத்து;

      parser->state.UNIX.முதன்மை = PL_UNIX_FILETYPE;
    }
    இல்லையெனில்
      result = CURLE_FTP_BAD_FILE_LIST;

    நிறுத்து;
  }
  திரும்பு result;
}

நிலையான CURLcode parse_unix(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு ftp_parselist_data *parser,
                           கட்டமைப்பு fileinfo *infop,
                           மாறிலி எழுத்து c)
{
  கட்டமைப்பு curl_fileinfo *finfo = &infop->info;
  CURLcode result = CURLE_OK;

  தேர்வு(parser->state.UNIX.முதன்மை) {
  நிலை PL_UNIX_TOTALSIZE:
    result = parse_unix_totalsize(parser, infop, c);
    எனில்(result)
      நிறுத்து;
    எனில்(parser->state.UNIX.முதன்மை != PL_UNIX_FILETYPE)
      நிறுத்து;
    FALLTHROUGH();
  நிலை PL_UNIX_FILETYPE:
    result = unix_filetype(c, &finfo->filetype);
    எனில்(!result) {
      parser->state.UNIX.முதன்மை = PL_UNIX_PERMISSION;
      parser->item_length = 0;
      parser->item_offset = 1;
    }
    நிறுத்து;
  நிலை PL_UNIX_PERMISSION:
    result = parse_unix_permission(parser, infop, c);
    நிறுத்து;
  நிலை PL_UNIX_HLINKS:
    result = parse_unix_hlinks(parser, infop, c);
    நிறுத்து;
  நிலை PL_UNIX_USER:
    result = parse_unix_user(parser, infop, c);
    நிறுத்து;
  நிலை PL_UNIX_GROUP:
    result = parse_unix_group(parser, infop, c);
    நிறுத்து;
  நிலை PL_UNIX_SIZE:
    result = parse_unix_size(parser, infop, c);
    நிறுத்து;
  நிலை PL_UNIX_TIME:
    result = parse_unix_time(parser, infop, c);
    நிறுத்து;
  நிலை PL_UNIX_FILENAME:
    result = parse_unix_filename(data, parser, infop, c);
    நிறுத்து;
  நிலை PL_UNIX_SYMLINK:
    result = parse_unix_symlink(data, parser, infop, c);
    நிறுத்து;
  }
  திரும்பு result;
}

நிலையான CURLcode parse_winnt(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு ftp_parselist_data *parser,
                            கட்டமைப்பு fileinfo *infop,
                            மாறிலி எழுத்து c)
{
  கட்டமைப்பு curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  எழுத்து *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  தேர்வு(parser->state.NT.முதன்மை) {
  நிலை PL_WINNT_DATE:
    parser->item_length++;
    எனில்(parser->item_length < 9) {
      எனில்(!strchr("0123456789-", c)) { /* only simple control */
        திரும்பு CURLE_FTP_BAD_FILE_LIST;
      }
    }
    இல்லையெனில் எனில்(parser->item_length == 9) {
      எனில்(c == ' ') {
        parser->state.NT.முதன்மை = PL_WINNT_TIME;
        parser->state.NT.sub.time = PL_WINNT_TIME_PRESPACE;
      }
      இல்லையெனில்
        திரும்பு CURLE_FTP_BAD_FILE_LIST;
    }
    இல்லையெனில்
      திரும்பு CURLE_FTP_BAD_FILE_LIST;
    நிறுத்து;
  நிலை PL_WINNT_TIME:
    parser->item_length++;
    தேர்வு(parser->state.NT.sub.time) {
    நிலை PL_WINNT_TIME_PRESPACE:
      எனில்(!ISBLANK(c))
        parser->state.NT.sub.time = PL_WINNT_TIME_TIME;
      நிறுத்து;
    நிலை PL_WINNT_TIME_TIME:
      எனில்(c == ' ') {
        parser->offsets.time = parser->item_offset;
        mem[parser->item_offset + parser->item_length - 1] = 0;
        parser->state.NT.முதன்மை = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_length = 0;
      }
      இல்லையெனில் எனில்(!strchr("APM0123456789:", c))
        திரும்பு CURLE_FTP_BAD_FILE_LIST;
      நிறுத்து;
    }
    நிறுத்து;
  நிலை PL_WINNT_DIRORSIZE:
    தேர்வு(parser->state.NT.sub.dirorsize) {
    நிலை PL_WINNT_DIRORSIZE_PRESPACE:
      எனில்(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      நிறுத்து;
    நிலை PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_length++;
      எனில்(c == ' ') {
        mem[parser->item_offset + parser->item_length - 1] = 0;
        எனில்(!strcmp("<DIR>", mem + parser->item_offset)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->size = 0;
        }
        இல்லையெனில் {
          மாறிலி எழுத்து *p = mem + parser->item_offset;
          எனில்(curlx_str_numblanks(&p, &finfo->size)) {
            திரும்பு CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_data->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
        parser->item_length = 0;
        parser->state.NT.முதன்மை = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      நிறுத்து;
    }
    நிறுத்து;
  நிலை PL_WINNT_FILENAME:
    தேர்வு(parser->state.NT.sub.filename) {
    நிலை PL_WINNT_FILENAME_PRESPACE:
      எனில்(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      நிறுத்து;
    நிலை PL_WINNT_FILENAME_CONTENT:
      parser->item_length++;
      எனில்(!len)
        திரும்பு CURLE_FTP_BAD_FILE_LIST;
      எனில்(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      இல்லையெனில் எனில்(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        mem[len - 1] = 0;
        result = ftp_pl_செருகு_finfo(data, infop);
        எனில்(result)
          திரும்பு result;

        parser->state.NT.முதன்மை = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      நிறுத்து;
    நிலை PL_WINNT_FILENAME_WINEOL:
      எனில்(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        result = ftp_pl_செருகு_finfo(data, infop);
        எனில்(result)
          திரும்பு result;

        parser->state.NT.முதன்மை = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      இல்லையெனில்
        திரும்பு CURLE_FTP_BAD_FILE_LIST;

      நிறுத்து;
    }
    நிறுத்து;
  }

  திரும்பு CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(கட்டமைப்பு Curl_easy *data)
{
  எனில்(data->state.wildcardmatch) {
    கட்டமைப்பு WildcardData * மாறிலி wildcard = data->wildcard;
    திரும்பு wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  திரும்பு FALSE;
}

size_t Curl_ftp_parselist(எழுத்து *buffer, size_t size, size_t nmemb,
                          வெறுமை *connptr)
{
  size_t bufflen = size * nmemb;
  கட்டமைப்பு Curl_easy *data = (கட்டமைப்பு Curl_easy *)connptr;
  கட்டமைப்பு ftp_wc *ftpwc = data->wildcard->ftpwc;
  கட்டமைப்பு ftp_parselist_data *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode result;
  size_t retsize = bufflen;

  எனில்(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    செல் fail;
  }

  எனில்(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(buffer[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  போது(i < bufflen) { /* FSM */
    எழுத்து c = buffer[i];
    கட்டமைப்பு fileinfo *infop;
    எனில்(!parser->file_data) { /* tmp file data is not allocated yet */
      parser->file_data = Curl_fileinfo_alloc();
      எனில்(!parser->file_data) {
        parser->error = CURLE_OUT_OF_MEMORY;
        செல் fail;
      }
      parser->item_offset = 0;
      parser->item_length = 0;
      curlx_dyn_init(&parser->file_data->buf, MAX_FTPLIST_BUFFER);
    }

    infop = parser->file_data;

    எனில்(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_OUT_OF_MEMORY;
      செல் fail;
    }

    தேர்வு(parser->os_type) {
    நிலை OS_TYPE_UNIX:
      result = parse_unix(data, parser, infop, c);
      நிறுத்து;
    நிலை OS_TYPE_WIN_NT:
      result = parse_winnt(data, parser, infop, c);
      நிறுத்து;
    இயல்பு:
      retsize = bufflen + 1;
      செல் fail;
    }
    எனில்(result) {
      parser->error = result;
      செல் fail;
    }

    i++;
  }
  திரும்பு retsize;

fail:

  /* Clean up any allocated memory. */
  எனில்(parser->file_data) {
    Curl_fileinfo_cleanup(parser->file_data);
    parser->file_data = வெற்று;
  }

  திரும்பு retsize;
}

#endif /* !CURL_DISABLE_FTP */
