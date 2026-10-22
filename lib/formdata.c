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
#അന്തര്ഭാവ "curl_setup.h"

ഘടന Curl_easy;

#അന്തര്ഭാവ "formdata.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#അന്തര്ഭാവ "urldata.h" /* for struct Curl_easy */
#അന്തര്ഭാവ "mime.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "bufref.h"
#അന്തര്ഭാവ "curlx/win32-fopen.h"


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
സ്ഥിതം ഘടന curl_httppost *httppost_add(ഘടന FormInfo *src,
                                          ഘടന curl_httppost *parent_post,
                                          ഘടന curl_httppost **httppost,
                                          ഘടന curl_httppost **last_post)
{
  ഘടന curl_httppost *post;
  size_t namelength = src->namelength;
  എങ്കിൽ(!namelength && Curl_bufref_ptr(&src->name))
    namelength = strlen(Curl_bufref_ptr(&src->name));
  എങ്കിൽ((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    തിരികെ ശൂന്യ;
  post = curlx_calloc(1, വലുപ്പം(ഘടന curl_httppost));
  എങ്കിൽ(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (long)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->മൂല്യം));
    post->contentlen = src->contentslength;
    post->buffer = src->buffer;
    post->bufferlength = (long)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  അല്ലെങ്കിൽ
    തിരികെ ശൂന്യ;

  എങ്കിൽ(parent_post) {
    /* now, point our 'more' to the original 'more' */
    post->more = parent_post->more;

    /* then move the original 'more' to point to ourselves */
    parent_post->more = post;
  }
  അല്ലെങ്കിൽ {
    /* make the previous point to this */
    എങ്കിൽ(*last_post)
      (*last_post)->next = post;
    അല്ലെങ്കിൽ
      (*httppost) = post;

    (*last_post) = post;
  }
  തിരികെ post;
}

/* Allocate and initialize a new FormInfo structure. */
സ്ഥിതം ഘടന FormInfo *forminfo_new(ശൂന്യം)
{
  ഘടന FormInfo *form_info = curlx_calloc(1, വലുപ്പം(ഘടന FormInfo));

  എങ്കിൽ(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->മൂല്യം);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  തിരികെ form_info;
}

/* Replace the target field data by a dynamic copy of it. */
സ്ഥിതം CURLcode forminfo_copyfield(ഘടന bufref *field, size_t len)
{
  സ്ഥിരം അക്ഷരം *മൂല്യം = Curl_bufref_ptr(field);
  CURLcode result = CURLE_OK;

  എങ്കിൽ(മൂല്യം) {
    എങ്കിൽ(!len)
      len = strlen(മൂല്യം);
    result = Curl_bufref_memdup0(field, മൂല്യം, len);
  }

  തിരികെ result;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
സ്ഥിതം ശൂന്യം forminfo_add(ഘടന FormInfo *form_info, ഘടന FormInfo *parent)
{
  form_info->flags |= HTTPPOST_FILENAME;

  എങ്കിൽ(parent) {
    /* now, point our 'more' to the original 'more' */
    form_info->more = parent->more;

    /* then move the original 'more' to point to ourselves */
    parent->more = form_info;
  }
}

സ്ഥിതം ശൂന്യം free_formlist(ഘടന FormInfo *ptr)
{
  വേണ്ടി(; ptr; ptr = ptr->more) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->മൂല്യം);
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

സ്ഥിതം CURLFORMcode formadd_check(ഘടന FormInfo *first_form,
                                  ഘടന curl_httppost **httppost,
                                  ഘടന curl_httppost **last_post)
{
  സ്ഥിരം അക്ഷരം *prevtype = ശൂന്യ;
  ഘടന FormInfo *form = ശൂന്യ;
  ഘടന curl_httppost *post = ശൂന്യ;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  വേണ്ടി(form = first_form; form; form = form->more) {
    സ്ഥിരം അക്ഷരം *name = Curl_bufref_ptr(&form->name);

    എങ്കിൽ(((!name || !Curl_bufref_ptr(&form->മൂല്യം)) && !post) ||
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
      തിരികെ CURL_FORMADD_INCOMPLETE;
    }
    എങ്കിൽ(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_BUFFER)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      സ്ഥിരം അക്ഷരം *f = Curl_bufref_ptr((form->flags & HTTPPOST_BUFFER) ?
                                      &form->showfilename : &form->മൂല്യം);
      സ്ഥിരം അക്ഷരം *type = Curl_mime_contenttype(f);
      എങ്കിൽ(!type)
        type = prevtype;
      എങ്കിൽ(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      എങ്കിൽ(Curl_bufref_memdup0(&form->contenttype, type, strlen(type)))
        തിരികെ CURL_FORMADD_MEMORY;
    }
    എങ്കിൽ(name && form->namelength && memchr(name, 0, form->namelength))
      തിരികെ CURL_FORMADD_NULL;
    എങ്കിൽ(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      തിരികെ CURL_FORMADD_MEMORY;
    എങ്കിൽ(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->മൂല്യം, (size_t)form->contentslength))
      തിരികെ CURL_FORMADD_MEMORY;
    post = httppost_add(form, post, httppost, last_post);

    എങ്കിൽ(!post)
      തിരികെ CURL_FORMADD_MEMORY;

    എങ്കിൽ(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  തിരികെ CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
സ്ഥിതം ശൂന്യം free_chain(ഘടന curl_httppost *c)
{
  ആകുമ്പോൾ(c) {
    ഘടന curl_httppost *next = c->next;
    എങ്കിൽ(c->more)
      free_chain(c->more);
    curlx_free(c);
    c = next;
  }
}

സ്ഥിതം CURLFORMcode formadd(ഘടന curl_httppost **httppost,
                            ഘടന curl_httppost **last_post, va_list params)
{
  ഘടന FormInfo *first_form, *curr, *form = ശൂന്യ;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  സ്ഥിരം ഘടന curl_forms *forms = ശൂന്യ;
  അക്ഷരം *avalue = ശൂന്യ;
  ഘടന curl_httppost *newchain = ശൂന്യ;
  ഘടന curl_httppost *lastnode = ശൂന്യ;

#define form_ptr_arg(t) (forms ? (t)(ശൂന്യം *)avalue : va_arg(params, t))
#ifdef HAVE_UINTPTR_T
#define form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#അല്ലെങ്കിൽ
#define form_int_arg(t) (forms ? (t)(ശൂന്യം *)avalue : va_arg(params, t))
#endif

  /*
   * We need to allocate the first struct to fill in.
   */
  first_form = forminfo_new();
  എങ്കിൽ(!first_form)
    തിരികെ CURL_FORMADD_MEMORY;

  curr = first_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  ആകുമ്പോൾ(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    എങ്കിൽ(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (അക്ഷരം *)CURL_UNCONST(forms->മൂല്യം);

      forms++; /* advance this to next entry */
      എങ്കിൽ(CURLFORM_END == option) {
        /* end of array state */
        forms = ശൂന്യ;
        തുടരുക;
      }
    }
    അല്ലെങ്കിൽ {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, പൂർണ്ണസംഖ്യ);
      എങ്കിൽ(CURLFORM_END == option)
        നിർത്തുക;
    }

    തിരഞ്ഞെടുപ്പ്(option) {
    അവസ്ഥ CURLFORM_ARRAY:
      എങ്കിൽ(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      അല്ലെങ്കിൽ {
        forms = va_arg(params, ഘടന curl_forms *);
        എങ്കിൽ(!forms)
          retval = CURL_FORMADD_NULL;
      }
      നിർത്തുക;

      /*
       * Set the Name property.
       */
    അവസ്ഥ CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    അവസ്ഥ CURLFORM_COPYNAME:
      എങ്കിൽ(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      അല്ലെങ്കിൽ {
        avalue = form_ptr_arg(അക്ഷരം *);
        എങ്കിൽ(avalue)
          Curl_bufref_set(&curr->name, avalue, 0, ശൂന്യ); /* No copy yet. */
        അല്ലെങ്കിൽ
          retval = CURL_FORMADD_NULL;
      }
      നിർത്തുക;
    അവസ്ഥ CURLFORM_NAMELENGTH:
      എങ്കിൽ(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      അല്ലെങ്കിൽ
        curr->namelength = (size_t)form_int_arg(long);
      നിർത്തുക;

      /*
       * Set the contents property.
       */
    അവസ്ഥ CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    അവസ്ഥ CURLFORM_COPYCONTENTS:
      എങ്കിൽ(Curl_bufref_ptr(&curr->മൂല്യം))
        retval = CURL_FORMADD_OPTION_TWICE;
      അല്ലെങ്കിൽ {
        avalue = form_ptr_arg(അക്ഷരം *);
        എങ്കിൽ(avalue)
          Curl_bufref_set(&curr->മൂല്യം, avalue, 0, ശൂന്യ); /* No copy yet. */
        അല്ലെങ്കിൽ
          retval = CURL_FORMADD_NULL;
      }
      നിർത്തുക;
    അവസ്ഥ CURLFORM_CONTENTSLENGTH: {
      long clen = form_int_arg(long);
      എങ്കിൽ(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      അല്ലെങ്കിൽ
        curr->contentslength = (curl_off_t)clen;
      നിർത്തുക;
    }

    അവസ്ഥ CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      എങ്കിൽ(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      അല്ലെങ്കിൽ {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      നിർത്തുക;
    }

      /* Get contents from a given filename */
    അവസ്ഥ CURLFORM_FILECONTENT:
      എങ്കിൽ(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      അല്ലെങ്കിൽ {
        avalue = form_ptr_arg(അക്ഷരം *);
        എങ്കിൽ(avalue) {
          എങ്കിൽ(Curl_bufref_memdup0(&curr->മൂല്യം, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          അല്ലെങ്കിൽ
            curr->flags |= HTTPPOST_READFILE;
        }
        അല്ലെങ്കിൽ
          retval = CURL_FORMADD_NULL;
      }
      നിർത്തുക;

      /* We upload a file */
    അവസ്ഥ CURLFORM_FILE:
      avalue = form_ptr_arg(അക്ഷരം *);
      എങ്കിൽ(Curl_bufref_ptr(&curr->മൂല്യം)) {
        എങ്കിൽ(curr->flags & HTTPPOST_FILENAME) {
          എങ്കിൽ(avalue) {
            form = forminfo_new();
            എങ്കിൽ(!form ||
               Curl_bufref_memdup0(&form->മൂല്യം, avalue, strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            അല്ലെങ്കിൽ {
              forminfo_add(form, curr);
              curr = form;
              form = ശൂന്യ;
            }
          }
          അല്ലെങ്കിൽ
            retval = CURL_FORMADD_NULL;
        }
        അല്ലെങ്കിൽ
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      അല്ലെങ്കിൽ {
        എങ്കിൽ(avalue) {
          എങ്കിൽ(Curl_bufref_memdup0(&curr->മൂല്യം, avalue, strlen(avalue)))
            retval = CURL_FORMADD_MEMORY;
          അല്ലെങ്കിൽ
            curr->flags |= HTTPPOST_FILENAME;
        }
        അല്ലെങ്കിൽ
          retval = CURL_FORMADD_NULL;
      }
      നിർത്തുക;

    അവസ്ഥ CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_BUFFER;
      എങ്കിൽ(curr->buffer)
        retval = CURL_FORMADD_OPTION_TWICE;
      അല്ലെങ്കിൽ {
        avalue = form_ptr_arg(അക്ഷരം *);
        എങ്കിൽ(avalue) {
          curr->buffer = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_set(&curr->മൂല്യം, avalue, 0, ശൂന്യ);
        }
        അല്ലെങ്കിൽ
          retval = CURL_FORMADD_NULL;
      }
      നിർത്തുക;

    അവസ്ഥ CURLFORM_BUFFERLENGTH:
      എങ്കിൽ(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      അല്ലെങ്കിൽ
        curr->bufferlength = (size_t)form_int_arg(long);
      നിർത്തുക;

    അവസ്ഥ CURLFORM_STREAM:
      curr->flags |= HTTPPOST_CALLBACK;
      എങ്കിൽ(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      അല്ലെങ്കിൽ {
        avalue = form_ptr_arg(അക്ഷരം *);
        എങ്കിൽ(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_set(&curr->മൂല്യം, avalue, 0, ശൂന്യ);
        }
        അല്ലെങ്കിൽ
          retval = CURL_FORMADD_NULL;
      }
      നിർത്തുക;

    അവസ്ഥ CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(അക്ഷരം *);
      എങ്കിൽ(Curl_bufref_ptr(&curr->contenttype)) {
        എങ്കിൽ(curr->flags & HTTPPOST_FILENAME) {
          എങ്കിൽ(avalue) {
            form = forminfo_new();
            എങ്കിൽ(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            strlen(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            അല്ലെങ്കിൽ {
              forminfo_add(form, curr);
              curr = form;
              form = ശൂന്യ;
            }
          }
          അല്ലെങ്കിൽ
            retval = CURL_FORMADD_NULL;
        }
        അല്ലെങ്കിൽ
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(avalue) {
        എങ്കിൽ(Curl_bufref_memdup0(&curr->contenttype, avalue, strlen(avalue)))
          retval = CURL_FORMADD_MEMORY;
      }
      അല്ലെങ്കിൽ
        retval = CURL_FORMADD_NULL;
      നിർത്തുക;

    അവസ്ഥ CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      ഘടന curl_slist *list = form_ptr_arg(ഘടന curl_slist *);

      എങ്കിൽ(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      അല്ലെങ്കിൽ
        curr->contentheader = list;

      നിർത്തുക;
    }
    അവസ്ഥ CURLFORM_FILENAME:
    അവസ്ഥ CURLFORM_BUFFER:
      avalue = form_ptr_arg(അക്ഷരം *);
      എങ്കിൽ(!avalue)
        retval = CURL_FORMADD_NULL;
      അല്ലെങ്കിൽ എങ്കിൽ(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      അല്ലെങ്കിൽ എങ്കിൽ(Curl_bufref_memdup0(&curr->showfilename, avalue, strlen(avalue)))
        retval = CURL_FORMADD_MEMORY;
      നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      നിർത്തുക;
    }
  }

  എങ്കിൽ(!retval)
    retval = formadd_check(first_form, &newchain, &lastnode);

  എങ്കിൽ(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(first_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  ആകുമ്പോൾ(first_form) {
    ഘടന FormInfo *ptr = first_form->more;
    curlx_free(first_form);
    first_form = ptr;
  }

  എങ്കിൽ(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    എങ്കിൽ(*last_post)
      (*last_post)->next = newchain;
    അല്ലെങ്കിൽ
      (*httppost) = newchain;

    (*last_post) = lastnode;
  }
  അല്ലെങ്കിൽ
    free_chain(newchain);

  തിരികെ retval;
#undef form_ptr_arg
#undef form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(ഘടന curl_httppost **httppost,
                          ഘടന curl_httppost **last_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, last_post);
  form = formadd(httppost, last_post, arg);
  va_end(arg);
  തിരികെ form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
പൂർണ്ണസംഖ്യ curl_formget(ഘടന curl_httppost *form, ശൂന്യം *arg,
                 curl_formget_callback append)
{
  CURLcode result;
  curl_mimepart toppart;

  /* Validate callback is provided */
  എങ്കിൽ(!append)
    തിരികെ (പൂർണ്ണസംഖ്യ)CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mime_initpart(&toppart); /* default form is empty */
  result = Curl_getformdata(ശൂന്യ, &toppart, form, ശൂന്യ);
  എങ്കിൽ(!result)
    result = Curl_mime_prepare_headers(ശൂന്യ, &toppart, "multipart/form-data",
                                       ശൂന്യ, MIMESTRATEGY_FORM);

  ആകുമ്പോൾ(!result) {
    അക്ഷരം buffer[8192];
    size_t nread = Curl_mime_read(buffer, 1, വലുപ്പം(buffer), &toppart);

    എങ്കിൽ(!nread)
      നിർത്തുക;

    എങ്കിൽ(nread > വലുപ്പം(buffer) || append(arg, buffer, nread) != nread) {
      result = CURLE_READ_ERROR;
      എങ്കിൽ(nread == CURL_READFUNC_ABORT)
        result = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  തിരികെ (പൂർണ്ണസംഖ്യ)result;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
ശൂന്യം curl_formfree(ഘടന curl_httppost *form)
{
  ഘടന curl_httppost *next;

  എങ്കിൽ(!form)
    /* no form to free, get out of this */
    തിരികെ;

  ചെയ്യുക {
    next = form->next;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->more);

    എങ്കിൽ(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    എങ്കിൽ(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_BUFFER | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = next;
  } ആകുമ്പോൾ(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
സ്ഥിതം CURLcode setname(curl_mimepart *part, സ്ഥിരം അക്ഷരം *name, size_t len)
{
  അക്ഷരം *zname;
  CURLcode result;

  എങ്കിൽ(!name || !len)
    തിരികെ curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  എങ്കിൽ(!zname)
    തിരികെ CURLE_OUT_OF_MEMORY;
  result = curl_mime_name(part, zname);
  curlx_free(zname);
  തിരികെ result;
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
                          ഘടന curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode result = CURLE_OK;
  curl_mime *form = ശൂന്യ;
  curl_mimepart *part;
  ഘടന curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  എങ്കിൽ(!post)
    തിരികെ result; /* no input => no output! */

  form = curl_mime_init(data);
  എങ്കിൽ(!form)
    result = CURLE_OUT_OF_MEMORY;

  എങ്കിൽ(!result)
    result = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  വേണ്ടി(; !result && post; post = post->next) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    എങ്കിൽ(post->more) {
      part = curl_mime_addpart(form);
      എങ്കിൽ(!part)
        result = CURLE_OUT_OF_MEMORY;
      എങ്കിൽ(!result)
        result = setname(part, post->name, post->namelength);
      എങ്കിൽ(!result) {
        multipart = curl_mime_init(data);
        എങ്കിൽ(!multipart)
          result = CURLE_OUT_OF_MEMORY;
      }
      എങ്കിൽ(!result)
        result = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    വേണ്ടി(file = post; !result && file; file = file->more) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      എങ്കിൽ(!part)
        result = CURLE_OUT_OF_MEMORY;

      /* Set the headers. */
      എങ്കിൽ(!result)
        result = curl_mime_headers(part, file->contentheader, 0);

      /* Set the content type. */
      എങ്കിൽ(!result && file->contenttype)
        result = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      എങ്കിൽ(!result && !post->more)
        result = setname(part, post->name, post->namelength);

      /* Process contents. */
      എങ്കിൽ(!result) {
        curl_off_t clen = post->contentslength;

        എങ്കിൽ(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        എങ്കിൽ(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          എങ്കിൽ(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#എങ്കിൽ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic തള്ളുക
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
            result = curl_mime_data_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       ശൂന്യ, (ശൂന്യം *)stdin);
#എങ്കിൽ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic എടുക്കുക
#endif
          }
          അല്ലെങ്കിൽ
            result = curl_mime_filedata(part, file->contents);
          എങ്കിൽ(!result && (post->flags & HTTPPOST_READFILE))
            result = curl_mime_filename(part, ശൂന്യ);
        }
        അല്ലെങ്കിൽ എങ്കിൽ(post->flags & HTTPPOST_BUFFER)
          result = curl_mime_data(part, post->buffer,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        അല്ലെങ്കിൽ എങ്കിൽ(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          എങ്കിൽ(!clen)
            clen = -1;
          result = curl_mime_data_cb(part, clen,
                                     fread_func, ശൂന്യ, ശൂന്യ, post->userp);
        }
        അല്ലെങ്കിൽ {
          size_t uclen;
          എങ്കിൽ(!clen)
            uclen = CURL_ZERO_TERMINATED;
          അല്ലെങ്കിൽ
            uclen = (size_t)clen;
          result = curl_mime_data(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      എങ്കിൽ(!result && post->showfilename &&
         (post->more || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_BUFFER |
                                        HTTPPOST_CALLBACK))))
        result = curl_mime_filename(part, post->showfilename);
    }
  }

  എങ്കിൽ(result)
    Curl_mime_cleanpart(finalform);

  തിരികെ result;
}

#അല്ലെങ്കിൽ /* if disabled */
CURLFORMcode curl_formadd(ഘടന curl_httppost **httppost,
                          ഘടന curl_httppost **last_post, ...)
{
  (ശൂന്യം)httppost;
  (ശൂന്യം)last_post;
  തിരികെ CURL_FORMADD_DISABLED;
}

പൂർണ്ണസംഖ്യ curl_formget(ഘടന curl_httppost *form, ശൂന്യം *arg,
                 curl_formget_callback append)
{
  (ശൂന്യം)form;
  (ശൂന്യം)arg;
  (ശൂന്യം)append;
  തിരികെ CURL_FORMADD_DISABLED;
}

ശൂന്യം curl_formfree(ഘടന curl_httppost *form)
{
  (ശൂന്യം)form;
  /* Nothing to do. */
}

#endif /* if disabled */
