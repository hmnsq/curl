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
#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "splay.h"

ساكن بنية Curl_وڻ *splay(timediff_t ڪنجي,
                               بنية Curl_وڻ *پاڙ);

#جيڪڏهن_وصف_ٿيل UNITTESTS
uint32_t Curl_splayget(بنية Curl_وڻ *ڳنڍ)
{
  DEBUGASSERT(ڳنڍ);
  أرجع ڳنڍ->id;
}

#جيڪڏهن_پورو

فراغ Curl_timeouts_init(بنية Curl_timeouts *timeouts,
                        ثابت بنية curltime *ptime_بنياد)
{
  بنية curltime وقت_بنياد = ptime_بنياد ? *ptime_بنياد : curlx_now();
  timeouts->وڻ = عدم;
  timeouts->وقت_بنياد_sec = وقت_بنياد.tv_sec;
}

bool Curl_timeouts_has(بنية Curl_easy *معلومات)
{
  بنية Curl_وڻ *ڳنڍ = معلومات ? &معلومات->state.timeouts.splaynode : عدم;
  أرجع ڳنڍ && ڳنڍ->registered;
}

timediff_t Curl_timeouts_وٿي_us(بنية Curl_timeouts *timeouts,
                                   ثابت بنية curltime *pts)
{
  بنية curltime وقت_بنياد;
  وقت_بنياد.tv_sec = timeouts->وقت_بنياد_sec;
  وقت_بنياد.tv_usec = 0;
  أرجع curlx_ptimediff_us(pts, &وقت_بنياد);
}

