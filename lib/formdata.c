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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

ರಚನೆ Curl_easy;

#ಅನ್ತರ್ಭಾವ "formdata.h"

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#ಅನ್ತರ್ಭಾವ "urldata.h" /* for struct Curl_easy */
#ಅನ್ತರ್ಭಾವ "mime.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "bufref.h"
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"


#ವ್ಯಾಖ್ಯಾನಿಸು HTTPPOST_PTRNAME     CURL_HTTPPOST_PTRNAME
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPPOST_FILENAME    CURL_HTTPPOST_FILENAME
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPPOST_PTRCONTENTS CURL_HTTPPOST_PTRCONTENTS
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPPOST_READFILE    CURL_HTTPPOST_READFILE
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPPOST_PTRBUFFER   CURL_HTTPPOST_PTRBUFFER
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPPOST_CALLBACK    CURL_HTTPPOST_CALLBACK
#ವ್ಯಾಖ್ಯಾನಿಸು HTTPPOST_ಬಫರ್      CURL_HTTPPOST_ಬಫರ್

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
ಸ್ಥಿರ ರಚನೆ curl_httppost *httppost_ಕೂಡಿಸು(ರಚನೆ FormInfo *src,
                                          ರಚನೆ curl_httppost *ಪೋಷಕ_post,
                                          ರಚನೆ curl_httppost **httppost,
                                          ರಚನೆ curl_httppost **ಕೊನೆಯ_post)
{
  ರಚನೆ curl_httppost *post;
  size_t namelength = src->namelength;
  ಆದರೆ(!namelength && Curl_bufref_ptr(&src->name))
    namelength = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(Curl_bufref_ptr(&src->name));
  ಆದರೆ((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  post = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ curl_httppost));
  ಆದರೆ(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (ದೀರ್ಘ)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->ಮೌಲ್ಯ));
    post->contentlen = src->contentslength;
    post->ಬಫರ್ = src->ಬಫರ್;
    post->bufferlength = (ದೀರ್ಘ)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ಆದರೆ(ಪೋಷಕ_post) {
    /* now, point our 'more' to the original 'more' */
    post->ಇನ್ನಷ್ಟು = ಪೋಷಕ_post->ಇನ್ನಷ್ಟು;

    /* then move the original 'more' to point to ourselves */
    ಪೋಷಕ_post->ಇನ್ನಷ್ಟು = post;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* make the previous point to this */
    ಆದರೆ(*ಕೊನೆಯ_post)
      (*ಕೊನೆಯ_post)->ಮುಂದಿನ = post;
    ಇಲ್ಲದಿದ್ದರೆ
      (*httppost) = post;

    (*ಕೊನೆಯ_post) = post;
  }
  ಹಿಂತಿರುಗಿ post;
}

/* Allocate and initialize a new FormInfo structure. */
ಸ್ಥಿರ ರಚನೆ FormInfo *forminfo_new(ಶೂನ್ಯ)
{
  ರಚನೆ FormInfo *form_info = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ FormInfo));

  ಆದರೆ(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->ಮೌಲ್ಯ);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  ಹಿಂತಿರುಗಿ form_info;
}

/* Replace the target field data by a dynamic copy of it. */
ಸ್ಥಿರ CURLcode forminfo_copyfield(ರಚನೆ bufref *ಕ್ಷೇತ್ರ, size_t len)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ = Curl_bufref_ptr(ಕ್ಷೇತ್ರ);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(ಮೌಲ್ಯ) {
    ಆದರೆ(!len)
      len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮೌಲ್ಯ);
    ಫಲಿತಾಂಶ = Curl_bufref_memdup0(ಕ್ಷೇತ್ರ, ಮೌಲ್ಯ, len);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
