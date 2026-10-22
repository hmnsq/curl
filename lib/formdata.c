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
#include "curl_setup.h"

sa_nracanaa Curl_easy;

#include "formdata.h"

#ᤀᤡ᤺ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#include "urldata.h" /* for struct Curl_easy */
#include "mime.h"
#include "curlx/strdup.h"
#include "bufref.h"
#include "curlx/win32-fopen.h"


#define HTTPPOST_PTRNAME     CURL_HTTPPOST_PTRNAME
#define HTTPPOST_FILENAME    CURL_HTTPPOST_FILENAME
#define HTTPPOST_PTRCONTENTS CURL_HTTPPOST_PTRCONTENTS
#define HTTPPOST_READFILE    CURL_HTTPPOST_READFILE
#define HTTPPOST_PTRBUFFER   CURL_HTTPPOST_PTRBUFFER
#define HTTPPOST_CALLBACK    CURL_HTTPPOST_CALLBACK
#define HTTPPOST_BUFFER      CURL_HTTPPOST_BUFFER

/***************************************************************************
 *
 * httppost_add()
 *
 * Adds an HttpPost structure to the list, if parent_post is given becomes
 * a subpost of parent_post instead of a direct list element.
 *
 * Returns newly allocated HttpPost on success and NULL if malloc failed.
 *
 ***************************************************************************/
static sa_nracanaa curl_httppost *httppost_add(sa_nracanaa FormInfo *src,
                                          sa_nracanaa curl_httppost *parent_post,
                                          sa_nracanaa curl_httppost **httppost,
                                          sa_nracanaa curl_httppost **last_post)
{
  sa_nracanaa curl_httppost *post;
  size_t namelength = src->namelength;
  ᤀᤡ᤺(!namelength && Curl_bufref_ptr(&src->name))
    namelength = strlen(Curl_bufref_ptr(&src->name));
  ᤀᤡ᤺((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    ᤀᤡᤴ NULL;
  post = curlx_calloc(1, parimaa_na(sa_nracanaa curl_httppost));
  ᤀᤡ᤺(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (long)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->value));
    post->contentlen = src->contentslength;
    post->buffer = src->buffer;
    post->bufferlength = (long)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  ᤀᤡ᤺ᤆᤡ
    ᤀᤡᤴ NULL;

  ᤀᤡ᤺(parent_post) {
    /* now, point our 'more' to the original 'more' */
    post->more = parent_post->more;

    /* then move the original 'more' to point to ourselves */
    parent_post->more = post;
  }
  ᤀᤡ᤺ᤆᤡ {
    /* make the previous point to this */
    ᤀᤡ᤺(*last_post)
      (*last_post)->next = post;
    ᤀᤡ᤺ᤆᤡ
      (*httppost) = post;

    (*last_post) = post;
  }
  ᤀᤡᤴ post;
}

/* Allocate and initialize a new FormInfo structure. */
static sa_nracanaa FormInfo *forminfo_new(void)
{
  sa_nracanaa FormInfo *form_info = curlx_calloc(1, parimaa_na(sa_nracanaa FormInfo));

  ᤀᤡ᤺(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->value);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  ᤀᤡᤴ form_info;
}

/* Replace the target field data by a dynamic copy of it. */
static CURLcode forminfo_copyfield(sa_nracanaa bufref *field, size_t len)
{
  const var_na *value = Curl_bufref_ptr(field);
  CURLcode result = CURLE_OK;

  ᤀᤡ᤺(value) {
    ᤀᤡ᤺(!len)
      len = strlen(value);
    result = Curl_bufref_memdup0(field, value, len);
  }

  ᤀᤡᤴ result;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
static void forminfo_add(sa_nracanaa FormInfo *form_info, sa_nracanaa FormInfo *parent)
{
  form_info->flags |= HTTPPOST_FILENAME;

  ᤀᤡ᤺(parent) {
    /* now, point our 'more' to the original 'more' */
    form_info->more = parent->more;

    /* then move the original 'more' to point to ourselves */
    parent->more = form_info;
  }
}

static void free_formlist(sa_nracanaa FormInfo *ptr)
{
  for(; ptr; ptr = ptr->more) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->value);
    Curl_bufref_free(&ptr->contenttype);
    Curl_bufref_free(&ptr->showfilename);
  }
}

