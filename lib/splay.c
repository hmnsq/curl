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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "splay.h"

ساکن ساخت Curl_درخت *splay(timediff_t کلید,
                               ساخت Curl_درخت *جڑ);

#اگر_تعریف_شدہ UNITTESTS
uint32_t Curl_splayget(ساخت Curl_درخت *گرہ)
{
  DEBUGASSERT(گرہ);
  واپس گرہ->id;
}

#اگر_ختم

خالی Curl_timeouts_init(ساخت Curl_timeouts *timeouts,
                        ثابت ساخت curltime *ptime_بنیاد)
{
  ساخت curltime وقت_بنیاد = ptime_بنیاد ? *ptime_بنیاد : curlx_now();
  timeouts->درخت = تہی;
  timeouts->وقت_بنیاد_sec = وقت_بنیاد.tv_sec;
}

bool Curl_timeouts_has(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_درخت *گرہ = ڈیٹا ? &ڈیٹا->state.timeouts.splaynode : تہی;
  واپس گرہ && گرہ->registered;
}

timediff_t Curl_timeouts_انحراف_us(ساخت Curl_timeouts *timeouts,
                                   ثابت ساخت curltime *pts)
{
  ساخت curltime وقت_بنیاد;
  وقت_بنیاد.tv_sec = timeouts->وقت_بنیاد_sec;
  وقت_بنیاد.tv_usec = 0;
  واپس curlx_ptimediff_us(pts, &وقت_بنیاد);
}

صحیح Curl_timeouts_اگلا_ms(ساخت Curl_timeouts *timeouts,
                          ثابت ساخت curltime *pnow,
                          timediff_t *pexpire_انحراف_us,
                          uint32_t *pmid)
{
  اگر(timeouts->درخت) { /* splay the lowest key to the root */
    timeouts->درخت = splay(TIMEDIFF_T_MIN, timeouts->درخت);
  }

  اگر(timeouts->درخت) {
    timediff_t elapsed_us = Curl_timeouts_انحراف_us(timeouts, pnow);
    timediff_t delta_us = timeouts->درخت->کلید - elapsed_us;
    اگر(pmid)
      *pmid = timeouts->درخت->id;
    اگر(pexpire_انحراف_us)
      *pexpire_انحراف_us = timeouts->درخت->کلید;
    اگر(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_مقصد_ceil_ms(delta_us);
      واپس (ms > زیادہ_سے_زیادہ_عدد) ? زیادہ_سے_زیادہ_عدد : (صحیح)ms;
    }
    ورنہ /* has expired */
      واپس 0;
  }
  اگر(pmid)
    *pmid = UINT32_MAX;
  اگر(pexpire_انحراف_us)
    *pexpire_انحراف_us = 0;
  واپس -1;
}

/*
 * Splay using the key i (which may or may not be in the tree).
 * This rotates the tree, so:
 * - root->smaller has all nodes smaller than `key`
 * - root->larger has all nodes larger than `key`
 * - root->key may equal `key` or not
 * <https://en.wikipedia.org/wiki/Splay_tree>
 */
