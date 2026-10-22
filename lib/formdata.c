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
#অন্তর্ভাৱ "curl_setup.h"

সংরচনা Curl_easy;

#অন্তর্ভাৱ "formdata.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#অন্তর্ভাৱ "urldata.h" /* for struct Curl_easy */
#অন্তর্ভাৱ "mime.h"
#অন্তর্ভাৱ "curlx/strdup.h"
#অন্তর্ভাৱ "bufref.h"
#অন্তর্ভাৱ "curlx/win32-fopen.h"


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
স্থির সংরচনা curl_httppost *httppost_add(সংরচনা FormInfo *src,
                                          সংরচনা curl_httppost *parent_post,
                                          সংরচনা curl_httppost **httppost,
                                          সংরচনা curl_httppost **last_post)
{
  সংরচনা curl_httppost *post;
  size_t namelength = src->namelength;
  যদি(!namelength && Curl_bufref_ptr(&src->name))
    namelength = strlen(Curl_bufref_ptr(&src->name));
  যদি((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    ঘূৰাই রিক্ত;
  post = curlx_calloc(1, পরিমাণ(সংরচনা curl_httppost));
  যদি(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (long)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->মান));
    post->contentlen = src->contentslength;
    post->buffer = src->buffer;
    post->bufferlength = (long)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  অন্যথা
    ঘূৰাই রিক্ত;

  যদি(parent_post) {
    /* now, point our 'more' to the original 'more' */
    post->more = parent_post->more;

    /* then move the original 'more' to point to ourselves */
    parent_post->more = post;
  }
  অন্যথা {
    /* make the previous point to this */
    যদি(*last_post)
      (*last_post)->next = post;
    অন্যথা
      (*httppost) = post;

    (*last_post) = post;
  }
  ঘূৰাই post;
}

/* Allocate and initialize a new FormInfo structure. */
স্থির সংরচনা FormInfo *forminfo_new(শূন্য)
{
  সংরচনা FormInfo *form_info = curlx_calloc(1, পরিমাণ(সংরচনা FormInfo));

  যদি(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->মান);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  ঘূৰাই form_info;
}

/* Replace the target field data by a dynamic copy of it. */
স্থির CURLcode forminfo_copyfield(সংরচনা bufref *field, size_t len)
{
  ধ্রুৱ ৱর্ণ *মান = Curl_bufref_ptr(field);
  CURLcode result = CURLE_OK;

  যদি(মান) {
    যদি(!len)
      len = strlen(মান);
    result = Curl_bufref_memdup0(field, মান, len);
  }

  ঘূৰাই result;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
স্থির শূন্য forminfo_add(সংরচনা FormInfo *form_info, সংরচনা FormInfo *parent)
{
  form_info->flags |= HTTPPOST_FILENAME;

  যদি(parent) {
    /* now, point our 'more' to the original 'more' */
    form_info->more = parent->more;

    /* then move the original 'more' to point to ourselves */
    parent->more = form_info;
  }
}

স্থির শূন্য free_formlist(সংরচনা FormInfo *ptr)
{
  বাবে(; ptr; ptr = ptr->more) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->মান);
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

স্থির CURLFORMcode formadd_check(সংরচনা FormInfo *first_form,
                                  সংরচনা curl_httppost **httppost,
                                  সংরচনা curl_httppost **last_post)
{
  ধ্রুৱ ৱর্ণ *prevtype = রিক্ত;
  সংরচনা FormInfo *form = রিক্ত;
  সংরচনা curl_httppost *post = রিক্ত;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  বাবে(form = first_form; form; form = form->more) {
    ধ্রুৱ ৱর্ণ *name = Curl_bufref_ptr(&form->name);

    যদি(((!name || !Curl_bufref_ptr(&form->মান)) && !post) ||
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
      ঘূৰাই CURL_FORMADD_INCOMPLETE;
    }
    যদি(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_BUFFER)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      ধ্রুৱ ৱর্ণ *f = Curl_bufref_ptr((form->flags & HTTPPOST_BUFFER) ?
                                      &form->showfilename : &form->মান);
      ধ্রুৱ ৱর্ণ *type = Curl_mime_contenttype(f);
      যদি(!type)
        type = prevtype;
      যদি(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      যদি(Curl_bufref_memdup0(&form->contenttype, type, strlen(type)))
        ঘূৰাই CURL_FORMADD_MEMORY;
    }
    যদি(name && form->namelength && memchr(name, 0, form->namelength))
      ঘূৰাই CURL_FORMADD_NULL;
    যদি(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      ঘূৰাই CURL_FORMADD_MEMORY;
    যদি(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->মান, (size_t)form->contentslength))
      ঘূৰাই CURL_FORMADD_MEMORY;
    post = httppost_add(form, post, httppost, last_post);

    যদি(!post)
      ঘূৰাই CURL_FORMADD_MEMORY;

    যদি(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  ঘূৰাই CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
স্থির শূন্য free_chain(সংরচনা curl_httppost *c)
{
  যেতিয়ালৈ(c) {
    সংরচনা curl_httppost *next = c->next;
    যদি(c->more)
      free_chain(c->more);
    curlx_free(c);
    c = next;
  }
}

স্থির CURLFORMcode formadd(সংরচনা curl_httppost **httppost,
                            সংরচনা curl_httppost **last_post, va_list params)
{
  সংরচনা FormInfo *first_form, *curr, *form = রিক্ত;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  ধ্রুৱ সংরচনা curl_forms *forms = রিক্ত;
  ৱর্ণ *avalue = রিক্ত;
  সংরচনা curl_httppost *newchain = রিক্ত;
  সংরচনা curl_httppost *lastnode = রিক্ত;

#define form_ptr_arg(t) (forms ? (t)(শূন্য *)avalue : va_arg(params, t))
#ifdef HAVE_UINTPTR_T
#define form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#অন্যথা
#define form_int_arg(t) (forms ? (t)(শূন্য *)avalue : va_arg(params, t))
#endif

  /*
   * We need to allocate the first struct to fill in.
   */
  first_form = forminfo_new();
  যদি(!first_form)
    ঘূৰাই CURL_FORMADD_MEMORY;

  curr = first_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  যেতিয়ালৈ(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    যদি(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (ৱর্ণ *)CURL_UNCONST(forms->মান);

      forms++; /* advance this to next entry */
      যদি(CURLFORM_END == option) {
        /* end of array state */
        forms = রিক্ত;
        অনুৱর্ত;
      }
    }
    অন্যথা {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, পূর্ণাঙ্ক);
      যদি(CURLFORM_END == option)
        ভঙ্গ;
    }

    ৱিকল্প(option) {
    পক্ষ CURLFORM_ARRAY:
      যদি(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      অন্যথা {
        forms = va_arg(params, সংরচনা curl_forms *);
        যদি(!forms)
          retval = CURL_FORMADD_NULL;
      }
      ভঙ্গ;

      /*
       * Set the Name property.
       */
    পক্ষ CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    পক্ষ CURLFORM_COPYNAME:
      যদি(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা {
        avalue = form_ptr_arg(ৱর্ণ *);
        যদি(avalue)
          Curl_bufref_set(&curr->name, avalue, 0, রিক্ত); /* No copy yet. */
        অন্যথা
          retval = CURL_FORMADD_NULL;
      }
      ভঙ্গ;
    পক্ষ CURLFORM_NAMELENGTH:
      যদি(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা
        curr->namelength = (size_t)form_int_arg(long);
      ভঙ্গ;

      /*
       * Set the contents property.
       */
    পক্ষ CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    পক্ষ CURLFORM_COPYCONTENTS:
      যদি(Curl_bufref_ptr(&curr->মান))
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা {
        avalue = form_ptr_arg(ৱর্ণ *);
        যদি(avalue)
          Curl_bufref_set(&curr->মান, avalue, 0, রিক্ত); /* No copy yet. */
        অন্যথা
          retval = CURL_FORMADD_NULL;
      }
      ভঙ্গ;
    পক্ষ CURLFORM_CONTENTSLENGTH: {
      long clen = form_int_arg(long);
      যদি(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      অন্যথা
        curr->contentslength = (curl_off_t)clen;
      ভঙ্গ;
    }

    পক্ষ CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      যদি(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      অন্যথা {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      ভঙ্গ;
    }

      /* Get contents from a given filename */
    পক্ষ CURLFORM_FILECONTENT:
      যদি(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা {
        avalue = form_ptr_arg(ৱর্ণ *);
        যদি(avalue) {
          যদি(Curl_bufref_memdup0(&curr->মান, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          অন্যথা
            curr->flags |= HTTPPOST_READFILE;
        }
        অন্যথা
          retval = CURL_FORMADD_NULL;
      }
      ভঙ্গ;

      /* We upload a file */
    পক্ষ CURLFORM_FILE:
      avalue = form_ptr_arg(ৱর্ণ *);
      যদি(Curl_bufref_ptr(&curr->মান)) {
        যদি(curr->flags & HTTPPOST_FILENAME) {
          যদি(avalue) {
            form = forminfo_new();
            যদি(!form ||
               Curl_bufref_memdup0(&form->মান, avalue, strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            অন্যথা {
              forminfo_add(form, curr);
              curr = form;
              form = রিক্ত;
            }
          }
          অন্যথা
            retval = CURL_FORMADD_NULL;
        }
        অন্যথা
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      অন্যথা {
        যদি(avalue) {
          যদি(Curl_bufref_memdup0(&curr->মান, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          অন্যথা
            curr->flags |= HTTPPOST_FILENAME;
        }
        অন্যথা
          retval = CURL_FORMADD_NULL;
      }
      ভঙ্গ;

    পক্ষ CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_BUFFER;
      যদি(curr->buffer)
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা {
        avalue = form_ptr_arg(ৱর্ণ *);
        যদি(avalue) {
          curr->buffer = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_set(&curr->মান, avalue, 0, রিক্ত);
        }
        অন্যথা
          retval = CURL_FORMADD_NULL;
      }
      ভঙ্গ;

    পক্ষ CURLFORM_BUFFERLENGTH:
      যদি(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা
        curr->bufferlength = (size_t)form_int_arg(long);
      ভঙ্গ;

    পক্ষ CURLFORM_STREAM:
      curr->flags |= HTTPPOST_CALLBACK;
      যদি(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা {
        avalue = form_ptr_arg(ৱর্ণ *);
        যদি(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_set(&curr->মান, avalue, 0, রিক্ত);
        }
        অন্যথা
          retval = CURL_FORMADD_NULL;
      }
      ভঙ্গ;

    পক্ষ CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(ৱর্ণ *);
      যদি(Curl_bufref_ptr(&curr->contenttype)) {
        যদি(curr->flags & HTTPPOST_FILENAME) {
          যদি(avalue) {
            form = forminfo_new();
            যদি(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            অন্যথা {
              forminfo_add(form, curr);
              curr = form;
              form = রিক্ত;
            }
          }
          অন্যথা
            retval = CURL_FORMADD_NULL;
        }
        অন্যথা
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      অন্যথা যদি(avalue) {
        যদি(Curl_bufref_memdup0(&curr->contenttype, avalue, strlen(avalue)))
          retval = CURL_FORMADD_MEMORY;
      }
      অন্যথা
        retval = CURL_FORMADD_NULL;
      ভঙ্গ;

    পক্ষ CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      সংরচনা curl_slist *list = form_ptr_arg(সংরচনা curl_slist *);

      যদি(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা
        curr->contentheader = list;

      ভঙ্গ;
    }
    পক্ষ CURLFORM_FILENAME:
    পক্ষ CURLFORM_BUFFER:
      avalue = form_ptr_arg(ৱর্ণ *);
      যদি(!avalue)
        retval = CURL_FORMADD_NULL;
      অন্যথা যদি(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      অন্যথা যদি(Curl_bufref_memdup0(&curr->showfilename, avalue, strlen(avalue)))
        retval = CURL_FORMADD_MEMORY;
      ভঙ্গ;

    স্ৱভাৱ:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      ভঙ্গ;
    }
  }

  যদি(!retval)
    retval = formadd_check(first_form, &newchain, &lastnode);

  যদি(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(first_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  যেতিয়ালৈ(first_form) {
    সংরচনা FormInfo *ptr = first_form->more;
    curlx_free(first_form);
    first_form = ptr;
  }

  যদি(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    যদি(*last_post)
      (*last_post)->next = newchain;
    অন্যথা
      (*httppost) = newchain;

    (*last_post) = lastnode;
  }
  অন্যথা
    free_chain(newchain);

  ঘূৰাই retval;
#undef form_ptr_arg
#undef form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(সংরচনা curl_httppost **httppost,
                          সংরচনা curl_httppost **last_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, last_post);
  form = formadd(httppost, last_post, arg);
  va_end(arg);
  ঘূৰাই form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
পূর্ণাঙ্ক curl_formget(সংরচনা curl_httppost *form, শূন্য *arg,
                 curl_formget_callback append)
{
  CURLcode result;
  curl_mimepart toppart;

  /* Validate callback is provided */
  যদি(!append)
    ঘূৰাই (পূর্ণাঙ্ক)CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mime_initpart(&toppart); /* default form is empty */
  result = Curl_getformdata(রিক্ত, &toppart, form, রিক্ত);
  যদি(!result)
    result = Curl_mime_prepare_headers(রিক্ত, &toppart, "multipart/form-data",
                                       রিক্ত, MIMESTRATEGY_FORM);

  যেতিয়ালৈ(!result) {
    ৱর্ণ buffer[8192];
    size_t nread = Curl_mime_read(buffer, 1, পরিমাণ(buffer), &toppart);

    যদি(!nread)
      ভঙ্গ;

    যদি(nread > পরিমাণ(buffer) || append(arg, buffer, nread) != nread) {
      result = CURLE_READ_ERROR;
      যদি(nread == CURL_READFUNC_ABORT)
        result = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  ঘূৰাই (পূর্ণাঙ্ক)result;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
শূন্য curl_formfree(সংরচনা curl_httppost *form)
{
  সংরচনা curl_httppost *next;

  যদি(!form)
    /* no form to free, get out of this */
    ঘূৰাই;

  কৰা {
    next = form->next;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->more);

    যদি(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    যদি(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_BUFFER | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = next;
  } যেতিয়ালৈ(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
স্থির CURLcode setname(curl_mimepart *part, ধ্রুৱ ৱর্ণ *name, size_t len)
{
  ৱর্ণ *zname;
  CURLcode result;

  যদি(!name || !len)
    ঘূৰাই curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  যদি(!zname)
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  result = curl_mime_name(part, zname);
  curlx_free(zname);
  ঘূৰাই result;
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
                          সংরচনা curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode result = CURLE_OK;
  curl_mime *form = রিক্ত;
  curl_mimepart *part;
  সংরচনা curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  যদি(!post)
    ঘূৰাই result; /* no input => no output! */

  form = curl_mime_init(data);
  যদি(!form)
    result = CURLE_OUT_OF_MEMORY;

  যদি(!result)
    result = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  বাবে(; !result && post; post = post->next) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    যদি(post->more) {
      part = curl_mime_addpart(form);
      যদি(!part)
        result = CURLE_OUT_OF_MEMORY;
      যদি(!result)
        result = setname(part, post->name, post->namelength);
      যদি(!result) {
        multipart = curl_mime_init(data);
        যদি(!multipart)
          result = CURLE_OUT_OF_MEMORY;
      }
      যদি(!result)
        result = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    বাবে(file = post; !result && file; file = file->more) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      যদি(!part)
        result = CURLE_OUT_OF_MEMORY;

      /* Set the headers. */
      যদি(!result)
        result = curl_mime_headers(part, file->contentheader, 0);

      /* Set the content type. */
      যদি(!result && file->contenttype)
        result = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      যদি(!result && !post->more)
        result = setname(part, post->name, post->namelength);

      /* Process contents. */
      যদি(!result) {
        curl_off_t clen = post->contentslength;

        যদি(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        যদি(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          যদি(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
            result = curl_mime_data_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       রিক্ত, (শূন্য *)stdin);
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic বেৰ_কৰো
#endif
          }
          অন্যথা
            result = curl_mime_filedata(part, file->contents);
          যদি(!result && (post->flags & HTTPPOST_READFILE))
            result = curl_mime_filename(part, রিক্ত);
        }
        অন্যথা যদি(post->flags & HTTPPOST_BUFFER)
          result = curl_mime_data(part, post->buffer,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        অন্যথা যদি(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          যদি(!clen)
            clen = -1;
          result = curl_mime_data_cb(part, clen,
                                     fread_func, রিক্ত, রিক্ত, post->userp);
        }
        অন্যথা {
          size_t uclen;
          যদি(!clen)
            uclen = CURL_ZERO_TERMINATED;
          অন্যথা
            uclen = (size_t)clen;
          result = curl_mime_data(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      যদি(!result && post->showfilename &&
         (post->more || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_BUFFER |
                                        HTTPPOST_CALLBACK))))
        result = curl_mime_filename(part, post->showfilename);
    }
  }

  যদি(result)
    Curl_mime_cleanpart(finalform);

  ঘূৰাই result;
}

#অন্যথা /* if disabled */
CURLFORMcode curl_formadd(সংরচনা curl_httppost **httppost,
                          সংরচনা curl_httppost **last_post, ...)
{
  (শূন্য)httppost;
  (শূন্য)last_post;
  ঘূৰাই CURL_FORMADD_DISABLED;
}

পূর্ণাঙ্ক curl_formget(সংরচনা curl_httppost *form, শূন্য *arg,
                 curl_formget_callback append)
{
  (শূন্য)form;
  (শূন্য)arg;
  (শূন্য)append;
  ঘূৰাই CURL_FORMADD_DISABLED;
}

শূন্য curl_formfree(সংরচনা curl_httppost *form)
{
  (শূন্য)form;
  /* Nothing to do. */
}

#endif /* if disabled */
