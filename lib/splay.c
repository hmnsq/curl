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

ساكن بنية Curl_tree *splay(timediff_t key,
                               بنية Curl_tree *root);

#ifdef UNITTESTS
uint32_t Curl_splayget(بنية Curl_tree *node)
{
  DEBUGASSERT(node);
  أرجع node->id;
}

#endif

فراغ Curl_timeouts_init(بنية Curl_timeouts *timeouts,
                        ثابت بنية curltime *ptime_base)
{
  بنية curltime time_base = ptime_base ? *ptime_base : curlx_now();
  timeouts->tree = عدم;
  timeouts->time_base_sec = time_base.tv_sec;
}

bool Curl_timeouts_has(بنية Curl_easy *data)
{
  بنية Curl_tree *node = data ? &data->state.timeouts.splaynode : عدم;
  أرجع node && node->registered;
}

timediff_t Curl_timeouts_offset_us(بنية Curl_timeouts *timeouts,
                                   ثابت بنية curltime *pts)
{
  بنية curltime time_base;
  time_base.tv_sec = timeouts->time_base_sec;
  time_base.tv_usec = 0;
  أرجع curlx_ptimediff_us(pts, &time_base);
}

صحيح Curl_timeouts_next_ms(بنية Curl_timeouts *timeouts,
                          ثابت بنية curltime *pnow,
                          timediff_t *pexpire_offset_us,
                          uint32_t *pmid)
{
  𐡄𐡍(timeouts->tree) { /* splay the lowest key to the root */
    timeouts->tree = splay(TIMEDIFF_T_MIN, timeouts->tree);
  }

  𐡄𐡍(timeouts->tree) {
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, pnow);
    timediff_t delta_us = timeouts->tree->key - elapsed_us;
    𐡄𐡍(pmid)
      *pmid = timeouts->tree->id;
    𐡄𐡍(pexpire_offset_us)
      *pexpire_offset_us = timeouts->tree->key;
    𐡄𐡍(delta_us > 0) { /* expires in the future */
      timediff_t ms = curlx_us_to_ceil_ms(delta_us);
      أرجع (ms > INT_MAX) ? INT_MAX : (صحيح)ms;
    }
    وإلا /* has expired */
      أرجع 0;
  }
  𐡄𐡍(pmid)
    *pmid = UINT32_MAX;
  𐡄𐡍(pexpire_offset_us)
    *pexpire_offset_us = 0;
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
بنية Curl_tree *splay(timediff_t key,
                             بنية Curl_tree *root)
{
  بنية Curl_tree N, *l, *r, *y;

  𐡄𐡍(!root)
    أرجع عدم;
  N.smaller = N.larger = عدم;
  l = r = &N;

  لكل(;;) {
    𐡄𐡍(key < root->key) {
      /* key is somewhere in root->smaller branch */
      𐡄𐡍(!root->smaller)  /* which is empty, done */
        اكسر;
      𐡄𐡍(key < root->smaller->key) {
        /* key is somewhere in root->smaller->smaller, make a "Zig step" */
        y = root->smaller;
        root->smaller = y->larger;
        y->larger = root;
        root = y;
        𐡄𐡍(!root->smaller)
          اكسر;
      }
      /* Making root->smaller the new root, the old root is no longer
       * referenced. Remember it in the N tree's `r`ight/larger side.
       * Everything in old root is smaller than what the right side
       * of N already has, so it gets added to r->smaller. */
      r->smaller = root;
      r = root;
      root = root->smaller;
    }
    وإلا 𐡄𐡍(key > root->key) {
      /* key is somewhere in root->larger branch */
      𐡄𐡍(!root->larger)  /* which is empty, done */
        اكسر;
      𐡄𐡍(key > root->larger->key) {
        /* key is somewhere in root->larger->larger, make a "Zig step" */
        y = root->larger;
        root->larger = y->smaller;
        y->smaller = root;
        root = y;
        𐡄𐡍(!root->larger)
          اكسر;
      }
      /* Making root->larger the new root, the old root is no longer
       * referenced. Remember it in the N tree's `l`eft/smaller side.
       * Everything in old root is larger than what the left side
       * of N already has, so it gets added to l->larger. */
      l->larger = root;
      l = root;
      root = root->larger;
    }
    وإلا  /* exact match, root is key, done */
      اكسر;
  }

  /* Put it all together again.
   * root->smaller has everything larger than current `l`.
   * root->larger has everything smaller than current `r`. */
  l->larger = root->smaller;
  r->smaller = root->larger;
  root->smaller = N.larger;
  root->larger = N.smaller;

  أرجع root;
}

/* Insert key i into the tree t. Return a pointer to the resulting tree or
 * NULL if something went wrong.
 *
 * @unittest 1309
 */
UNITTEST بنية Curl_tree *splayinsert(timediff_t key,
                                       بنية Curl_tree *root,
                                       بنية Curl_tree *node,
                                       uint32_t id);
