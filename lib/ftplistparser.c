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
#અન્તર્ભાવ "curl_setup.h"

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

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "fileinfo.h"
#અન્તર્ભાવ "llist.h"
#અન્તર્ભાવ "ftp.h"
#અન્તર્ભાવ "ftp-int.h"
#અન્તર્ભાવ "ftplistparser.h"
#અન્તર્ભાવ "curl_fnmatch.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "curlx/strparse.h"

પ્રકારવ્યાખ્યા ગણના {
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

પ્રકારવ્યાખ્યા સંઘ {
  ગણના {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } total_dirsize;

  ગણના {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_સંખ્યા
  } hlinks;

  ગણના {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  ગણના {
    PL_UNIX_GROUP_PRESPACE = 0,
    PL_UNIX_GROUP_NAME
  } group;

  ગણના {
    PL_UNIX_SIZE_PRESPACE = 0,
    PL_UNIX_SIZE_સંખ્યા
  } size;

  ગણના {
    PL_UNIX_TIME_PREPART1 = 0,
    PL_UNIX_TIME_PART1,
    PL_UNIX_TIME_PREPART2,
    PL_UNIX_TIME_PART2,
    PL_UNIX_TIME_PREPART3,
    PL_UNIX_TIME_PART3
  } time;

  ગણના {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  ગણના {
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

પ્રકારવ્યાખ્યા ગણના {
  PL_WINNT_DATE = 0,
  PL_WINNT_TIME,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

પ્રકારવ્યાખ્યા સંઘ {
  ગણના {
    PL_WINNT_TIME_PRESPACE = 0,
    PL_WINNT_TIME_TIME
  } time;
  ગણના {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  ગણના {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
સંરચના ftp_parselist_data {
  ગણના {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  સંઘ {
    સંરચના {
      pl_unix_mainstate મુખ્ય;
      pl_unix_substate sub;
    } UNIX;

    સંરચના {
      pl_winNT_mainstate મુખ્ય;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  સંરચના fileinfo *file_data;
  unsigned પૂર્ણાંક item_length;
  size_t item_offset;
  સંરચના {
    size_t filename;
    size_t user;
    size_t group;
    size_t time;
    size_t perm;
    size_t symlink_target;
  } offsets;
};

સ્થિર શૂન્ય fileinfo_dtor(શૂન્ય *user, શૂન્ય *element)
{
  (શૂન્ય)user;
  Curl_fileinfo_cleanup(element);
}

શૂન્ય Curl_wildcard_init(સંરચના WildcardData *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

શૂન્ય Curl_wildcard_dtor(સંરચના WildcardData **wcp)
{
  સંરચના WildcardData *wc = *wcp;
  જો(!wc)
    પાછા;

  જો(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = ખાલી;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, ખાલી);
  curlx_safefree(wc->path);
  curlx_safefree(wc->pattern);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = ખાલી;
}

સંરચના ftp_parselist_data *Curl_ftp_parselist_data_alloc(શૂન્ય)
{
  પાછા curlx_calloc(1, કદ(સંરચના ftp_parselist_data));
}

શૂન્ય Curl_ftp_parselist_data_free(સંરચના ftp_parselist_data **parserp)
{
  સંરચના ftp_parselist_data *parser = *parserp;
  જો(parser)
    Curl_fileinfo_cleanup(parser->file_data);
  curlx_free(parser);
  *parserp = ખાલી;
}

CURLcode Curl_ftp_parselist_geterror(સંરચના ftp_parselist_data *pl_data)
{
  પાછા pl_data->error;
}

#define FTP_LP_MALFORMATED_PERM 0x01000000

સ્થિર unsigned પૂર્ણાંક ftp_pl_get_permission(અચલ અક્ષર *str)
{
  unsigned પૂર્ણાંક permissions = 0;
  /* USER */
  જો(str[0] == 'r')
    permissions |= 1 << 8;
  નહીંતર જો(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  જો(str[1] == 'w')
    permissions |= 1 << 7;
  નહીંતર જો(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  જો(str[2] == 'x')
    permissions |= 1 << 6;
  નહીંતર જો(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  નહીંતર જો(str[2] == 'S')
    permissions |= 1 << 11;
  નહીંતર જો(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* GROUP */
  જો(str[3] == 'r')
    permissions |= 1 << 5;
  નહીંતર જો(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  જો(str[4] == 'w')
    permissions |= 1 << 4;
  નહીંતર જો(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  જો(str[5] == 'x')
    permissions |= 1 << 3;
  નહીંતર જો(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  નહીંતર જો(str[5] == 'S')
    permissions |= 1 << 10;
  નહીંતર જો(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  જો(str[6] == 'r')
    permissions |= 1 << 2;
  નહીંતર જો(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  જો(str[7] == 'w')
    permissions |= 1 << 1;
  નહીંતર જો(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  જો(str[8] == 'x')
    permissions |= 1;
  નહીંતર જો(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  નહીંતર જો(str[8] == 'T')
    permissions |= 1 << 9;
  નહીંતર જો(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  પાછા permissions;
}

સ્થિર CURLcode ftp_pl_ઉમેરો_finfo(સંરચના Curl_easy *data,
                                    સંરચના fileinfo *infop)
{
  curl_fnmatch_callback compare;
  સંરચના WildcardData *wc = data->wildcard;
  સંરચના ftp_wc *ftpwc = wc->ftpwc;
  સંરચના Curl_llist *llist = &wc->filelist;
  સંરચના ftp_parselist_data *parser = ftpwc->parser;
  bool add = TRUE;
  સંરચના curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  અક્ષર *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->strings.group  = parser->offsets.group ?
                          str + parser->offsets.group : ખાલી;
  finfo->strings.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : ખાલી;
  finfo->strings.target = parser->offsets.symlink_target &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_target : ખાલી;
  finfo->strings.time   = str + parser->offsets.time;
  finfo->strings.user   = parser->offsets.user ?
                          str + parser->offsets.user : ખાલી;

  /* get correct fnmatch callback */
  compare = data->set.fnmatch;
  જો(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  જો(strchr(finfo->filename, '/'))
    add = FALSE;
  નહીંતર {
    /* a real file name cannot contain a slash */
    સંરચના Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fnmatch_data);
    જો(compare(data->set.fnmatch_data, wc->pattern, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      જો((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->strings.target &&
         strstr(finfo->strings.target, " -> ")) {
        add = FALSE;
      }
    }
    નહીંતર {
      add = FALSE;
    }
    CURL_CBAPI_END(&guard);
  }

  જો(add) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  નહીંતર {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_data = ખાલી;
  પાછા CURLE_OK;
}

#define MAX_FTPLIST_BUFFER 10000 /* arbitrarily set */

સ્થિર CURLcode unix_filetype(અચલ અક્ષર c, curlfiletype *t)
{
  પસંદગી(c) {
  સ્થિતિ '-':
    *t = CURLFILETYPE_FILE;
    તોડો;
  સ્થિતિ 'd':
    *t = CURLFILETYPE_DIRECTORY;
    તોડો;
  સ્થિતિ 'l':
    *t = CURLFILETYPE_SYMLINK;
    તોડો;
  સ્થિતિ 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    તોડો;
  સ્થિતિ 's':
    *t = CURLFILETYPE_SOCKET;
    તોડો;
  સ્થિતિ 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    તોડો;
  સ્થિતિ 'b':
    *t = CURLFILETYPE_DEVICE_BLOCK;
    તોડો;
  સ્થિતિ 'D':
    *t = CURLFILETYPE_DOOR;
    તોડો;
  મૂળભૂત:
    પાછા CURLE_FTP_BAD_FILE_LIST;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode parse_unix_totalsize(સંરચના ftp_parselist_data *parser,
                                     સંરચના fileinfo *infop,
                                     અચલ અક્ષર c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  અક્ષર *mem = curlx_dyn_ptr(&infop->buf);
  પસંદગી(parser->state.UNIX.sub.total_dirsize) {
  સ્થિતિ PL_UNIX_TOTALSIZE_INIT:
    જો(c == 't') {
      parser->state.UNIX.sub.total_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_length++;
    }
    નહીંતર {
      parser->state.UNIX.મુખ્ય = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    તોડો;
  સ્થિતિ PL_UNIX_TOTALSIZE_READING:
    parser->item_length++;
    જો(c == '\r') {
      parser->item_length--;
      જો(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    નહીંતર જો(c == '\n') {
      mem[parser->item_length - 1] = 0;
      જો(!strncmp("total ", mem, 6)) {
        અચલ અક્ષર *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        જ્યારે(ISDIGIT(*endptr))
          endptr++;
        જો(*endptr) {
          પાછા CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.મુખ્ય = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      નહીંતર
        પાછા CURLE_FTP_BAD_FILE_LIST;
    }
    તોડો;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode parse_unix_permission(સંરચના ftp_parselist_data *parser,
                                      સંરચના fileinfo *infop,
                                      અચલ અક્ષર c)
{
  અક્ષર *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_length++;
  જો((parser->item_length <= 9) && !strchr("rwx-tTsS", c))
    પાછા CURLE_FTP_BAD_FILE_LIST;

  નહીંતર જો(parser->item_length == 10) {
    unsigned પૂર્ણાંક perm;
    જો(c != ' ')
      પાછા CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_offset);
    જો(perm & FTP_LP_MALFORMATED_PERM)
      પાછા CURLE_FTP_BAD_FILE_LIST;

    parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_PERM;
    parser->file_data->info.perm = perm;
    parser->offsets.perm = parser->item_offset;

    parser->item_length = 0;
    parser->state.UNIX.મુખ્ય = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode parse_unix_hlinks(સંરચના ftp_parselist_data *parser,
                                  સંરચના fileinfo *infop,
                                  અચલ અક્ષર c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  અક્ષર *mem = curlx_dyn_ptr(&infop->buf);

  પસંદગી(parser->state.UNIX.sub.hlinks) {
  સ્થિતિ PL_UNIX_HLINKS_PRESPACE:
    જો(c != ' ') {
      જો(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_સંખ્યા;
      }
      નહીંતર
        પાછા CURLE_FTP_BAD_FILE_LIST;
    }
    તોડો;
  સ્થિતિ PL_UNIX_HLINKS_સંખ્યા:
    parser->item_length++;
    જો(c == ' ') {
      અચલ અક્ષર *p = &mem[parser->item_offset];
      curl_off_t hlinks;
      mem[parser->item_offset + parser->item_length - 1] = 0;

      જો(!curlx_str_સંખ્યા(&p, &hlinks, LONG_MAX)) {
        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_HLINKCOUNT;
        parser->file_data->info.hardlinks = (long)hlinks;
      }
      parser->item_length = 0;
      parser->item_offset = 0;
      parser->state.UNIX.મુખ્ય = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    નહીંતર જો(!ISDIGIT(c))
      પાછા CURLE_FTP_BAD_FILE_LIST;

    તોડો;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode parse_unix_user(સંરચના ftp_parselist_data *parser,
                                સંરચના fileinfo *infop,
                                અચલ અક્ષર c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  અક્ષર *mem = curlx_dyn_ptr(&infop->buf);
  પસંદગી(parser->state.UNIX.sub.user) {
  સ્થિતિ PL_UNIX_USER_PRESPACE:
    જો(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    તોડો;
  સ્થિતિ PL_UNIX_USER_PARSING:
    parser->item_length++;
    જો(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.user = parser->item_offset;
      parser->state.UNIX.મુખ્ય = PL_UNIX_GROUP;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    તોડો;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode parse_unix_group(સંરચના ftp_parselist_data *parser,
                                 સંરચના fileinfo *infop,
                                 અચલ અક્ષર c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  અક્ષર *mem = curlx_dyn_ptr(&infop->buf);
  પસંદગી(parser->state.UNIX.sub.group) {
  સ્થિતિ PL_UNIX_GROUP_PRESPACE:
    જો(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_NAME;
    }
    તોડો;
  સ્થિતિ PL_UNIX_GROUP_NAME:
    parser->item_length++;
    જો(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.group = parser->item_offset;
      parser->state.UNIX.મુખ્ય = PL_UNIX_SIZE;
      parser->state.UNIX.sub.size = PL_UNIX_SIZE_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    તોડો;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode parse_unix_size(સંરચના ftp_parselist_data *parser,
                                સંરચના fileinfo *infop,
                                અચલ અક્ષર c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  અક્ષર *mem = curlx_dyn_ptr(&infop->buf);
  પસંદગી(parser->state.UNIX.sub.size) {
  સ્થિતિ PL_UNIX_SIZE_PRESPACE:
    જો(c != ' ') {
      જો(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.size = PL_UNIX_SIZE_સંખ્યા;
      }
      નહીંતર
        પાછા CURLE_FTP_BAD_FILE_LIST;
    }
    તોડો;
  સ્થિતિ PL_UNIX_SIZE_સંખ્યા:
    parser->item_length++;
    જો(c == ' ') {
      અચલ અક્ષર *p = mem + parser->item_offset;
      curl_off_t fsize;
      mem[parser->item_offset + parser->item_length - 1] = 0;
      જો(!curlx_str_numblanks(&p, &fsize)) {
        જો(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
          parser->file_data->info.size = fsize;
        }
        parser->item_length = 0;
        parser->item_offset = 0;
        parser->state.UNIX.મુખ્ય = PL_UNIX_TIME;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART1;
      }
    }
    નહીંતર જો(!ISDIGIT(c))
      પાછા CURLE_FTP_BAD_FILE_LIST;

    તોડો;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode parse_unix_time(સંરચના ftp_parselist_data *parser,
                                સંરચના fileinfo *infop,
                                અચલ અક્ષર c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  અક્ષર *mem = curlx_dyn_ptr(&infop->buf);
  સંરચના curl_fileinfo *finfo = &infop->info;

  પસંદગી(parser->state.UNIX.sub.time) {
  સ્થિતિ PL_UNIX_TIME_PREPART1:
    જો(c != ' ') {
      જો(ISALNUM(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART1;
      }
      નહીંતર
        પાછા CURLE_FTP_BAD_FILE_LIST;
    }
    તોડો;
  સ્થિતિ PL_UNIX_TIME_PART1:
    parser->item_length++;
    જો(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART2;

    નહીંતર જો(!ISALNUM(c) && c != '.')
      પાછા CURLE_FTP_BAD_FILE_LIST;

    તોડો;
  સ્થિતિ PL_UNIX_TIME_PREPART2:
    parser->item_length++;
    જો(c != ' ') {
      જો(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART2;
      નહીંતર
        પાછા CURLE_FTP_BAD_FILE_LIST;
    }
    તોડો;
  સ્થિતિ PL_UNIX_TIME_PART2:
    parser->item_length++;
    જો(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART3;
    નહીંતર જો(!ISALNUM(c) && c != '.')
      પાછા CURLE_FTP_BAD_FILE_LIST;
    તોડો;
  સ્થિતિ PL_UNIX_TIME_PREPART3:
    parser->item_length++;
    જો(c != ' ') {
      જો(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART3;
      નહીંતર
        પાછા CURLE_FTP_BAD_FILE_LIST;
    }
    તોડો;
  સ્થિતિ PL_UNIX_TIME_PART3:
    parser->item_length++;
    જો(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.time = parser->item_offset;
      જો(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.મુખ્ય = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      નહીંતર {
        parser->state.UNIX.મુખ્ય = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    નહીંતર જો(!ISALNUM(c) && c != '.' && c != ':')
      પાછા CURLE_FTP_BAD_FILE_LIST;
    તોડો;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode parse_unix_filename(સંરચના Curl_easy *data,
                                    સંરચના ftp_parselist_data *parser,
                                    સંરચના fileinfo *infop,
                                    અચલ અક્ષર c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  અક્ષર *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  પસંદગી(parser->state.UNIX.sub.filename) {
  સ્થિતિ PL_UNIX_FILENAME_PRESPACE:
    જો(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    તોડો;
  સ્થિતિ PL_UNIX_FILENAME_NAME:
    parser->item_length++;
    જો(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    નહીંતર જો(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.મુખ્ય = PL_UNIX_FILETYPE;
      result = ftp_pl_ઉમેરો_finfo(data, infop);
    }
    તોડો;
  સ્થિતિ PL_UNIX_FILENAME_WINDOWSEOL:
    જો(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.મુખ્ય = PL_UNIX_FILETYPE;
      result = ftp_pl_ઉમેરો_finfo(data, infop);
    }
    નહીંતર
      result = CURLE_FTP_BAD_FILE_LIST;
    તોડો;
  }
  પાછા result;
}

સ્થિર CURLcode parse_unix_symlink(સંરચના Curl_easy *data,
                                   સંરચના ftp_parselist_data *parser,
                                   સંરચના fileinfo *infop,
                                   અચલ અક્ષર c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  અક્ષર *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  પસંદગી(parser->state.UNIX.sub.symlink) {
  સ્થિતિ PL_UNIX_SYMLINK_PRESPACE:
    જો(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    તોડો;
  સ્થિતિ PL_UNIX_SYMLINK_NAME:
    parser->item_length++;
    જો(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    નહીંતર જો(c == '\r' || c == '\n')
      પાછા CURLE_FTP_BAD_FILE_LIST;

    તોડો;
  સ્થિતિ PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_length++;
    જો(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    નહીંતર જો(c == '\r' || c == '\n')
      પાછા CURLE_FTP_BAD_FILE_LIST;
    નહીંતર
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    તોડો;
  સ્થિતિ PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_length++;
    જો(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    નહીંતર જો(c == '\r' || c == '\n')
      પાછા CURLE_FTP_BAD_FILE_LIST;
    નહીંતર
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    તોડો;
  સ્થિતિ PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_length++;
    જો(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_offset + parser->item_length - 4] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->item_length = 0;
      parser->item_offset = 0;
    }
    નહીંતર જો(c == '\r' || c == '\n')
      પાછા CURLE_FTP_BAD_FILE_LIST;
    નહીંતર
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    તોડો;
  સ્થિતિ PL_UNIX_SYMLINK_PRETARGET4:
    જો(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_offset = len - 1;
      parser->item_length = 1;
    }
    નહીંતર
      પાછા CURLE_FTP_BAD_FILE_LIST;

    તોડો;
  સ્થિતિ PL_UNIX_SYMLINK_TARGET:
    parser->item_length++;
    જો(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    નહીંતર જો(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_ઉમેરો_finfo(data, infop);
      જો(result)
        તોડો;

      parser->state.UNIX.મુખ્ય = PL_UNIX_FILETYPE;
    }
    તોડો;
  સ્થિતિ PL_UNIX_SYMLINK_WINDOWSEOL:
    જો(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_ઉમેરો_finfo(data, infop);
      જો(result)
        તોડો;

      parser->state.UNIX.મુખ્ય = PL_UNIX_FILETYPE;
    }
    નહીંતર
      result = CURLE_FTP_BAD_FILE_LIST;

    તોડો;
  }
  પાછા result;
}

સ્થિર CURLcode parse_unix(સંરચના Curl_easy *data,
                           સંરચના ftp_parselist_data *parser,
                           સંરચના fileinfo *infop,
                           અચલ અક્ષર c)
{
  સંરચના curl_fileinfo *finfo = &infop->info;
  CURLcode result = CURLE_OK;

  પસંદગી(parser->state.UNIX.મુખ્ય) {
  સ્થિતિ PL_UNIX_TOTALSIZE:
    result = parse_unix_totalsize(parser, infop, c);
    જો(result)
      તોડો;
    જો(parser->state.UNIX.મુખ્ય != PL_UNIX_FILETYPE)
      તોડો;
    FALLTHROUGH();
  સ્થિતિ PL_UNIX_FILETYPE:
    result = unix_filetype(c, &finfo->filetype);
    જો(!result) {
      parser->state.UNIX.મુખ્ય = PL_UNIX_PERMISSION;
      parser->item_length = 0;
      parser->item_offset = 1;
    }
    તોડો;
  સ્થિતિ PL_UNIX_PERMISSION:
    result = parse_unix_permission(parser, infop, c);
    તોડો;
  સ્થિતિ PL_UNIX_HLINKS:
    result = parse_unix_hlinks(parser, infop, c);
    તોડો;
  સ્થિતિ PL_UNIX_USER:
    result = parse_unix_user(parser, infop, c);
    તોડો;
  સ્થિતિ PL_UNIX_GROUP:
    result = parse_unix_group(parser, infop, c);
    તોડો;
  સ્થિતિ PL_UNIX_SIZE:
    result = parse_unix_size(parser, infop, c);
    તોડો;
  સ્થિતિ PL_UNIX_TIME:
    result = parse_unix_time(parser, infop, c);
    તોડો;
  સ્થિતિ PL_UNIX_FILENAME:
    result = parse_unix_filename(data, parser, infop, c);
    તોડો;
  સ્થિતિ PL_UNIX_SYMLINK:
    result = parse_unix_symlink(data, parser, infop, c);
    તોડો;
  }
  પાછા result;
}

સ્થિર CURLcode parse_winnt(સંરચના Curl_easy *data,
                            સંરચના ftp_parselist_data *parser,
                            સંરચના fileinfo *infop,
                            અચલ અક્ષર c)
{
  સંરચના curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  અક્ષર *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  પસંદગી(parser->state.NT.મુખ્ય) {
  સ્થિતિ PL_WINNT_DATE:
    parser->item_length++;
    જો(parser->item_length < 9) {
      જો(!strchr("0123456789-", c)) { /* only simple control */
        પાછા CURLE_FTP_BAD_FILE_LIST;
      }
    }
    નહીંતર જો(parser->item_length == 9) {
      જો(c == ' ') {
        parser->state.NT.મુખ્ય = PL_WINNT_TIME;
        parser->state.NT.sub.time = PL_WINNT_TIME_PRESPACE;
      }
      નહીંતર
        પાછા CURLE_FTP_BAD_FILE_LIST;
    }
    નહીંતર
      પાછા CURLE_FTP_BAD_FILE_LIST;
    તોડો;
  સ્થિતિ PL_WINNT_TIME:
    parser->item_length++;
    પસંદગી(parser->state.NT.sub.time) {
    સ્થિતિ PL_WINNT_TIME_PRESPACE:
      જો(!ISBLANK(c))
        parser->state.NT.sub.time = PL_WINNT_TIME_TIME;
      તોડો;
    સ્થિતિ PL_WINNT_TIME_TIME:
      જો(c == ' ') {
        parser->offsets.time = parser->item_offset;
        mem[parser->item_offset + parser->item_length - 1] = 0;
        parser->state.NT.મુખ્ય = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_length = 0;
      }
      નહીંતર જો(!strchr("APM0123456789:", c))
        પાછા CURLE_FTP_BAD_FILE_LIST;
      તોડો;
    }
    તોડો;
  સ્થિતિ PL_WINNT_DIRORSIZE:
    પસંદગી(parser->state.NT.sub.dirorsize) {
    સ્થિતિ PL_WINNT_DIRORSIZE_PRESPACE:
      જો(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      તોડો;
    સ્થિતિ PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_length++;
      જો(c == ' ') {
        mem[parser->item_offset + parser->item_length - 1] = 0;
        જો(!strcmp("<DIR>", mem + parser->item_offset)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->size = 0;
        }
        નહીંતર {
          અચલ અક્ષર *p = mem + parser->item_offset;
          જો(curlx_str_numblanks(&p, &finfo->size)) {
            પાછા CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_data->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
        parser->item_length = 0;
        parser->state.NT.મુખ્ય = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      તોડો;
    }
    તોડો;
  સ્થિતિ PL_WINNT_FILENAME:
    પસંદગી(parser->state.NT.sub.filename) {
    સ્થિતિ PL_WINNT_FILENAME_PRESPACE:
      જો(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      તોડો;
    સ્થિતિ PL_WINNT_FILENAME_CONTENT:
      parser->item_length++;
      જો(!len)
        પાછા CURLE_FTP_BAD_FILE_LIST;
      જો(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      નહીંતર જો(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        mem[len - 1] = 0;
        result = ftp_pl_ઉમેરો_finfo(data, infop);
        જો(result)
          પાછા result;

        parser->state.NT.મુખ્ય = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      તોડો;
    સ્થિતિ PL_WINNT_FILENAME_WINEOL:
      જો(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        result = ftp_pl_ઉમેરો_finfo(data, infop);
        જો(result)
          પાછા result;

        parser->state.NT.મુખ્ય = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      નહીંતર
        પાછા CURLE_FTP_BAD_FILE_LIST;

      તોડો;
    }
    તોડો;
  }

  પાછા CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(સંરચના Curl_easy *data)
{
  જો(data->state.wildcardmatch) {
    સંરચના WildcardData * અચલ wildcard = data->wildcard;
    પાછા wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  પાછા FALSE;
}

size_t Curl_ftp_parselist(અક્ષર *buffer, size_t size, size_t nmemb,
                          શૂન્ય *connptr)
{
  size_t bufflen = size * nmemb;
  સંરચના Curl_easy *data = (સંરચના Curl_easy *)connptr;
  સંરચના ftp_wc *ftpwc = data->wildcard->ftpwc;
  સંરચના ftp_parselist_data *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode result;
  size_t retsize = bufflen;

  જો(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    જાઓ fail;
  }

  જો(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(buffer[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  જ્યારે(i < bufflen) { /* FSM */
    અક્ષર c = buffer[i];
    સંરચના fileinfo *infop;
    જો(!parser->file_data) { /* tmp file data is not allocated yet */
      parser->file_data = Curl_fileinfo_alloc();
      જો(!parser->file_data) {
        parser->error = CURLE_OUT_OF_MEMORY;
        જાઓ fail;
      }
      parser->item_offset = 0;
      parser->item_length = 0;
      curlx_dyn_init(&parser->file_data->buf, MAX_FTPLIST_BUFFER);
    }

    infop = parser->file_data;

    જો(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_OUT_OF_MEMORY;
      જાઓ fail;
    }

    પસંદગી(parser->os_type) {
    સ્થિતિ OS_TYPE_UNIX:
      result = parse_unix(data, parser, infop, c);
      તોડો;
    સ્થિતિ OS_TYPE_WIN_NT:
      result = parse_winnt(data, parser, infop, c);
      તોડો;
    મૂળભૂત:
      retsize = bufflen + 1;
      જાઓ fail;
    }
    જો(result) {
      parser->error = result;
      જાઓ fail;
    }

    i++;
  }
  પાછા retsize;

fail:

  /* Clean up any allocated memory. */
  જો(parser->file_data) {
    Curl_fileinfo_cleanup(parser->file_data);
    parser->file_data = ખાલી;
  }

  પાછા retsize;
}

#endif /* !CURL_DISABLE_FTP */
