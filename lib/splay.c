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

#समावेश "urldata.h"
#समावेश "splay.h"

अटल काष्ठा Curl_वृक्ष *splay(timediff_t कुंजी,
                               काष्ठा Curl_वृक्ष *मूल);

#अगर_घोषित UNITTESTS
uint32_t Curl_splayget(काष्ठा Curl_वृक्ष *गाँठ)
{
  DEBUGASSERT(गाँठ);
  वापस गाँठ->id;
}

#पूर्ण_अगर

व्योम Curl_timeouts_init(काष्ठा Curl_timeouts *timeouts,
                        स्थिर काष्ठा curltime *ptime_base)
{
  काष्ठा curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->वृक्ष = शून्य;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_वृक्ष *गाँठ = तथ्य ? &तथ्य->state.timeouts.splaynode : शून्य;
  वापस गाँठ && गाँठ->registered;
}

timediff_t Curl_timeouts_offset_us(काष्ठा Curl_timeouts *timeouts,
                                   स्थिर काष्ठा curltime *pts)
{
  काष्ठा curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  वापस curlx_ptimediff_us(pts, &time_base);
}

पूर्णांक Curl_timeouts_अग्र_ms(काष्ठा Curl_timeouts *timeouts,
                          स्थिर काष्ठा curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  अगर(timeouts->वृक्ष) { /* splay the lowest key to the root */
    timeouts->वृक्ष = splay(TIMEDIFF_T_MIN, timeouts->वृक्ष);
  }

  अगर(timeouts->वृक्ष) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->वृक्ष->कुंजी - elapsed_us;
    अगर(pmid)
      *pmid = timeouts->वृक्ष->id;
    अगर(pexpire_offset_us)
      *pexpire_offset_us = timeouts->वृक्ष->कुंजी;
    अगर(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      वापस (ms > पूर्णांक_उच्च) ? पूर्णांक_उच्च : (पूर्णांक)ms;
    }
    अन्यथा /* has expired */
      वापस 0;
  }
  अगर(pmid)
    *pmid = UINT32_MAX;
  अगर(pexpire_offset_us)
    *pexpire_offset_us = 0;
  वापस -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
