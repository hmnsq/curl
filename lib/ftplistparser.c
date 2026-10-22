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

#यदि_अपरिभाषितम् CURL_DISABLE_FTP

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
  PL_UNIX_गणः,
  PL_UNIX_आकार,
  PL_UNIX_TIME,
  PL_UNIX_FILENAME,
  PL_UNIX_SYMLINK
} pl_unix_mainstate;

प्रकारनिर्देश सङ्घ {
  गणना {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } योग_dirsize;

  गणना {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_सङ्ख्या
  } hlinks;

  गणना {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  गणना {
    PL_UNIX_गणः_PRESPACE = 0,
    PL_UNIX_गणः_NAME
  } गणः;

  गणना {
    PL_UNIX_आकार_PRESPACE = 0,
    PL_UNIX_आकार_सङ्ख्या
  } आकार;

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
संरचना ftp_parselist_दत्त {
  गणना {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  सङ्घ {
    संरचना {
      pl_unix_mainstate मुख्यम्;
      pl_unix_substate sub;
    } UNIX;

    संरचना {
      pl_winNT_mainstate मुख्यम्;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  संरचना fileinfo *file_दत्त;
  अचिह्नित पूर्णाङ्क item_दीर्घता;
  size_t item_विस्थापनम्;
  संरचना {
    size_t filename;
    size_t user;
    size_t गणः;
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

शून्य Curl_wildcard_init(संरचना Wildcard_दत्त *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

शून्य Curl_wildcard_dtor(संरचना Wildcard_दत्त **wcp)
{
  संरचना Wildcard_दत्त *wc = *wcp;
  यदि(!wc)
    प्रत्यागम;

  यदि(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = रिक्त;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, रिक्त);
  curlx_safefree(wc->मार्गः);
  curlx_safefree(wc->आदर्शः);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = रिक्त;
}

संरचना ftp_parselist_दत्त *Curl_ftp_parselist_दत्त_alloc(शून्य)
{
  प्रत्यागम curlx_calloc(1, परिमाण(संरचना ftp_parselist_दत्त));
}

शून्य Curl_ftp_parselist_दत्त_free(संरचना ftp_parselist_दत्त **parserp)
{
  संरचना ftp_parselist_दत्त *parser = *parserp;
  यदि(parser)
    Curl_fileinfo_cleanup(parser->file_दत्त);
  curlx_free(parser);
  *parserp = रिक्त;
}

CURLcode Curl_ftp_parselist_geterror(संरचना ftp_parselist_दत्त *pl_दत्त)
{
  प्रत्यागम pl_दत्त->error;
}

#परिभाषय FTP_LP_MALFORMATED_PERM 0x01000000

स्थिर अचिह्नित पूर्णाङ्क ftp_pl_get_permission(ध्रुव वर्ण *str)
{
  अचिह्नित पूर्णाङ्क permissions = 0;
  /* USER */
  यदि(str[0] == 'r')
    permissions |= 1 << 8;
  अन्यथा यदि(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  यदि(str[1] == 'w')
    permissions |= 1 << 7;
  अन्यथा यदि(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  यदि(str[2] == 'x')
    permissions |= 1 << 6;
  अन्यथा यदि(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  अन्यथा यदि(str[2] == 'S')
    permissions |= 1 << 11;
  अन्यथा यदि(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /*गणः*/
  यदि(str[3] == 'r')
    permissions |= 1 << 5;
  अन्यथा यदि(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  यदि(str[4] == 'w')
    permissions |= 1 << 4;
  अन्यथा यदि(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  यदि(str[5] == 'x')
    permissions |= 1 << 3;
  अन्यथा यदि(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  अन्यथा यदि(str[5] == 'S')
    permissions |= 1 << 10;
  अन्यथा यदि(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  यदि(str[6] == 'r')
    permissions |= 1 << 2;
  अन्यथा यदि(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  यदि(str[7] == 'w')
    permissions |= 1 << 1;
  अन्यथा यदि(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  यदि(str[8] == 'x')
    permissions |= 1;
  अन्यथा यदि(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  अन्यथा यदि(str[8] == 'T')
    permissions |= 1 << 9;
  अन्यथा यदि(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  प्रत्यागम permissions;
}

स्थिर CURLcode ftp_pl_निवेशय_finfo(संरचना Curl_easy *दत्त,
                                    संरचना fileinfo *infop)
{
  curl_fnmatch_callback compare;
  संरचना Wildcard_दत्त *wc = दत्त->wildcard;
  संरचना ftp_wc *ftpwc = wc->ftpwc;
  संरचना Curl_llist *llist = &wc->filelist;
  संरचना ftp_parselist_दत्त *parser = ftpwc->parser;
  bool सङ्कलय = TRUE;
  संरचना curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  वर्ण *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->वाक्यानि.गणः  = parser->offsets.गणः ?
                          str + parser->offsets.गणः : रिक्त;
  finfo->वाक्यानि.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : रिक्त;
  finfo->वाक्यानि.target = parser->offsets.symlink_target &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_target : रिक्त;
  finfo->वाक्यानि.time   = str + parser->offsets.time;
  finfo->वाक्यानि.user   = parser->offsets.user ?
                          str + parser->offsets.user : रिक्त;

  /* get correct fnmatch callback */
  compare = दत्त->समुच्चयः.fnmatch;
  यदि(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  यदि(strchr(finfo->filename, '/'))
    सङ्कलय = FALSE;
  अन्यथा {
    /* a real file name cannot contain a slash */
    संरचना Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, दत्त, easy_fnmatch_दत्त);
    यदि(compare(दत्त->समुच्चयः.fnmatch_दत्त, wc->आदर्शः, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      यदि((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->वाक्यानि.target &&
         वाक्ये_अन्विष(finfo->वाक्यानि.target, " -> ")) {
        सङ्कलय = FALSE;
      }
    }
    अन्यथा {
      सङ्कलय = FALSE;
    }
    CURL_CBAPI_अन्तः(&guard);
  }

  यदि(सङ्कलय) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  अन्यथा {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_दत्त = रिक्त;
  प्रत्यागम CURLE_OK;
}

#परिभाषय MAX_FTPLIST_बफर 10000 /* arbitrarily set */

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
    *t = CURLFILETYPE_DEVICE_खण्डम्;
    भङ्ग;
  पक्ष 'D':
    *t = CURLFILETYPE_DOOR;
    भङ्ग;
  स्वभाव:
    प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_totalsize(संरचना ftp_parselist_दत्त *parser,
                                     संरचना fileinfo *infop,
                                     ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  विकल्प(parser->state.UNIX.sub.योग_dirsize) {
  पक्ष PL_UNIX_TOTALSIZE_INIT:
    यदि(c == 't') {
      parser->state.UNIX.sub.योग_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_दीर्घता++;
    }
    अन्यथा {
      parser->state.UNIX.मुख्यम् = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    भङ्ग;
  पक्ष PL_UNIX_TOTALSIZE_READING:
    parser->item_दीर्घता++;
    यदि(c == '\r') {
      parser->item_दीर्घता--;
      यदि(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    अन्यथा यदि(c == '\n') {
      mem[parser->item_दीर्घता - 1] = 0;
      यदि(!वाक्ये_तुलय("total ", mem, 6)) {
        ध्रुव वर्ण *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        यावत्(ISDIGIT(*endptr))
          endptr++;
        यदि(*endptr) {
          प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.मुख्यम् = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      अन्यथा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    }
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_permission(संरचना ftp_parselist_दत्त *parser,
                                      संरचना fileinfo *infop,
                                      ध्रुव वर्ण c)
{
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_दीर्घता++;
  यदि((parser->item_दीर्घता <= 9) && !strchr("rwx-tTsS", c))
    प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

  अन्यथा यदि(parser->item_दीर्घता == 10) {
    अचिह्नित पूर्णाङ्क perm;
    यदि(c != ' ')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_विस्थापनम्);
    यदि(perm & FTP_LP_MALFORMATED_PERM)
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

    parser->file_दत्त->info.flags |= CURLFINFOFLAG_ज्ञातम्_PERM;
    parser->file_दत्त->info.perm = perm;
    parser->offsets.perm = parser->item_विस्थापनम्;

    parser->item_दीर्घता = 0;
    parser->state.UNIX.मुख्यम् = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_hlinks(संरचना ftp_parselist_दत्त *parser,
                                  संरचना fileinfo *infop,
                                  ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);

  विकल्प(parser->state.UNIX.sub.hlinks) {
  पक्ष PL_UNIX_HLINKS_PRESPACE:
    यदि(c != ' ') {
      यदि(ISDIGIT(c) && len) {
        parser->item_विस्थापनम् = len - 1;
        parser->item_दीर्घता = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_सङ्ख्या;
      }
      अन्यथा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    }
    भङ्ग;
  पक्ष PL_UNIX_HLINKS_सङ्ख्या:
    parser->item_दीर्घता++;
    यदि(c == ' ') {
      ध्रुव वर्ण *p = &mem[parser->item_विस्थापनम्];
      curl_off_t hlinks;
      mem[parser->item_विस्थापनम् + parser->item_दीर्घता - 1] = 0;

      यदि(!curlx_str_सङ्ख्या(&p, &hlinks, LONG_MAX)) {
        parser->file_दत्त->info.flags |= CURLFINFOFLAG_ज्ञातम्_HLINKCOUNT;
        parser->file_दत्त->info.hardlinks = (दीर्घ)hlinks;
      }
      parser->item_दीर्घता = 0;
      parser->item_विस्थापनम् = 0;
      parser->state.UNIX.मुख्यम् = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    अन्यथा यदि(!ISDIGIT(c))
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_user(संरचना ftp_parselist_दत्त *parser,
                                संरचना fileinfo *infop,
                                ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  विकल्प(parser->state.UNIX.sub.user) {
  पक्ष PL_UNIX_USER_PRESPACE:
    यदि(c != ' ' && len) {
      parser->item_विस्थापनम् = len - 1;
      parser->item_दीर्घता = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    भङ्ग;
  पक्ष PL_UNIX_USER_PARSING:
    parser->item_दीर्घता++;
    यदि(c == ' ') {
      mem[parser->item_विस्थापनम् + parser->item_दीर्घता - 1] = 0;
      parser->offsets.user = parser->item_विस्थापनम्;
      parser->state.UNIX.मुख्यम् = PL_UNIX_गणः;
      parser->state.UNIX.sub.गणः = PL_UNIX_गणः_PRESPACE;
      parser->item_विस्थापनम् = 0;
      parser->item_दीर्घता = 0;
    }
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_गणः(संरचना ftp_parselist_दत्त *parser,
                                 संरचना fileinfo *infop,
                                 ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  विकल्प(parser->state.UNIX.sub.गणः) {
  पक्ष PL_UNIX_गणः_PRESPACE:
    यदि(c != ' ' && len) {
      parser->item_विस्थापनम् = len - 1;
      parser->item_दीर्घता = 1;
      parser->state.UNIX.sub.गणः = PL_UNIX_गणः_NAME;
    }
    भङ्ग;
  पक्ष PL_UNIX_गणः_NAME:
    parser->item_दीर्घता++;
    यदि(c == ' ') {
      mem[parser->item_विस्थापनम् + parser->item_दीर्घता - 1] = 0;
      parser->offsets.गणः = parser->item_विस्थापनम्;
      parser->state.UNIX.मुख्यम् = PL_UNIX_आकार;
      parser->state.UNIX.sub.आकार = PL_UNIX_आकार_PRESPACE;
      parser->item_विस्थापनम् = 0;
      parser->item_दीर्घता = 0;
    }
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_आकार(संरचना ftp_parselist_दत्त *parser,
                                संरचना fileinfo *infop,
                                ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  विकल्प(parser->state.UNIX.sub.आकार) {
  पक्ष PL_UNIX_आकार_PRESPACE:
    यदि(c != ' ') {
      यदि(ISDIGIT(c) && len) {
        parser->item_विस्थापनम् = len - 1;
        parser->item_दीर्घता = 1;
        parser->state.UNIX.sub.आकार = PL_UNIX_आकार_सङ्ख्या;
      }
      अन्यथा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    }
    भङ्ग;
  पक्ष PL_UNIX_आकार_सङ्ख्या:
    parser->item_दीर्घता++;
    यदि(c == ' ') {
      ध्रुव वर्ण *p = mem + parser->item_विस्थापनम्;
      curl_off_t fsize;
      mem[parser->item_विस्थापनम् + parser->item_दीर्घता - 1] = 0;
      यदि(!curlx_str_numblanks(&p, &fsize)) {
        यदि(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_दत्त->info.flags |= CURLFINFOFLAG_ज्ञातम्_आकार;
          parser->file_दत्त->info.आकार = fsize;
        }
        parser->item_दीर्घता = 0;
        parser->item_विस्थापनम् = 0;
        parser->state.UNIX.मुख्यम् = PL_UNIX_TIME;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART1;
      }
    }
    अन्यथा यदि(!ISDIGIT(c))
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_time(संरचना ftp_parselist_दत्त *parser,
                                संरचना fileinfo *infop,
                                ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  संरचना curl_fileinfo *finfo = &infop->info;

  विकल्प(parser->state.UNIX.sub.time) {
  पक्ष PL_UNIX_TIME_PREPART1:
    यदि(c != ' ') {
      यदि(ISALNUM(c) && len) {
        parser->item_विस्थापनम् = len - 1;
        parser->item_दीर्घता = 1;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART1;
      }
      अन्यथा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    }
    भङ्ग;
  पक्ष PL_UNIX_TIME_PART1:
    parser->item_दीर्घता++;
    यदि(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART2;

    अन्यथा यदि(!ISALNUM(c) && c != '.')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

    भङ्ग;
  पक्ष PL_UNIX_TIME_PREPART2:
    parser->item_दीर्घता++;
    यदि(c != ' ') {
      यदि(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART2;
      अन्यथा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    }
    भङ्ग;
  पक्ष PL_UNIX_TIME_PART2:
    parser->item_दीर्घता++;
    यदि(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART3;
    अन्यथा यदि(!ISALNUM(c) && c != '.')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    भङ्ग;
  पक्ष PL_UNIX_TIME_PREPART3:
    parser->item_दीर्घता++;
    यदि(c != ' ') {
      यदि(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART3;
      अन्यथा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    }
    भङ्ग;
  पक्ष PL_UNIX_TIME_PART3:
    parser->item_दीर्घता++;
    यदि(c == ' ') {
      mem[parser->item_विस्थापनम् + parser->item_दीर्घता - 1] = 0;
      parser->offsets.time = parser->item_विस्थापनम्;
      यदि(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.मुख्यम् = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      अन्यथा {
        parser->state.UNIX.मुख्यम् = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    अन्यथा यदि(!ISALNUM(c) && c != '.' && c != ':')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode parse_unix_filename(संरचना Curl_easy *दत्त,
                                    संरचना ftp_parselist_दत्त *parser,
                                    संरचना fileinfo *infop,
                                    ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode परिणाम = CURLE_OK;

  विकल्प(parser->state.UNIX.sub.filename) {
  पक्ष PL_UNIX_FILENAME_PRESPACE:
    यदि(c != ' ' && len) {
      parser->item_विस्थापनम् = len - 1;
      parser->item_दीर्घता = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    भङ्ग;
  पक्ष PL_UNIX_FILENAME_NAME:
    parser->item_दीर्घता++;
    यदि(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    अन्यथा यदि(c == '\n') {
      mem[parser->item_विस्थापनम् + parser->item_दीर्घता - 1] = 0;
      parser->offsets.filename = parser->item_विस्थापनम्;
      parser->state.UNIX.मुख्यम् = PL_UNIX_FILETYPE;
      परिणाम = ftp_pl_निवेशय_finfo(दत्त, infop);
    }
    भङ्ग;
  पक्ष PL_UNIX_FILENAME_WINDOWSEOL:
    यदि(c == '\n') {
      mem[parser->item_विस्थापनम् + parser->item_दीर्घता - 1] = 0;
      parser->offsets.filename = parser->item_विस्थापनम्;
      parser->state.UNIX.मुख्यम् = PL_UNIX_FILETYPE;
      परिणाम = ftp_pl_निवेशय_finfo(दत्त, infop);
    }
    अन्यथा
      परिणाम = CURLE_FTP_BAD_FILE_LIST;
    भङ्ग;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode parse_unix_symlink(संरचना Curl_easy *दत्त,
                                   संरचना ftp_parselist_दत्त *parser,
                                   संरचना fileinfo *infop,
                                   ध्रुव वर्ण c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode परिणाम = CURLE_OK;

  विकल्प(parser->state.UNIX.sub.symlink) {
  पक्ष PL_UNIX_SYMLINK_PRESPACE:
    यदि(c != ' ' && len) {
      parser->item_विस्थापनम् = len - 1;
      parser->item_दीर्घता = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    भङ्ग;
  पक्ष PL_UNIX_SYMLINK_NAME:
    parser->item_दीर्घता++;
    यदि(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    अन्यथा यदि(c == '\r' || c == '\n')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

    भङ्ग;
  पक्ष PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_दीर्घता++;
    यदि(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    अन्यथा यदि(c == '\r' || c == '\n')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    अन्यथा
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    भङ्ग;
  पक्ष PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_दीर्घता++;
    यदि(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    अन्यथा यदि(c == '\r' || c == '\n')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    अन्यथा
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    भङ्ग;
  पक्ष PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_दीर्घता++;
    यदि(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_विस्थापनम् + parser->item_दीर्घता - 4] = 0;
      parser->offsets.filename = parser->item_विस्थापनम्;
      parser->item_दीर्घता = 0;
      parser->item_विस्थापनम् = 0;
    }
    अन्यथा यदि(c == '\r' || c == '\n')
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    अन्यथा
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    भङ्ग;
  पक्ष PL_UNIX_SYMLINK_PRETARGET4:
    यदि(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_विस्थापनम् = len - 1;
      parser->item_दीर्घता = 1;
    }
    अन्यथा
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

    भङ्ग;
  पक्ष PL_UNIX_SYMLINK_TARGET:
    parser->item_दीर्घता++;
    यदि(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    अन्यथा यदि(c == '\n') {
      mem[parser->item_विस्थापनम् + parser->item_दीर्घता - 1] = 0;
      parser->offsets.symlink_target = parser->item_विस्थापनम्;
      परिणाम = ftp_pl_निवेशय_finfo(दत्त, infop);
      यदि(परिणाम)
        भङ्ग;

      parser->state.UNIX.मुख्यम् = PL_UNIX_FILETYPE;
    }
    भङ्ग;
  पक्ष PL_UNIX_SYMLINK_WINDOWSEOL:
    यदि(c == '\n') {
      mem[parser->item_विस्थापनम् + parser->item_दीर्घता - 1] = 0;
      parser->offsets.symlink_target = parser->item_विस्थापनम्;
      परिणाम = ftp_pl_निवेशय_finfo(दत्त, infop);
      यदि(परिणाम)
        भङ्ग;

      parser->state.UNIX.मुख्यम् = PL_UNIX_FILETYPE;
    }
    अन्यथा
      परिणाम = CURLE_FTP_BAD_FILE_LIST;

    भङ्ग;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode parse_unix(संरचना Curl_easy *दत्त,
                           संरचना ftp_parselist_दत्त *parser,
                           संरचना fileinfo *infop,
                           ध्रुव वर्ण c)
{
  संरचना curl_fileinfo *finfo = &infop->info;
  CURLcode परिणाम = CURLE_OK;

  विकल्प(parser->state.UNIX.मुख्यम्) {
  पक्ष PL_UNIX_TOTALSIZE:
    परिणाम = parse_unix_totalsize(parser, infop, c);
    यदि(परिणाम)
      भङ्ग;
    यदि(parser->state.UNIX.मुख्यम् != PL_UNIX_FILETYPE)
      भङ्ग;
    FALLTHROUGH();
  पक्ष PL_UNIX_FILETYPE:
    परिणाम = unix_filetype(c, &finfo->filetype);
    यदि(!परिणाम) {
      parser->state.UNIX.मुख्यम् = PL_UNIX_PERMISSION;
      parser->item_दीर्घता = 0;
      parser->item_विस्थापनम् = 1;
    }
    भङ्ग;
  पक्ष PL_UNIX_PERMISSION:
    परिणाम = parse_unix_permission(parser, infop, c);
    भङ्ग;
  पक्ष PL_UNIX_HLINKS:
    परिणाम = parse_unix_hlinks(parser, infop, c);
    भङ्ग;
  पक्ष PL_UNIX_USER:
    परिणाम = parse_unix_user(parser, infop, c);
    भङ्ग;
  पक्ष PL_UNIX_गणः:
    परिणाम = parse_unix_गणः(parser, infop, c);
    भङ्ग;
  पक्ष PL_UNIX_आकार:
    परिणाम = parse_unix_आकार(parser, infop, c);
    भङ्ग;
  पक्ष PL_UNIX_TIME:
    परिणाम = parse_unix_time(parser, infop, c);
    भङ्ग;
  पक्ष PL_UNIX_FILENAME:
    परिणाम = parse_unix_filename(दत्त, parser, infop, c);
    भङ्ग;
  पक्ष PL_UNIX_SYMLINK:
    परिणाम = parse_unix_symlink(दत्त, parser, infop, c);
    भङ्ग;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode parse_winnt(संरचना Curl_easy *दत्त,
                            संरचना ftp_parselist_दत्त *parser,
                            संरचना fileinfo *infop,
                            ध्रुव वर्ण c)
{
  संरचना curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  वर्ण *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode परिणाम = CURLE_OK;

  विकल्प(parser->state.NT.मुख्यम्) {
  पक्ष PL_WINNT_DATE:
    parser->item_दीर्घता++;
    यदि(parser->item_दीर्घता < 9) {
      यदि(!strchr("0123456789-", c)) { /* only simple control */
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
      }
    }
    अन्यथा यदि(parser->item_दीर्घता == 9) {
      यदि(c == ' ') {
        parser->state.NT.मुख्यम् = PL_WINNT_TIME;
        parser->state.NT.sub.time = PL_WINNT_TIME_PRESPACE;
      }
      अन्यथा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    }
    अन्यथा
      प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
    भङ्ग;
  पक्ष PL_WINNT_TIME:
    parser->item_दीर्घता++;
    विकल्प(parser->state.NT.sub.time) {
    पक्ष PL_WINNT_TIME_PRESPACE:
      यदि(!ISBLANK(c))
        parser->state.NT.sub.time = PL_WINNT_TIME_TIME;
      भङ्ग;
    पक्ष PL_WINNT_TIME_TIME:
      यदि(c == ' ') {
        parser->offsets.time = parser->item_विस्थापनम्;
        mem[parser->item_विस्थापनम् + parser->item_दीर्घता - 1] = 0;
        parser->state.NT.मुख्यम् = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_दीर्घता = 0;
      }
      अन्यथा यदि(!strchr("APM0123456789:", c))
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
      भङ्ग;
    }
    भङ्ग;
  पक्ष PL_WINNT_DIRORSIZE:
    विकल्प(parser->state.NT.sub.dirorsize) {
    पक्ष PL_WINNT_DIRORSIZE_PRESPACE:
      यदि(c != ' ' && len) {
        parser->item_विस्थापनम् = len - 1;
        parser->item_दीर्घता = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      भङ्ग;
    पक्ष PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_दीर्घता++;
      यदि(c == ' ') {
        mem[parser->item_विस्थापनम् + parser->item_दीर्घता - 1] = 0;
        यदि(!वाक्यं_तुलय("<DIR>", mem + parser->item_विस्थापनम्)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->आकार = 0;
        }
        अन्यथा {
          ध्रुव वर्ण *p = mem + parser->item_विस्थापनम्;
          यदि(curlx_str_numblanks(&p, &finfo->आकार)) {
            प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_दत्त->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_दत्त->info.flags |= CURLFINFOFLAG_ज्ञातम्_आकार;
        parser->item_दीर्घता = 0;
        parser->state.NT.मुख्यम् = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      भङ्ग;
    }
    भङ्ग;
  पक्ष PL_WINNT_FILENAME:
    विकल्प(parser->state.NT.sub.filename) {
    पक्ष PL_WINNT_FILENAME_PRESPACE:
      यदि(c != ' ' && len) {
        parser->item_विस्थापनम् = len - 1;
        parser->item_दीर्घता = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      भङ्ग;
    पक्ष PL_WINNT_FILENAME_CONTENT:
      parser->item_दीर्घता++;
      यदि(!len)
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;
      यदि(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      अन्यथा यदि(c == '\n') {
        parser->offsets.filename = parser->item_विस्थापनम्;
        mem[len - 1] = 0;
        परिणाम = ftp_pl_निवेशय_finfo(दत्त, infop);
        यदि(परिणाम)
          प्रत्यागम परिणाम;

        parser->state.NT.मुख्यम् = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      भङ्ग;
    पक्ष PL_WINNT_FILENAME_WINEOL:
      यदि(c == '\n') {
        parser->offsets.filename = parser->item_विस्थापनम्;
        परिणाम = ftp_pl_निवेशय_finfo(दत्त, infop);
        यदि(परिणाम)
          प्रत्यागम परिणाम;

        parser->state.NT.मुख्यम् = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      अन्यथा
        प्रत्यागम CURLE_FTP_BAD_FILE_LIST;

      भङ्ग;
    }
    भङ्ग;
  }

  प्रत्यागम CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(संरचना Curl_easy *दत्त)
{
  यदि(दत्त->state.wildcardmatch) {
    संरचना Wildcard_दत्त * ध्रुव wildcard = दत्त->wildcard;
    प्रत्यागम wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  प्रत्यागम FALSE;
}

size_t Curl_ftp_parselist(वर्ण *बफर, size_t आकार, size_t nmemb,
                          शून्य *connptr)
{
  size_t bufflen = आकार * nmemb;
  संरचना Curl_easy *दत्त = (संरचना Curl_easy *)connptr;
  संरचना ftp_wc *ftpwc = दत्त->wildcard->ftpwc;
  संरचना ftp_parselist_दत्त *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode परिणाम;
  size_t retsize = bufflen;

  यदि(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    गच्छ fail;
  }

  यदि(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(बफर[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  यावत्(i < bufflen) { /* FSM */
    वर्ण c = बफर[i];
    संरचना fileinfo *infop;
    यदि(!parser->file_दत्त) { /* tmp file data is not allocated yet */
      parser->file_दत्त = Curl_fileinfo_alloc();
      यदि(!parser->file_दत्त) {
        parser->error = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ fail;
      }
      parser->item_विस्थापनम् = 0;
      parser->item_दीर्घता = 0;
      curlx_dyn_init(&parser->file_दत्त->buf, MAX_FTPLIST_बफर);
    }

    infop = parser->file_दत्त;

    यदि(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ fail;
    }

    विकल्प(parser->os_type) {
    पक्ष OS_TYPE_UNIX:
      परिणाम = parse_unix(दत्त, parser, infop, c);
      भङ्ग;
    पक्ष OS_TYPE_WIN_NT:
      परिणाम = parse_winnt(दत्त, parser, infop, c);
      भङ्ग;
    स्वभाव:
      retsize = bufflen + 1;
      गच्छ fail;
    }
    यदि(परिणाम) {
      parser->error = परिणाम;
      गच्छ fail;
    }

    i++;
  }
  प्रत्यागम retsize;

fail:

  /* Clean up any allocated memory. */
  यदि(parser->file_दत्त) {
    Curl_fileinfo_cleanup(parser->file_दत्त);
    parser->file_दत्त = रिक्त;
  }

  प्रत्यागम retsize;
}

#यदि_अन्तः /* !CURL_DISABLE_FTP */
