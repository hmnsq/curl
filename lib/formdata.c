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

கட்டமைப்பு Curl_easy;

#அந்தர்பாவ "formdata.h"

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#அந்தர்பாவ "urldata.h" /* for struct Curl_easy */
#அந்தர்பாவ "mime.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "curlx/win32-fopen.h"


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
நிலையான கட்டமைப்பு curl_httppost *httppost_add(கட்டமைப்பு FormInfo *src,
                                          கட்டமைப்பு curl_httppost *parent_post,
                                          கட்டமைப்பு curl_httppost **httppost,
                                          கட்டமைப்பு curl_httppost **last_post)
{
  கட்டமைப்பு curl_httppost *post;
  size_t namelength = src->namelength;
  எனில்(!namelength && Curl_bufref_ptr(&src->name))
    namelength = strlen(Curl_bufref_ptr(&src->name));
  எனில்((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    திரும்பு வெற்று;
  post = curlx_calloc(1, அளவு(கட்டமைப்பு curl_httppost));
  எனில்(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (long)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->மதிப்பு));
    post->contentlen = src->contentslength;
    post->buffer = src->buffer;
    post->bufferlength = (long)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  இல்லையெனில்
    திரும்பு வெற்று;

  எனில்(parent_post) {
    /* now, point our 'more' to the original 'more' */
    post->more = parent_post->more;

    /* then move the original 'more' to point to ourselves */
    parent_post->more = post;
  }
  இல்லையெனில் {
    /* make the previous point to this */
    எனில்(*last_post)
      (*last_post)->next = post;
    இல்லையெனில்
      (*httppost) = post;

    (*last_post) = post;
  }
  திரும்பு post;
}

/* Allocate and initialize a new FormInfo structure. */
நிலையான கட்டமைப்பு FormInfo *forminfo_new(வெறுமை)
{
  கட்டமைப்பு FormInfo *form_info = curlx_calloc(1, அளவு(கட்டமைப்பு FormInfo));

  எனில்(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->மதிப்பு);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  திரும்பு form_info;
}

/* Replace the target field data by a dynamic copy of it. */
நிலையான CURLcode forminfo_copyfield(கட்டமைப்பு bufref *field, size_t len)
{
  மாறிலி எழுத்து *மதிப்பு = Curl_bufref_ptr(field);
  CURLcode result = CURLE_OK;

  எனில்(மதிப்பு) {
    எனில்(!len)
      len = strlen(மதிப்பு);
    result = Curl_bufref_memdup0(field, மதிப்பு, len);
  }

  திரும்பு result;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
நிலையான வெறுமை forminfo_add(கட்டமைப்பு FormInfo *form_info, கட்டமைப்பு FormInfo *parent)
{
  form_info->flags |= HTTPPOST_FILENAME;

  எனில்(parent) {
    /* now, point our 'more' to the original 'more' */
    form_info->more = parent->more;

    /* then move the original 'more' to point to ourselves */
    parent->more = form_info;
  }
}

நிலையான வெறுமை free_formlist(கட்டமைப்பு FormInfo *ptr)
{
  க்கு(; ptr; ptr = ptr->more) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->மதிப்பு);
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

நிலையான CURLFORMcode formadd_check(கட்டமைப்பு FormInfo *first_form,
                                  கட்டமைப்பு curl_httppost **httppost,
                                  கட்டமைப்பு curl_httppost **last_post)
{
  மாறிலி எழுத்து *prevtype = வெற்று;
  கட்டமைப்பு FormInfo *form = வெற்று;
  கட்டமைப்பு curl_httppost *post = வெற்று;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  க்கு(form = first_form; form; form = form->more) {
    மாறிலி எழுத்து *name = Curl_bufref_ptr(&form->name);

    எனில்(((!name || !Curl_bufref_ptr(&form->மதிப்பு)) && !post) ||
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
      திரும்பு CURL_FORMADD_INCOMPLETE;
    }
    எனில்(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_BUFFER)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      மாறிலி எழுத்து *f = Curl_bufref_ptr((form->flags & HTTPPOST_BUFFER) ?
                                      &form->showfilename : &form->மதிப்பு);
      மாறிலி எழுத்து *type = Curl_mime_contenttype(f);
      எனில்(!type)
        type = prevtype;
      எனில்(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      எனில்(Curl_bufref_memdup0(&form->contenttype, type, strlen(type)))
        திரும்பு CURL_FORMADD_MEMORY;
    }
    எனில்(name && form->namelength && memchr(name, 0, form->namelength))
      திரும்பு CURL_FORMADD_NULL;
    எனில்(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      திரும்பு CURL_FORMADD_MEMORY;
    எனில்(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->மதிப்பு, (size_t)form->contentslength))
      திரும்பு CURL_FORMADD_MEMORY;
    post = httppost_add(form, post, httppost, last_post);

    எனில்(!post)
      திரும்பு CURL_FORMADD_MEMORY;

    எனில்(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  திரும்பு CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
நிலையான வெறுமை free_chain(கட்டமைப்பு curl_httppost *c)
{
  போது(c) {
    கட்டமைப்பு curl_httppost *next = c->next;
    எனில்(c->more)
      free_chain(c->more);
    curlx_free(c);
    c = next;
  }
}

நிலையான CURLFORMcode formadd(கட்டமைப்பு curl_httppost **httppost,
                            கட்டமைப்பு curl_httppost **last_post, va_list params)
{
  கட்டமைப்பு FormInfo *first_form, *curr, *form = வெற்று;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  மாறிலி கட்டமைப்பு curl_forms *forms = வெற்று;
  எழுத்து *avalue = வெற்று;
  கட்டமைப்பு curl_httppost *newchain = வெற்று;
  கட்டமைப்பு curl_httppost *lastnode = வெற்று;

#define form_ptr_arg(t) (forms ? (t)(வெறுமை *)avalue : va_arg(params, t))
#ifdef HAVE_UINTPTR_T
#define form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#இல்லையெனில்
#define form_int_arg(t) (forms ? (t)(வெறுமை *)avalue : va_arg(params, t))
#endif

  /*
   * We need to allocate the first struct to fill in.
   */
  first_form = forminfo_new();
  எனில்(!first_form)
    திரும்பு CURL_FORMADD_MEMORY;

  curr = first_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  போது(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    எனில்(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (எழுத்து *)CURL_UNCONST(forms->மதிப்பு);

      forms++; /* advance this to next entry */
      எனில்(CURLFORM_END == option) {
        /* end of array state */
        forms = வெற்று;
        தொடர்;
      }
    }
    இல்லையெனில் {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, முழு);
      எனில்(CURLFORM_END == option)
        நிறுத்து;
    }

    தேர்வு(option) {
    நிலை CURLFORM_ARRAY:
      எனில்(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      இல்லையெனில் {
        forms = va_arg(params, கட்டமைப்பு curl_forms *);
        எனில்(!forms)
          retval = CURL_FORMADD_NULL;
      }
      நிறுத்து;

      /*
       * Set the Name property.
       */
    நிலை CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    நிலை CURLFORM_COPYNAME:
      எனில்(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      இல்லையெனில் {
        avalue = form_ptr_arg(எழுத்து *);
        எனில்(avalue)
          Curl_bufref_set(&curr->name, avalue, 0, வெற்று); /* No copy yet. */
        இல்லையெனில்
          retval = CURL_FORMADD_NULL;
      }
      நிறுத்து;
    நிலை CURLFORM_NAMELENGTH:
      எனில்(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      இல்லையெனில்
        curr->namelength = (size_t)form_int_arg(long);
      நிறுத்து;

      /*
       * Set the contents property.
       */
    நிலை CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    நிலை CURLFORM_COPYCONTENTS:
      எனில்(Curl_bufref_ptr(&curr->மதிப்பு))
        retval = CURL_FORMADD_OPTION_TWICE;
      இல்லையெனில் {
        avalue = form_ptr_arg(எழுத்து *);
        எனில்(avalue)
          Curl_bufref_set(&curr->மதிப்பு, avalue, 0, வெற்று); /* No copy yet. */
        இல்லையெனில்
          retval = CURL_FORMADD_NULL;
      }
      நிறுத்து;
    நிலை CURLFORM_CONTENTSLENGTH: {
      long clen = form_int_arg(long);
      எனில்(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      இல்லையெனில்
        curr->contentslength = (curl_off_t)clen;
      நிறுத்து;
    }

    நிலை CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      எனில்(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      இல்லையெனில் {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      நிறுத்து;
    }

      /* Get contents from a given filename */
    நிலை CURLFORM_FILECONTENT:
      எனில்(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      இல்லையெனில் {
        avalue = form_ptr_arg(எழுத்து *);
        எனில்(avalue) {
          எனில்(Curl_bufref_memdup0(&curr->மதிப்பு, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          இல்லையெனில்
            curr->flags |= HTTPPOST_READFILE;
        }
        இல்லையெனில்
          retval = CURL_FORMADD_NULL;
      }
      நிறுத்து;

      /* We upload a file */
    நிலை CURLFORM_FILE:
      avalue = form_ptr_arg(எழுத்து *);
      எனில்(Curl_bufref_ptr(&curr->மதிப்பு)) {
        எனில்(curr->flags & HTTPPOST_FILENAME) {
          எனில்(avalue) {
            form = forminfo_new();
            எனில்(!form ||
               Curl_bufref_memdup0(&form->மதிப்பு, avalue, strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            இல்லையெனில் {
              forminfo_add(form, curr);
              curr = form;
              form = வெற்று;
            }
          }
          இல்லையெனில்
            retval = CURL_FORMADD_NULL;
        }
        இல்லையெனில்
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      இல்லையெனில் {
        எனில்(avalue) {
          எனில்(Curl_bufref_memdup0(&curr->மதிப்பு, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          இல்லையெனில்
            curr->flags |= HTTPPOST_FILENAME;
        }
        இல்லையெனில்
          retval = CURL_FORMADD_NULL;
      }
      நிறுத்து;

    நிலை CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_BUFFER;
      எனில்(curr->buffer)
        retval = CURL_FORMADD_OPTION_TWICE;
      இல்லையெனில் {
        avalue = form_ptr_arg(எழுத்து *);
        எனில்(avalue) {
          curr->buffer = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_set(&curr->மதிப்பு, avalue, 0, வெற்று);
        }
        இல்லையெனில்
          retval = CURL_FORMADD_NULL;
      }
      நிறுத்து;

    நிலை CURLFORM_BUFFERLENGTH:
      எனில்(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      இல்லையெனில்
        curr->bufferlength = (size_t)form_int_arg(long);
      நிறுத்து;

    நிலை CURLFORM_STREAM:
      curr->flags |= HTTPPOST_CALLBACK;
      எனில்(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      இல்லையெனில் {
        avalue = form_ptr_arg(எழுத்து *);
        எனில்(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_set(&curr->மதிப்பு, avalue, 0, வெற்று);
        }
        இல்லையெனில்
          retval = CURL_FORMADD_NULL;
      }
      நிறுத்து;

    நிலை CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(எழுத்து *);
      எனில்(Curl_bufref_ptr(&curr->contenttype)) {
        எனில்(curr->flags & HTTPPOST_FILENAME) {
          எனில்(avalue) {
            form = forminfo_new();
            எனில்(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            இல்லையெனில் {
              forminfo_add(form, curr);
              curr = form;
              form = வெற்று;
            }
          }
          இல்லையெனில்
            retval = CURL_FORMADD_NULL;
        }
        இல்லையெனில்
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      இல்லையெனில் எனில்(avalue) {
        எனில்(Curl_bufref_memdup0(&curr->contenttype, avalue, strlen(avalue)))
          retval = CURL_FORMADD_MEMORY;
      }
      இல்லையெனில்
        retval = CURL_FORMADD_NULL;
      நிறுத்து;

    நிலை CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      கட்டமைப்பு curl_slist *list = form_ptr_arg(கட்டமைப்பு curl_slist *);

      எனில்(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      இல்லையெனில்
        curr->contentheader = list;

      நிறுத்து;
    }
    நிலை CURLFORM_FILENAME:
    நிலை CURLFORM_BUFFER:
      avalue = form_ptr_arg(எழுத்து *);
      எனில்(!avalue)
        retval = CURL_FORMADD_NULL;
      இல்லையெனில் எனில்(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      இல்லையெனில் எனில்(Curl_bufref_memdup0(&curr->showfilename, avalue, strlen(avalue)))
        retval = CURL_FORMADD_MEMORY;
      நிறுத்து;

    இயல்பு:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      நிறுத்து;
    }
  }

  எனில்(!retval)
    retval = formadd_check(first_form, &newchain, &lastnode);

  எனில்(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(first_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  போது(first_form) {
    கட்டமைப்பு FormInfo *ptr = first_form->more;
    curlx_free(first_form);
    first_form = ptr;
  }

  எனில்(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    எனில்(*last_post)
      (*last_post)->next = newchain;
    இல்லையெனில்
      (*httppost) = newchain;

    (*last_post) = lastnode;
  }
  இல்லையெனில்
    free_chain(newchain);

  திரும்பு retval;
#undef form_ptr_arg
#undef form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(கட்டமைப்பு curl_httppost **httppost,
                          கட்டமைப்பு curl_httppost **last_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, last_post);
  form = formadd(httppost, last_post, arg);
  va_end(arg);
  திரும்பு form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
முழு curl_formget(கட்டமைப்பு curl_httppost *form, வெறுமை *arg,
                 curl_formget_callback append)
{
  CURLcode result;
  curl_mimepart toppart;

  /* Validate callback is provided */
  எனில்(!append)
    திரும்பு (முழு)CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mime_initpart(&toppart); /* default form is empty */
  result = Curl_getformdata(வெற்று, &toppart, form, வெற்று);
  எனில்(!result)
    result = Curl_mime_prepare_headers(வெற்று, &toppart, "multipart/form-data",
                                       வெற்று, MIMESTRATEGY_FORM);

  போது(!result) {
    எழுத்து buffer[8192];
    size_t nread = Curl_mime_read(buffer, 1, அளவு(buffer), &toppart);

    எனில்(!nread)
      நிறுத்து;

    எனில்(nread > அளவு(buffer) || append(arg, buffer, nread) != nread) {
      result = CURLE_READ_ERROR;
      எனில்(nread == CURL_READFUNC_ABORT)
        result = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  திரும்பு (முழு)result;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
வெறுமை curl_formfree(கட்டமைப்பு curl_httppost *form)
{
  கட்டமைப்பு curl_httppost *next;

  எனில்(!form)
    /* no form to free, get out of this */
    திரும்பு;

  செய் {
    next = form->next;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->more);

    எனில்(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    எனில்(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_BUFFER | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = next;
  } போது(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
நிலையான CURLcode setname(curl_mimepart *part, மாறிலி எழுத்து *name, size_t len)
{
  எழுத்து *zname;
  CURLcode result;

  எனில்(!name || !len)
    திரும்பு curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  எனில்(!zname)
    திரும்பு CURLE_OUT_OF_MEMORY;
  result = curl_mime_name(part, zname);
  curlx_free(zname);
  திரும்பு result;
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
                          கட்டமைப்பு curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode result = CURLE_OK;
  curl_mime *form = வெற்று;
  curl_mimepart *part;
  கட்டமைப்பு curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  எனில்(!post)
    திரும்பு result; /* no input => no output! */

  form = curl_mime_init(data);
  எனில்(!form)
    result = CURLE_OUT_OF_MEMORY;

  எனில்(!result)
    result = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  க்கு(; !result && post; post = post->next) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    எனில்(post->more) {
      part = curl_mime_addpart(form);
      எனில்(!part)
        result = CURLE_OUT_OF_MEMORY;
      எனில்(!result)
        result = setname(part, post->name, post->namelength);
      எனில்(!result) {
        multipart = curl_mime_init(data);
        எனில்(!multipart)
          result = CURLE_OUT_OF_MEMORY;
      }
      எனில்(!result)
        result = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    க்கு(file = post; !result && file; file = file->more) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      எனில்(!part)
        result = CURLE_OUT_OF_MEMORY;

      /* Set the headers. */
      எனில்(!result)
        result = curl_mime_headers(part, file->contentheader, 0);

      /* Set the content type. */
      எனில்(!result && file->contenttype)
        result = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      எனில்(!result && !post->more)
        result = setname(part, post->name, post->namelength);

      /* Process contents. */
      எனில்(!result) {
        curl_off_t clen = post->contentslength;

        எனில்(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        எனில்(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          எனில்(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic தள்ளு
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
            result = curl_mime_data_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       வெற்று, (வெறுமை *)stdin);
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic எடு
#endif
          }
          இல்லையெனில்
            result = curl_mime_filedata(part, file->contents);
          எனில்(!result && (post->flags & HTTPPOST_READFILE))
            result = curl_mime_filename(part, வெற்று);
        }
        இல்லையெனில் எனில்(post->flags & HTTPPOST_BUFFER)
          result = curl_mime_data(part, post->buffer,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        இல்லையெனில் எனில்(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          எனில்(!clen)
            clen = -1;
          result = curl_mime_data_cb(part, clen,
                                     fread_func, வெற்று, வெற்று, post->userp);
        }
        இல்லையெனில் {
          size_t uclen;
          எனில்(!clen)
            uclen = CURL_ZERO_TERMINATED;
          இல்லையெனில்
            uclen = (size_t)clen;
          result = curl_mime_data(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      எனில்(!result && post->showfilename &&
         (post->more || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_BUFFER |
                                        HTTPPOST_CALLBACK))))
        result = curl_mime_filename(part, post->showfilename);
    }
  }

  எனில்(result)
    Curl_mime_cleanpart(finalform);

  திரும்பு result;
}

#இல்லையெனில் /* if disabled */
CURLFORMcode curl_formadd(கட்டமைப்பு curl_httppost **httppost,
                          கட்டமைப்பு curl_httppost **last_post, ...)
{
  (வெறுமை)httppost;
  (வெறுமை)last_post;
  திரும்பு CURL_FORMADD_DISABLED;
}

முழு curl_formget(கட்டமைப்பு curl_httppost *form, வெறுமை *arg,
                 curl_formget_callback append)
{
  (வெறுமை)form;
  (வெறுமை)arg;
  (வெறுமை)append;
  திரும்பு CURL_FORMADD_DISABLED;
}

வெறுமை curl_formfree(கட்டமைப்பு curl_httppost *form)
{
  (வெறுமை)form;
  /* Nothing to do. */
}

#endif /* if disabled */
