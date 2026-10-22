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

#अगर_अघोषित CURL_DISABLE_FTP

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

#समावेश "urldata.h"
#समावेश "fileinfo.h"
#समावेश "llist.h"
#समावेश "ftp.h"
#समावेश "ftp-int.h"
#समावेश "ftplistparser.h"
#समावेश "curl_fnmatch.h"
#समावेश "multiif.h"
#समावेश "curlx/strparse.h"

प्रकार क्रमागत {
  PL_UNIX_TOTALSIZE = 0,
  PL_UNIX_FILETYPE,
  PL_UNIX_PERMISSION,
  PL_UNIX_HLINKS,
  PL_UNIX_USER,
  PL_UNIX_GROUP,
  PL_UNIX_आकार,
  PL_UNIX_TIME,
  PL_UNIX_FILENAME,
  PL_UNIX_SYMLINK
} pl_unix_mainstate;

प्रकार जोड़ {
  क्रमागत {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } योग_dirsize;

  क्रमागत {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_संख्या
  } hlinks;

  क्रमागत {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  क्रमागत {
    PL_UNIX_GROUP_PRESPACE = 0,
    PL_UNIX_GROUP_NAME
  } group;

  क्रमागत {
    PL_UNIX_आकार_PRESPACE = 0,
    PL_UNIX_आकार_संख्या
  } आकार;

  क्रमागत {
    PL_UNIX_TIME_PREPART1 = 0,
    PL_UNIX_TIME_PART1,
    PL_UNIX_TIME_PREPART2,
    PL_UNIX_TIME_PART2,
    PL_UNIX_TIME_PREPART3,
    PL_UNIX_TIME_PART3
  } समय;

  क्रमागत {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  क्रमागत {
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

प्रकार क्रमागत {
  PL_WINNT_DATE = 0,
  PL_WINNT_TIME,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

प्रकार जोड़ {
  क्रमागत {
    PL_WINNT_TIME_PRESPACE = 0,
    PL_WINNT_TIME_TIME
  } समय;
  क्रमागत {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  क्रमागत {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
काष्ठा ftp_parselist_तथ्य {
  क्रमागत {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  जोड़ {
    काष्ठा {
      pl_unix_mainstate मुख्य;
      pl_unix_substate sub;
    } UNIX;

    काष्ठा {
      pl_winNT_mainstate मुख्य;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  काष्ठा fileinfo *file_तथ्य;
  अचिन्हित पूर्णांक item_लंबाई;
  माप_प्रकार item_offset;
  काष्ठा {
    माप_प्रकार filename;
    माप_प्रकार user;
    माप_प्रकार group;
    माप_प्रकार समय;
    माप_प्रकार perm;
    माप_प्रकार symlink_target;
  } offsets;
};

अटल व्योम fileinfo_dtor(व्योम *user, व्योम *element)
{
  (व्योम)user;
  Curl_fileinfo_cleanup(element);
}

व्योम Curl_wildcard_init(काष्ठा Wildcard_तथ्य *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

व्योम Curl_wildcard_dtor(काष्ठा Wildcard_तथ्य **wcp)
{
  काष्ठा Wildcard_तथ्य *wc = *wcp;
  अगर(!wc)
    वापस;

  अगर(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = शून्य;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, शून्य);
  curlx_safefree(wc->path);
  curlx_safefree(wc->pattern);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = शून्य;
}

काष्ठा ftp_parselist_तथ्य *Curl_ftp_parselist_तथ्य_alloc(व्योम)
{
  वापस curlx_calloc(1, माप(काष्ठा ftp_parselist_तथ्य));
}

व्योम Curl_ftp_parselist_तथ्य_free(काष्ठा ftp_parselist_तथ्य **parserp)
{
  काष्ठा ftp_parselist_तथ्य *parser = *parserp;
  अगर(parser)
    Curl_fileinfo_cleanup(parser->file_तथ्य);
  curlx_free(parser);
  *parserp = शून्य;
}

CURLcode Curl_ftp_parselist_geterror(काष्ठा ftp_parselist_तथ्य *pl_तथ्य)
{
  वापस pl_तथ्य->error;
}

#घोषणा FTP_LP_MALFORMATED_PERM 0x01000000

अटल अचिन्हित पूर्णांक ftp_pl_get_permission(स्थिर अक्षर *str)
{
  अचिन्हित पूर्णांक permissions = 0;
  /* USER */
  अगर(str[0] == 'r')
    permissions |= 1 << 8;
  अन्यथा अगर(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  अगर(str[1] == 'w')
    permissions |= 1 << 7;
  अन्यथा अगर(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  अगर(str[2] == 'x')
    permissions |= 1 << 6;
  अन्यथा अगर(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  अन्यथा अगर(str[2] == 'S')
    permissions |= 1 << 11;
  अन्यथा अगर(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* GROUP */
  अगर(str[3] == 'r')
    permissions |= 1 << 5;
  अन्यथा अगर(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  अगर(str[4] == 'w')
    permissions |= 1 << 4;
  अन्यथा अगर(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  अगर(str[5] == 'x')
    permissions |= 1 << 3;
  अन्यथा अगर(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  अन्यथा अगर(str[5] == 'S')
    permissions |= 1 << 10;
  अन्यथा अगर(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  अगर(str[6] == 'r')
    permissions |= 1 << 2;
  अन्यथा अगर(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  अगर(str[7] == 'w')
    permissions |= 1 << 1;
  अन्यथा अगर(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  अगर(str[8] == 'x')
    permissions |= 1;
  अन्यथा अगर(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  अन्यथा अगर(str[8] == 'T')
    permissions |= 1 << 9;
  अन्यथा अगर(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  वापस permissions;
}

अटल CURLcode ftp_pl_जोड़ो_finfo(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा fileinfo *infop)
{
  curl_fnmatch_callback compare;
  काष्ठा Wildcard_तथ्य *wc = तथ्य->wildcard;
  काष्ठा ftp_wc *ftpwc = wc->ftpwc;
  काष्ठा Curl_llist *llist = &wc->filelist;
  काष्ठा ftp_parselist_तथ्य *parser = ftpwc->parser;
  bool add = TRUE;
  काष्ठा curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  अक्षर *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->strings.group  = parser->offsets.group ?
                          str + parser->offsets.group : शून्य;
  finfo->strings.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : शून्य;
  finfo->strings.target = parser->offsets.symlink_target &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_target : शून्य;
  finfo->strings.समय   = str + parser->offsets.समय;
  finfo->strings.user   = parser->offsets.user ?
                          str + parser->offsets.user : शून्य;

  /* get correct fnmatch callback */
  compare = तथ्य->set.fnmatch;
  अगर(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  अगर(म_अक्षर(finfo->filename, '/'))
    add = FALSE;
  अन्यथा {
    /* a real file name cannot contain a slash */
    काष्ठा Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, तथ्य, easy_fnmatch_तथ्य);
    अगर(compare(तथ्य->set.fnmatch_तथ्य, wc->pattern, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      अगर((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->strings.target &&
         म_माला(finfo->strings.target, " -> ")) {
        add = FALSE;
      }
    }
    अन्यथा {
      add = FALSE;
    }
    CURL_CBAPI_END(&guard);
  }

  अगर(add) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  अन्यथा {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_तथ्य = शून्य;
  वापस CURLE_OK;
}

#घोषणा MAX_FTPLIST_बफर 10000 /* arbitrarily set */

अटल CURLcode unix_filetype(स्थिर अक्षर c, curlfiletype *t)
{
  चयन(c) {
  हाल '-':
    *t = CURLFILETYPE_FILE;
    अवरोध;
  हाल 'd':
    *t = CURLFILETYPE_DIRECTORY;
    अवरोध;
  हाल 'l':
    *t = CURLFILETYPE_SYMLINK;
    अवरोध;
  हाल 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    अवरोध;
  हाल 's':
    *t = CURLFILETYPE_SOCKET;
    अवरोध;
  हाल 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    अवरोध;
  हाल 'b':
    *t = CURLFILETYPE_DEVICE_BLOCK;
    अवरोध;
  हाल 'D':
    *t = CURLFILETYPE_DOOR;
    अवरोध;
  शेष:
    वापस CURLE_FTP_BAD_FILE_LIST;
  }
  वापस CURLE_OK;
}

अटल CURLcode parse_unix_totalsize(काष्ठा ftp_parselist_तथ्य *parser,
                                     काष्ठा fileinfo *infop,
                                     स्थिर अक्षर c)
{
  माप_प्रकार len = curlx_dyn_len(&infop->buf);
  अक्षर *mem = curlx_dyn_ptr(&infop->buf);
  चयन(parser->state.UNIX.sub.योग_dirsize) {
  हाल PL_UNIX_TOTALSIZE_INIT:
    अगर(c == 't') {
      parser->state.UNIX.sub.योग_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_लंबाई++;
    }
    अन्यथा {
      parser->state.UNIX.मुख्य = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    अवरोध;
  हाल PL_UNIX_TOTALSIZE_READING:
    parser->item_लंबाई++;
    अगर(c == '\r') {
      parser->item_लंबाई--;
      अगर(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    अन्यथा अगर(c == '\n') {
      mem[parser->item_लंबाई - 1] = 0;
      अगर(!म_भेदन("total ", mem, 6)) {
        स्थिर अक्षर *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        जबतक(ISDIGIT(*endptr))
          endptr++;
        अगर(*endptr) {
          वापस CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.मुख्य = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      अन्यथा
        वापस CURLE_FTP_BAD_FILE_LIST;
    }
    अवरोध;
  }
  वापस CURLE_OK;
}

अटल CURLcode parse_unix_permission(काष्ठा ftp_parselist_तथ्य *parser,
                                      काष्ठा fileinfo *infop,
                                      स्थिर अक्षर c)
{
  अक्षर *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_लंबाई++;
  अगर((parser->item_लंबाई <= 9) && !म_अक्षर("rwx-tTsS", c))
    वापस CURLE_FTP_BAD_FILE_LIST;

  अन्यथा अगर(parser->item_लंबाई == 10) {
    अचिन्हित पूर्णांक perm;
    अगर(c != ' ')
      वापस CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_offset);
    अगर(perm & FTP_LP_MALFORMATED_PERM)
      वापस CURLE_FTP_BAD_FILE_LIST;

    parser->file_तथ्य->info.flags |= CURLFINFOFLAG_KNOWN_PERM;
    parser->file_तथ्य->info.perm = perm;
    parser->offsets.perm = parser->item_offset;

    parser->item_लंबाई = 0;
    parser->state.UNIX.मुख्य = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  वापस CURLE_OK;
}

अटल CURLcode parse_unix_hlinks(काष्ठा ftp_parselist_तथ्य *parser,
                                  काष्ठा fileinfo *infop,
                                  स्थिर अक्षर c)
{
  माप_प्रकार len = curlx_dyn_len(&infop->buf);
  अक्षर *mem = curlx_dyn_ptr(&infop->buf);

  चयन(parser->state.UNIX.sub.hlinks) {
  हाल PL_UNIX_HLINKS_PRESPACE:
    अगर(c != ' ') {
      अगर(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_लंबाई = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_संख्या;
      }
      अन्यथा
        वापस CURLE_FTP_BAD_FILE_LIST;
    }
    अवरोध;
  हाल PL_UNIX_HLINKS_संख्या:
    parser->item_लंबाई++;
    अगर(c == ' ') {
      स्थिर अक्षर *p = &mem[parser->item_offset];
      curl_off_t hlinks;
      mem[parser->item_offset + parser->item_लंबाई - 1] = 0;

      अगर(!curlx_str_संख्या(&p, &hlinks, दीर्घ_उच्च)) {
        parser->file_तथ्य->info.flags |= CURLFINFOFLAG_KNOWN_HLINKCOUNT;
        parser->file_तथ्य->info.hardlinks = (दीर्घ)hlinks;
      }
      parser->item_लंबाई = 0;
      parser->item_offset = 0;
      parser->state.UNIX.मुख्य = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    अन्यथा अगर(!ISDIGIT(c))
      वापस CURLE_FTP_BAD_FILE_LIST;

    अवरोध;
  }
  वापस CURLE_OK;
}

अटल CURLcode parse_unix_user(काष्ठा ftp_parselist_तथ्य *parser,
                                काष्ठा fileinfo *infop,
                                स्थिर अक्षर c)
{
  माप_प्रकार len = curlx_dyn_len(&infop->buf);
  अक्षर *mem = curlx_dyn_ptr(&infop->buf);
  चयन(parser->state.UNIX.sub.user) {
  हाल PL_UNIX_USER_PRESPACE:
    अगर(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_लंबाई = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    अवरोध;
  हाल PL_UNIX_USER_PARSING:
    parser->item_लंबाई++;
    अगर(c == ' ') {
      mem[parser->item_offset + parser->item_लंबाई - 1] = 0;
      parser->offsets.user = parser->item_offset;
      parser->state.UNIX.मुख्य = PL_UNIX_GROUP;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_PRESPACE;
      parser->item_offset = 0;
      parser->item_लंबाई = 0;
    }
    अवरोध;
  }
  वापस CURLE_OK;
}

अटल CURLcode parse_unix_group(काष्ठा ftp_parselist_तथ्य *parser,
                                 काष्ठा fileinfo *infop,
                                 स्थिर अक्षर c)
{
  माप_प्रकार len = curlx_dyn_len(&infop->buf);
  अक्षर *mem = curlx_dyn_ptr(&infop->buf);
  चयन(parser->state.UNIX.sub.group) {
  हाल PL_UNIX_GROUP_PRESPACE:
    अगर(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_लंबाई = 1;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_NAME;
    }
    अवरोध;
  हाल PL_UNIX_GROUP_NAME:
    parser->item_लंबाई++;
    अगर(c == ' ') {
      mem[parser->item_offset + parser->item_लंबाई - 1] = 0;
      parser->offsets.group = parser->item_offset;
      parser->state.UNIX.मुख्य = PL_UNIX_आकार;
      parser->state.UNIX.sub.आकार = PL_UNIX_आकार_PRESPACE;
      parser->item_offset = 0;
      parser->item_लंबाई = 0;
    }
    अवरोध;
  }
  वापस CURLE_OK;
}

अटल CURLcode parse_unix_आकार(काष्ठा ftp_parselist_तथ्य *parser,
                                काष्ठा fileinfo *infop,
                                स्थिर अक्षर c)
{
  माप_प्रकार len = curlx_dyn_len(&infop->buf);
  अक्षर *mem = curlx_dyn_ptr(&infop->buf);
  चयन(parser->state.UNIX.sub.आकार) {
  हाल PL_UNIX_आकार_PRESPACE:
    अगर(c != ' ') {
      अगर(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_लंबाई = 1;
        parser->state.UNIX.sub.आकार = PL_UNIX_आकार_संख्या;
      }
      अन्यथा
        वापस CURLE_FTP_BAD_FILE_LIST;
    }
    अवरोध;
  हाल PL_UNIX_आकार_संख्या:
    parser->item_लंबाई++;
    अगर(c == ' ') {
      स्थिर अक्षर *p = mem + parser->item_offset;
      curl_off_t fsize;
      mem[parser->item_offset + parser->item_लंबाई - 1] = 0;
      अगर(!curlx_str_numblanks(&p, &fsize)) {
        अगर(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_तथ्य->info.flags |= CURLFINFOFLAG_KNOWN_आकार;
          parser->file_तथ्य->info.आकार = fsize;
        }
        parser->item_लंबाई = 0;
        parser->item_offset = 0;
        parser->state.UNIX.मुख्य = PL_UNIX_TIME;
        parser->state.UNIX.sub.समय = PL_UNIX_TIME_PREPART1;
      }
    }
    अन्यथा अगर(!ISDIGIT(c))
      वापस CURLE_FTP_BAD_FILE_LIST;

    अवरोध;
  }
  वापस CURLE_OK;
}

अटल CURLcode parse_unix_time(काष्ठा ftp_parselist_तथ्य *parser,
                                काष्ठा fileinfo *infop,
                                स्थिर अक्षर c)
{
  माप_प्रकार len = curlx_dyn_len(&infop->buf);
  अक्षर *mem = curlx_dyn_ptr(&infop->buf);
  काष्ठा curl_fileinfo *finfo = &infop->info;

  चयन(parser->state.UNIX.sub.समय) {
  हाल PL_UNIX_TIME_PREPART1:
    अगर(c != ' ') {
      अगर(ISALNUM(c) && len) {
        parser->item_offset = len - 1;
        parser->item_लंबाई = 1;
        parser->state.UNIX.sub.समय = PL_UNIX_TIME_PART1;
      }
      अन्यथा
        वापस CURLE_FTP_BAD_FILE_LIST;
    }
    अवरोध;
  हाल PL_UNIX_TIME_PART1:
    parser->item_लंबाई++;
    अगर(c == ' ')
      parser->state.UNIX.sub.समय = PL_UNIX_TIME_PREPART2;

    अन्यथा अगर(!ISALNUM(c) && c != '.')
      वापस CURLE_FTP_BAD_FILE_LIST;

    अवरोध;
  हाल PL_UNIX_TIME_PREPART2:
    parser->item_लंबाई++;
    अगर(c != ' ') {
      अगर(ISALNUM(c))
        parser->state.UNIX.sub.समय = PL_UNIX_TIME_PART2;
      अन्यथा
        वापस CURLE_FTP_BAD_FILE_LIST;
    }
    अवरोध;
  हाल PL_UNIX_TIME_PART2:
    parser->item_लंबाई++;
    अगर(c == ' ')
      parser->state.UNIX.sub.समय = PL_UNIX_TIME_PREPART3;
    अन्यथा अगर(!ISALNUM(c) && c != '.')
      वापस CURLE_FTP_BAD_FILE_LIST;
    अवरोध;
  हाल PL_UNIX_TIME_PREPART3:
    parser->item_लंबाई++;
    अगर(c != ' ') {
      अगर(ISALNUM(c))
        parser->state.UNIX.sub.समय = PL_UNIX_TIME_PART3;
      अन्यथा
        वापस CURLE_FTP_BAD_FILE_LIST;
    }
    अवरोध;
  हाल PL_UNIX_TIME_PART3:
    parser->item_लंबाई++;
    अगर(c == ' ') {
      mem[parser->item_offset + parser->item_लंबाई - 1] = 0;
      parser->offsets.समय = parser->item_offset;
      अगर(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.मुख्य = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      अन्यथा {
        parser->state.UNIX.मुख्य = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    अन्यथा अगर(!ISALNUM(c) && c != '.' && c != ':')
      वापस CURLE_FTP_BAD_FILE_LIST;
    अवरोध;
  }
  वापस CURLE_OK;
}

अटल CURLcode parse_unix_filename(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा ftp_parselist_तथ्य *parser,
                                    काष्ठा fileinfo *infop,
                                    स्थिर अक्षर c)
{
  माप_प्रकार len = curlx_dyn_len(&infop->buf);
  अक्षर *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode परिणाम = CURLE_OK;

  चयन(parser->state.UNIX.sub.filename) {
  हाल PL_UNIX_FILENAME_PRESPACE:
    अगर(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_लंबाई = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    अवरोध;
  हाल PL_UNIX_FILENAME_NAME:
    parser->item_लंबाई++;
    अगर(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    अन्यथा अगर(c == '\n') {
      mem[parser->item_offset + parser->item_लंबाई - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.मुख्य = PL_UNIX_FILETYPE;
      परिणाम = ftp_pl_जोड़ो_finfo(तथ्य, infop);
    }
    अवरोध;
  हाल PL_UNIX_FILENAME_WINDOWSEOL:
    अगर(c == '\n') {
      mem[parser->item_offset + parser->item_लंबाई - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.मुख्य = PL_UNIX_FILETYPE;
      परिणाम = ftp_pl_जोड़ो_finfo(तथ्य, infop);
    }
    अन्यथा
      परिणाम = CURLE_FTP_BAD_FILE_LIST;
    अवरोध;
  }
  वापस परिणाम;
}

अटल CURLcode parse_unix_symlink(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ftp_parselist_तथ्य *parser,
                                   काष्ठा fileinfo *infop,
                                   स्थिर अक्षर c)
{
  माप_प्रकार len = curlx_dyn_len(&infop->buf);
  अक्षर *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode परिणाम = CURLE_OK;

  चयन(parser->state.UNIX.sub.symlink) {
  हाल PL_UNIX_SYMLINK_PRESPACE:
    अगर(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_लंबाई = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    अवरोध;
  हाल PL_UNIX_SYMLINK_NAME:
    parser->item_लंबाई++;
    अगर(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    अन्यथा अगर(c == '\r' || c == '\n')
      वापस CURLE_FTP_BAD_FILE_LIST;

    अवरोध;
  हाल PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_लंबाई++;
    अगर(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    अन्यथा अगर(c == '\r' || c == '\n')
      वापस CURLE_FTP_BAD_FILE_LIST;
    अन्यथा
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    अवरोध;
  हाल PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_लंबाई++;
    अगर(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    अन्यथा अगर(c == '\r' || c == '\n')
      वापस CURLE_FTP_BAD_FILE_LIST;
    अन्यथा
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    अवरोध;
  हाल PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_लंबाई++;
    अगर(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_offset + parser->item_लंबाई - 4] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->item_लंबाई = 0;
      parser->item_offset = 0;
    }
    अन्यथा अगर(c == '\r' || c == '\n')
      वापस CURLE_FTP_BAD_FILE_LIST;
    अन्यथा
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    अवरोध;
  हाल PL_UNIX_SYMLINK_PRETARGET4:
    अगर(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_offset = len - 1;
      parser->item_लंबाई = 1;
    }
    अन्यथा
      वापस CURLE_FTP_BAD_FILE_LIST;

    अवरोध;
  हाल PL_UNIX_SYMLINK_TARGET:
    parser->item_लंबाई++;
    अगर(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    अन्यथा अगर(c == '\n') {
      mem[parser->item_offset + parser->item_लंबाई - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      परिणाम = ftp_pl_जोड़ो_finfo(तथ्य, infop);
      अगर(परिणाम)
        अवरोध;

      parser->state.UNIX.मुख्य = PL_UNIX_FILETYPE;
    }
    अवरोध;
  हाल PL_UNIX_SYMLINK_WINDOWSEOL:
    अगर(c == '\n') {
      mem[parser->item_offset + parser->item_लंबाई - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      परिणाम = ftp_pl_जोड़ो_finfo(तथ्य, infop);
      अगर(परिणाम)
        अवरोध;

      parser->state.UNIX.मुख्य = PL_UNIX_FILETYPE;
    }
    अन्यथा
      परिणाम = CURLE_FTP_BAD_FILE_LIST;

    अवरोध;
  }
  वापस परिणाम;
}

अटल CURLcode parse_unix(काष्ठा Curl_easy *तथ्य,
                           काष्ठा ftp_parselist_तथ्य *parser,
                           काष्ठा fileinfo *infop,
                           स्थिर अक्षर c)
{
  काष्ठा curl_fileinfo *finfo = &infop->info;
  CURLcode परिणाम = CURLE_OK;

  चयन(parser->state.UNIX.मुख्य) {
  हाल PL_UNIX_TOTALSIZE:
    परिणाम = parse_unix_totalsize(parser, infop, c);
    अगर(परिणाम)
      अवरोध;
    अगर(parser->state.UNIX.मुख्य != PL_UNIX_FILETYPE)
      अवरोध;
    FALLTHROUGH();
  हाल PL_UNIX_FILETYPE:
    परिणाम = unix_filetype(c, &finfo->filetype);
    अगर(!परिणाम) {
      parser->state.UNIX.मुख्य = PL_UNIX_PERMISSION;
      parser->item_लंबाई = 0;
      parser->item_offset = 1;
    }
    अवरोध;
  हाल PL_UNIX_PERMISSION:
    परिणाम = parse_unix_permission(parser, infop, c);
    अवरोध;
  हाल PL_UNIX_HLINKS:
    परिणाम = parse_unix_hlinks(parser, infop, c);
    अवरोध;
  हाल PL_UNIX_USER:
    परिणाम = parse_unix_user(parser, infop, c);
    अवरोध;
  हाल PL_UNIX_GROUP:
    परिणाम = parse_unix_group(parser, infop, c);
    अवरोध;
  हाल PL_UNIX_आकार:
    परिणाम = parse_unix_आकार(parser, infop, c);
    अवरोध;
  हाल PL_UNIX_TIME:
    परिणाम = parse_unix_time(parser, infop, c);
    अवरोध;
  हाल PL_UNIX_FILENAME:
    परिणाम = parse_unix_filename(तथ्य, parser, infop, c);
    अवरोध;
  हाल PL_UNIX_SYMLINK:
    परिणाम = parse_unix_symlink(तथ्य, parser, infop, c);
    अवरोध;
  }
  वापस परिणाम;
}

अटल CURLcode parse_winnt(काष्ठा Curl_easy *तथ्य,
                            काष्ठा ftp_parselist_तथ्य *parser,
                            काष्ठा fileinfo *infop,
                            स्थिर अक्षर c)
{
  काष्ठा curl_fileinfo *finfo = &infop->info;
  माप_प्रकार len = curlx_dyn_len(&infop->buf);
  अक्षर *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode परिणाम = CURLE_OK;

  चयन(parser->state.NT.मुख्य) {
  हाल PL_WINNT_DATE:
    parser->item_लंबाई++;
    अगर(parser->item_लंबाई < 9) {
      अगर(!म_अक्षर("0123456789-", c)) { /* only simple control */
        वापस CURLE_FTP_BAD_FILE_LIST;
      }
    }
    अन्यथा अगर(parser->item_लंबाई == 9) {
      अगर(c == ' ') {
        parser->state.NT.मुख्य = PL_WINNT_TIME;
        parser->state.NT.sub.समय = PL_WINNT_TIME_PRESPACE;
      }
      अन्यथा
        वापस CURLE_FTP_BAD_FILE_LIST;
    }
    अन्यथा
      वापस CURLE_FTP_BAD_FILE_LIST;
    अवरोध;
  हाल PL_WINNT_TIME:
    parser->item_लंबाई++;
    चयन(parser->state.NT.sub.समय) {
    हाल PL_WINNT_TIME_PRESPACE:
      अगर(!ISBLANK(c))
        parser->state.NT.sub.समय = PL_WINNT_TIME_TIME;
      अवरोध;
    हाल PL_WINNT_TIME_TIME:
      अगर(c == ' ') {
        parser->offsets.समय = parser->item_offset;
        mem[parser->item_offset + parser->item_लंबाई - 1] = 0;
        parser->state.NT.मुख्य = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_लंबाई = 0;
      }
      अन्यथा अगर(!म_अक्षर("APM0123456789:", c))
        वापस CURLE_FTP_BAD_FILE_LIST;
      अवरोध;
    }
    अवरोध;
  हाल PL_WINNT_DIRORSIZE:
    चयन(parser->state.NT.sub.dirorsize) {
    हाल PL_WINNT_DIRORSIZE_PRESPACE:
      अगर(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_लंबाई = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      अवरोध;
    हाल PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_लंबाई++;
      अगर(c == ' ') {
        mem[parser->item_offset + parser->item_लंबाई - 1] = 0;
        अगर(!म_भेद("<DIR>", mem + parser->item_offset)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->आकार = 0;
        }
        अन्यथा {
          स्थिर अक्षर *p = mem + parser->item_offset;
          अगर(curlx_str_numblanks(&p, &finfo->आकार)) {
            वापस CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_तथ्य->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_तथ्य->info.flags |= CURLFINFOFLAG_KNOWN_आकार;
        parser->item_लंबाई = 0;
        parser->state.NT.मुख्य = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      अवरोध;
    }
    अवरोध;
  हाल PL_WINNT_FILENAME:
    चयन(parser->state.NT.sub.filename) {
    हाल PL_WINNT_FILENAME_PRESPACE:
      अगर(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_लंबाई = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      अवरोध;
    हाल PL_WINNT_FILENAME_CONTENT:
      parser->item_लंबाई++;
      अगर(!len)
        वापस CURLE_FTP_BAD_FILE_LIST;
      अगर(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      अन्यथा अगर(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        mem[len - 1] = 0;
        परिणाम = ftp_pl_जोड़ो_finfo(तथ्य, infop);
        अगर(परिणाम)
          वापस परिणाम;

        parser->state.NT.मुख्य = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      अवरोध;
    हाल PL_WINNT_FILENAME_WINEOL:
      अगर(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        परिणाम = ftp_pl_जोड़ो_finfo(तथ्य, infop);
        अगर(परिणाम)
          वापस परिणाम;

        parser->state.NT.मुख्य = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      अन्यथा
        वापस CURLE_FTP_BAD_FILE_LIST;

      अवरोध;
    }
    अवरोध;
  }

  वापस CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(काष्ठा Curl_easy *तथ्य)
{
  अगर(तथ्य->state.wildcardmatch) {
    काष्ठा Wildcard_तथ्य * स्थिर wildcard = तथ्य->wildcard;
    वापस wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  वापस FALSE;
}

माप_प्रकार Curl_ftp_parselist(अक्षर *बफर, माप_प्रकार आकार, माप_प्रकार nmemb,
                          व्योम *connptr)
{
  माप_प्रकार bufflen = आकार * nmemb;
  काष्ठा Curl_easy *तथ्य = (काष्ठा Curl_easy *)connptr;
  काष्ठा ftp_wc *ftpwc = तथ्य->wildcard->ftpwc;
  काष्ठा ftp_parselist_तथ्य *parser = ftpwc->parser;
  माप_प्रकार i = 0;
  CURLcode परिणाम;
  माप_प्रकार retsize = bufflen;

  अगर(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    जाओ fail;
  }

  अगर(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(बफर[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  जबतक(i < bufflen) { /* FSM */
    अक्षर c = बफर[i];
    काष्ठा fileinfo *infop;
    अगर(!parser->file_तथ्य) { /* tmp file data is not allocated yet */
      parser->file_तथ्य = Curl_fileinfo_alloc();
      अगर(!parser->file_तथ्य) {
        parser->error = CURLE_OUT_OF_MEMORY;
        जाओ fail;
      }
      parser->item_offset = 0;
      parser->item_लंबाई = 0;
      curlx_dyn_init(&parser->file_तथ्य->buf, MAX_FTPLIST_बफर);
    }

    infop = parser->file_तथ्य;

    अगर(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_OUT_OF_MEMORY;
      जाओ fail;
    }

    चयन(parser->os_type) {
    हाल OS_TYPE_UNIX:
      परिणाम = parse_unix(तथ्य, parser, infop, c);
      अवरोध;
    हाल OS_TYPE_WIN_NT:
      परिणाम = parse_winnt(तथ्य, parser, infop, c);
      अवरोध;
    शेष:
      retsize = bufflen + 1;
      जाओ fail;
    }
    अगर(परिणाम) {
      parser->error = परिणाम;
      जाओ fail;
    }

    i++;
  }
  वापस retsize;

fail:

  /* Clean up any allocated memory. */
  अगर(parser->file_तथ्य) {
    Curl_fileinfo_cleanup(parser->file_तथ्य);
    parser->file_तथ्य = शून्य;
  }

  वापस retsize;
}

#पूर्ण_अगर /* !CURL_DISABLE_FTP */
