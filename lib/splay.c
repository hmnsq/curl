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

स्थिर संरचना Curl_वृक्षः *splay(timediff_t कुञ्चिका,
                               संरचना Curl_वृक्षः *मूलम्);

#ifdef UNITTESTS
uint32_t Curl_splayget(संरचना Curl_वृक्षः *ग्रन्थिः)
{
  DEBUGASSERT(ग्रन्थिः);
  पटिनिवत्त ग्रन्थिः->id;
}

#endif

शून्य Curl_timeouts_init(संरचना Curl_timeouts *timeouts,
                        ध्रुव संरचना curltime *ptime_base)
{
  संरचना curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->वृक्षः = रिक्त;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(संरचना Curl_easy *data)
{
  संरचना Curl_वृक्षः *ग्रन्थिः = data ? &data->state.timeouts.splaynode : रिक्त;
  पटिनिवत्त ग्रन्थिः && ग्रन्थिः->registered;
}

timediff_t Curl_timeouts_offset_us(संरचना Curl_timeouts *timeouts,
                                   ध्रुव संरचना curltime *pts)
{
  संरचना curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  पटिनिवत्त curlx_ptimediff_us(pts, &time_base);
}

पूर्णाङ्क Curl_timeouts_next_ms(संरचना Curl_timeouts *timeouts,
                          ध्रुव संरचना curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  सचे(timeouts->वृक्षः) { /* splay the lowest key to the root */
    timeouts->वृक्षः = splay(TIMEDIFF_T_MIN, timeouts->वृक्षः);
  }

  सचे(timeouts->वृक्षः) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->वृक्षः->कुञ्चिका - elapsed_us;
    सचे(pmid)
      *pmid = timeouts->वृक्षः->id;
    सचे(pexpire_offset_us)
      *pexpire_offset_us = timeouts->वृक्षः->कुञ्चिका;
    सचे(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      पटिनिवत्त (ms > INT_MAX) ? INT_MAX : (पूर्णाङ्क)ms;
    }
    अञ्ञथा /* has expired */
      पटिनिवत्त 0;
  }
  सचे(pmid)
    *pmid = UINT32_MAX;
  सचे(pexpire_offset_us)
    *pexpire_offset_us = 0;
  पटिनिवत्त -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
संरचना Curl_वृक्षः *splay(timediff_t कुञ्चिका,
                             संरचना Curl_वृक्षः *मूलम्)
{
  संरचना Curl_वृक्षः N, *l, *r, *y;

  सचे(!मूलम्)
    पटिनिवत्त रिक्त;
  N.smaller = N.larger = रिक्त;
  l = r = &N;

  अत्थाय(;;) {
    सचे(कुञ्चिका < मूलम्->कुञ्चिका) {
      /* key is somewhere in root->smaller branch */
      सचे(!मूलम्->smaller)  /* which is empty, done */
        भङ्ग;
      सचे(कुञ्चिका < मूलम्->smaller->कुञ्चिका) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = मूलम्->smaller;
        मूलम्->smaller = y->larger;
        y->larger = मूलम्;
        मूलम् = y;
        सचे(!मूलम्->smaller)
          भङ्ग;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = मूलम्;
      r = मूलम्;
      मूलम् = मूलम्->smaller;
    }
    अञ्ञथा सचे(कुञ्चिका > मूलम्->कुञ्चिका) {
      /* key is somewhere in root->larger branch */
      सचे(!मूलम्->larger)  /* which is empty, done */
        भङ्ग;
      सचे(कुञ्चिका > मूलम्->larger->कुञ्चिका) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = मूलम्->larger;
        मूलम्->larger = y->smaller;
        y->smaller = मूलम्;
        मूलम् = y;
        सचे(!मूलम्->larger)
          भङ्ग;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = मूलम्;
      l = मूलम्;
      मूलम् = मूलम्->larger;
    }
    अञ्ञथा  /* exact match, root is key, done */
      भङ्ग;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = मूलम्->smaller;
  r->smaller = मूलम्->larger;
  मूलम्->smaller = N.larger;
  मूलम्->larger = N.smaller;

  पटिनिवत्त मूलम्;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST संरचना Curl_वृक्षः *splayinsert(timediff_t कुञ्चिका,
                                       संरचना Curl_वृक्षः *मूलम्,
                                       संरचना Curl_वृक्षः *ग्रन्थिः,
                                       uint32_t id);
UNITTEST संरचना Curl_वृक्षः *splayinsert(timediff_t कुञ्चिका,
                                       संरचना Curl_वृक्षः *मूलम्,
                                       संरचना Curl_वृक्षः *ग्रन्थिः,
                                       uint32_t id)
{
  DEBUGASSERT(ग्रन्थिः);

  ग्रन्थिः->कुञ्चिका = कुञ्चिका;
  ग्रन्थिः->id = id;
  ग्रन्थिः->same = रिक्त;
  ग्रन्थिः->registered = TRUE;
  सचे(मूलम्) {
    मूलम् = splay(कुञ्चिका, मूलम्);
    DEBUGASSERT(मूलम्);
    सचे(कुञ्चिका == मूलम्->कुञ्चिका) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      संरचना Curl_वृक्षः **panchor = &मूलम्->same;
      याव(*panchor)
        panchor = &(*panchor)->same;
      *panchor = ग्रन्थिः;
      पटिनिवत्त मूलम्; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  सचे(!मूलम्) {
    ग्रन्थिः->smaller = ग्रन्थिः->larger = रिक्त;
  }
  अञ्ञथा सचे(कुञ्चिका < मूलम्->कुञ्चिका) {
    ग्रन्थिः->smaller = मूलम्->smaller;
    ग्रन्थिः->larger = मूलम्;
    मूलम्->smaller = रिक्त;
  }
  अञ्ञथा {
    ग्रन्थिः->larger = मूलम्->larger;
    ग्रन्थिः->smaller = मूलम्;
    मूलम्->larger = रिक्त;
  }

  पटिनिवत्त ग्रन्थिः;
}

शून्य Curl_timeouts_add(संरचना Curl_timeouts *timeouts,
                       संरचना Curl_easy *data,
                       timediff_t offset_us)
{
  संरचना Curl_वृक्षः *ग्रन्थिः = &data->state.timeouts.splaynode;
  DEBUGASSERT(!ग्रन्थिः->registered);
  timeouts->वृक्षः = splayinsert(offset_us, timeouts->वृक्षः,
                               ग्रन्थिः, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST संरचना Curl_वृक्षः *splaygetbest(timediff_t कुञ्चिका,
                                        संरचना Curl_वृक्षः *मूलम्,
                                        संरचना Curl_वृक्षः **removed);
UNITTEST संरचना Curl_वृक्षः *splaygetbest(timediff_t कुञ्चिका,
                                        संरचना Curl_वृक्षः *मूलम्,
                                        संरचना Curl_वृक्षः **removed)
{
  संरचना Curl_वृक्षः *x;

  सचे(!मूलम्) {
    *removed = रिक्त; /* none removed since there was no root */
    पटिनिवत्त रिक्त;
  }

  /* find smallest */
  मूलम् = splay(TIMEDIFF_T_MIN, मूलम्);
  DEBUGASSERT(मूलम्);
  सचे(कुञ्चिका < मूलम्->कुञ्चिका) {
    /* even the smallest is too big */
    *removed = रिक्त;
    पटिनिवत्त मूलम्;
  }

  /* FIRST! Check if there is a list with identical keys */
  सचे(मूलम्->same) {
    x = मूलम्->same;
    DEBUGASSERT(x->कुञ्चिका == मूलम्->कुञ्चिका);
    /* 'x' becomes the new root node */
    x->larger = मूलम्->larger;
    x->smaller = मूलम्->smaller;
    मूलम्->same = रिक्त;
    मूलम्->registered = FALSE;
    *removed = मूलम्;
    पटिनिवत्त x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = मूलम्->larger;
  मूलम्->registered = FALSE;
  *removed = मूलम्;

  पटिनिवत्त x;
}

bool Curl_timeouts_remove_expired(संरचना Curl_timeouts *timeouts,
                                  ध्रुव संरचना curltime *ts,
                                  uint32_t *pmid)
{
  सचे(timeouts->वृक्षः) {
    संरचना Curl_वृक्षः *t = रिक्त;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->वृक्षः = splaygetbest(elapsed_us, timeouts->वृक्षः, &t);
    सचे(t) {
      *pmid = t->id;
      पटिनिवत्त TRUE;
    }
  }
  *pmid = UINT32_MAX;
  पटिनिवत्त FALSE;
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
UNITTEST पूर्णाङ्क splayremove(संरचना Curl_वृक्षः *मूलम्,
                         संरचना Curl_वृक्षः *removenode,
                         संरचना Curl_वृक्षः **newroot);
UNITTEST पूर्णाङ्क splayremove(संरचना Curl_वृक्षः *मूलम्,
                         संरचना Curl_वृक्षः *removenode,
                         संरचना Curl_वृक्षः **newroot)
{
  संरचना Curl_वृक्षः *x;

  सचे(!मूलम्)
    पटिनिवत्त 1;

  DEBUGASSERT(removenode);
  सचे(!removenode->registered)
    पटिनिवत्त 2;

  मूलम् = splay(removenode->कुञ्चिका, मूलम्);
  DEBUGASSERT(मूलम्);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  सचे(मूलम्->कुञ्चिका != removenode->कुञ्चिका) {
    DEBUGASSERT(0);
    पटिनिवत्त 2;
  }

  सचे(मूलम् != removenode) {
    /* Should be in the root->same list then */
    संरचना Curl_वृक्षः **panchor;
    अत्थाय(panchor = &मूलम्->same; *panchor; panchor = &(*panchor)->same) {
      सचे(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = रिक्त;
        removenode->registered = FALSE;
        *newroot = मूलम्;
        पटिनिवत्त 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    पटिनिवत्त 2;
  }
  /* removing the root node */
  सचे(मूलम्->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = मूलम्->same;
    x->larger = मूलम्->larger;
    x->smaller = मूलम्->smaller;
    मूलम्->same = रिक्त;
  }
  अञ्ञथा {
    /* Remove the root node */
    सचे(!मूलम्->smaller)
      x = मूलम्->larger;
    अञ्ञथा {
      x = splay(removenode->कुञ्चिका, मूलम्->smaller);
      DEBUGASSERT(x);
      x->larger = मूलम्->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  पटिनिवत्त 0;
}

bool Curl_timeouts_remove(संरचना Curl_timeouts *timeouts,
                          संरचना Curl_easy *data)
{
  संरचना Curl_वृक्षः *ग्रन्थिः = &data->state.timeouts.splaynode;
  सचे(ग्रन्थिः->registered) {
    पूर्णाङ्क rc = splayremove(timeouts->वृक्षः, ग्रन्थिः, &timeouts->वृक्षः);
#ifdef DEBUGBUILD
    सचे(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#अञ्ञथा
    (शून्य)rc;
#endif
    पटिनिवत्त TRUE;
  }
  पटिनिवत्त FALSE;
}