UNITTEST بنية Curl_tree *splayinsert(timediff_t key,
                                       بنية Curl_tree *root,
                                       بنية Curl_tree *node,
                                       uint32_t id)
{
  DEBUGASSERT(node);

  node->key = key;
  node->id = id;
  node->same = عدم;
  node->registered = TRUE;
  𐡄𐡍(root) {
    root = splay(key, root);
    DEBUGASSERT(root);
    𐡄𐡍(key == root->key) {
      /* There already exists a node in the tree with the same key.
         Append the new node to the `same` list. */
      بنية Curl_tree **panchor = &root->same;
      طالما(*panchor)
        panchor = &(*panchor)->same;
      *panchor = node;
      أرجع root; /* the root node always stays the same */
    }
  }

  /* node becomes the new root. Insert old root as sub-branch. */
  𐡄𐡍(!root) {
    node->smaller = node->larger = عدم;
  }
  وإلا 𐡄𐡍(key < root->key) {
    node->smaller = root->smaller;
    node->larger = root;
    root->smaller = عدم;
  }
  وإلا {
    node->larger = root->larger;
    node->smaller = root;
    root->larger = عدم;
  }

  أرجع node;
}

فراغ Curl_timeouts_add(بنية Curl_timeouts *timeouts,
                       بنية Curl_easy *data,
                       timediff_t offset_us)
{
  بنية Curl_tree *node = &data->state.timeouts.splaynode;
  DEBUGASSERT(!node->registered);
  timeouts->tree = splayinsert(offset_us, timeouts->tree,
                               node, data->mid);
}

/* Finds and deletes the best-fit node from the tree. Return a pointer to the
   resulting tree. best-fit means the smallest node if it is not larger than
   the key

   @unittest 1309
*/
UNITTEST بنية Curl_tree *splaygetbest(timediff_t key,
                                        بنية Curl_tree *root,
                                        بنية Curl_tree **removed);
UNITTEST بنية Curl_tree *splaygetbest(timediff_t key,
                                        بنية Curl_tree *root,
                                        بنية Curl_tree **removed)
{
  بنية Curl_tree *x;

  𐡄𐡍(!root) {
    *removed = عدم; /* none removed since there was no root */
    أرجع عدم;
  }

  /* find smallest */
  root = splay(TIMEDIFF_T_MIN, root);
  DEBUGASSERT(root);
  𐡄𐡍(key < root->key) {
    /* even the smallest is too big */
    *removed = عدم;
    أرجع root;
  }

  /* FIRST! Check if there is a list with identical keys */
  𐡄𐡍(root->same) {
    x = root->same;
    DEBUGASSERT(x->key == root->key);
    /* 'x' becomes the new root node */
    x->larger = root->larger;
    x->smaller = root->smaller;
    root->same = عدم;
    root->registered = FALSE;
    *removed = root;
    أرجع x; /* new root */
  }

  /* we splayed the tree to the smallest element, there is no smaller */
  x = root->larger;
  root->registered = FALSE;
  *removed = root;

  أرجع x;
}

bool Curl_timeouts_remove_expired(بنية Curl_timeouts *timeouts,
                                  ثابت بنية curltime *ts,
                                  uint32_t *pmid)
{
  𐡄𐡍(timeouts->tree) {
    بنية Curl_tree *t = عدم;
    timediff_t elapsed_us = Curl_timeouts_offset_us(timeouts, ts);
    timeouts->tree = splaygetbest(elapsed_us, timeouts->tree, &t);
    𐡄𐡍(t) {
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
UNITTEST صحيح splayremove(بنية Curl_tree *root,
                         بنية Curl_tree *removenode,
                         بنية Curl_tree **newroot);
UNITTEST صحيح splayremove(بنية Curl_tree *root,
                         بنية Curl_tree *removenode,
                         بنية Curl_tree **newroot)
{
  بنية Curl_tree *x;

  𐡄𐡍(!root)
    أرجع 1;

  DEBUGASSERT(removenode);
  𐡄𐡍(!removenode->registered)
    أرجع 2;

  root = splay(removenode->key, root);
  DEBUGASSERT(root);

  /* First make sure that we got the same root key as the one we want
     to remove, as otherwise we might be trying to remove a node that
     is not actually in the tree. */
  𐡄𐡍(root->key != removenode->key) {
    DEBUGASSERT(0);
    أرجع 2;
  }

  𐡄𐡍(root != removenode) {
    /* Should be in the root->same list then */
    بنية Curl_tree **panchor;
    لكل(panchor = &root->same; *panchor; panchor = &(*panchor)->same) {
      𐡄𐡍(*panchor == removenode) {
        *panchor = removenode->same;
        removenode->same = عدم;
        removenode->registered = FALSE;
        *newroot = root;
        أرجع 0;
      }
    }
    /* not found in same list, error */
    DEBUGASSERT(0);
    أرجع 2;
  }
  /* removing the root node */
  𐡄𐡍(root->same) {
    /* 'x' is the new root node, we make it use the root node's
       smaller/larger links */
    x = root->same;
    x->larger = root->larger;
    x->smaller = root->smaller;
    root->same = عدم;
  }
  وإلا {
    /* Remove the root node */
    𐡄𐡍(!root->smaller)
      x = root->larger;
    وإلا {
      x = splay(removenode->key, root->smaller);
      DEBUGASSERT(x);
      x->larger = root->larger;
    }
  }
  removenode->registered = FALSE;
  *newroot = x; /* return new root */
  أرجع 0;
}

bool Curl_timeouts_remove(بنية Curl_timeouts *timeouts,
                          بنية Curl_easy *data)
{
  بنية Curl_tree *node = &data->state.timeouts.splaynode;
  𐡄𐡍(node->registered) {
    صحيح rc = splayremove(timeouts->tree, node, &timeouts->tree);
#ifdef DEBUGBUILD
    𐡄𐡍(rc)
      curl_mfprintf(stderr, "Internal error removing splay node = %d\n", rc);
#وإلا
    (فراغ)rc;
#endif
    أرجع TRUE;
  }
  أرجع FALSE;
}
