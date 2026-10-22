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

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "splay.h"

நிலையான கட்டமைப்பு Curl_மரம் *splay(timediff_t சாவி,
                               கட்டமைப்பு Curl_மரம் *வேர்);

#ifdef UNITTESTS
uint32_t Curl_splayget(கட்டமைப்பு Curl_மரம் *முனை)
{
  DEBUGASSERT(முனை);
  திரும்பு முனை->id;
}

#endif

வெறுமை Curl_timeouts_init(கட்டமைப்பு Curl_timeouts *timeouts,
                        மாறிலி கட்டமைப்பு curltime *ptime_base)
{
  கட்டமைப்பு curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->மரம் = வெற்று;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_மரம் *முனை = data ? &data->state.timeouts.splaynode : வெற்று;
  திரும்பு முனை && முனை->registered;
}

timediff_t Curl_timeouts_offset_us(கட்டமைப்பு Curl_timeouts *timeouts,
                                   மாறிலி கட்டமைப்பு curltime *pts)
{
  கட்டமைப்பு curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  திரும்பு curlx_ptimediff_us(pts, &time_base);
}

முழு Curl_timeouts_next_ms(கட்டமைப்பு Curl_timeouts *timeouts,
                          மாறிலி கட்டமைப்பு curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  எனில்(timeouts->மரம்) { /* splay the lowest key to the root */
    timeouts->மரம் = splay(TIMEDIFF_T_MIN, timeouts->மரம்);
  }

  எனில்(timeouts->மரம்) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->மரம்->சாவி - elapsed_us;
    எனில்(pmid)
      *pmid = timeouts->மரம்->id;
    எனில்(pexpire_offset_us)
      *pexpire_offset_us = timeouts->மரம்->சாவி;
    எனில்(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      திரும்பு (ms > INT_MAX) ? INT_MAX : (முழு)ms;
    }
    இல்லையெனில் /* has expired */
      திரும்பு 0;
  }
  எனில்(pmid)
    *pmid = UINT32_MAX;
  எனில்(pexpire_offset_us)
    *pexpire_offset_us = 0;
  திரும்பு -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