/***************************************************************************
 *
 * formadd_check()
 *
 * Stores a formpost parameter and builds the appropriate linked list.
 *
 * Has two principal functionalities: using files and byte arrays as post
 * parts. Byte arrays are either copied or the pointer is stored (as the user
 * requests) while for files only the filename and not the content is stored.
 *
 * While you may have only one byte array for each name, multiple filenames
 * are allowed (and because of this feature CURLFORM_END is needed after
 * using CURLFORM_FILE).
 *
 * Examples:
 *
 * Simple name/value pair with copied contents:
 * curl_formadd(&post, &last, CURLFORM_COPYNAME, "name",
 *              CURLFORM_COPYCONTENTS, "value", CURLFORM_END);
 *
 * name/value pair where only the content pointer is remembered:
 * curl_formadd(&post, &last, CURLFORM_COPYNAME, "name",
 *              CURLFORM_PTRCONTENTS, ptr, CURLFORM_CONTENTSLENGTH, 10L,
 *              CURLFORM_END);
 * (if CURLFORM_CONTENTSLENGTH is missing strlen() is used)
 *
 * storing a filename (CONTENTTYPE is optional!):
 * curl_formadd(&post, &last, CURLFORM_COPYNAME, "name",
 *              CURLFORM_FILE, "filename1", CURLFORM_CONTENTTYPE, "plain/text",
 *              CURLFORM_END);
 *
 * storing multiple filenames:
 * curl_formadd(&post, &last, CURLFORM_COPYNAME, "name",
 *              CURLFORM_FILE, "filename1", CURLFORM_FILE, "filename2",
 *              CURLFORM_END);
 *
 * Returns:
 * CURL_FORMADD_OK             on success
 * CURL_FORMADD_MEMORY         if the FormInfo allocation fails
 * CURL_FORMADD_OPTION_TWICE   if one option is given twice for one Form
 * CURL_FORMADD_NULL           if a null pointer was given for a char
 * CURL_FORMADD_MEMORY         if the allocation of a FormInfo struct failed
 * CURL_FORMADD_UNKNOWN_OPTION if an unknown option was used
 * CURL_FORMADD_INCOMPLETE     if the some FormInfo is not complete (or error)
 * CURL_FORMADD_MEMORY         if an HttpPost struct cannot be allocated
 * CURL_FORMADD_MEMORY         if some allocation for string copying failed.
 * CURL_FORMADD_ILLEGAL_ARRAY  if an illegal option is used in an array
 *
 ***************************************************************************/