ಸ್ಥಿರ ಶೂನ್ಯ forminfo_ಕೂಡಿಸು(ರಚನೆ FormInfo *form_info, ರಚನೆ FormInfo *ಪೋಷಕ)
{
  form_info->flags |= HTTPPOST_FILENAME;

  ಆದರೆ(ಪೋಷಕ) {
    /* now, point our 'more' to the original 'more' */
    form_info->ಇನ್ನಷ್ಟು = ಪೋಷಕ->ಇನ್ನಷ್ಟು;

    /* then move the original 'more' to point to ourselves */
    ಪೋಷಕ->ಇನ್ನಷ್ಟು = form_info;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ free_formlist(ರಚನೆ FormInfo *ptr)
{
  ಗಾಗಿ(; ptr; ptr = ptr->ಇನ್ನಷ್ಟು) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->ಮೌಲ್ಯ);
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

ಸ್ಥಿರ CURLFORMcode formadd_check(ರಚನೆ FormInfo *ಮೊದಲ_form,
                                  ರಚನೆ curl_httppost **httppost,
                                  ರಚನೆ curl_httppost **ಕೊನೆಯ_post)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *prevtype = ಖಾಲಿ;
  ರಚನೆ FormInfo *form = ಖಾಲಿ;
  ರಚನೆ curl_httppost *post = ಖಾಲಿ;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  ಗಾಗಿ(form = ಮೊದಲ_form; form; form = form->ಇನ್ನಷ್ಟು) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name = Curl_bufref_ptr(&form->name);

    ಆದರೆ(((!name || !Curl_bufref_ptr(&form->ಮೌಲ್ಯ)) && !post) ||
       (form->contentslength &&
        (form->flags & HTTPPOST_FILENAME)) ||
       ((form->flags & HTTPPOST_FILENAME) &&
        (form->flags & HTTPPOST_PTRCONTENTS)) ||

       (!form->ಬಫರ್ &&
        (form->flags & HTTPPOST_ಬಫರ್) &&
        (form->flags & HTTPPOST_PTRBUFFER)) ||

       ((form->flags & HTTPPOST_READFILE) &&
        (form->flags & HTTPPOST_PTRCONTENTS))
      ) {
      ಹಿಂತಿರುಗಿ CURL_FORMADD_INCOMPLETE;
    }
    ಆದರೆ(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_ಬಫರ್)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f = Curl_bufref_ptr((form->flags & HTTPPOST_ಬಫರ್) ?
                                      &form->showfilename : &form->ಮೌಲ್ಯ);
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type = Curl_mime_contenttype(f);
      ಆದರೆ(!type)
        type = prevtype;
      ಆದರೆ(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      ಆದರೆ(Curl_bufref_memdup0(&form->contenttype, type, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(type)))
        ಹಿಂತಿರುಗಿ CURL_FORMADD_ಸ್ಮರಣೆ;
    }
    ಆದರೆ(name && form->namelength && memchr(name, 0, form->namelength))
      ಹಿಂತಿರುಗಿ CURL_FORMADD_NULL;
    ಆದರೆ(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      ಹಿಂತಿರುಗಿ CURL_FORMADD_ಸ್ಮರಣೆ;
    ಆದರೆ(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->ಮೌಲ್ಯ, (size_t)form->contentslength))
      ಹಿಂತಿರುಗಿ CURL_FORMADD_ಸ್ಮರಣೆ;
    post = httppost_ಕೂಡಿಸು(form, post, httppost, ಕೊನೆಯ_post);

    ಆದರೆ(!post)
      ಹಿಂತಿರುಗಿ CURL_FORMADD_ಸ್ಮರಣೆ;

    ಆದರೆ(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  ಹಿಂತಿರುಗಿ CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
ಸ್ಥಿರ ಶೂನ್ಯ free_chain(ರಚನೆ curl_httppost *c)
{
  ಆಗ(c) {
    ರಚನೆ curl_httppost *ಮುಂದಿನ = c->ಮುಂದಿನ;
    ಆದರೆ(c->ಇನ್ನಷ್ಟು)
      free_chain(c->ಇನ್ನಷ್ಟು);
    curlx_free(c);
    c = ಮುಂದಿನ;
  }
}

ಸ್ಥಿರ CURLFORMcode formadd(ರಚನೆ curl_httppost **httppost,
                            ರಚನೆ curl_httppost **ಕೊನೆಯ_post, va_list params)
{
  ರಚನೆ FormInfo *ಮೊದಲ_form, *curr, *form = ಖಾಲಿ;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  ಸ್ಥಿರಾಂಕ ರಚನೆ curl_forms *forms = ಖಾಲಿ;
  ಅಕ್ಷರ *avalue = ಖಾಲಿ;
  ರಚನೆ curl_httppost *newchain = ಖಾಲಿ;
  ರಚನೆ curl_httppost *lastnode = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿಸು form_ptr_arg(t) (forms ? (t)(ಶೂನ್ಯ *)avalue : va_arg(params, t))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_UINTPTR_T
#ವ್ಯಾಖ್ಯಾನಿಸು form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು form_int_arg(t) (forms ? (t)(ಶೂನ್ಯ *)avalue : va_arg(params, t))
#ಆದರೆ_ಅಂತ್ಯ

  /*
   * We need to allocate the first struct to fill in.
   */
  ಮೊದಲ_form = forminfo_new();
  ಆದರೆ(!ಮೊದಲ_form)
    ಹಿಂತಿರುಗಿ CURL_FORMADD_ಸ್ಮರಣೆ;

  curr = ಮೊದಲ_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  ಆಗ(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    ಆದರೆ(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (ಅಕ್ಷರ *)CURL_UNCONST(forms->ಮೌಲ್ಯ);

      forms++; /* advance this to next entry */
      ಆದರೆ(CURLFORM_ಕೊನೆ == option) {
        /* end of array state */
        forms = ಖಾಲಿ;
        ಮುಂದುವರಿ;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, ಪೂರ್ಣಾಂಕ);
      ಆದರೆ(CURLFORM_ಕೊನೆ == option)
        ನಿಲ್ಲಿಸು;
    }

    ಆಯ್ಕೆ(option) {
    ಸ್ಥಿತಿ CURLFORM_ARRAY:
      ಆದರೆ(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      ಇಲ್ಲದಿದ್ದರೆ {
        forms = va_arg(params, ರಚನೆ curl_forms *);
        ಆದರೆ(!forms)
          retval = CURL_FORMADD_NULL;
      }
      ನಿಲ್ಲಿಸು;

      /*
       * Set the Name property.
       */
    ಸ್ಥಿತಿ CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    ಸ್ಥಿತಿ CURLFORM_COPYNAME:
      ಆದರೆ(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      ಇಲ್ಲದಿದ್ದರೆ {
        avalue = form_ptr_arg(ಅಕ್ಷರ *);
        ಆದರೆ(avalue)
          Curl_bufref_ಗಣ(&curr->name, avalue, 0, ಖಾಲಿ); /* No copy yet. */
        ಇಲ್ಲದಿದ್ದರೆ
          retval = CURL_FORMADD_NULL;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLFORM_NAMELENGTH:
      ಆದರೆ(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      ಇಲ್ಲದಿದ್ದರೆ
        curr->namelength = (size_t)form_int_arg(ದೀರ್ಘ);
      ನಿಲ್ಲಿಸು;

      /*
       * Set the contents property.
       */
    ಸ್ಥಿತಿ CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    ಸ್ಥಿತಿ CURLFORM_COPYCONTENTS:
      ಆದರೆ(Curl_bufref_ptr(&curr->ಮೌಲ್ಯ))
        retval = CURL_FORMADD_OPTION_TWICE;
      ಇಲ್ಲದಿದ್ದರೆ {
        avalue = form_ptr_arg(ಅಕ್ಷರ *);
        ಆದರೆ(avalue)
          Curl_bufref_ಗಣ(&curr->ಮೌಲ್ಯ, avalue, 0, ಖಾಲಿ); /* No copy yet. */
        ಇಲ್ಲದಿದ್ದರೆ
          retval = CURL_FORMADD_NULL;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURLFORM_CONTENTSLENGTH: {
      ದೀರ್ಘ clen = form_int_arg(ದೀರ್ಘ);
      ಆದರೆ(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      ಇಲ್ಲದಿದ್ದರೆ
        curr->contentslength = (curl_off_t)clen;
      ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      ಆದರೆ(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      ಇಲ್ಲದಿದ್ದರೆ {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      ನಿಲ್ಲಿಸು;
    }

      /* Get contents from a given filename */
    ಸ್ಥಿತಿ CURLFORM_FILECONTENT:
      ಆದರೆ(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      ಇಲ್ಲದಿದ್ದರೆ {
        avalue = form_ptr_arg(ಅಕ್ಷರ *);
        ಆದರೆ(avalue) {
          ಆದರೆ(Curl_bufref_memdup0(&curr->ಮೌಲ್ಯ, avalue, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(avalue)))
            retval = CURL_FORMADD_ಸ್ಮರಣೆ;
          ಇಲ್ಲದಿದ್ದರೆ
            curr->flags |= HTTPPOST_READFILE;
        }
        ಇಲ್ಲದಿದ್ದರೆ
          retval = CURL_FORMADD_NULL;
      }
      ನಿಲ್ಲಿಸು;

      /* We upload a file */
    ಸ್ಥಿತಿ CURLFORM_FILE:
      avalue = form_ptr_arg(ಅಕ್ಷರ *);
      ಆದರೆ(Curl_bufref_ptr(&curr->ಮೌಲ್ಯ)) {
        ಆದರೆ(curr->flags & HTTPPOST_FILENAME) {
          ಆದರೆ(avalue) {
            form = forminfo_new();
            ಆದರೆ(!form ||
               Curl_bufref_memdup0(&form->ಮೌಲ್ಯ, avalue, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_ಸ್ಮರಣೆ;
            }
            ಇಲ್ಲದಿದ್ದರೆ {
              forminfo_ಕೂಡಿಸು(form, curr);
              curr = form;
              form = ಖಾಲಿ;
            }
          }
          ಇಲ್ಲದಿದ್ದರೆ
            retval = CURL_FORMADD_NULL;
        }
        ಇಲ್ಲದಿದ್ದರೆ
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(avalue) {
          ಆದರೆ(Curl_bufref_memdup0(&curr->ಮೌಲ್ಯ, avalue, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(avalue)))
            retval = CURL_FORMADD_ಸ್ಮರಣೆ;
          ಇಲ್ಲದಿದ್ದರೆ
            curr->flags |= HTTPPOST_FILENAME;
        }
        ಇಲ್ಲದಿದ್ದರೆ
          retval = CURL_FORMADD_NULL;
      }
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_ಬಫರ್;
      ಆದರೆ(curr->ಬಫರ್)
        retval = CURL_FORMADD_OPTION_TWICE;
      ಇಲ್ಲದಿದ್ದರೆ {
        avalue = form_ptr_arg(ಅಕ್ಷರ *);
        ಆದರೆ(avalue) {
          curr->ಬಫರ್ = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_ಗಣ(&curr->ಮೌಲ್ಯ, avalue, 0, ಖಾಲಿ);
        }
        ಇಲ್ಲದಿದ್ದರೆ
          retval = CURL_FORMADD_NULL;
      }
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURLFORM_BUFFERLENGTH:
      ಆದರೆ(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      ಇಲ್ಲದಿದ್ದರೆ
        curr->bufferlength = (size_t)form_int_arg(ದೀರ್ಘ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURLFORM_ಹರಿವು:
      curr->flags |= HTTPPOST_CALLBACK;
      ಆದರೆ(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      ಇಲ್ಲದಿದ್ದರೆ {
        avalue = form_ptr_arg(ಅಕ್ಷರ *);
        ಆದರೆ(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_ಗಣ(&curr->ಮೌಲ್ಯ, avalue, 0, ಖಾಲಿ);
        }
        ಇಲ್ಲದಿದ್ದರೆ
          retval = CURL_FORMADD_NULL;
      }
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(ಅಕ್ಷರ *);
      ಆದರೆ(Curl_bufref_ptr(&curr->contenttype)) {
        ಆದರೆ(curr->flags & HTTPPOST_FILENAME) {
          ಆದರೆ(avalue) {
            form = forminfo_new();
            ಆದರೆ(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_ಸ್ಮರಣೆ;
            }
            ಇಲ್ಲದಿದ್ದರೆ {
              forminfo_ಕೂಡಿಸು(form, curr);
              curr = form;
              form = ಖಾಲಿ;
            }
          }
          ಇಲ್ಲದಿದ್ದರೆ
            retval = CURL_FORMADD_NULL;
        }
        ಇಲ್ಲದಿದ್ದರೆ
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(avalue) {
        ಆದರೆ(Curl_bufref_memdup0(&curr->contenttype, avalue, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(avalue)))
          retval = CURL_FORMADD_ಸ್ಮರಣೆ;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        retval = CURL_FORMADD_NULL;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      ರಚನೆ curl_slist *list = form_ptr_arg(ರಚನೆ curl_slist *);

      ಆದರೆ(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      ಇಲ್ಲದಿದ್ದರೆ
        curr->contentheader = list;

      ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ CURLFORM_FILENAME:
    ಸ್ಥಿತಿ CURLFORM_ಬಫರ್:
      avalue = form_ptr_arg(ಅಕ್ಷರ *);
      ಆದರೆ(!avalue)
        retval = CURL_FORMADD_NULL;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(Curl_bufref_memdup0(&curr->showfilename, avalue, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(avalue)))
        retval = CURL_FORMADD_ಸ್ಮರಣೆ;
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      ನಿಲ್ಲಿಸು;
    }
  }

  ಆದರೆ(!retval)
    retval = formadd_check(ಮೊದಲ_form, &newchain, &lastnode);

  ಆದರೆ(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(ಮೊದಲ_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  ಆಗ(ಮೊದಲ_form) {
    ರಚನೆ FormInfo *ptr = ಮೊದಲ_form->ಇನ್ನಷ್ಟು;
    curlx_free(ಮೊದಲ_form);
    ಮೊದಲ_form = ptr;
  }

  ಆದರೆ(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    ಆದರೆ(*ಕೊನೆಯ_post)
      (*ಕೊನೆಯ_post)->ಮುಂದಿನ = newchain;
    ಇಲ್ಲದಿದ್ದರೆ
      (*httppost) = newchain;

    (*ಕೊನೆಯ_post) = lastnode;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    free_chain(newchain);

  ಹಿಂತಿರುಗಿ retval;
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು form_ptr_arg
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(ರಚನೆ curl_httppost **httppost,
                          ರಚನೆ curl_httppost **ಕೊನೆಯ_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, ಕೊನೆಯ_post);
  form = formadd(httppost, ಕೊನೆಯ_post, arg);
  va_end(arg);
  ಹಿಂತಿರುಗಿ form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
ಪೂರ್ಣಾಂಕ curl_formget(ರಚನೆ curl_httppost *form, ಶೂನ್ಯ *arg,
                 curl_formget_callback append)
{
  CURLcode ಫಲಿತಾಂಶ;
  curl_mimepart toppart;

  /* Validate callback is provided */
  ಆದರೆ(!append)
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)CURLE_BAD_FUNCTION_ವಾದ;

  Curl_mime_initpart(&toppart); /* default form is empty */
  ಫಲಿತಾಂಶ = Curl_getformdata(ಖಾಲಿ, &toppart, form, ಖಾಲಿ);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = Curl_mime_prepare_ಶೀರ್ಷಿಕೆಗಳು(ಖಾಲಿ, &toppart, "multipart/form-data",
                                       ಖಾಲಿ, MIMESTRATEGY_FORM);

  ಆಗ(!ಫಲಿತಾಂಶ) {
    ಅಕ್ಷರ ಬಫರ್[8192];
    size_t nread = Curl_mime_read(ಬಫರ್, 1, ಗಾತ್ರ(ಬಫರ್), &toppart);

    ಆದರೆ(!nread)
      ನಿಲ್ಲಿಸು;

    ಆದರೆ(nread > ಗಾತ್ರ(ಬಫರ್) || append(arg, ಬಫರ್, nread) != nread) {
      ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
      ಆದರೆ(nread == CURL_READFUNC_ABORT)
        ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
ಶೂನ್ಯ curl_formfree(ರಚನೆ curl_httppost *form)
{
  ರಚನೆ curl_httppost *ಮುಂದಿನ;

  ಆದರೆ(!form)
    /* no form to free, get out of this */
    ಹಿಂತಿರುಗಿ;

  ಮಾಡು {
    ಮುಂದಿನ = form->ಮುಂದಿನ;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->ಇನ್ನಷ್ಟು);

    ಆದರೆ(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    ಆದರೆ(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_ಬಫರ್ | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = ಮುಂದಿನ;
  } ಆಗ(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
ಸ್ಥಿರ CURLcode setname(curl_mimepart *part, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, size_t len)
{
  ಅಕ್ಷರ *zname;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!name || !len)
    ಹಿಂತಿರುಗಿ curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  ಆದರೆ(!zname)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಫಲಿತಾಂಶ = curl_mime_name(part, zname);
  curlx_free(zname);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
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

CURLcode Curl_getformdata(CURL *ದತ್ತಾಂಶ,
                          curl_mimepart *finalform,
                          ರಚನೆ curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  curl_mime *form = ಖಾಲಿ;
  curl_mimepart *part;
  ರಚನೆ curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  ಆದರೆ(!post)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ; /* no input => no output! */

  form = curl_mime_init(ದತ್ತಾಂಶ);
  ಆದರೆ(!form)
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  ಗಾಗಿ(; !ಫಲಿತಾಂಶ && post; post = post->ಮುಂದಿನ) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    ಆದರೆ(post->ಇನ್ನಷ್ಟು) {
      part = curl_mime_addpart(form);
      ಆದರೆ(!part)
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = setname(part, post->name, post->namelength);
      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        multipart = curl_mime_init(ದತ್ತಾಂಶ);
        ಆದರೆ(!multipart)
          ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    ಗಾಗಿ(file = post; !ಫಲಿತಾಂಶ && file; file = file->ಇನ್ನಷ್ಟು) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      ಆದರೆ(!part)
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

      /* Set the headers. */
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = curl_mime_ಶೀರ್ಷಿಕೆಗಳು(part, file->contentheader, 0);

      /* Set the content type. */
      ಆದರೆ(!ಫಲಿತಾಂಶ && file->contenttype)
        ಫಲಿತಾಂಶ = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      ಆದರೆ(!ಫಲಿತಾಂಶ && !post->ಇನ್ನಷ್ಟು)
        ಫಲಿತಾಂಶ = setname(part, post->name, post->namelength);

      /* Process contents. */
      ಆದರೆ(!ಫಲಿತಾಂಶ) {
        curl_off_t clen = post->contentslength;

        ಆದರೆ(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        ಆದರೆ(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          ಆದರೆ(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತಳ್ಳು
#ಸೂಚನೆ clang diagnostic ignored "-Wcast-function-type-strict"
#ಆದರೆ_ಅಂತ್ಯ
            ಫಲಿತಾಂಶ = curl_mime_ದತ್ತಾಂಶ_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       ಖಾಲಿ, (ಶೂನ್ಯ *)stdin);
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
          }
          ಇಲ್ಲದಿದ್ದರೆ
            ಫಲಿತಾಂಶ = curl_mime_filedata(part, file->contents);
          ಆದರೆ(!ಫಲಿತಾಂಶ && (post->flags & HTTPPOST_READFILE))
            ಫಲಿತಾಂಶ = curl_mime_filename(part, ಖಾಲಿ);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(post->flags & HTTPPOST_ಬಫರ್)
          ಫಲಿತಾಂಶ = curl_mime_ದತ್ತಾಂಶ(part, post->ಬಫರ್,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          ಆದರೆ(!clen)
            clen = -1;
          ಫಲಿತಾಂಶ = curl_mime_ದತ್ತಾಂಶ_cb(part, clen,
                                     fread_func, ಖಾಲಿ, ಖಾಲಿ, post->userp);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          size_t uclen;
          ಆದರೆ(!clen)
            uclen = CURL_ZERO_TERMINATED;
          ಇಲ್ಲದಿದ್ದರೆ
            uclen = (size_t)clen;
          ಫಲಿತಾಂಶ = curl_mime_ದತ್ತಾಂಶ(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      ಆದರೆ(!ಫಲಿತಾಂಶ && post->showfilename &&
         (post->ಇನ್ನಷ್ಟು || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_ಬಫರ್ |
                                        HTTPPOST_CALLBACK))))
        ಫಲಿತಾಂಶ = curl_mime_filename(part, post->showfilename);
    }
  }

  ಆದರೆ(ಫಲಿತಾಂಶ)
    Curl_mime_cleanpart(finalform);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಇಲ್ಲದಿದ್ದರೆ /* if disabled */
CURLFORMcode curl_formadd(ರಚನೆ curl_httppost **httppost,
                          ರಚನೆ curl_httppost **ಕೊನೆಯ_post, ...)
{
  (ಶೂನ್ಯ)httppost;
  (ಶೂನ್ಯ)ಕೊನೆಯ_post;
  ಹಿಂತಿರುಗಿ CURL_FORMADD_DISABLED;
}

ಪೂರ್ಣಾಂಕ curl_formget(ರಚನೆ curl_httppost *form, ಶೂನ್ಯ *arg,
                 curl_formget_callback append)
{
  (ಶೂನ್ಯ)form;
  (ಶೂನ್ಯ)arg;
  (ಶೂನ್ಯ)append;
  ಹಿಂತಿರುಗಿ CURL_FORMADD_DISABLED;
}

ಶೂನ್ಯ curl_formfree(ರಚನೆ curl_httppost *form)
{
  (ಶೂನ್ಯ)form;
  /* Nothing to do. */
}

#ಆದರೆ_ಅಂತ್ಯ /* if disabled */
