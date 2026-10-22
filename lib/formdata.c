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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

ਬਣਤਰ Curl_easy;

#ਅਨ੍ਤਰ੍ਭਾਵ "formdata.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h" /* for struct Curl_easy */
#ਅਨ੍ਤਰ੍ਭਾਵ "mime.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bufref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPPOST_PTRNAME     CURL_HTTPPOST_PTRNAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPPOST_FILENAME    CURL_HTTPPOST_FILENAME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPPOST_PTRCONTENTS CURL_HTTPPOST_PTRCONTENTS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPPOST_READFILE    CURL_HTTPPOST_READFILE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPPOST_PTRBUFFER   CURL_HTTPPOST_PTRBUFFER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPPOST_CALLBACK    CURL_HTTPPOST_CALLBACK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTTPPOST_ਬਫਰ      CURL_HTTPPOST_ਬਫਰ

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
ਸਥਿਰ ਬਣਤਰ curl_httppost *httppost_ਜੋੜੋ(ਬਣਤਰ FormInfo *src,
                                          ਬਣਤਰ curl_httppost *ਮਾਪਾ_post,
                                          ਬਣਤਰ curl_httppost **httppost,
                                          ਬਣਤਰ curl_httppost **ਆਖਰੀ_post)
{
  ਬਣਤਰ curl_httppost *post;
  size_t namelength = src->namelength;
  ਜੇ(!namelength && Curl_bufref_ptr(&src->name))
    namelength = ਲੜੀ_ਲੰਬਾਈ(Curl_bufref_ptr(&src->name));
  ਜੇ((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    ਵਾਪਸ ਨਿਰਾ;
  post = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ curl_httppost));
  ਜੇ(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (ਲੰਮਾ)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->ਮੁੱਲ));
    post->contentlen = src->contentslength;
    post->ਬਫਰ = src->ਬਫਰ;
    post->bufferlength = (ਲੰਮਾ)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ ਨਿਰਾ;

  ਜੇ(ਮਾਪਾ_post) {
    /* now, point our 'more' to the original 'more' */
    post->ਹੋਰ = ਮਾਪਾ_post->ਹੋਰ;

    /* then move the original 'more' to point to ourselves */
    ਮਾਪਾ_post->ਹੋਰ = post;
  }
  ਨਹੀਂਤਾਂ {
    /* make the previous point to this */
    ਜੇ(*ਆਖਰੀ_post)
      (*ਆਖਰੀ_post)->ਅਗਲਾ = post;
    ਨਹੀਂਤਾਂ
      (*httppost) = post;

    (*ਆਖਰੀ_post) = post;
  }
  ਵਾਪਸ post;
}

/* Allocate and initialize a new FormInfo structure. */
ਸਥਿਰ ਬਣਤਰ FormInfo *forminfo_new(ਖਾਲੀ)
{
  ਬਣਤਰ FormInfo *form_info = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ FormInfo));

  ਜੇ(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->ਮੁੱਲ);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  ਵਾਪਸ form_info;
}