ساخت Curl_درخت *splay(timediff_t کلید,
                             ساخت Curl_درخت *جڑ)
{
  ساخت Curl_درخت N, *l, *r, *y;

  اگر(!جڑ)
    واپس تہی;
  N.smaller = N.larger = تہی;
  l = r = &N;

  برائے(;;) {
    اگر(کلید < جڑ->کلید) {
      /* key is somewhere in root->smaller branch */
      اگر(!جڑ->smaller)  /* which is empty, done */
        توڑو;
      اگر(کلید < جڑ->smaller->کلید) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = جڑ->smaller;
        جڑ->smaller = y->larger;
        y->larger = جڑ;
        جڑ = y;
        اگر(!جڑ->smaller)
          توڑو;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = جڑ;
      r = جڑ;
      جڑ = جڑ->smaller;
    }
    ورنہ اگر(کلید > جڑ->کلید) {
      /* key is somewhere in root->larger branch */
      اگر(!جڑ->larger)  /* which is empty, done */
        توڑو;
      اگر(کلید > جڑ->larger->کلید) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = جڑ->larger;
        جڑ->larger = y->smaller;
        y->smaller = جڑ;
        جڑ = y;
        اگر(!جڑ->larger)
          توڑو;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = جڑ;
      l = جڑ;
      جڑ = جڑ->larger;
    }
    ورنہ  /* exact match, root is key, done */
      توڑو;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = جڑ->smaller;
  r->smaller = جڑ->larger;
  جڑ->smaller = N.larger;
  جڑ->larger = N.smaller;

  واپس جڑ;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST ساخت Curl_درخت *splayinsert(timediff_t کلید,
                                       ساخت Curl_درخت *جڑ,
                                       ساخت Curl_درخت *گرہ,
                                       uint32_t id);
UNITTEST ساخت Curl_درخت *splayinsert(timediff_t کلید,
                                       ساخت Curl_درخت *جڑ,
                                       ساخت Curl_درخت *گرہ,
                                       uint32_t id)
{
  DEBUGASSERT(گرہ);

  گرہ->کلید = کلید;
  گرہ->id = id;
  گرہ->same = تہی;
  گرہ->registered = TRUE;
  اگر(جڑ) {
    جڑ = splay(کلید, جڑ);
    DEBUGASSERT(جڑ);
    اگر(کلید == جڑ->کلید) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      ساخت Curl_درخت **panchor = &جڑ->same;
      جب(*panchor)
        panchor = &(*panchor)->same;
      *panchor = گرہ;
      واپس جڑ; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  اگر(!جڑ) {
    گرہ->smaller = گرہ->larger = تہی;
  }
  ورنہ اگر(کلید < جڑ->کلید) {
    گرہ->smaller = جڑ->smaller;
    گرہ->larger = جڑ;
    جڑ->smaller = تہی;
  }
  ورنہ {
    گرہ->larger = جڑ->larger;
    گرہ->smaller = جڑ;
    جڑ->larger = تہی;
  }

  واپس گرہ;
}

خالی Curl_timeouts_جوڑو(ساخت Curl_timeouts *timeouts,
                       ساخت Curl_easy *ڈیٹا,
                       timediff_t انحراف_us)
{
  ساخت Curl_درخت *گرہ = &ڈیٹا->state.timeouts.splaynode;
  DEBUGASSERT(!گرہ->registered);
  timeouts->درخت = splayinsert(انحراف_us, timeouts->درخت,
                               گرہ, ڈیٹا->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST ساخت Curl_درخت *splaygetbest(timediff_t کلید,
                                        ساخت Curl_درخت *جڑ,
                                        ساخت Curl_درخت **removed);
UNITTEST ساخت Curl_درخت *splaygetbest(timediff_t کلید,
                                        ساخت Curl_درخت *جڑ,
                                        ساخت Curl_درخت **removed)
{
  ساخت Curl_درخت *x;

  اگر(!جڑ) {
    *removed = تہی; /* none removed since there was no root */
    واپس تہی;
  }

  /* find smallest */
  جڑ = splay(TIMEDIFF_T_MIN, جڑ);
  DEBUGASSERT(جڑ);
  اگر(کلید < جڑ->کلید) {
    /* even the smallest is too big */
    *removed = تہی;
    واپس جڑ;
  }

  /* FIRST! Check if there is a list with identical keys */
  اگر(جڑ->same) {
    x = جڑ->same;
    DEBUGASSERT(x->کلید == جڑ->کلید);
    /* 'x' becomes the new root node */
    x->larger = جڑ->larger;
    x->smaller = جڑ->smaller;
    جڑ->same = تہی;
    جڑ->registered = FALSE;
    *removed = جڑ;
    واپس x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = جڑ->larger;
  جڑ->registered = FALSE;
  *removed = جڑ;

  واپس x;
}

bool Curl_timeouts_remove_expired(ساخت Curl_timeouts *timeouts,
                                  ثابت ساخت curltime *ts,
                                  uint32_t *pmid)
{
  اگر(timeouts->درخت) {
    ساخت Curl_درخت *t = تہی;
    timediff_t elapsed_us = Curl_timeouts_انحراف_us(timeouts, ts);
    timeouts->درخت = splaygetbest(elapsed_us, timeouts->درخت, &t);
    اگر(t) {
      *pmid = t->id;
      واپس TRUE;
    }
  }
  *pmid = UINT32_MAX;
  واپس FALSE;
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
UNITTEST صحیح splayremove(ساخت Curl_درخت *جڑ,
                         ساخت Curl_درخت *removenode,
                         ساخت Curl_درخت **newroot);
UNITTEST صحیح splayremove(ساخت Curl_درخت *جڑ,
                         ساخت Curl_درخت *removenode,
                         ساخت Curl_درخت **newroot)
{
  ساخت Curl_درخت *x;

  اگر(!جڑ)
    واپس 1;

  DEBUGASSERT(removenode);
  اگر(!removenode->registered)
    واپس 2;

  جڑ = splay(removenode->کلید, جڑ);
  DEBUGASSERT(جڑ);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  اگر(جڑ->کلید != removenode->کلید) {
    DEBUGASSERT(0);
    واپس 2;
  }

  اگر(جڑ != removenode) {
    /* Should be in the root->same list then */
    ساخت Curl_درخت **panchor;
    برائے(panchor = &جڑ->same; *panchor; panchor = &(*panchor)->same) {
      اگر(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = تہی;
        removenode->registered = FALSE;
        *newroot = جڑ;
        واپس 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    واپس 2;
  }
  /* removing the root node */
  اگر(جڑ->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = جڑ->same;
    x->larger = جڑ->larger;
    x->smaller = جڑ->smaller;
    جڑ->same = تہی;
  }
  ورنہ {
    /* Remove the root node */
    اگر(!جڑ->smaller)
      x = جڑ->larger;
    ورنہ {
      x = splay(removenode->کلید, جڑ->smaller);
      DEBUGASSERT(x);
      x->larger = جڑ->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  واپس 0;
}

bool Curl_timeouts_remove(ساخت Curl_timeouts *timeouts,
                          ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_درخت *گرہ = &ڈیٹا->state.timeouts.splaynode;
  اگر(گرہ->registered) {
    صحیح rc = splayremove(timeouts->درخت, گرہ, &timeouts->درخت);
#اگر_تعریف_شدہ DEBUGBUILD
    اگر(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#ورنہ
    (خالی)rc;
#اگر_ختم
    واپس TRUE;
  }
  واپس FALSE;
}