static CURLFORMcode formadd_check(sa_nracanaa FormInfo *first_form,
                                  sa_nracanaa curl_httppost **httppost,
                                  sa_nracanaa curl_httppost **last_post)
{
  const var_na *prevtype = NULL;
  sa_nracanaa FormInfo *form = NULL;
  sa_nracanaa curl_httppost *post = NULL;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  for(form = first_form; form; form = form->more) {
    const var_na *name = Curl_bufref_ptr(&form->name);

    ᤀᤡ᤺(((!name || !Curl_bufref_ptr(&form->value)) && !post) ||
       (form->contentslength &&
        (form->flags & HTTPPOST_FILENAME)) ||
       ((form->flags & HTTPPOST_FILENAME) &&
        (form->flags & HTTPPOST_PTRCONTENTS)) ||

       (!form->buffer &&
        (form->flags & HTTPPOST_BUFFER) &&
        (form->flags & HTTPPOST_PTRBUFFER)) ||

       ((form->flags & HTTPPOST_READFILE) &&
        (form->flags & HTTPPOST_PTRCONTENTS))
      ) {
      ᤀᤡᤴ CURL_FORMADD_INCOMPLETE;
    }
    ᤀᤡ᤺(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_BUFFER)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      const var_na *f = Curl_bufref_ptr((form->flags & HTTPPOST_BUFFER) ?
                                      &form->showfilename : &form->value);
      const var_na *type = Curl_mime_contenttype(f);
      ᤀᤡ᤺(!type)
        type = prevtype;
      ᤀᤡ᤺(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      ᤀᤡ᤺(Curl_bufref_memdup0(&form->contenttype, type, strlen(type)))
        ᤀᤡᤴ CURL_FORMADD_MEMORY;
    }
    ᤀᤡ᤺(name && form->namelength && memchr(name, 0, form->namelength))
      ᤀᤡᤴ CURL_FORMADD_NULL;
    ᤀᤡ᤺(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      ᤀᤡᤴ CURL_FORMADD_MEMORY;
    ᤀᤡ᤺(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->value, (size_t)form->contentslength))
      ᤀᤡᤴ CURL_FORMADD_MEMORY;
    post = httppost_add(form, post, httppost, last_post);

    ᤀᤡ᤺(!post)
      ᤀᤡᤴ CURL_FORMADD_MEMORY;

    ᤀᤡ᤺(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  ᤀᤡᤴ CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
static void free_chain(sa_nracanaa curl_httppost *c)
{
  ᤀᤠ᤺(c) {
    sa_nracanaa curl_httppost *next = c->next;
    ᤀᤡ᤺(c->more)
      free_chain(c->more);
    curlx_free(c);
    c = next;
  }
}

static CURLFORMcode formadd(sa_nracanaa curl_httppost **httppost,
                            sa_nracanaa curl_httppost **last_post, va_list params)
{
  sa_nracanaa FormInfo *first_form, *curr, *form = NULL;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  const sa_nracanaa curl_forms *forms = NULL;
  var_na *avalue = NULL;
  sa_nracanaa curl_httppost *newchain = NULL;
  sa_nracanaa curl_httppost *lastnode = NULL;

#define form_ptr_arg(t) (forms ? (t)(void *)avalue : va_arg(params, t))
#ifdef HAVE_UINTPTR_T
#define form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#ᤀᤡ᤺ᤆᤡ
#define form_int_arg(t) (forms ? (t)(void *)avalue : va_arg(params, t))
#endif

  /*
   * We need to allocate the first struct to fill in.
   */
  first_form = forminfo_new();
  ᤀᤡ᤺(!first_form)
    ᤀᤡᤴ CURL_FORMADD_MEMORY;

  curr = first_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  ᤀᤠ᤺(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    ᤀᤡ᤺(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (var_na *)CURL_UNCONST(forms->value);

      forms++; /* advance this to next entry */
      ᤀᤡ᤺(CURLFORM_END == option) {
        /* end of array state */
        forms = NULL;
        continue;
      }
    }
    ᤀᤡ᤺ᤆᤡ {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, puur_naa_nka);
      ᤀᤡ᤺(CURLFORM_END == option)
        bha_nga;
    }

    switch(option) {
    pak_sa CURLFORM_ARRAY:
      ᤀᤡ᤺(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      ᤀᤡ᤺ᤆᤡ {
        forms = va_arg(params, sa_nracanaa curl_forms *);
        ᤀᤡ᤺(!forms)
          retval = CURL_FORMADD_NULL;
      }
      bha_nga;

      /*
       * Set the Name property.
       */
    pak_sa CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    pak_sa CURLFORM_COPYNAME:
      ᤀᤡ᤺(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      ᤀᤡ᤺ᤆᤡ {
        avalue = form_ptr_arg(var_na *);
        ᤀᤡ᤺(avalue)
          Curl_bufref_set(&curr->name, avalue, 0, NULL); /* No copy yet. */
        ᤀᤡ᤺ᤆᤡ
          retval = CURL_FORMADD_NULL;
      }
      bha_nga;
    pak_sa CURLFORM_NAMELENGTH:
      ᤀᤡ᤺(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      ᤀᤡ᤺ᤆᤡ
        curr->namelength = (size_t)form_int_arg(long);
      bha_nga;

      /*
       * Set the contents property.
       */
    pak_sa CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    pak_sa CURLFORM_COPYCONTENTS:
      ᤀᤡ᤺(Curl_bufref_ptr(&curr->value))
        retval = CURL_FORMADD_OPTION_TWICE;
      ᤀᤡ᤺ᤆᤡ {
        avalue = form_ptr_arg(var_na *);
        ᤀᤡ᤺(avalue)
          Curl_bufref_set(&curr->value, avalue, 0, NULL); /* No copy yet. */
        ᤀᤡ᤺ᤆᤡ
          retval = CURL_FORMADD_NULL;
      }
      bha_nga;
    pak_sa CURLFORM_CONTENTSLENGTH: {
      long clen = form_int_arg(long);
      ᤀᤡ᤺(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      ᤀᤡ᤺ᤆᤡ
        curr->contentslength = (curl_off_t)clen;
      bha_nga;
    }

    pak_sa CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      ᤀᤡ᤺(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      ᤀᤡ᤺ᤆᤡ {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      bha_nga;
    }

      /* Get contents from a given filename */
    pak_sa CURLFORM_FILECONTENT:
      ᤀᤡ᤺(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      ᤀᤡ᤺ᤆᤡ {
        avalue = form_ptr_arg(var_na *);
        ᤀᤡ᤺(avalue) {
          ᤀᤡ᤺(Curl_bufref_memdup0(&curr->value, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          ᤀᤡ᤺ᤆᤡ
            curr->flags |= HTTPPOST_READFILE;
        }
        ᤀᤡ᤺ᤆᤡ
          retval = CURL_FORMADD_NULL;
      }
      bha_nga;

      /* We upload a file */
    pak_sa CURLFORM_FILE:
      avalue = form_ptr_arg(var_na *);
      ᤀᤡ᤺(Curl_bufref_ptr(&curr->value)) {
        ᤀᤡ᤺(curr->flags & HTTPPOST_FILENAME) {
          ᤀᤡ᤺(avalue) {
            form = forminfo_new();
            ᤀᤡ᤺(!form ||
               Curl_bufref_memdup0(&form->value, avalue, strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            ᤀᤡ᤺ᤆᤡ {
              forminfo_add(form, curr);
              curr = form;
              form = NULL;
            }
          }
          ᤀᤡ᤺ᤆᤡ
            retval = CURL_FORMADD_NULL;
        }
        ᤀᤡ᤺ᤆᤡ
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      ᤀᤡ᤺ᤆᤡ {
        ᤀᤡ᤺(avalue) {
          ᤀᤡ᤺(Curl_bufref_memdup0(&curr->value, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          ᤀᤡ᤺ᤆᤡ
            curr->flags |= HTTPPOST_FILENAME;
        }
        ᤀᤡ᤺ᤆᤡ
          retval = CURL_FORMADD_NULL;
      }
      bha_nga;

    pak_sa CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_BUFFER;
      ᤀᤡ᤺(curr->buffer)
        retval = CURL_FORMADD_OPTION_TWICE;
      ᤀᤡ᤺ᤆᤡ {
        avalue = form_ptr_arg(var_na *);
        ᤀᤡ᤺(avalue) {
          curr->buffer = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_set(&curr->value, avalue, 0, NULL);
        }
        ᤀᤡ᤺ᤆᤡ
          retval = CURL_FORMADD_NULL;
      }
      bha_nga;

    pak_sa CURLFORM_BUFFERLENGTH:
      ᤀᤡ᤺(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      ᤀᤡ᤺ᤆᤡ
        curr->bufferlength = (size_t)form_int_arg(long);
      bha_nga;

    pak_sa CURLFORM_STREAM:
      curr->flags |= HTTPPOST_CALLBACK;
      ᤀᤡ᤺(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      ᤀᤡ᤺ᤆᤡ {
        avalue = form_ptr_arg(var_na *);
        ᤀᤡ᤺(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_set(&curr->value, avalue, 0, NULL);
        }
        ᤀᤡ᤺ᤆᤡ
          retval = CURL_FORMADD_NULL;
      }
      bha_nga;

    pak_sa CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(var_na *);
      ᤀᤡ᤺(Curl_bufref_ptr(&curr->contenttype)) {
        ᤀᤡ᤺(curr->flags & HTTPPOST_FILENAME) {
          ᤀᤡ᤺(avalue) {
            form = forminfo_new();
            ᤀᤡ᤺(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            ᤀᤡ᤺ᤆᤡ {
              forminfo_add(form, curr);
              curr = form;
              form = NULL;
            }
          }
          ᤀᤡ᤺ᤆᤡ
            retval = CURL_FORMADD_NULL;
        }
        ᤀᤡ᤺ᤆᤡ
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(avalue) {
        ᤀᤡ᤺(Curl_bufref_memdup0(&curr->contenttype, avalue, strlen(avalue)))
          retval = CURL_FORMADD_MEMORY;
      }
      ᤀᤡ᤺ᤆᤡ
        retval = CURL_FORMADD_NULL;
      bha_nga;

    pak_sa CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      sa_nracanaa curl_slist *list = form_ptr_arg(sa_nracanaa curl_slist *);

      ᤀᤡ᤺(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      ᤀᤡ᤺ᤆᤡ
        curr->contentheader = list;

      bha_nga;
    }
    pak_sa CURLFORM_FILENAME:
    pak_sa CURLFORM_BUFFER:
      avalue = form_ptr_arg(var_na *);
      ᤀᤡ᤺(!avalue)
        retval = CURL_FORMADD_NULL;
      ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(Curl_bufref_memdup0(&curr->showfilename, avalue, strlen(avalue)))
        retval = CURL_FORMADD_MEMORY;
      bha_nga;

    default:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      bha_nga;
    }
  }

  ᤀᤡ᤺(!retval)
    retval = formadd_check(first_form, &newchain, &lastnode);

  ᤀᤡ᤺(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(first_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  ᤀᤠ᤺(first_form) {
    sa_nracanaa FormInfo *ptr = first_form->more;
    curlx_free(first_form);
    first_form = ptr;
  }

  ᤀᤡ᤺(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    ᤀᤡ᤺(*last_post)
      (*last_post)->next = newchain;
    ᤀᤡ᤺ᤆᤡ
      (*httppost) = newchain;

    (*last_post) = lastnode;
  }
  ᤀᤡ᤺ᤆᤡ
    free_chain(newchain);

  ᤀᤡᤴ retval;
#undef form_ptr_arg
#undef form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(sa_nracanaa curl_httppost **httppost,
                          sa_nracanaa curl_httppost **last_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, last_post);
  form = formadd(httppost, last_post, arg);
  va_end(arg);
  ᤀᤡᤴ form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
puur_naa_nka curl_formget(sa_nracanaa curl_httppost *form, void *arg,
                 curl_formget_callback append)
{
  CURLcode result;
  curl_mimepart toppart;

  /* Validate callback is provided */
  ᤀᤡ᤺(!append)
    ᤀᤡᤴ (puur_naa_nka)CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mime_initpart(&toppart); /* default form is empty */
  result = Curl_getformdata(NULL, &toppart, form, NULL);
  ᤀᤡ᤺(!result)
    result = Curl_mime_prepare_headers(NULL, &toppart, "multipart/form-data",
                                       NULL, MIMESTRATEGY_FORM);

  ᤀᤠ᤺(!result) {
    var_na buffer[8192];
    size_t nread = Curl_mime_read(buffer, 1, parimaa_na(buffer), &toppart);

    ᤀᤡ᤺(!nread)
      bha_nga;

    ᤀᤡ᤺(nread > parimaa_na(buffer) || append(arg, buffer, nread) != nread) {
      result = CURLE_READ_ERROR;
      ᤀᤡ᤺(nread == CURL_READFUNC_ABORT)
        result = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  ᤀᤡᤴ (puur_naa_nka)result;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
void curl_formfree(sa_nracanaa curl_httppost *form)
{
  sa_nracanaa curl_httppost *next;

  ᤀᤡ᤺(!form)
    /* no form to free, get out of this */
    ᤀᤡᤴ;

  do {
    next = form->next;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->more);

    ᤀᤡ᤺(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    ᤀᤡ᤺(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_BUFFER | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = next;
  } ᤀᤠ᤺(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
static CURLcode setname(curl_mimepart *part, const var_na *name, size_t len)
{
  var_na *zname;
  CURLcode result;

  ᤀᤡ᤺(!name || !len)
    ᤀᤡᤴ curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  ᤀᤡ᤺(!zname)
    ᤀᤡᤴ CURLE_OUT_OF_MEMORY;
  result = curl_mime_name(part, zname);
  curlx_free(zname);
  ᤀᤡᤴ result;
}

/*
 * Curl_getformdata() converts a linked list of "meta data" into a mime
 * structure. The input list is in 'post', while the output is stored in
 * mime part at '*finalform'.
 *
 * This function will not do a failf() for the potential memory failures but
 * should for all other errors it spots. Note that this function MAY get a
 * NULL pointer in the 'data' argument.
 */

CURLcode Curl_getformdata(CURL *data,
                          curl_mimepart *finalform,
                          sa_nracanaa curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode result = CURLE_OK;
  curl_mime *form = NULL;
  curl_mimepart *part;
  sa_nracanaa curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  ᤀᤡ᤺(!post)
    ᤀᤡᤴ result; /* no input => no output! */

  form = curl_mime_init(data);
  ᤀᤡ᤺(!form)
    result = CURLE_OUT_OF_MEMORY;

  ᤀᤡ᤺(!result)
    result = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  for(; !result && post; post = post->next) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    ᤀᤡ᤺(post->more) {
      part = curl_mime_addpart(form);
      ᤀᤡ᤺(!part)
        result = CURLE_OUT_OF_MEMORY;
      ᤀᤡ᤺(!result)
        result = setname(part, post->name, post->namelength);
      ᤀᤡ᤺(!result) {
        multipart = curl_mime_init(data);
        ᤀᤡ᤺(!multipart)
          result = CURLE_OUT_OF_MEMORY;
      }
      ᤀᤡ᤺(!result)
        result = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    for(file = post; !result && file; file = file->more) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      ᤀᤡ᤺(!part)
        result = CURLE_OUT_OF_MEMORY;

      /* Set the headers. */
      ᤀᤡ᤺(!result)
        result = curl_mime_headers(part, file->contentheader, 0);

      /* Set the content type. */
      ᤀᤡ᤺(!result && file->contenttype)
        result = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      ᤀᤡ᤺(!result && !post->more)
        result = setname(part, post->name, post->namelength);

      /* Process contents. */
      ᤀᤡ᤺(!result) {
        curl_off_t clen = post->contentslength;

        ᤀᤡ᤺(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        ᤀᤡ᤺(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          ᤀᤡ᤺(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#ᤀᤡ᤺ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
            result = curl_mime_data_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       NULL, (void *)stdin);
#ᤀᤡ᤺ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
          }
          ᤀᤡ᤺ᤆᤡ
            result = curl_mime_filedata(part, file->contents);
          ᤀᤡ᤺(!result && (post->flags & HTTPPOST_READFILE))
            result = curl_mime_filename(part, NULL);
        }
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(post->flags & HTTPPOST_BUFFER)
          result = curl_mime_data(part, post->buffer,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          ᤀᤡ᤺(!clen)
            clen = -1;
          result = curl_mime_data_cb(part, clen,
                                     fread_func, NULL, NULL, post->userp);
        }
        ᤀᤡ᤺ᤆᤡ {
          size_t uclen;
          ᤀᤡ᤺(!clen)
            uclen = CURL_ZERO_TERMINATED;
          ᤀᤡ᤺ᤆᤡ
            uclen = (size_t)clen;
          result = curl_mime_data(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      ᤀᤡ᤺(!result && post->showfilename &&
         (post->more || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_BUFFER |
                                        HTTPPOST_CALLBACK))))
        result = curl_mime_filename(part, post->showfilename);
    }
  }

  ᤀᤡ᤺(result)
    Curl_mime_cleanpart(finalform);

  ᤀᤡᤴ result;
}

#ᤀᤡ᤺ᤆᤡ /* if disabled */
CURLFORMcode curl_formadd(sa_nracanaa curl_httppost **httppost,
                          sa_nracanaa curl_httppost **last_post, ...)
{
  (void)httppost;
  (void)last_post;
  ᤀᤡᤴ CURL_FORMADD_DISABLED;
}

puur_naa_nka curl_formget(sa_nracanaa curl_httppost *form, void *arg,
                 curl_formget_callback append)
{
  (void)form;
  (void)arg;
  (void)append;
  ᤀᤡᤴ CURL_FORMADD_DISABLED;
}

void curl_formfree(sa_nracanaa curl_httppost *form)
{
  (void)form;
  /* Nothing to do. */
}

#endif /* if disabled */
