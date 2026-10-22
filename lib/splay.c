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

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "splay.h"

स्थिर संरचना Curl_रूख *splay(timediff_t साँचो,
                               संरचना Curl_रूख *जरा);

#ifdef UNITTESTS
uint32_t Curl_splayget(संरचना Curl_रूख *गाँठो)
{
  DEBUGASSERT(गाँठो);
  फर्काऊ गाँठो->id;
}

#endif

शून्य Curl_timeouts_init(संरचना Curl_timeouts *timeouts,
                        अचर संरचना curltime *ptime_base)
{
  संरचना curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->रूख = रिक्त;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(संरचना Curl_easy *data)
{
  संरचना Curl_रूख *गाँठो = data ? &data->state.timeouts.splaynode : रिक्त;
  फर्काऊ गाँठो && गाँठो->registered;
}

timediff_t Curl_timeouts_offset_us(संरचना Curl_timeouts *timeouts,
                                   अचर संरचना curltime *pts)
{
  संरचना curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  फर्काऊ curlx_ptimediff_us(pts, &time_base);
}

पूर्णाङ्क Curl_timeouts_next_ms(संरचना Curl_timeouts *timeouts,
                          अचर संरचना curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  यदि(timeouts->रूख) { /* splay the lowest key to the root */
    timeouts->रूख = splay(TIMEDIFF_T_MIN, timeouts->रूख);
  }

  यदि(timeouts->रूख) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->रूख->साँचो - elapsed_us;
    यदि(pmid)
      *pmid = timeouts->रूख->id;
    यदि(pexpire_offset_us)
      *pexpire_offset_us = timeouts->रूख->साँचो;
    यदि(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      फर्काऊ (ms > पूर्णांक_उच्च) ? पूर्णांक_उच्च : (पूर्णाङ्क)ms;
    }
    अन्यथा /* has expired */
      फर्काऊ 0;
  }
  यदि(pmid)
    *pmid = UINT32_MAX;
  यदि(pexpire_offset_us)
    *pexpire_offset_us = 0;
  फर्काऊ -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
संरचना Curl_रूख *splay(timediff_t साँचो,
                             संरचना Curl_रूख *जरा)
{
  संरचना Curl_रूख N, *l, *r, *y;

  यदि(!जरा)
    फर्काऊ रिक्त;
  N.smaller = N.larger = रिक्त;
  l = r = &N;

  लागि(;;) {
    यदि(साँचो < जरा->साँचो) {
      /* key is somewhere in root->smaller branch */
      यदि(!जरा->smaller)  /* which is empty, done */
        रोक;
      यदि(साँचो < जरा->smaller->साँचो) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = जरा->smaller;
        जरा->smaller = y->larger;
        y->larger = जरा;
        जरा = y;
        यदि(!जरा->smaller)
          रोक;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = जरा;
      r = जरा;
      जरा = जरा->smaller;
    }
    अन्यथा यदि(साँचो > जरा->साँचो) {
      /* key is somewhere in root->larger branch */
      यदि(!जरा->larger)  /* which is empty, done */
        रोक;
      यदि(साँचो > जरा->larger->साँचो) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = जरा->larger;
        जरा->larger = y->smaller;
        y->smaller = जरा;
        जरा = y;
        यदि(!जरा->larger)
          रोक;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = जरा;
      l = जरा;
      जरा = जरा->larger;
    }
    अन्यथा  /* exact match, root is key, done */
      रोक;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = जरा->smaller;
  r->smaller = जरा->larger;
  जरा->smaller = N.larger;
  जरा->larger = N.smaller;

  फर्काऊ जरा;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST संरचना Curl_रूख *splayinsert(timediff_t साँचो,
                                       संरचना Curl_रूख *जरा,
                                       संरचना Curl_रूख *गाँठो,
                                       uint32_t id);
UNITTEST संरचना Curl_रूख *splayinsert(timediff_t साँचो,
                                       संरचना Curl_रूख *जरा,
                                       संरचना Curl_रूख *गाँठो,
                                       uint32_t id)
{
  DEBUGASSERT(गाँठो);

  गाँठो->साँचो = साँचो;
  गाँठो->id = id;
  गाँठो->same = रिक्त;
  गाँठो->registered = TRUE;
  यदि(जरा) {
    जरा = splay(साँचो, जरा);
    DEBUGASSERT(जरा);
    यदि(साँचो == जरा->साँचो) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      संरचना Curl_रूख **panchor = &जरा->same;
      जबसम्म(*panchor)
        panchor = &(*panchor)->same;
      *panchor = गाँठो;
      फर्काऊ जरा; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  यदि(!जरा) {
    गाँठो->smaller = गाँठो->larger = रिक्त;
  }
  अन्यथा यदि(साँचो < जरा->साँचो) {
    गाँठो->smaller = जरा->smaller;
    गाँठो->larger = जरा;
    जरा->smaller = रिक्त;
  }
  अन्यथा {
    गाँठो->larger = जरा->larger;
    गाँठो->smaller = जरा;
    जरा->larger = रिक्त;
  }

  फर्काऊ गाँठो;
}

शून्य Curl_timeouts_add(संरचना Curl_timeouts *timeouts,
                       संरचना Curl_easy *data,
                       timediff_t offset_us)
{
  संरचना Curl_रूख *गाँठो = &data->state.timeouts.splaynode;
  DEBUGASSERT(!गाँठो->registered);
  timeouts->रूख = splayinsert(offset_us, timeouts->रूख,
                               गाँठो, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST संरचना Curl_रूख *splaygetbest(timediff_t साँचो,
                                        संरचना Curl_रूख *जरा,
                                        संरचना Curl_रूख **removed);
UNITTEST संरचना Curl_रूख *splaygetbest(timediff_t साँचो,
                                        संरचना Curl_रूख *जरा,
                                        संरचना Curl_रूख **removed)
{
  संरचना Curl_रूख *x;

  यदि(!जरा) {
    *removed = रिक्त; /* none removed since there was no root */
    फर्काऊ रिक्त;
  }

  /* find smallest */
  जरा = splay(TIMEDIFF_T_MIN, जरा);
  DEBUGASSERT(जरा);
  यदि(साँचो < जरा->साँचो) {
    /* even the smallest is too big */
    *removed = रिक्त;
    फर्काऊ जरा;
  }

  /* FIRST! Check if there is a list with identical keys */
  यदि(जरा->same) {
    x = जरा->same;
    DEBUGASSERT(x->साँचो == जरा->साँचो);
    /* 'x' becomes the new root node */
    x->larger = जरा->larger;
    x->smaller = जरा->smaller;
    जरा->same = रिक्त;
    जरा->registered = FALSE;
    *removed = जरा;
    फर्काऊ x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = जरा->larger;
  जरा->registered = FALSE;
  *removed = जरा;

  फर्काऊ x;
}

bool Curl_timeouts_remove_expired(संरचना Curl_timeouts *timeouts,
                                  अचर संरचना curltime *ts,
                                  uint32_t *pmid)
{
  यदि(timeouts->रूख) {
    संरचना Curl_रूख *t = रिक्त;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->रूख = splaygetbest(elapsed_us, timeouts->रूख, &t);
    यदि(t) {
      *pmid = t->id;
      फर्काऊ TRUE;
    }
  }
  *pmid = UINT32_MAX;
  फर्काऊ FALSE;
}

/* Deletes the node we point out from the tree if it is there. Stores a
 * pointer to the new resulting tree in 'newroot'.
 *
 * Returns zero on success and non-zero on errors!
 * When returning error, it does not touch the 'newroot' pointer.
 *
 * NOTE: when the last node of the tree is removed, there is no tree left so
 * 'newroot' will be made to point to NULL.
 *
 * @unittest 1309
 */
UNITTEST पूर्णाङ्क splayremove(संरचना Curl_रूख *जरा,
                         संरचना Curl_रूख *removenode,
                         संरचना Curl_रूख **newroot);
UNITTEST पूर्णाङ्क splayremove(संरचना Curl_रूख *जरा,
                         संरचना Curl_रूख *removenode,
                         संरचना Curl_रूख **newroot)
{
  संरचना Curl_रूख *x;

  यदि(!जरा)
    फर्काऊ 1;

  DEBUGASSERT(removenode);
  यदि(!removenode->registered)
    फर्काऊ 2;

  जरा = splay(removenode->साँचो, जरा);
  DEBUGASSERT(जरा);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  यदि(जरा->साँचो != removenode->साँचो) {
    DEBUGASSERT(0);
    फर्काऊ 2;
  }

  यदि(जरा != removenode) {
    /* Should be in the root->same list then */
    संरचना Curl_रूख **panchor;
    लागि(panchor = &जरा->same; *panchor; panchor = &(*panchor)->same) {
      यदि(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = रिक्त;
        removenode->registered = FALSE;
        *newroot = जरा;
        फर्काऊ 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    फर्काऊ 2;
  }
  /* removing the root node */
  यदि(जरा->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = जरा->same;
    x->larger = जरा->larger;
    x->smaller = जरा->smaller;
    जरा->same = रिक्त;
  }
  अन्यथा {
    /* Remove the root node */
    यदि(!जरा->smaller)
      x = जरा->larger;
    अन्यथा {
      x = splay(removenode->साँचो, जरा->smaller);
      DEBUGASSERT(x);
      x->larger = जरा->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  फर्काऊ 0;
}

bool Curl_timeouts_remove(संरचना Curl_timeouts *timeouts,
                          संरचना Curl_easy *data)
{
  संरचना Curl_रूख *गाँठो = &data->state.timeouts.splaynode;
  यदि(गाँठो->registered) {
    पूर्णाङ्क rc = splayremove(timeouts->रूख, गाँठो, &timeouts->रूख);
#ifdef DEBUGBUILD
    यदि(rc)
      curl_mfprintf(मानक_त्रुटि, "Internal error removing splay node = %d\n", rc);
#अन्यथा
    (शून्य)rc;
#endif
    फर्काऊ TRUE;
  }
  फर्काऊ FALSE;
}