/* Replace the target field data by a dynamic copy of it. */
ਸਥਿਰ CURLcode forminfo_copyfield(ਬਣਤਰ bufref *ਖੇਤਰ, size_t len)
{
  ਅਚਲ ਅੱਖਰ *ਮੁੱਲ = Curl_bufref_ptr(ਖੇਤਰ);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(ਮੁੱਲ) {
    ਜੇ(!len)
      len = ਲੜੀ_ਲੰਬਾਈ(ਮੁੱਲ);
    ਨਤੀਜਾ = Curl_bufref_memdup0(ਖੇਤਰ, ਮੁੱਲ, len);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
ਸਥਿਰ ਖਾਲੀ forminfo_ਜੋੜੋ(ਬਣਤਰ FormInfo *form_info, ਬਣਤਰ FormInfo *ਮਾਪਾ)
{
  form_info->flags |= HTTPPOST_FILENAME;

  ਜੇ(ਮਾਪਾ) {
    /* now, point our 'more' to the original 'more' */
    form_info->ਹੋਰ = ਮਾਪਾ->ਹੋਰ;

    /* then move the original 'more' to point to ourselves */
    ਮਾਪਾ->ਹੋਰ = form_info;
  }
}

ਸਥਿਰ ਖਾਲੀ free_formlist(ਬਣਤਰ FormInfo *ptr)
{
  ਲਈ(; ptr; ptr = ptr->ਹੋਰ) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->ਮੁੱਲ);
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

ਸਥਿਰ CURLFORMcode formadd_check(ਬਣਤਰ FormInfo *ਪਹਿਲਾ_form,
                                  ਬਣਤਰ curl_httppost **httppost,
                                  ਬਣਤਰ curl_httppost **ਆਖਰੀ_post)
{
  ਅਚਲ ਅੱਖਰ *prevtype = ਨਿਰਾ;
  ਬਣਤਰ FormInfo *form = ਨਿਰਾ;
  ਬਣਤਰ curl_httppost *post = ਨਿਰਾ;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  ਲਈ(form = ਪਹਿਲਾ_form; form; form = form->ਹੋਰ) {
    ਅਚਲ ਅੱਖਰ *name = Curl_bufref_ptr(&form->name);

    ਜੇ(((!name || !Curl_bufref_ptr(&form->ਮੁੱਲ)) && !post) ||
       (form->contentslength &&
        (form->flags & HTTPPOST_FILENAME)) ||
       ((form->flags & HTTPPOST_FILENAME) &&
        (form->flags & HTTPPOST_PTRCONTENTS)) ||

       (!form->ਬਫਰ &&
        (form->flags & HTTPPOST_ਬਫਰ) &&
        (form->flags & HTTPPOST_PTRBUFFER)) ||

       ((form->flags & HTTPPOST_READFILE) &&
        (form->flags & HTTPPOST_PTRCONTENTS))
      ) {
      ਵਾਪਸ CURL_FORMADD_INCOMPLETE;
    }
    ਜੇ(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_ਬਫਰ)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      ਅਚਲ ਅੱਖਰ *f = Curl_bufref_ptr((form->flags & HTTPPOST_ਬਫਰ) ?
                                      &form->showfilename : &form->ਮੁੱਲ);
      ਅਚਲ ਅੱਖਰ *type = Curl_mime_contenttype(f);
      ਜੇ(!type)
        type = prevtype;
      ਜੇ(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      ਜੇ(Curl_bufref_memdup0(&form->contenttype, type, ਲੜੀ_ਲੰਬਾਈ(type)))
        ਵਾਪਸ CURL_FORMADD_ਸਮ੍ਰਿਤੀ;
    }
    ਜੇ(name && form->namelength && memchr(name, 0, form->namelength))
      ਵਾਪਸ CURL_FORMADD_NULL;
    ਜੇ(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      ਵਾਪਸ CURL_FORMADD_ਸਮ੍ਰਿਤੀ;
    ਜੇ(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->ਮੁੱਲ, (size_t)form->contentslength))
      ਵਾਪਸ CURL_FORMADD_ਸਮ੍ਰਿਤੀ;
    post = httppost_ਜੋੜੋ(form, post, httppost, ਆਖਰੀ_post);

    ਜੇ(!post)
      ਵਾਪਸ CURL_FORMADD_ਸਮ੍ਰਿਤੀ;

    ਜੇ(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  ਵਾਪਸ CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
ਸਥਿਰ ਖਾਲੀ free_chain(ਬਣਤਰ curl_httppost *c)
{
  ਜਦੋਂ(c) {
    ਬਣਤਰ curl_httppost *ਅਗਲਾ = c->ਅਗਲਾ;
    ਜੇ(c->ਹੋਰ)
      free_chain(c->ਹੋਰ);
    curlx_free(c);
    c = ਅਗਲਾ;
  }
}

ਸਥਿਰ CURLFORMcode formadd(ਬਣਤਰ curl_httppost **httppost,
                            ਬਣਤਰ curl_httppost **ਆਖਰੀ_post, va_list params)
{
  ਬਣਤਰ FormInfo *ਪਹਿਲਾ_form, *curr, *form = ਨਿਰਾ;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  ਅਚਲ ਬਣਤਰ curl_forms *forms = ਨਿਰਾ;
  ਅੱਖਰ *avalue = ਨਿਰਾ;
  ਬਣਤਰ curl_httppost *newchain = ਨਿਰਾ;
  ਬਣਤਰ curl_httppost *lastnode = ਨਿਰਾ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ form_ptr_arg(t) (forms ? (t)(ਖਾਲੀ *)avalue : va_arg(params, t))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_UINTPTR_T
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ form_int_arg(t) (forms ? (t)(ਖਾਲੀ *)avalue : va_arg(params, t))
#ਜੇ_ਅੰਤ

  /*
   * We need to allocate the first struct to fill in.
   */
  ਪਹਿਲਾ_form = forminfo_new();
  ਜੇ(!ਪਹਿਲਾ_form)
    ਵਾਪਸ CURL_FORMADD_ਸਮ੍ਰਿਤੀ;

  curr = ਪਹਿਲਾ_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  ਜਦੋਂ(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    ਜੇ(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (ਅੱਖਰ *)CURL_UNCONST(forms->ਮੁੱਲ);

      forms++; /* advance this to next entry */
      ਜੇ(CURLFORM_ਸਿਰਾ == option) {
        /* end of array state */
        forms = ਨਿਰਾ;
        ਜਾਰੀ;
      }
    }
    ਨਹੀਂਤਾਂ {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, ਪੂਰਨ);
      ਜੇ(CURLFORM_ਸਿਰਾ == option)
        ਰੋਕੋ;
    }

    ਚੋਣ(option) {
    ਹਾਲਤ CURLFORM_ARRAY:
      ਜੇ(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      ਨਹੀਂਤਾਂ {
        forms = va_arg(params, ਬਣਤਰ curl_forms *);
        ਜੇ(!forms)
          retval = CURL_FORMADD_NULL;
      }
      ਰੋਕੋ;

      /*
       * Set the Name property.
       */
    ਹਾਲਤ CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    ਹਾਲਤ CURLFORM_COPYNAME:
      ਜੇ(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      ਨਹੀਂਤਾਂ {
        avalue = form_ptr_arg(ਅੱਖਰ *);
        ਜੇ(avalue)
          Curl_bufref_ਸੈੱਟ(&curr->name, avalue, 0, ਨਿਰਾ); /* No copy yet. */
        ਨਹੀਂਤਾਂ
          retval = CURL_FORMADD_NULL;
      }
      ਰੋਕੋ;
    ਹਾਲਤ CURLFORM_NAMELENGTH:
      ਜੇ(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      ਨਹੀਂਤਾਂ
        curr->namelength = (size_t)form_int_arg(ਲੰਮਾ);
      ਰੋਕੋ;

      /*
       * Set the contents property.
       */
    ਹਾਲਤ CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    ਹਾਲਤ CURLFORM_COPYCONTENTS:
      ਜੇ(Curl_bufref_ptr(&curr->ਮੁੱਲ))
        retval = CURL_FORMADD_OPTION_TWICE;
      ਨਹੀਂਤਾਂ {
        avalue = form_ptr_arg(ਅੱਖਰ *);
        ਜੇ(avalue)
          Curl_bufref_ਸੈੱਟ(&curr->ਮੁੱਲ, avalue, 0, ਨਿਰਾ); /* No copy yet. */
        ਨਹੀਂਤਾਂ
          retval = CURL_FORMADD_NULL;
      }
      ਰੋਕੋ;
    ਹਾਲਤ CURLFORM_CONTENTSLENGTH: {
      ਲੰਮਾ clen = form_int_arg(ਲੰਮਾ);
      ਜੇ(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      ਨਹੀਂਤਾਂ
        curr->contentslength = (curl_off_t)clen;
      ਰੋਕੋ;
    }

    ਹਾਲਤ CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      ਜੇ(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      ਨਹੀਂਤਾਂ {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      ਰੋਕੋ;
    }

      /* Get contents from a given filename */
    ਹਾਲਤ CURLFORM_FILECONTENT:
      ਜੇ(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      ਨਹੀਂਤਾਂ {
        avalue = form_ptr_arg(ਅੱਖਰ *);
        ਜੇ(avalue) {
          ਜੇ(Curl_bufref_memdup0(&curr->ਮੁੱਲ, avalue, ਲੜੀ_ਲੰਬਾਈ(avalue)))
            retval = CURL_FORMADD_ਸਮ੍ਰਿਤੀ;
          ਨਹੀਂਤਾਂ
            curr->flags |= HTTPPOST_READFILE;
        }
        ਨਹੀਂਤਾਂ
          retval = CURL_FORMADD_NULL;
      }
      ਰੋਕੋ;

      /* We upload a file */
    ਹਾਲਤ CURLFORM_FILE:
      avalue = form_ptr_arg(ਅੱਖਰ *);
      ਜੇ(Curl_bufref_ptr(&curr->ਮੁੱਲ)) {
        ਜੇ(curr->flags & HTTPPOST_FILENAME) {
          ਜੇ(avalue) {
            form = forminfo_new();
            ਜੇ(!form ||
               Curl_bufref_memdup0(&form->ਮੁੱਲ, avalue, ਲੜੀ_ਲੰਬਾਈ(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_ਸਮ੍ਰਿਤੀ;
            }
            ਨਹੀਂਤਾਂ {
              forminfo_ਜੋੜੋ(form, curr);
              curr = form;
              form = ਨਿਰਾ;
            }
          }
          ਨਹੀਂਤਾਂ
            retval = CURL_FORMADD_NULL;
        }
        ਨਹੀਂਤਾਂ
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      ਨਹੀਂਤਾਂ {
        ਜੇ(avalue) {
          ਜੇ(Curl_bufref_memdup0(&curr->ਮੁੱਲ, avalue, ਲੜੀ_ਲੰਬਾਈ(avalue)))
            retval = CURL_FORMADD_ਸਮ੍ਰਿਤੀ;
          ਨਹੀਂਤਾਂ
            curr->flags |= HTTPPOST_FILENAME;
        }
        ਨਹੀਂਤਾਂ
          retval = CURL_FORMADD_NULL;
      }
      ਰੋਕੋ;

    ਹਾਲਤ CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_ਬਫਰ;
      ਜੇ(curr->ਬਫਰ)
        retval = CURL_FORMADD_OPTION_TWICE;
      ਨਹੀਂਤਾਂ {
        avalue = form_ptr_arg(ਅੱਖਰ *);
        ਜੇ(avalue) {
          curr->ਬਫਰ = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_ਸੈੱਟ(&curr->ਮੁੱਲ, avalue, 0, ਨਿਰਾ);
        }
        ਨਹੀਂਤਾਂ
          retval = CURL_FORMADD_NULL;
      }
      ਰੋਕੋ;

    ਹਾਲਤ CURLFORM_BUFFERLENGTH:
      ਜੇ(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      ਨਹੀਂਤਾਂ
        curr->bufferlength = (size_t)form_int_arg(ਲੰਮਾ);
      ਰੋਕੋ;

    ਹਾਲਤ CURLFORM_ਧਾਰਾ:
      curr->flags |= HTTPPOST_CALLBACK;
      ਜੇ(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      ਨਹੀਂਤਾਂ {
        avalue = form_ptr_arg(ਅੱਖਰ *);
        ਜੇ(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_ਸੈੱਟ(&curr->ਮੁੱਲ, avalue, 0, ਨਿਰਾ);
        }
        ਨਹੀਂਤਾਂ
          retval = CURL_FORMADD_NULL;
      }
      ਰੋਕੋ;

    ਹਾਲਤ CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(ਅੱਖਰ *);
      ਜੇ(Curl_bufref_ptr(&curr->contenttype)) {
        ਜੇ(curr->flags & HTTPPOST_FILENAME) {
          ਜੇ(avalue) {
            form = forminfo_new();
            ਜੇ(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            ਲੜੀ_ਲੰਬਾਈ(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_ਸਮ੍ਰਿਤੀ;
            }
            ਨਹੀਂਤਾਂ {
              forminfo_ਜੋੜੋ(form, curr);
              curr = form;
              form = ਨਿਰਾ;
            }
          }
          ਨਹੀਂਤਾਂ
            retval = CURL_FORMADD_NULL;
        }
        ਨਹੀਂਤਾਂ
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      ਨਹੀਂਤਾਂ ਜੇ(avalue) {
        ਜੇ(Curl_bufref_memdup0(&curr->contenttype, avalue, ਲੜੀ_ਲੰਬਾਈ(avalue)))
          retval = CURL_FORMADD_ਸਮ੍ਰਿਤੀ;
      }
      ਨਹੀਂਤਾਂ
        retval = CURL_FORMADD_NULL;
      ਰੋਕੋ;

    ਹਾਲਤ CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      ਬਣਤਰ curl_slist *list = form_ptr_arg(ਬਣਤਰ curl_slist *);

      ਜੇ(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      ਨਹੀਂਤਾਂ
        curr->contentheader = list;

      ਰੋਕੋ;
    }
    ਹਾਲਤ CURLFORM_FILENAME:
    ਹਾਲਤ CURLFORM_ਬਫਰ:
      avalue = form_ptr_arg(ਅੱਖਰ *);
      ਜੇ(!avalue)
        retval = CURL_FORMADD_NULL;
      ਨਹੀਂਤਾਂ ਜੇ(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      ਨਹੀਂਤਾਂ ਜੇ(Curl_bufref_memdup0(&curr->showfilename, avalue, ਲੜੀ_ਲੰਬਾਈ(avalue)))
        retval = CURL_FORMADD_ਸਮ੍ਰਿਤੀ;
      ਰੋਕੋ;

    ਮੂਲ:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      ਰੋਕੋ;
    }
  }

  ਜੇ(!retval)
    retval = formadd_check(ਪਹਿਲਾ_form, &newchain, &lastnode);

  ਜੇ(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(ਪਹਿਲਾ_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  ਜਦੋਂ(ਪਹਿਲਾ_form) {
    ਬਣਤਰ FormInfo *ptr = ਪਹਿਲਾ_form->ਹੋਰ;
    curlx_free(ਪਹਿਲਾ_form);
    ਪਹਿਲਾ_form = ptr;
  }

  ਜੇ(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    ਜੇ(*ਆਖਰੀ_post)
      (*ਆਖਰੀ_post)->ਅਗਲਾ = newchain;
    ਨਹੀਂਤਾਂ
      (*httppost) = newchain;

    (*ਆਖਰੀ_post) = lastnode;
  }
  ਨਹੀਂਤਾਂ
    free_chain(newchain);

  ਵਾਪਸ retval;
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ form_ptr_arg
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(ਬਣਤਰ curl_httppost **httppost,
                          ਬਣਤਰ curl_httppost **ਆਖਰੀ_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, ਆਖਰੀ_post);
  form = formadd(httppost, ਆਖਰੀ_post, arg);
  va_end(arg);
  ਵਾਪਸ form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
ਪੂਰਨ curl_formget(ਬਣਤਰ curl_httppost *form, ਖਾਲੀ *arg,
                 curl_formget_callback append)
{
  CURLcode ਨਤੀਜਾ;
  curl_mimepart toppart;

  /* Validate callback is provided */
  ਜੇ(!append)
    ਵਾਪਸ (ਪੂਰਨ)CURLE_BAD_FUNCTION_ਦਲੀਲ;

  Curl_mime_initpart(&toppart); /* default form is empty */
  ਨਤੀਜਾ = Curl_getformdata(ਨਿਰਾ, &toppart, form, ਨਿਰਾ);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = Curl_mime_prepare_ਸਿਰਲੇਖਾਂ(ਨਿਰਾ, &toppart, "multipart/form-data",
                                       ਨਿਰਾ, MIMESTRATEGY_FORM);

  ਜਦੋਂ(!ਨਤੀਜਾ) {
    ਅੱਖਰ ਬਫਰ[8192];
    size_t nread = Curl_mime_read(ਬਫਰ, 1, ਆਕਾਰ(ਬਫਰ), &toppart);

    ਜੇ(!nread)
      ਰੋਕੋ;

    ਜੇ(nread > ਆਕਾਰ(ਬਫਰ) || append(arg, ਬਫਰ, nread) != nread) {
      ਨਤੀਜਾ = CURLE_READ_ERROR;
      ਜੇ(nread == CURL_READFUNC_ABORT)
        ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  ਵਾਪਸ (ਪੂਰਨ)ਨਤੀਜਾ;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
ਖਾਲੀ curl_formfree(ਬਣਤਰ curl_httppost *form)
{
  ਬਣਤਰ curl_httppost *ਅਗਲਾ;

  ਜੇ(!form)
    /* no form to free, get out of this */
    ਵਾਪਸ;

  ਕਰੋ {
    ਅਗਲਾ = form->ਅਗਲਾ;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->ਹੋਰ);

    ਜੇ(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    ਜੇ(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_ਬਫਰ | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = ਅਗਲਾ;
  } ਜਦੋਂ(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
ਸਥਿਰ CURLcode setname(curl_mimepart *part, ਅਚਲ ਅੱਖਰ *name, size_t len)
{
  ਅੱਖਰ *zname;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!name || !len)
    ਵਾਪਸ curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  ਜੇ(!zname)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਨਤੀਜਾ = curl_mime_name(part, zname);
  curlx_free(zname);
  ਵਾਪਸ ਨਤੀਜਾ;
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

CURLcode Curl_getformdata(CURL *ਅੰਕੜੇ,
                          curl_mimepart *finalform,
                          ਬਣਤਰ curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  curl_mime *form = ਨਿਰਾ;
  curl_mimepart *part;
  ਬਣਤਰ curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  ਜੇ(!post)
    ਵਾਪਸ ਨਤੀਜਾ; /* no input => no output! */

  form = curl_mime_init(ਅੰਕੜੇ);
  ਜੇ(!form)
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  ਲਈ(; !ਨਤੀਜਾ && post; post = post->ਅਗਲਾ) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    ਜੇ(post->ਹੋਰ) {
      part = curl_mime_addpart(form);
      ਜੇ(!part)
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = setname(part, post->name, post->namelength);
      ਜੇ(!ਨਤੀਜਾ) {
        multipart = curl_mime_init(ਅੰਕੜੇ);
        ਜੇ(!multipart)
          ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    ਲਈ(file = post; !ਨਤੀਜਾ && file; file = file->ਹੋਰ) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      ਜੇ(!part)
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

      /* Set the headers. */
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = curl_mime_ਸਿਰਲੇਖਾਂ(part, file->contentheader, 0);

      /* Set the content type. */
      ਜੇ(!ਨਤੀਜਾ && file->contenttype)
        ਨਤੀਜਾ = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      ਜੇ(!ਨਤੀਜਾ && !post->ਹੋਰ)
        ਨਤੀਜਾ = setname(part, post->name, post->namelength);

      /* Process contents. */
      ਜੇ(!ਨਤੀਜਾ) {
        curl_off_t clen = post->contentslength;

        ਜੇ(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        ਜੇ(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          ਜੇ(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਧੱਕੋ
#ਹਦਾਇਤ clang diagnostic ignored "-Wcast-function-type-strict"
#ਜੇ_ਅੰਤ
            ਨਤੀਜਾ = curl_mime_ਅੰਕੜੇ_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       ਨਿਰਾ, (ਖਾਲੀ *)stdin);
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
          }
          ਨਹੀਂਤਾਂ
            ਨਤੀਜਾ = curl_mime_filedata(part, file->contents);
          ਜੇ(!ਨਤੀਜਾ && (post->flags & HTTPPOST_READFILE))
            ਨਤੀਜਾ = curl_mime_filename(part, ਨਿਰਾ);
        }
        ਨਹੀਂਤਾਂ ਜੇ(post->flags & HTTPPOST_ਬਫਰ)
          ਨਤੀਜਾ = curl_mime_ਅੰਕੜੇ(part, post->ਬਫਰ,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        ਨਹੀਂਤਾਂ ਜੇ(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          ਜੇ(!clen)
            clen = -1;
          ਨਤੀਜਾ = curl_mime_ਅੰਕੜੇ_cb(part, clen,
                                     fread_func, ਨਿਰਾ, ਨਿਰਾ, post->userp);
        }
        ਨਹੀਂਤਾਂ {
          size_t uclen;
          ਜੇ(!clen)
            uclen = CURL_ZERO_TERMINATED;
          ਨਹੀਂਤਾਂ
            uclen = (size_t)clen;
          ਨਤੀਜਾ = curl_mime_ਅੰਕੜੇ(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      ਜੇ(!ਨਤੀਜਾ && post->showfilename &&
         (post->ਹੋਰ || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_ਬਫਰ |
                                        HTTPPOST_CALLBACK))))
        ਨਤੀਜਾ = curl_mime_filename(part, post->showfilename);
    }
  }

  ਜੇ(ਨਤੀਜਾ)
    Curl_mime_cleanpart(finalform);

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਨਹੀਂਤਾਂ /* if disabled */
CURLFORMcode curl_formadd(ਬਣਤਰ curl_httppost **httppost,
                          ਬਣਤਰ curl_httppost **ਆਖਰੀ_post, ...)
{
  (ਖਾਲੀ)httppost;
  (ਖਾਲੀ)ਆਖਰੀ_post;
  ਵਾਪਸ CURL_FORMADD_DISABLED;
}

ਪੂਰਨ curl_formget(ਬਣਤਰ curl_httppost *form, ਖਾਲੀ *arg,
                 curl_formget_callback append)
{
  (ਖਾਲੀ)form;
  (ਖਾਲੀ)arg;
  (ਖਾਲੀ)append;
  ਵਾਪਸ CURL_FORMADD_DISABLED;
}

ਖਾਲੀ curl_formfree(ਬਣਤਰ curl_httppost *form)
{
  (ਖਾਲੀ)form;
  /* Nothing to do. */
}

#ਜੇ_ਅੰਤ /* if disabled */