صحيح Curl_timeouts_ايندڙ_ms(بنية Curl_timeouts *timeouts,
                          ثابت بنية curltime *pnow,
                          timediff_t *pexpire_وٿي_us,
                          uint32_t *pmid)
{
  إذا(timeouts->وڻ) { /* splay the lowest key to the root */
    timeouts->وڻ = splay(TIMEDIFF_T_MIN, timeouts->وڻ);
  }

  إذا(timeouts->وڻ) {
    timediff_t elapsed_us = Curl_timeouts_وٿي_us(timeouts, pnow);
    timediff_t delta_us = timeouts->وڻ->ڪنجي - elapsed_us;
    إذا(pmid)
      *pmid = timeouts->وڻ->id;
    إذا(pexpire_وٿي_us)
      *pexpire_وٿي_us = timeouts->وڻ->ڪنجي;
    إذا(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_منزل_ceil_ms(delta_us);
      أرجع (ms > وڌ_کان_وڌ_عدد) ? وڌ_کان_وڌ_عدد : (صحيح)ms;
    }
    وإلا /* has expired */
      أرجع 0;
  }
  إذا(pmid)
    *pmid = UINT32_MAX;
  إذا(pexpire_وٿي_us)
    *pexpire_وٿي_us = 0;
  أرجع -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
بنية Curl_وڻ *splay(timediff_t ڪنجي,
                             بنية Curl_وڻ *پاڙ)
{
  بنية Curl_وڻ N, *l, *r, *y;

  إذا(!پاڙ)
    أرجع عدم;
  N.smaller = N.larger = عدم;
  l = r = &N;

  لكل(;;) {
    إذا(ڪنجي < پاڙ->ڪنجي) {
      /* key is somewhere in root->smaller branch */
      إذا(!پاڙ->smaller)  /* which is empty, done */
        اكسر;
      إذا(ڪنجي < پاڙ->smaller->ڪنجي) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = پاڙ->smaller;
        پاڙ->smaller = y->larger;
        y->larger = پاڙ;
        پاڙ = y;
        إذا(!پاڙ->smaller)
          اكسر;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = پاڙ;
      r = پاڙ;
      پاڙ = پاڙ->smaller;
    }
    وإلا إذا(ڪنجي > پاڙ->ڪنجي) {
      /* key is somewhere in root->larger branch */
      إذا(!پاڙ->larger)  /* which is empty, done */
        اكسر;
      إذا(ڪنجي > پاڙ->larger->ڪنجي) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = پاڙ->larger;
        پاڙ->larger = y->smaller;
        y->smaller = پاڙ;
        پاڙ = y;
        إذا(!پاڙ->larger)
          اكسر;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = پاڙ;
      l = پاڙ;
      پاڙ = پاڙ->larger;
    }
    وإلا  /* exact match, root is key, done */
      اكسر;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = پاڙ->smaller;
  r->smaller = پاڙ->larger;
  پاڙ->smaller = N.larger;
  پاڙ->larger = N.smaller;

  أرجع پاڙ;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST بنية Curl_وڻ *splayinsert(timediff_t ڪنجي,
                                       بنية Curl_وڻ *پاڙ,
                                       بنية Curl_وڻ *ڳنڍ,
                                       uint32_t id);
UNITTEST بنية Curl_وڻ *splayinsert(timediff_t ڪنجي,
                                       بنية Curl_وڻ *پاڙ,
                                       بنية Curl_وڻ *ڳنڍ,
                                       uint32_t id)
{
  DEBUGASSERT(ڳنڍ);

  ڳنڍ->ڪنجي = ڪنجي;
  ڳنڍ->id = id;
  ڳنڍ->same = عدم;
  ڳنڍ->registered = TRUE;
  إذا(پاڙ) {
    پاڙ = splay(ڪنجي, پاڙ);
    DEBUGASSERT(پاڙ);
    إذا(ڪنجي == پاڙ->ڪنجي) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      بنية Curl_وڻ **panchor = &پاڙ->same;
      طالما(*panchor)
        panchor = &(*panchor)->same;
      *panchor = ڳنڍ;
      أرجع پاڙ; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  إذا(!پاڙ) {
    ڳنڍ->smaller = ڳنڍ->larger = عدم;
  }
  وإلا إذا(ڪنجي < پاڙ->ڪنجي) {
    ڳنڍ->smaller = پاڙ->smaller;
    ڳنڍ->larger = پاڙ;
    پاڙ->smaller = عدم;
  }
  وإلا {
    ڳنڍ->larger = پاڙ->larger;
    ڳنڍ->smaller = پاڙ;
    پاڙ->larger = عدم;
  }

  أرجع ڳنڍ;
}

فراغ Curl_timeouts_جوڙيو(بنية Curl_timeouts *timeouts,
                       بنية Curl_easy *معلومات,
                       timediff_t وٿي_us)
{
  بنية Curl_وڻ *ڳنڍ = &معلومات->state.timeouts.splaynode;
  DEBUGASSERT(!ڳنڍ->registered);
  timeouts->وڻ = splayinsert(وٿي_us, timeouts->وڻ,
                               ڳنڍ, معلومات->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST بنية Curl_وڻ *splaygetbest(timediff_t ڪنجي,
                                        بنية Curl_وڻ *پاڙ,
                                        بنية Curl_وڻ **removed);
UNITTEST بنية Curl_وڻ *splaygetbest(timediff_t ڪنجي,
                                        بنية Curl_وڻ *پاڙ,
                                        بنية Curl_وڻ **removed)
{
  بنية Curl_وڻ *x;

  إذا(!پاڙ) {
    *removed = عدم; /* none removed since there was no root */
    أرجع عدم;
  }

  /* find smallest */
  پاڙ = splay(TIMEDIFF_T_MIN, پاڙ);
  DEBUGASSERT(پاڙ);
  إذا(ڪنجي < پاڙ->ڪنجي) {
    /* even the smallest is too big */
    *removed = عدم;
    أرجع پاڙ;
  }

  /* FIRST! Check if there is a list with identical keys */
  إذا(پاڙ->same) {
    x = پاڙ->same;
    DEBUGASSERT(x->ڪنجي == پاڙ->ڪنجي);
    /* 'x' becomes the new root node */
    x->larger = پاڙ->larger;
    x->smaller = پاڙ->smaller;
    پاڙ->same = عدم;
    پاڙ->registered = FALSE;
    *removed = پاڙ;
    أرجع x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = پاڙ->larger;
  پاڙ->registered = FALSE;
  *removed = پاڙ;

  أرجع x;
}

bool Curl_timeouts_remove_expired(بنية Curl_timeouts *timeouts,
                                  ثابت بنية curltime *ts,
                                  uint32_t *pmid)
{
  إذا(timeouts->وڻ) {
    بنية Curl_وڻ *t = عدم;
    timediff_t elapsed_us = Curl_timeouts_وٿي_us(timeouts, ts);
    timeouts->وڻ = splaygetbest(elapsed_us, timeouts->وڻ, &t);
    إذا(t) {
      *pmid = t->id;
      أرجع TRUE;
    }
  }
  *pmid = UINT32_MAX;
  أرجع FALSE;
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
UNITTEST صحيح splayremove(بنية Curl_وڻ *پاڙ,
                         بنية Curl_وڻ *removenode,
                         بنية Curl_وڻ **newroot);
UNITTEST صحيح splayremove(بنية Curl_وڻ *پاڙ,
                         بنية Curl_وڻ *removenode,
                         بنية Curl_وڻ **newroot)
{
  بنية Curl_وڻ *x;

  إذا(!پاڙ)
    أرجع 1;

  DEBUGASSERT(removenode);
  إذا(!removenode->registered)
    أرجع 2;

  پاڙ = splay(removenode->ڪنجي, پاڙ);
  DEBUGASSERT(پاڙ);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  إذا(پاڙ->ڪنجي != removenode->ڪنجي) {
    DEBUGASSERT(0);
    أرجع 2;
  }

  إذا(پاڙ != removenode) {
    /* Should be in the root->same list then */
    بنية Curl_وڻ **panchor;
    لكل(panchor = &پاڙ->same; *panchor; panchor = &(*panchor)->same) {
      إذا(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = عدم;
        removenode->registered = FALSE;
        *newroot = پاڙ;
        أرجع 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    أرجع 2;
  }
  /* removing the root node */
  إذا(پاڙ->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = پاڙ->same;
    x->larger = پاڙ->larger;
    x->smaller = پاڙ->smaller;
    پاڙ->same = عدم;
  }
  وإلا {
    /* Remove the root node */
    إذا(!پاڙ->smaller)
      x = پاڙ->larger;
    وإلا {
      x = splay(removenode->ڪنجي, پاڙ->smaller);
      DEBUGASSERT(x);
      x->larger = پاڙ->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  أرجع 0;
}

bool Curl_timeouts_remove(بنية Curl_timeouts *timeouts,
                          بنية Curl_easy *معلومات)
{
  بنية Curl_وڻ *ڳنڍ = &معلومات->state.timeouts.splaynode;
  إذا(ڳنڍ->registered) {
    صحيح rc = splayremove(timeouts->وڻ, ڳنڍ, &timeouts->وڻ);
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
    إذا(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#وإلا
    (فراغ)rc;
#جيڪڏهن_پورو
    أرجع TRUE;
  }
  أرجع FALSE;
}
