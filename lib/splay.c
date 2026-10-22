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

स्थिर संरचना Curl_वृक्ष *splay(timediff_t कुंजी,
                               संरचना Curl_वृक्ष *मूल);

#ifdef UNITTESTS
uint32_t Curl_splayget(संरचना Curl_वृक्ष *गाँठ)
{
  DEBUGASSERT(गाँठ);
  प्रत्यागम गाँठ->id;
}

#endif

शून्य Curl_timeouts_init(संरचना Curl_timeouts *timeouts,
                        ध्रुव संरचना curltime *ptime_base)
{
  संरचना curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->वृक्ष = रिक्त;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(संरचना Curl_easy *data)
{
  संरचना Curl_वृक्ष *गाँठ = data ? &data->state.timeouts.splaynode : रिक्त;
  प्रत्यागम गाँठ && गाँठ->registered;
}

timediff_t Curl_timeouts_offset_us(संरचना Curl_timeouts *timeouts,
                                   ध्रुव संरचना curltime *pts)
{
  संरचना curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  प्रत्यागम curlx_ptimediff_us(pts, &time_base);
}

पूर्णाङ्क Curl_timeouts_next_ms(संरचना Curl_timeouts *timeouts,
                          ध्रुव संरचना curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  यदि(timeouts->वृक्ष) { /* splay the lowest key to the root */
    timeouts->वृक्ष = splay(TIMEDIFF_T_MIN, timeouts->वृक्ष);
  }

  यदि(timeouts->वृक्ष) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->वृक्ष->कुंजी - elapsed_us;
    यदि(pmid)
      *pmid = timeouts->वृक्ष->id;
    यदि(pexpire_offset_us)
      *pexpire_offset_us = timeouts->वृक्ष->कुंजी;
    यदि(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      प्रत्यागम (ms > INT_MAX) ? INT_MAX : (पूर्णाङ्क)ms;
    }
    अन्यथा /* has expired */
      प्रत्यागम 0;
  }
  यदि(pmid)
    *pmid = UINT32_MAX;
  यदि(pexpire_offset_us)
    *pexpire_offset_us = 0;
  प्रत्यागम -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
संरचना Curl_वृक्ष *splay(timediff_t कुंजी,
                             संरचना Curl_वृक्ष *मूल)
{
  संरचना Curl_वृक्ष N, *l, *r, *y;

  यदि(!मूल)
    प्रत्यागम रिक्त;
  N.smaller = N.larger = रिक्त;
  l = r = &N;

  प्रति(;;) {
    यदि(कुंजी < मूल->कुंजी) {
      /* key is somewhere in root->smaller branch */
      यदि(!मूल->smaller)  /* which is empty, done */
        भङ्ग;
      यदि(कुंजी < मूल->smaller->कुंजी) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = मूल->smaller;
        मूल->smaller = y->larger;
        y->larger = मूल;
        मूल = y;
        यदि(!मूल->smaller)
          भङ्ग;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = मूल;
      r = मूल;
      मूल = मूल->smaller;
    }
    अन्यथा यदि(कुंजी > मूल->कुंजी) {
      /* key is somewhere in root->larger branch */
      यदि(!मूल->larger)  /* which is empty, done */
        भङ्ग;
      यदि(कुंजी > मूल->larger->कुंजी) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = मूल->larger;
        मूल->larger = y->smaller;
        y->smaller = मूल;
        मूल = y;
        यदि(!मूल->larger)
          भङ्ग;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = मूल;
      l = मूल;
      मूल = मूल->larger;
    }
    अन्यथा  /* exact match, root is key, done */
      भङ्ग;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = मूल->smaller;
  r->smaller = मूल->larger;
  मूल->smaller = N.larger;
  मूल->larger = N.smaller;

  प्रत्यागम मूल;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST संरचना Curl_वृक्ष *splayinsert(timediff_t कुंजी,
                                       संरचना Curl_वृक्ष *मूल,
                                       संरचना Curl_वृक्ष *गाँठ,
                                       uint32_t id);
UNITTEST संरचना Curl_वृक्ष *splayinsert(timediff_t कुंजी,
                                       संरचना Curl_वृक्ष *मूल,
                                       संरचना Curl_वृक्ष *गाँठ,
                                       uint32_t id)
{
  DEBUGASSERT(गाँठ);

  गाँठ->कुंजी = कुंजी;
  गाँठ->id = id;
  गाँठ->same = रिक्त;
  गाँठ->registered = TRUE;
  यदि(मूल) {
    मूल = splay(कुंजी, मूल);
    DEBUGASSERT(मूल);
    यदि(कुंजी == मूल->कुंजी) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      संरचना Curl_वृक्ष **panchor = &मूल->same;
      यावत्(*panchor)
        panchor = &(*panchor)->same;
      *panchor = गाँठ;
      प्रत्यागम मूल; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  यदि(!मूल) {
    गाँठ->smaller = गाँठ->larger = रिक्त;
  }
  अन्यथा यदि(कुंजी < मूल->कुंजी) {
    गाँठ->smaller = मूल->smaller;
    गाँठ->larger = मूल;
    मूल->smaller = रिक्त;
  }
  अन्यथा {
    गाँठ->larger = मूल->larger;
    गाँठ->smaller = मूल;
    मूल->larger = रिक्त;
  }

  प्रत्यागम गाँठ;
}

शून्य Curl_timeouts_add(संरचना Curl_timeouts *timeouts,
                       संरचना Curl_easy *data,
                       timediff_t offset_us)
{
  संरचना Curl_वृक्ष *गाँठ = &data->state.timeouts.splaynode;
  DEBUGASSERT(!गाँठ->registered);
  timeouts->वृक्ष = splayinsert(offset_us, timeouts->वृक्ष,
                               गाँठ, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST संरचना Curl_वृक्ष *splaygetbest(timediff_t कुंजी,
                                        संरचना Curl_वृक्ष *मूल,
                                        संरचना Curl_वृक्ष **removed);
UNITTEST संरचना Curl_वृक्ष *splaygetbest(timediff_t कुंजी,
                                        संरचना Curl_वृक्ष *मूल,
                                        संरचना Curl_वृक्ष **removed)
{
  संरचना Curl_वृक्ष *x;

  यदि(!मूल) {
    *removed = रिक्त; /* none removed since there was no root */
    प्रत्यागम रिक्त;
  }

  /* find smallest */
  मूल = splay(TIMEDIFF_T_MIN, मूल);
  DEBUGASSERT(मूल);
  यदि(कुंजी < मूल->कुंजी) {
    /* even the smallest is too big */
    *removed = रिक्त;
    प्रत्यागम मूल;
  }

  /* FIRST! Check if there is a list with identical keys */
  यदि(मूल->same) {
    x = मूल->same;
    DEBUGASSERT(x->कुंजी == मूल->कुंजी);
    /* 'x' becomes the new root node */
    x->larger = मूल->larger;
    x->smaller = मूल->smaller;
    मूल->same = रिक्त;
    मूल->registered = FALSE;
    *removed = मूल;
    प्रत्यागम x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = मूल->larger;
  मूल->registered = FALSE;
  *removed = मूल;

  प्रत्यागम x;
}

bool Curl_timeouts_remove_expired(संरचना Curl_timeouts *timeouts,
                                  ध्रुव संरचना curltime *ts,
                                  uint32_t *pmid)
{
  यदि(timeouts->वृक्ष) {
    संरचना Curl_वृक्ष *t = रिक्त;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->वृक्ष = splaygetbest(elapsed_us, timeouts->वृक्ष, &t);
    यदि(t) {
      *pmid = t->id;
      प्रत्यागम TRUE;
    }
  }
  *pmid = UINT32_MAX;
  प्रत्यागम FALSE;
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
UNITTEST पूर्णाङ्क splayremove(संरचना Curl_वृक्ष *मूल,
                         संरचना Curl_वृक्ष *removenode,
                         संरचना Curl_वृक्ष **newroot);
UNITTEST पूर्णाङ्क splayremove(संरचना Curl_वृक्ष *मूल,
                         संरचना Curl_वृक्ष *removenode,
                         संरचना Curl_वृक्ष **newroot)
{
  संरचना Curl_वृक्ष *x;

  यदि(!मूल)
    प्रत्यागम 1;

  DEBUGASSERT(removenode);
  यदि(!removenode->registered)
    प्रत्यागम 2;

  मूल = splay(removenode->कुंजी, मूल);
  DEBUGASSERT(मूल);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  यदि(मूल->कुंजी != removenode->कुंजी) {
    DEBUGASSERT(0);
    प्रत्यागम 2;
  }

  यदि(मूल != removenode) {
    /* Should be in the root->same list then */
    संरचना Curl_वृक्ष **panchor;
    प्रति(panchor = &मूल->same; *panchor; panchor = &(*panchor)->same) {
      यदि(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = रिक्त;
        removenode->registered = FALSE;
        *newroot = मूल;
        प्रत्यागम 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    प्रत्यागम 2;
  }
  /* removing the root node */
  यदि(मूल->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = मूल->same;
    x->larger = मूल->larger;
    x->smaller = मूल->smaller;
    मूल->same = रिक्त;
  }
  अन्यथा {
    /* Remove the root node */
    यदि(!मूल->smaller)
      x = मूल->larger;
    अन्यथा {
      x = splay(removenode->कुंजी, मूल->smaller);
      DEBUGASSERT(x);
      x->larger = मूल->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  प्रत्यागम 0;
}

bool Curl_timeouts_remove(संरचना Curl_timeouts *timeouts,
                          संरचना Curl_easy *data)
{
  संरचना Curl_वृक्ष *गाँठ = &data->state.timeouts.splaynode;
  यदि(गाँठ->registered) {
    पूर्णाङ्क rc = splayremove(timeouts->वृक्ष, गाँठ, &timeouts->वृक्ष);
#ifdef DEBUGBUILD
    यदि(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#अन्यथा
    (शून्य)rc;
#endif
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}
