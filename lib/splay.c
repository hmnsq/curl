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

स्थिर संरचना Curl_वृक्ष *splay(timediff_t किल्ली,
                               संरचना Curl_वृक्ष *मूळ);

#ifdef UNITTESTS
uint32_t Curl_splayget(संरचना Curl_वृक्ष *गाठ)
{
  DEBUGASSERT(गाठ);
  परत गाठ->id;
}

#endif

शून्य Curl_timeouts_init(संरचना Curl_timeouts *timeouts,
                        स्थिरांक संरचना curltime *ptime_base)
{
  संरचना curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->वृक्ष = रिक्त;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(संरचना Curl_easy *data)
{
  संरचना Curl_वृक्ष *गाठ = data ? &data->state.timeouts.splaynode : रिक्त;
  परत गाठ && गाठ->registered;
}

timediff_t Curl_timeouts_offset_us(संरचना Curl_timeouts *timeouts,
                                   स्थिरांक संरचना curltime *pts)
{
  संरचना curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  परत curlx_ptimediff_us(pts, &time_base);
}

पूर्णांक Curl_timeouts_next_ms(संरचना Curl_timeouts *timeouts,
                          स्थिरांक संरचना curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  जर(timeouts->वृक्ष) { /* splay the lowest key to the root */
    timeouts->वृक्ष = splay(TIMEDIFF_T_MIN, timeouts->वृक्ष);
  }

  जर(timeouts->वृक्ष) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->वृक्ष->किल्ली - elapsed_us;
    जर(pmid)
      *pmid = timeouts->वृक्ष->id;
    जर(pexpire_offset_us)
      *pexpire_offset_us = timeouts->वृक्ष->किल्ली;
    जर(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      परत (ms > पूर्णांक_उच्च) ? पूर्णांक_उच्च : (पूर्णांक)ms;
    }
    अन्यथा /* has expired */
      परत 0;
  }
  जर(pmid)
    *pmid = UINT32_MAX;
  जर(pexpire_offset_us)
    *pexpire_offset_us = 0;
  परत -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
संरचना Curl_वृक्ष *splay(timediff_t किल्ली,
                             संरचना Curl_वृक्ष *मूळ)
{
  संरचना Curl_वृक्ष N, *l, *r, *y;

  जर(!मूळ)
    परत रिक्त;
  N.smaller = N.larger = रिक्त;
  l = r = &N;

  साठी(;;) {
    जर(किल्ली < मूळ->किल्ली) {
      /* key is somewhere in root->smaller branch */
      जर(!मूळ->smaller)  /* which is empty, done */
        थांबा;
      जर(किल्ली < मूळ->smaller->किल्ली) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = मूळ->smaller;
        मूळ->smaller = y->larger;
        y->larger = मूळ;
        मूळ = y;
        जर(!मूळ->smaller)
          थांबा;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = मूळ;
      r = मूळ;
      मूळ = मूळ->smaller;
    }
    अन्यथा जर(किल्ली > मूळ->किल्ली) {
      /* key is somewhere in root->larger branch */
      जर(!मूळ->larger)  /* which is empty, done */
        थांबा;
      जर(किल्ली > मूळ->larger->किल्ली) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = मूळ->larger;
        मूळ->larger = y->smaller;
        y->smaller = मूळ;
        मूळ = y;
        जर(!मूळ->larger)
          थांबा;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = मूळ;
      l = मूळ;
      मूळ = मूळ->larger;
    }
    अन्यथा  /* exact match, root is key, done */
      थांबा;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = मूळ->smaller;
  r->smaller = मूळ->larger;
  मूळ->smaller = N.larger;
  मूळ->larger = N.smaller;

  परत मूळ;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST संरचना Curl_वृक्ष *splayinsert(timediff_t किल्ली,
                                       संरचना Curl_वृक्ष *मूळ,
                                       संरचना Curl_वृक्ष *गाठ,
                                       uint32_t id);
UNITTEST संरचना Curl_वृक्ष *splayinsert(timediff_t किल्ली,
                                       संरचना Curl_वृक्ष *मूळ,
                                       संरचना Curl_वृक्ष *गाठ,
                                       uint32_t id)
{
  DEBUGASSERT(गाठ);

  गाठ->किल्ली = किल्ली;
  गाठ->id = id;
  गाठ->same = रिक्त;
  गाठ->registered = TRUE;
  जर(मूळ) {
    मूळ = splay(किल्ली, मूळ);
    DEBUGASSERT(मूळ);
    जर(किल्ली == मूळ->किल्ली) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      संरचना Curl_वृक्ष **panchor = &मूळ->same;
      पर्यंत(*panchor)
        panchor = &(*panchor)->same;
      *panchor = गाठ;
      परत मूळ; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  जर(!मूळ) {
    गाठ->smaller = गाठ->larger = रिक्त;
  }
  अन्यथा जर(किल्ली < मूळ->किल्ली) {
    गाठ->smaller = मूळ->smaller;
    गाठ->larger = मूळ;
    मूळ->smaller = रिक्त;
  }
  अन्यथा {
    गाठ->larger = मूळ->larger;
    गाठ->smaller = मूळ;
    मूळ->larger = रिक्त;
  }

  परत गाठ;
}

शून्य Curl_timeouts_add(संरचना Curl_timeouts *timeouts,
                       संरचना Curl_easy *data,
                       timediff_t offset_us)
{
  संरचना Curl_वृक्ष *गाठ = &data->state.timeouts.splaynode;
  DEBUGASSERT(!गाठ->registered);
  timeouts->वृक्ष = splayinsert(offset_us, timeouts->वृक्ष,
                               गाठ, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST संरचना Curl_वृक्ष *splaygetbest(timediff_t किल्ली,
                                        संरचना Curl_वृक्ष *मूळ,
                                        संरचना Curl_वृक्ष **removed);
UNITTEST संरचना Curl_वृक्ष *splaygetbest(timediff_t किल्ली,
                                        संरचना Curl_वृक्ष *मूळ,
                                        संरचना Curl_वृक्ष **removed)
{
  संरचना Curl_वृक्ष *x;

  जर(!मूळ) {
    *removed = रिक्त; /* none removed since there was no root */
    परत रिक्त;
  }

  /* find smallest */
  मूळ = splay(TIMEDIFF_T_MIN, मूळ);
  DEBUGASSERT(मूळ);
  जर(किल्ली < मूळ->किल्ली) {
    /* even the smallest is too big */
    *removed = रिक्त;
    परत मूळ;
  }

  /* FIRST! Check if there is a list with identical keys */
  जर(मूळ->same) {
    x = मूळ->same;
    DEBUGASSERT(x->किल्ली == मूळ->किल्ली);
    /* 'x' becomes the new root node */
    x->larger = मूळ->larger;
    x->smaller = मूळ->smaller;
    मूळ->same = रिक्त;
    मूळ->registered = FALSE;
    *removed = मूळ;
    परत x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = मूळ->larger;
  मूळ->registered = FALSE;
  *removed = मूळ;

  परत x;
}

bool Curl_timeouts_remove_expired(संरचना Curl_timeouts *timeouts,
                                  स्थिरांक संरचना curltime *ts,
                                  uint32_t *pmid)
{
  जर(timeouts->वृक्ष) {
    संरचना Curl_वृक्ष *t = रिक्त;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->वृक्ष = splaygetbest(elapsed_us, timeouts->वृक्ष, &t);
    जर(t) {
      *pmid = t->id;
      परत TRUE;
    }
  }
  *pmid = UINT32_MAX;
  परत FALSE;
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
UNITTEST पूर्णांक splayremove(संरचना Curl_वृक्ष *मूळ,
                         संरचना Curl_वृक्ष *removenode,
                         संरचना Curl_वृक्ष **newroot);
UNITTEST पूर्णांक splayremove(संरचना Curl_वृक्ष *मूळ,
                         संरचना Curl_वृक्ष *removenode,
                         संरचना Curl_वृक्ष **newroot)
{
  संरचना Curl_वृक्ष *x;

  जर(!मूळ)
    परत 1;

  DEBUGASSERT(removenode);
  जर(!removenode->registered)
    परत 2;

  मूळ = splay(removenode->किल्ली, मूळ);
  DEBUGASSERT(मूळ);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  जर(मूळ->किल्ली != removenode->किल्ली) {
    DEBUGASSERT(0);
    परत 2;
  }

  जर(मूळ != removenode) {
    /* Should be in the root->same list then */
    संरचना Curl_वृक्ष **panchor;
    साठी(panchor = &मूळ->same; *panchor; panchor = &(*panchor)->same) {
      जर(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = रिक्त;
        removenode->registered = FALSE;
        *newroot = मूळ;
        परत 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    परत 2;
  }
  /* removing the root node */
  जर(मूळ->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = मूळ->same;
    x->larger = मूळ->larger;
    x->smaller = मूळ->smaller;
    मूळ->same = रिक्त;
  }
  अन्यथा {
    /* Remove the root node */
    जर(!मूळ->smaller)
      x = मूळ->larger;
    अन्यथा {
      x = splay(removenode->किल्ली, मूळ->smaller);
      DEBUGASSERT(x);
      x->larger = मूळ->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  परत 0;
}

bool Curl_timeouts_remove(संरचना Curl_timeouts *timeouts,
                          संरचना Curl_easy *data)
{
  संरचना Curl_वृक्ष *गाठ = &data->state.timeouts.splaynode;
  जर(गाठ->registered) {
    पूर्णांक rc = splayremove(timeouts->वृक्ष, गाठ, &timeouts->वृक्ष);
#ifdef DEBUGBUILD
    जर(rc)
      curl_mfprintf(मानक_त्रुटि, "Internal error removing splay node = %d\n", rc);
#अन्यथा
    (शून्य)rc;
#endif
    परत TRUE;
  }
  परत FALSE;
}