கட்டமைப்பு Curl_மரம் *splay(timediff_t சாவி,
                             கட்டமைப்பு Curl_மரம் *வேர்)
{
  கட்டமைப்பு Curl_மரம் N, *l, *r, *y;

  எனில்(!வேர்)
    திரும்பு வெற்று;
  N.smaller = N.larger = வெற்று;
  l = r = &N;

  க்கு(;;) {
    எனில்(சாவி < வேர்->சாவி) {
      /* key is somewhere in root->smaller branch */
      எனில்(!வேர்->smaller)  /* which is empty, done */
        நிறுத்து;
      எனில்(சாவி < வேர்->smaller->சாவி) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = வேர்->smaller;
        வேர்->smaller = y->larger;
        y->larger = வேர்;
        வேர் = y;
        எனில்(!வேர்->smaller)
          நிறுத்து;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = வேர்;
      r = வேர்;
      வேர் = வேர்->smaller;
    }
    இல்லையெனில் எனில்(சாவி > வேர்->சாவி) {
      /* key is somewhere in root->larger branch */
      எனில்(!வேர்->larger)  /* which is empty, done */
        நிறுத்து;
      எனில்(சாவி > வேர்->larger->சாவி) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = வேர்->larger;
        வேர்->larger = y->smaller;
        y->smaller = வேர்;
        வேர் = y;
        எனில்(!வேர்->larger)
          நிறுத்து;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = வேர்;
      l = வேர்;
      வேர் = வேர்->larger;
    }
    இல்லையெனில்  /* exact match, root is key, done */
      நிறுத்து;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = வேர்->smaller;
  r->smaller = வேர்->larger;
  வேர்->smaller = N.larger;
  வேர்->larger = N.smaller;

  திரும்பு வேர்;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST கட்டமைப்பு Curl_மரம் *splayinsert(timediff_t சாவி,
                                       கட்டமைப்பு Curl_மரம் *வேர்,
                                       கட்டமைப்பு Curl_மரம் *முனை,
                                       uint32_t id);
UNITTEST கட்டமைப்பு Curl_மரம் *splayinsert(timediff_t சாவி,
                                       கட்டமைப்பு Curl_மரம் *வேர்,
                                       கட்டமைப்பு Curl_மரம் *முனை,
                                       uint32_t id)
{
  DEBUGASSERT(முனை);

  முனை->சாவி = சாவி;
  முனை->id = id;
  முனை->same = வெற்று;
  முனை->registered = TRUE;
  எனில்(வேர்) {
    வேர் = splay(சாவி, வேர்);
    DEBUGASSERT(வேர்);
    எனில்(சாவி == வேர்->சாவி) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      கட்டமைப்பு Curl_மரம் **panchor = &வேர்->same;
      போது(*panchor)
        panchor = &(*panchor)->same;
      *panchor = முனை;
      திரும்பு வேர்; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  எனில்(!வேர்) {
    முனை->smaller = முனை->larger = வெற்று;
  }
  இல்லையெனில் எனில்(சாவி < வேர்->சாவி) {
    முனை->smaller = வேர்->smaller;
    முனை->larger = வேர்;
    வேர்->smaller = வெற்று;
  }
  இல்லையெனில் {
    முனை->larger = வேர்->larger;
    முனை->smaller = வேர்;
    வேர்->larger = வெற்று;
  }

  திரும்பு முனை;
}

வெறுமை Curl_timeouts_add(கட்டமைப்பு Curl_timeouts *timeouts,
                       கட்டமைப்பு Curl_easy *data,
                       timediff_t offset_us)
{
  கட்டமைப்பு Curl_மரம் *முனை = &data->state.timeouts.splaynode;
  DEBUGASSERT(!முனை->registered);
  timeouts->மரம் = splayinsert(offset_us, timeouts->மரம்,
                               முனை, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST கட்டமைப்பு Curl_மரம் *splaygetbest(timediff_t சாவி,
                                        கட்டமைப்பு Curl_மரம் *வேர்,
                                        கட்டமைப்பு Curl_மரம் **removed);
UNITTEST கட்டமைப்பு Curl_மரம் *splaygetbest(timediff_t சாவி,
                                        கட்டமைப்பு Curl_மரம் *வேர்,
                                        கட்டமைப்பு Curl_மரம் **removed)
{
  கட்டமைப்பு Curl_மரம் *x;

  எனில்(!வேர்) {
    *removed = வெற்று; /* none removed since there was no root */
    திரும்பு வெற்று;
  }

  /* find smallest */
  வேர் = splay(TIMEDIFF_T_MIN, வேர்);
  DEBUGASSERT(வேர்);
  எனில்(சாவி < வேர்->சாவி) {
    /* even the smallest is too big */
    *removed = வெற்று;
    திரும்பு வேர்;
  }

  /* FIRST! Check if there is a list with identical keys */
  எனில்(வேர்->same) {
    x = வேர்->same;
    DEBUGASSERT(x->சாவி == வேர்->சாவி);
    /* 'x' becomes the new root node */
    x->larger = வேர்->larger;
    x->smaller = வேர்->smaller;
    வேர்->same = வெற்று;
    வேர்->registered = FALSE;
    *removed = வேர்;
    திரும்பு x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = வேர்->larger;
  வேர்->registered = FALSE;
  *removed = வேர்;

  திரும்பு x;
}

bool Curl_timeouts_remove_expired(கட்டமைப்பு Curl_timeouts *timeouts,
                                  மாறிலி கட்டமைப்பு curltime *ts,
                                  uint32_t *pmid)
{
  எனில்(timeouts->மரம்) {
    கட்டமைப்பு Curl_மரம் *t = வெற்று;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->மரம் = splaygetbest(elapsed_us, timeouts->மரம், &t);
    எனில்(t) {
      *pmid = t->id;
      திரும்பு TRUE;
    }
  }
  *pmid = UINT32_MAX;
  திரும்பு FALSE;
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
UNITTEST முழு splayremove(கட்டமைப்பு Curl_மரம் *வேர்,
                         கட்டமைப்பு Curl_மரம் *removenode,
                         கட்டமைப்பு Curl_மரம் **newroot);
UNITTEST முழு splayremove(கட்டமைப்பு Curl_மரம் *வேர்,
                         கட்டமைப்பு Curl_மரம் *removenode,
                         கட்டமைப்பு Curl_மரம் **newroot)
{
  கட்டமைப்பு Curl_மரம் *x;

  எனில்(!வேர்)
    திரும்பு 1;

  DEBUGASSERT(removenode);
  எனில்(!removenode->registered)
    திரும்பு 2;

  வேர் = splay(removenode->சாவி, வேர்);
  DEBUGASSERT(வேர்);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  எனில்(வேர்->சாவி != removenode->சாவி) {
    DEBUGASSERT(0);
    திரும்பு 2;
  }

  எனில்(வேர் != removenode) {
    /* Should be in the root->same list then */
    கட்டமைப்பு Curl_மரம் **panchor;
    க்கு(panchor = &வேர்->same; *panchor; panchor = &(*panchor)->same) {
      எனில்(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = வெற்று;
        removenode->registered = FALSE;
        *newroot = வேர்;
        திரும்பு 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    திரும்பு 2;
  }
  /* removing the root node */
  எனில்(வேர்->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = வேர்->same;
    x->larger = வேர்->larger;
    x->smaller = வேர்->smaller;
    வேர்->same = வெற்று;
  }
  இல்லையெனில் {
    /* Remove the root node */
    எனில்(!வேர்->smaller)
      x = வேர்->larger;
    இல்லையெனில் {
      x = splay(removenode->சாவி, வேர்->smaller);
      DEBUGASSERT(x);
      x->larger = வேர்->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  திரும்பு 0;
}

bool Curl_timeouts_remove(கட்டமைப்பு Curl_timeouts *timeouts,
                          கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_மரம் *முனை = &data->state.timeouts.splaynode;
  எனில்(முனை->registered) {
    முழு rc = splayremove(timeouts->மரம், முனை, &timeouts->மரம்);
#ifdef DEBUGBUILD
    எனில்(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#இல்லையெனில்
    (வெறுமை)rc;
#endif
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}
