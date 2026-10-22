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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

ସଂରଚନା Curl_easy;

#ଅନ୍ତର୍ଭାଵ "formdata.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#ଅନ୍ତର୍ଭାଵ "urldata.h" /* for struct Curl_easy */
#ଅନ୍ତର୍ଭାଵ "mime.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "bufref.h"
#ଅନ୍ତର୍ଭାଵ "curlx/win32-fopen.h"


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
ସ୍ଥିର ସଂରଚନା curl_httppost *httppost_add(ସଂରଚନା FormInfo *src,
                                          ସଂରଚନା curl_httppost *parent_post,
                                          ସଂରଚନା curl_httppost **httppost,
                                          ସଂରଚନା curl_httppost **last_post)
{
  ସଂରଚନା curl_httppost *post;
  size_t namelength = src->namelength;
  ଯଦି(!namelength && Curl_bufref_ptr(&src->name))
    namelength = strlen(Curl_bufref_ptr(&src->name));
  ଯଦି((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    ଫେରନ୍ତୁ ଖାଲି;
  post = curlx_calloc(1, ଆକାର(ସଂରଚନା curl_httppost));
  ଯଦି(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (long)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->ମୂଲ୍ୟ));
    post->contentlen = src->contentslength;
    post->buffer = src->buffer;
    post->bufferlength = (long)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ ଖାଲି;

  ଯଦି(parent_post) {
    /* now, point our 'more' to the original 'more' */
    post->more = parent_post->more;

    /* then move the original 'more' to point to ourselves */
    parent_post->more = post;
  }
  ଅନ୍ୟଥା {
    /* make the previous point to this */
    ଯଦି(*last_post)
      (*last_post)->next = post;
    ଅନ୍ୟଥା
      (*httppost) = post;

    (*last_post) = post;
  }
  ଫେରନ୍ତୁ post;
}

/* Allocate and initialize a new FormInfo structure. */
ସ୍ଥିର ସଂରଚନା FormInfo *forminfo_new(ଶୂନ୍ୟ)
{
  ସଂରଚନା FormInfo *form_info = curlx_calloc(1, ଆକାର(ସଂରଚନା FormInfo));

  ଯଦି(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->ମୂଲ୍ୟ);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  ଫେରନ୍ତୁ form_info;
}

/* Replace the target field data by a dynamic copy of it. */
ସ୍ଥିର CURLcode forminfo_copyfield(ସଂରଚନା bufref *field, size_t len)
{
  ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ = Curl_bufref_ptr(field);
  CURLcode result = CURLE_OK;

  ଯଦି(ମୂଲ୍ୟ) {
    ଯଦି(!len)
      len = strlen(ମୂଲ୍ୟ);
    result = Curl_bufref_memdup0(field, ମୂଲ୍ୟ, len);
  }

  ଫେରନ୍ତୁ result;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
ସ୍ଥିର ଶୂନ୍ୟ forminfo_add(ସଂରଚନା FormInfo *form_info, ସଂରଚନା FormInfo *parent)
{
  form_info->flags |= HTTPPOST_FILENAME;

  ଯଦି(parent) {
    /* now, point our 'more' to the original 'more' */
    form_info->more = parent->more;

    /* then move the original 'more' to point to ourselves */
    parent->more = form_info;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ free_formlist(ସଂରଚନା FormInfo *ptr)
{
  ପାଇଁ(; ptr; ptr = ptr->more) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->ମୂଲ୍ୟ);
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

ସ୍ଥିର CURLFORMcode formadd_check(ସଂରଚନା FormInfo *first_form,
                                  ସଂରଚନା curl_httppost **httppost,
                                  ସଂରଚନା curl_httppost **last_post)
{
  ଅଚଳ ଅକ୍ଷର *prevtype = ଖାଲି;
  ସଂରଚନା FormInfo *form = ଖାଲି;
  ସଂରଚନା curl_httppost *post = ଖାଲି;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  ପାଇଁ(form = first_form; form; form = form->more) {
    ଅଚଳ ଅକ୍ଷର *name = Curl_bufref_ptr(&form->name);

    ଯଦି(((!name || !Curl_bufref_ptr(&form->ମୂଲ୍ୟ)) && !post) ||
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
      ଫେରନ୍ତୁ CURL_FORMADD_INCOMPLETE;
    }
    ଯଦି(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_BUFFER)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      ଅଚଳ ଅକ୍ଷର *f = Curl_bufref_ptr((form->flags & HTTPPOST_BUFFER) ?
                                      &form->showfilename : &form->ମୂଲ୍ୟ);
      ଅଚଳ ଅକ୍ଷର *type = Curl_mime_contenttype(f);
      ଯଦି(!type)
        type = prevtype;
      ଯଦି(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      ଯଦି(Curl_bufref_memdup0(&form->contenttype, type, strlen(type)))
        ଫେରନ୍ତୁ CURL_FORMADD_MEMORY;
    }
    ଯଦି(name && form->namelength && memchr(name, 0, form->namelength))
      ଫେରନ୍ତୁ CURL_FORMADD_NULL;
    ଯଦି(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      ଫେରନ୍ତୁ CURL_FORMADD_MEMORY;
    ଯଦି(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->ମୂଲ୍ୟ, (size_t)form->contentslength))
      ଫେରନ୍ତୁ CURL_FORMADD_MEMORY;
    post = httppost_add(form, post, httppost, last_post);

    ଯଦି(!post)
      ଫେରନ୍ତୁ CURL_FORMADD_MEMORY;

    ଯଦି(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  ଫେରନ୍ତୁ CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
ସ୍ଥିର ଶୂନ୍ୟ free_chain(ସଂରଚନା curl_httppost *c)
{
  ଯେତେବେଳେ(c) {
    ସଂରଚନା curl_httppost *next = c->next;
    ଯଦି(c->more)
      free_chain(c->more);
    curlx_free(c);
    c = next;
  }
}

ସ୍ଥିର CURLFORMcode formadd(ସଂରଚନା curl_httppost **httppost,
                            ସଂରଚନା curl_httppost **last_post, va_list params)
{
  ସଂରଚନା FormInfo *first_form, *curr, *form = ଖାଲି;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  ଅଚଳ ସଂରଚନା curl_forms *forms = ଖାଲି;
  ଅକ୍ଷର *avalue = ଖାଲି;
  ସଂରଚନା curl_httppost *newchain = ଖାଲି;
  ସଂରଚନା curl_httppost *lastnode = ଖାଲି;

#define form_ptr_arg(t) (forms ? (t)(ଶୂନ୍ୟ *)avalue : va_arg(params, t))
#ifdef HAVE_UINTPTR_T
#define form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#ଅନ୍ୟଥା
#define form_int_arg(t) (forms ? (t)(ଶୂନ୍ୟ *)avalue : va_arg(params, t))
#endif

  /*
   * We need to allocate the first struct to fill in.
   */
  first_form = forminfo_new();
  ଯଦି(!first_form)
    ଫେରନ୍ତୁ CURL_FORMADD_MEMORY;

  curr = first_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  ଯେତେବେଳେ(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    ଯଦି(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (ଅକ୍ଷର *)CURL_UNCONST(forms->ମୂଲ୍ୟ);

      forms++; /* advance this to next entry */
      ଯଦି(CURLFORM_END == option) {
        /* end of array state */
        forms = ଖାଲି;
        ଜାରି;
      }
    }
    ଅନ୍ୟଥା {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
      ଯଦି(CURLFORM_END == option)
        ଭାଙ୍ଗ;
    }

    ବାଛ(option) {
    ସ୍ଥିତି CURLFORM_ARRAY:
      ଯଦି(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      ଅନ୍ୟଥା {
        forms = va_arg(params, ସଂରଚନା curl_forms *);
        ଯଦି(!forms)
          retval = CURL_FORMADD_NULL;
      }
      ଭାଙ୍ଗ;

      /*
       * Set the Name property.
       */
    ସ୍ଥିତି CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    ସ୍ଥିତି CURLFORM_COPYNAME:
      ଯଦି(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      ଅନ୍ୟଥା {
        avalue = form_ptr_arg(ଅକ୍ଷର *);
        ଯଦି(avalue)
          Curl_bufref_set(&curr->name, avalue, 0, ଖାଲି); /* No copy yet. */
        ଅନ୍ୟଥା
          retval = CURL_FORMADD_NULL;
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURLFORM_NAMELENGTH:
      ଯଦି(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      ଅନ୍ୟଥା
        curr->namelength = (size_t)form_int_arg(long);
      ଭାଙ୍ଗ;

      /*
       * Set the contents property.
       */
    ସ୍ଥିତି CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    ସ୍ଥିତି CURLFORM_COPYCONTENTS:
      ଯଦି(Curl_bufref_ptr(&curr->ମୂଲ୍ୟ))
        retval = CURL_FORMADD_OPTION_TWICE;
      ଅନ୍ୟଥା {
        avalue = form_ptr_arg(ଅକ୍ଷର *);
        ଯଦି(avalue)
          Curl_bufref_set(&curr->ମୂଲ୍ୟ, avalue, 0, ଖାଲି); /* No copy yet. */
        ଅନ୍ୟଥା
          retval = CURL_FORMADD_NULL;
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURLFORM_CONTENTSLENGTH: {
      long clen = form_int_arg(long);
      ଯଦି(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      ଅନ୍ୟଥା
        curr->contentslength = (curl_off_t)clen;
      ଭାଙ୍ଗ;
    }

    ସ୍ଥିତି CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      ଯଦି(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      ଅନ୍ୟଥା {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      ଭାଙ୍ଗ;
    }

      /* Get contents from a given filename */
    ସ୍ଥିତି CURLFORM_FILECONTENT:
      ଯଦି(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      ଅନ୍ୟଥା {
        avalue = form_ptr_arg(ଅକ୍ଷର *);
        ଯଦି(avalue) {
          ଯଦି(Curl_bufref_memdup0(&curr->ମୂଲ୍ୟ, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          ଅନ୍ୟଥା
            curr->flags |= HTTPPOST_READFILE;
        }
        ଅନ୍ୟଥା
          retval = CURL_FORMADD_NULL;
      }
      ଭାଙ୍ଗ;

      /* We upload a file */
    ସ୍ଥିତି CURLFORM_FILE:
      avalue = form_ptr_arg(ଅକ୍ଷର *);
      ଯଦି(Curl_bufref_ptr(&curr->ମୂଲ୍ୟ)) {
        ଯଦି(curr->flags & HTTPPOST_FILENAME) {
          ଯଦି(avalue) {
            form = forminfo_new();
            ଯଦି(!form ||
               Curl_bufref_memdup0(&form->ମୂଲ୍ୟ, avalue, strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            ଅନ୍ୟଥା {
              forminfo_add(form, curr);
              curr = form;
              form = ଖାଲି;
            }
          }
          ଅନ୍ୟଥା
            retval = CURL_FORMADD_NULL;
        }
        ଅନ୍ୟଥା
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      ଅନ୍ୟଥା {
        ଯଦି(avalue) {
          ଯଦି(Curl_bufref_memdup0(&curr->ମୂଲ୍ୟ, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          ଅନ୍ୟଥା
            curr->flags |= HTTPPOST_FILENAME;
        }
        ଅନ୍ୟଥା
          retval = CURL_FORMADD_NULL;
      }
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_BUFFER;
      ଯଦି(curr->buffer)
        retval = CURL_FORMADD_OPTION_TWICE;
      ଅନ୍ୟଥା {
        avalue = form_ptr_arg(ଅକ୍ଷର *);
        ଯଦି(avalue) {
          curr->buffer = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_set(&curr->ମୂଲ୍ୟ, avalue, 0, ଖାଲି);
        }
        ଅନ୍ୟଥା
          retval = CURL_FORMADD_NULL;
      }
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURLFORM_BUFFERLENGTH:
      ଯଦି(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      ଅନ୍ୟଥା
        curr->bufferlength = (size_t)form_int_arg(long);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURLFORM_STREAM:
      curr->flags |= HTTPPOST_CALLBACK;
      ଯଦି(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      ଅନ୍ୟଥା {
        avalue = form_ptr_arg(ଅକ୍ଷର *);
        ଯଦି(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_set(&curr->ମୂଲ୍ୟ, avalue, 0, ଖାଲି);
        }
        ଅନ୍ୟଥା
          retval = CURL_FORMADD_NULL;
      }
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(ଅକ୍ଷର *);
      ଯଦି(Curl_bufref_ptr(&curr->contenttype)) {
        ଯଦି(curr->flags & HTTPPOST_FILENAME) {
          ଯଦି(avalue) {
            form = forminfo_new();
            ଯଦି(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            ଅନ୍ୟଥା {
              forminfo_add(form, curr);
              curr = form;
              form = ଖାଲି;
            }
          }
          ଅନ୍ୟଥା
            retval = CURL_FORMADD_NULL;
        }
        ଅନ୍ୟଥା
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      ଅନ୍ୟଥା ଯଦି(avalue) {
        ଯଦି(Curl_bufref_memdup0(&curr->contenttype, avalue, strlen(avalue)))
          retval = CURL_FORMADD_MEMORY;
      }
      ଅନ୍ୟଥା
        retval = CURL_FORMADD_NULL;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      ସଂରଚନା curl_slist *list = form_ptr_arg(ସଂରଚନା curl_slist *);

      ଯଦି(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      ଅନ୍ୟଥା
        curr->contentheader = list;

      ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି CURLFORM_FILENAME:
    ସ୍ଥିତି CURLFORM_BUFFER:
      avalue = form_ptr_arg(ଅକ୍ଷର *);
      ଯଦି(!avalue)
        retval = CURL_FORMADD_NULL;
      ଅନ୍ୟଥା ଯଦି(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      ଅନ୍ୟଥା ଯଦି(Curl_bufref_memdup0(&curr->showfilename, avalue, strlen(avalue)))
        retval = CURL_FORMADD_MEMORY;
      ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      ଭାଙ୍ଗ;
    }
  }

  ଯଦି(!retval)
    retval = formadd_check(first_form, &newchain, &lastnode);

  ଯଦି(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(first_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  ଯେତେବେଳେ(first_form) {
    ସଂରଚନା FormInfo *ptr = first_form->more;
    curlx_free(first_form);
    first_form = ptr;
  }

  ଯଦି(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    ଯଦି(*last_post)
      (*last_post)->next = newchain;
    ଅନ୍ୟଥା
      (*httppost) = newchain;

    (*last_post) = lastnode;
  }
  ଅନ୍ୟଥା
    free_chain(newchain);

  ଫେରନ୍ତୁ retval;
#undef form_ptr_arg
#undef form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(ସଂରଚନା curl_httppost **httppost,
                          ସଂରଚନା curl_httppost **last_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, last_post);
  form = formadd(httppost, last_post, arg);
  va_end(arg);
  ଫେରନ୍ତୁ form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_formget(ସଂରଚନା curl_httppost *form, ଶୂନ୍ୟ *arg,
                 curl_formget_callback append)
{
  CURLcode result;
  curl_mimepart toppart;

  /* Validate callback is provided */
  ଯଦି(!append)
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mime_initpart(&toppart); /* default form is empty */
  result = Curl_getformdata(ଖାଲି, &toppart, form, ଖାଲି);
  ଯଦି(!result)
    result = Curl_mime_prepare_headers(ଖାଲି, &toppart, "multipart/form-data",
                                       ଖାଲି, MIMESTRATEGY_FORM);

  ଯେତେବେଳେ(!result) {
    ଅକ୍ଷର buffer[8192];
    size_t nread = Curl_mime_read(buffer, 1, ଆକାର(buffer), &toppart);

    ଯଦି(!nread)
      ଭାଙ୍ଗ;

    ଯଦି(nread > ଆକାର(buffer) || append(arg, buffer, nread) != nread) {
      result = CURLE_READ_ERROR;
      ଯଦି(nread == CURL_READFUNC_ABORT)
        result = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
ଶୂନ୍ୟ curl_formfree(ସଂରଚନା curl_httppost *form)
{
  ସଂରଚନା curl_httppost *next;

  ଯଦି(!form)
    /* no form to free, get out of this */
    ଫେରନ୍ତୁ;

  କର {
    next = form->next;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->more);

    ଯଦି(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    ଯଦି(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_BUFFER | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = next;
  } ଯେତେବେଳେ(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
ସ୍ଥିର CURLcode setname(curl_mimepart *part, ଅଚଳ ଅକ୍ଷର *name, size_t len)
{
  ଅକ୍ଷର *zname;
  CURLcode result;

  ଯଦି(!name || !len)
    ଫେରନ୍ତୁ curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  ଯଦି(!zname)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  result = curl_mime_name(part, zname);
  curlx_free(zname);
  ଫେରନ୍ତୁ result;
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
                          ସଂରଚନା curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode result = CURLE_OK;
  curl_mime *form = ଖାଲି;
  curl_mimepart *part;
  ସଂରଚନା curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  ଯଦି(!post)
    ଫେରନ୍ତୁ result; /* no input => no output! */

  form = curl_mime_init(data);
  ଯଦି(!form)
    result = CURLE_OUT_OF_MEMORY;

  ଯଦି(!result)
    result = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  ପାଇଁ(; !result && post; post = post->next) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    ଯଦି(post->more) {
      part = curl_mime_addpart(form);
      ଯଦି(!part)
        result = CURLE_OUT_OF_MEMORY;
      ଯଦି(!result)
        result = setname(part, post->name, post->namelength);
      ଯଦି(!result) {
        multipart = curl_mime_init(data);
        ଯଦି(!multipart)
          result = CURLE_OUT_OF_MEMORY;
      }
      ଯଦି(!result)
        result = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    ପାଇଁ(file = post; !result && file; file = file->more) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      ଯଦି(!part)
        result = CURLE_OUT_OF_MEMORY;

      /* Set the headers. */
      ଯଦି(!result)
        result = curl_mime_headers(part, file->contentheader, 0);

      /* Set the content type. */
      ଯଦି(!result && file->contenttype)
        result = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      ଯଦି(!result && !post->more)
        result = setname(part, post->name, post->namelength);

      /* Process contents. */
      ଯଦି(!result) {
        curl_off_t clen = post->contentslength;

        ଯଦି(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        ଯଦି(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          ଯଦି(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#ଯଦି defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ଠେଲ
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
            result = curl_mime_data_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       ଖାଲି, (ଶୂନ୍ୟ *)stdin);
#ଯଦି defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic କାଢ଼
#endif
          }
          ଅନ୍ୟଥା
            result = curl_mime_filedata(part, file->contents);
          ଯଦି(!result && (post->flags & HTTPPOST_READFILE))
            result = curl_mime_filename(part, ଖାଲି);
        }
        ଅନ୍ୟଥା ଯଦି(post->flags & HTTPPOST_BUFFER)
          result = curl_mime_data(part, post->buffer,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        ଅନ୍ୟଥା ଯଦି(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          ଯଦି(!clen)
            clen = -1;
          result = curl_mime_data_cb(part, clen,
                                     fread_func, ଖାଲି, ଖାଲି, post->userp);
        }
        ଅନ୍ୟଥା {
          size_t uclen;
          ଯଦି(!clen)
            uclen = CURL_ZERO_TERMINATED;
          ଅନ୍ୟଥା
            uclen = (size_t)clen;
          result = curl_mime_data(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      ଯଦି(!result && post->showfilename &&
         (post->more || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_BUFFER |
                                        HTTPPOST_CALLBACK))))
        result = curl_mime_filename(part, post->showfilename);
    }
  }

  ଯଦି(result)
    Curl_mime_cleanpart(finalform);

  ଫେରନ୍ତୁ result;
}

#ଅନ୍ୟଥା /* if disabled */
CURLFORMcode curl_formadd(ସଂରଚନା curl_httppost **httppost,
                          ସଂରଚନା curl_httppost **last_post, ...)
{
  (ଶୂନ୍ୟ)httppost;
  (ଶୂନ୍ୟ)last_post;
  ଫେରନ୍ତୁ CURL_FORMADD_DISABLED;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା curl_formget(ସଂରଚନା curl_httppost *form, ଶୂନ୍ୟ *arg,
                 curl_formget_callback append)
{
  (ଶୂନ୍ୟ)form;
  (ଶୂନ୍ୟ)arg;
  (ଶୂନ୍ୟ)append;
  ଫେରନ୍ତୁ CURL_FORMADD_DISABLED;
}

ଶୂନ୍ୟ curl_formfree(ସଂରଚନା curl_httppost *form)
{
  (ଶୂନ୍ୟ)form;
  /* Nothing to do. */
}

#endif /* if disabled */