काष्ठा Curl_वृक्ष *splay(timediff_t कुंजी,
                             काष्ठा Curl_वृक्ष *मूल)
{
  काष्ठा Curl_वृक्ष N, *l, *r, *y;

  अगर(!मूल)
    वापस शून्य;
  N.smaller = N.larger = शून्य;
  l = r = &N;

  क्रम(;;) {
    अगर(कुंजी < मूल->कुंजी) {
      /* key is somewhere in root->smaller branch */
      अगर(!मूल->smaller)  /* which is empty, done */
        अवरोध;
      अगर(कुंजी < मूल->smaller->कुंजी) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = मूल->smaller;
        मूल->smaller = y->larger;
        y->larger = मूल;
        मूल = y;
        अगर(!मूल->smaller)
          अवरोध;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = मूल;
      r = मूल;
      मूल = मूल->smaller;
    }
    अन्यथा अगर(कुंजी > मूल->कुंजी) {
      /* key is somewhere in root->larger branch */
      अगर(!मूल->larger)  /* which is empty, done */
        अवरोध;
      अगर(कुंजी > मूल->larger->कुंजी) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = मूल->larger;
        मूल->larger = y->smaller;
        y->smaller = मूल;
        मूल = y;
        अगर(!मूल->larger)
          अवरोध;
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
      अवरोध;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = मूल->smaller;
  r->smaller = मूल->larger;
  मूल->smaller = N.larger;
  मूल->larger = N.smaller;

  वापस मूल;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST काष्ठा Curl_वृक्ष *splayinsert(timediff_t कुंजी,
                                       काष्ठा Curl_वृक्ष *मूल,
                                       काष्ठा Curl_वृक्ष *गाँठ,
                                       uint32_t id);
UNITTEST काष्ठा Curl_वृक्ष *splayinsert(timediff_t कुंजी,
                                       काष्ठा Curl_वृक्ष *मूल,
                                       काष्ठा Curl_वृक्ष *गाँठ,
                                       uint32_t id)
{
  DEBUGASSERT(गाँठ);

  गाँठ->कुंजी = कुंजी;
  गाँठ->id = id;
  गाँठ->same = शून्य;
  गाँठ->registered = TRUE;
  अगर(मूल) {
    मूल = splay(कुंजी, मूल);
    DEBUGASSERT(मूल);
    अगर(कुंजी == मूल->कुंजी) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      काष्ठा Curl_वृक्ष **panchor = &मूल->same;
      जबतक(*panchor)
        panchor = &(*panchor)->same;
      *panchor = गाँठ;
      वापस मूल; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  अगर(!मूल) {
    गाँठ->smaller = गाँठ->larger = शून्य;
  }
  अन्यथा अगर(कुंजी < मूल->कुंजी) {
    गाँठ->smaller = मूल->smaller;
    गाँठ->larger = मूल;
    मूल->smaller = शून्य;
  }
  अन्यथा {
    गाँठ->larger = मूल->larger;
    गाँठ->smaller = मूल;
    मूल->larger = शून्य;
  }

  वापस गाँठ;
}

व्योम Curl_timeouts_add(काष्ठा Curl_timeouts *timeouts,
                       काष्ठा Curl_easy *तथ्य,
                       timediff_t offset_us)
{
  काष्ठा Curl_वृक्ष *गाँठ = &तथ्य->state.timeouts.splaynode;
  DEBUGASSERT(!गाँठ->registered);
  timeouts->वृक्ष = splayinsert(offset_us, timeouts->वृक्ष,
                               गाँठ, तथ्य->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST काष्ठा Curl_वृक्ष *splaygetbest(timediff_t कुंजी,
                                        काष्ठा Curl_वृक्ष *मूल,
                                        काष्ठा Curl_वृक्ष **removed);
UNITTEST काष्ठा Curl_वृक्ष *splaygetbest(timediff_t कुंजी,
                                        काष्ठा Curl_वृक्ष *मूल,
                                        काष्ठा Curl_वृक्ष **removed)
{
  काष्ठा Curl_वृक्ष *x;

  अगर(!मूल) {
    *removed = शून्य; /* none removed since there was no root */
    वापस शून्य;
  }

  /* find smallest */
  मूल = splay(TIMEDIFF_T_MIN, मूल);
  DEBUGASSERT(मूल);
  अगर(कुंजी < मूल->कुंजी) {
    /* even the smallest is too big */
    *removed = शून्य;
    वापस मूल;
  }

  /* FIRST! Check if there is a list with identical keys */
  अगर(मूल->same) {
    x = मूल->same;
    DEBUGASSERT(x->कुंजी == मूल->कुंजी);
    /* 'x' becomes the new root node */
    x->larger = मूल->larger;
    x->smaller = मूल->smaller;
    मूल->same = शून्य;
    मूल->registered = FALSE;
    *removed = मूल;
    वापस x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = मूल->larger;
  मूल->registered = FALSE;
  *removed = मूल;

  वापस x;
}

bool Curl_timeouts_remove_expired(काष्ठा Curl_timeouts *timeouts,
                                  स्थिर काष्ठा curltime *ts,
                                  uint32_t *pmid)
{
  अगर(timeouts->वृक्ष) {
    काष्ठा Curl_वृक्ष *t = शून्य;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->वृक्ष = splaygetbest(elapsed_us, timeouts->वृक्ष, &t);
    अगर(t) {
      *pmid = t->id;
      वापस TRUE;
    }
  }
  *pmid = UINT32_MAX;
  वापस FALSE;
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
UNITTEST पूर्णांक splayremove(काष्ठा Curl_वृक्ष *मूल,
                         काष्ठा Curl_वृक्ष *removenode,
                         काष्ठा Curl_वृक्ष **newroot);
UNITTEST पूर्णांक splayremove(काष्ठा Curl_वृक्ष *मूल,
                         काष्ठा Curl_वृक्ष *removenode,
                         काष्ठा Curl_वृक्ष **newroot)
{
  काष्ठा Curl_वृक्ष *x;

  अगर(!मूल)
    वापस 1;

  DEBUGASSERT(removenode);
  अगर(!removenode->registered)
    वापस 2;

  मूल = splay(removenode->कुंजी, मूल);
  DEBUGASSERT(मूल);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  अगर(मूल->कुंजी != removenode->कुंजी) {
    DEBUGASSERT(0);
    वापस 2;
  }

  अगर(मूल != removenode) {
    /* Should be in the root->same list then */
    काष्ठा Curl_वृक्ष **panchor;
    क्रम(panchor = &मूल->same; *panchor; panchor = &(*panchor)->same) {
      अगर(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = शून्य;
        removenode->registered = FALSE;
        *newroot = मूल;
        वापस 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    वापस 2;
  }
  /* removing the root node */
  अगर(मूल->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = मूल->same;
    x->larger = मूल->larger;
    x->smaller = मूल->smaller;
    मूल->same = शून्य;
  }
  अन्यथा {
    /* Remove the root node */
    अगर(!मूल->smaller)
      x = मूल->larger;
    अन्यथा {
      x = splay(removenode->कुंजी, मूल->smaller);
      DEBUGASSERT(x);
      x->larger = मूल->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  वापस 0;
}

bool Curl_timeouts_remove(काष्ठा Curl_timeouts *timeouts,
                          काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_वृक्ष *गाँठ = &तथ्य->state.timeouts.splaynode;
  अगर(गाँठ->registered) {
    पूर्णांक rc = splayremove(timeouts->वृक्ष, गाँठ, &timeouts->वृक्ष);
#अगर_घोषित DEBUGBUILD
    अगर(rc)
      curl_mfprintf(मानक_त्रुटि, "Internal error removing splay node = %d\n", rc);
#अन्यथा
    (व्योम)rc;
#पूर्ण_अगर
    वापस TRUE;
  }
  वापस FALSE;
}
