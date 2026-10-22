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

ساخت Curl_easy;

#شامل "formdata.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#شامل "urldata.h" /* for struct Curl_easy */
#شامل "mime.h"
#شامل "curlx/strdup.h"
#شامل "bufref.h"
#شامل "curlx/win32-fopen.h"


#تعریف_کرو HTTPPOST_PTRNAME     CURL_HTTPPOST_PTRNAME
#تعریف_کرو HTTPPOST_FILENAME    CURL_HTTPPOST_FILENAME
#تعریف_کرو HTTPPOST_PTRCONTENTS CURL_HTTPPOST_PTRCONTENTS
#تعریف_کرو HTTPPOST_READFILE    CURL_HTTPPOST_READFILE
#تعریف_کرو HTTPPOST_PTRBUFFER   CURL_HTTPPOST_PTRBUFFER
#تعریف_کرو HTTPPOST_CALLBACK    CURL_HTTPPOST_CALLBACK
#تعریف_کرو HTTPPOST_بفر      CURL_HTTPPOST_بفر

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
ساکن ساخت curl_httppost *httppost_جوڑو(ساخت FormInfo *src,
                                          ساخت curl_httppost *والد_post,
                                          ساخت curl_httppost **httppost,
                                          ساخت curl_httppost **آخری_post)
{
  ساخت curl_httppost *post;
  size_t namelength = src->namelength;
  اگر(!namelength && Curl_bufref_ptr(&src->name))
    namelength = لڑی_لمبائی(Curl_bufref_ptr(&src->name));
  اگر((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    واپس تہی;
  post = curlx_calloc(1, حجم(ساخت curl_httppost));
  اگر(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (طویل)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->قدر));
    post->contentlen = src->contentslength;
    post->بفر = src->بفر;
    post->bufferlength = (طویل)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  ورنہ
    واپس تہی;

  اگر(والد_post) {
    /* now, point our 'more' to the original 'more' */
    post->مزید = والد_post->مزید;

    /* then move the original 'more' to point to ourselves */
    والد_post->مزید = post;
  }
  ورنہ {
    /* make the previous point to this */
    اگر(*آخری_post)
      (*آخری_post)->اگلا = post;
    ورنہ
      (*httppost) = post;

    (*آخری_post) = post;
  }
  واپس post;
}

/* Allocate and initialize a new FormInfo structure. */
ساکن ساخت FormInfo *forminfo_new(خالی)
{
  ساخت FormInfo *form_info = curlx_calloc(1, حجم(ساخت FormInfo));

  اگر(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->قدر);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  واپس form_info;
}

/* Replace the target field data by a dynamic copy of it. */
ساکن CURLcode forminfo_copyfield(ساخت bufref *میدان, size_t len)
{
  ثابت حرف *قدر = Curl_bufref_ptr(میدان);
  CURLcode نتیجہ = CURLE_OK;

  اگر(قدر) {
    اگر(!len)
      len = لڑی_لمبائی(قدر);
    نتیجہ = Curl_bufref_memdup0(میدان, قدر, len);
  }

  واپس نتیجہ;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
ساکن خالی forminfo_جوڑو(ساخت FormInfo *form_info, ساخت FormInfo *والد)
{
  form_info->flags |= HTTPPOST_FILENAME;

  اگر(والد) {
    /* now, point our 'more' to the original 'more' */
    form_info->مزید = والد->مزید;

    /* then move the original 'more' to point to ourselves */
    والد->مزید = form_info;
  }
}

ساکن خالی free_formlist(ساخت FormInfo *ptr)
{
  برائے(; ptr; ptr = ptr->مزید) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->قدر);
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

ساکن CURLFORMcode formadd_check(ساخت FormInfo *پہلا_form,
                                  ساخت curl_httppost **httppost,
                                  ساخت curl_httppost **آخری_post)
{
  ثابت حرف *prevtype = تہی;
  ساخت FormInfo *form = تہی;
  ساخت curl_httppost *post = تہی;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  برائے(form = پہلا_form; form; form = form->مزید) {
    ثابت حرف *name = Curl_bufref_ptr(&form->name);

    اگر(((!name || !Curl_bufref_ptr(&form->قدر)) && !post) ||
       (form->contentslength &&
        (form->flags & HTTPPOST_FILENAME)) ||
       ((form->flags & HTTPPOST_FILENAME) &&
        (form->flags & HTTPPOST_PTRCONTENTS)) ||

       (!form->بفر &&
        (form->flags & HTTPPOST_بفر) &&
        (form->flags & HTTPPOST_PTRBUFFER)) ||

       ((form->flags & HTTPPOST_READFILE) &&
        (form->flags & HTTPPOST_PTRCONTENTS))
      ) {
      واپس CURL_FORMADD_INCOMPLETE;
    }
    اگر(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_بفر)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      ثابت حرف *f = Curl_bufref_ptr((form->flags & HTTPPOST_بفر) ?
                                      &form->showfilename : &form->قدر);
      ثابت حرف *type = Curl_mime_contenttype(f);
      اگر(!type)
        type = prevtype;
      اگر(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      اگر(Curl_bufref_memdup0(&form->contenttype, type, لڑی_لمبائی(type)))
        واپس CURL_FORMADD_حافظہ;
    }
    اگر(name && form->namelength && memchr(name, 0, form->namelength))
      واپس CURL_FORMADD_NULL;
    اگر(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      واپس CURL_FORMADD_حافظہ;
    اگر(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->قدر, (size_t)form->contentslength))
      واپس CURL_FORMADD_حافظہ;
    post = httppost_جوڑو(form, post, httppost, آخری_post);

    اگر(!post)
      واپس CURL_FORMADD_حافظہ;

    اگر(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  واپس CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
ساکن خالی free_chain(ساخت curl_httppost *c)
{
  جب(c) {
    ساخت curl_httppost *اگلا = c->اگلا;
    اگر(c->مزید)
      free_chain(c->مزید);
    curlx_free(c);
    c = اگلا;
  }
}

ساکن CURLFORMcode formadd(ساخت curl_httppost **httppost,
                            ساخت curl_httppost **آخری_post, va_list params)
{
  ساخت FormInfo *پہلا_form, *curr, *form = تہی;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  ثابت ساخت curl_forms *forms = تہی;
  حرف *avalue = تہی;
  ساخت curl_httppost *newchain = تہی;
  ساخت curl_httppost *lastnode = تہی;

#تعریف_کرو form_ptr_arg(t) (forms ? (t)(خالی *)avalue : va_arg(params, t))
#اگر_تعریف_شدہ HAVE_UINTPTR_T
#تعریف_کرو form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#ورنہ
#تعریف_کرو form_int_arg(t) (forms ? (t)(خالی *)avalue : va_arg(params, t))
#اگر_ختم

  /*
   * We need to allocate the first struct to fill in.
   */
  پہلا_form = forminfo_new();
  اگر(!پہلا_form)
    واپس CURL_FORMADD_حافظہ;

  curr = پہلا_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  جب(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    اگر(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (حرف *)CURL_UNCONST(forms->قدر);

      forms++; /* advance this to next entry */
      اگر(CURLFORM_انتہا == option) {
        /* end of array state */
        forms = تہی;
        جاری;
      }
    }
    ورنہ {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, صحیح);
      اگر(CURLFORM_انتہا == option)
        توڑو;
    }

    انتخاب(option) {
    حالت CURLFORM_ARRAY:
      اگر(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      ورنہ {
        forms = va_arg(params, ساخت curl_forms *);
        اگر(!forms)
          retval = CURL_FORMADD_NULL;
      }
      توڑو;

      /*
       * Set the Name property.
       */
    حالت CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    حالت CURLFORM_COPYNAME:
      اگر(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      ورنہ {
        avalue = form_ptr_arg(حرف *);
        اگر(avalue)
          Curl_bufref_سیٹ(&curr->name, avalue, 0, تہی); /* No copy yet. */
        ورنہ
          retval = CURL_FORMADD_NULL;
      }
      توڑو;
    حالت CURLFORM_NAMELENGTH:
      اگر(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      ورنہ
        curr->namelength = (size_t)form_int_arg(طویل);
      توڑو;

      /*
       * Set the contents property.
       */
    حالت CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    حالت CURLFORM_COPYCONTENTS:
      اگر(Curl_bufref_ptr(&curr->قدر))
        retval = CURL_FORMADD_OPTION_TWICE;
      ورنہ {
        avalue = form_ptr_arg(حرف *);
        اگر(avalue)
          Curl_bufref_سیٹ(&curr->قدر, avalue, 0, تہی); /* No copy yet. */
        ورنہ
          retval = CURL_FORMADD_NULL;
      }
      توڑو;
    حالت CURLFORM_CONTENTSLENGTH: {
      طویل clen = form_int_arg(طویل);
      اگر(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      ورنہ
        curr->contentslength = (curl_off_t)clen;
      توڑو;
    }

    حالت CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      اگر(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      ورنہ {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      توڑو;
    }

      /* Get contents from a given filename */
    حالت CURLFORM_FILECONTENT:
      اگر(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      ورنہ {
        avalue = form_ptr_arg(حرف *);
        اگر(avalue) {
          اگر(Curl_bufref_memdup0(&curr->قدر, avalue, لڑی_لمبائی(avalue)))
            retval = CURL_FORMADD_حافظہ;
          ورنہ
            curr->flags |= HTTPPOST_READFILE;
        }
        ورنہ
          retval = CURL_FORMADD_NULL;
      }
      توڑو;

      /* We upload a file */
    حالت CURLFORM_FILE:
      avalue = form_ptr_arg(حرف *);
      اگر(Curl_bufref_ptr(&curr->قدر)) {
        اگر(curr->flags & HTTPPOST_FILENAME) {
          اگر(avalue) {
            form = forminfo_new();
            اگر(!form ||
               Curl_bufref_memdup0(&form->قدر, avalue, لڑی_لمبائی(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_حافظہ;
            }
            ورنہ {
              forminfo_جوڑو(form, curr);
              curr = form;
              form = تہی;
            }
          }
          ورنہ
            retval = CURL_FORMADD_NULL;
        }
        ورنہ
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      ورنہ {
        اگر(avalue) {
          اگر(Curl_bufref_memdup0(&curr->قدر, avalue, لڑی_لمبائی(avalue)))
            retval = CURL_FORMADD_حافظہ;
          ورنہ
            curr->flags |= HTTPPOST_FILENAME;
        }
        ورنہ
          retval = CURL_FORMADD_NULL;
      }
      توڑو;

    حالت CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_بفر;
      اگر(curr->بفر)
        retval = CURL_FORMADD_OPTION_TWICE;
      ورنہ {
        avalue = form_ptr_arg(حرف *);
        اگر(avalue) {
          curr->بفر = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_سیٹ(&curr->قدر, avalue, 0, تہی);
        }
        ورنہ
          retval = CURL_FORMADD_NULL;
      }
      توڑو;

    حالت CURLFORM_BUFFERLENGTH:
      اگر(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      ورنہ
        curr->bufferlength = (size_t)form_int_arg(طویل);
      توڑو;

    حالت CURLFORM_دھارا:
      curr->flags |= HTTPPOST_CALLBACK;
      اگر(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      ورنہ {
        avalue = form_ptr_arg(حرف *);
        اگر(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_سیٹ(&curr->قدر, avalue, 0, تہی);
        }
        ورنہ
          retval = CURL_FORMADD_NULL;
      }
      توڑو;

    حالت CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(حرف *);
      اگر(Curl_bufref_ptr(&curr->contenttype)) {
        اگر(curr->flags & HTTPPOST_FILENAME) {
          اگر(avalue) {
            form = forminfo_new();
            اگر(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            لڑی_لمبائی(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_حافظہ;
            }
            ورنہ {
              forminfo_جوڑو(form, curr);
              curr = form;
              form = تہی;
            }
          }
          ورنہ
            retval = CURL_FORMADD_NULL;
        }
        ورنہ
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      ورنہ اگر(avalue) {
        اگر(Curl_bufref_memdup0(&curr->contenttype, avalue, لڑی_لمبائی(avalue)))
          retval = CURL_FORMADD_حافظہ;
      }
      ورنہ
        retval = CURL_FORMADD_NULL;
      توڑو;

    حالت CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      ساخت curl_slist *list = form_ptr_arg(ساخت curl_slist *);

      اگر(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      ورنہ
        curr->contentheader = list;

      توڑو;
    }
    حالت CURLFORM_FILENAME:
    حالت CURLFORM_بفر:
      avalue = form_ptr_arg(حرف *);
      اگر(!avalue)
        retval = CURL_FORMADD_NULL;
      ورنہ اگر(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      ورنہ اگر(Curl_bufref_memdup0(&curr->showfilename, avalue, لڑی_لمبائی(avalue)))
        retval = CURL_FORMADD_حافظہ;
      توڑو;

    طےشدہ:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      توڑو;
    }
  }

  اگر(!retval)
    retval = formadd_check(پہلا_form, &newchain, &lastnode);

  اگر(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(پہلا_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  جب(پہلا_form) {
    ساخت FormInfo *ptr = پہلا_form->مزید;
    curlx_free(پہلا_form);
    پہلا_form = ptr;
  }

  اگر(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    اگر(*آخری_post)
      (*آخری_post)->اگلا = newchain;
    ورنہ
      (*httppost) = newchain;

    (*آخری_post) = lastnode;
  }
  ورنہ
    free_chain(newchain);

  واپس retval;
#تعریف_منسوخ form_ptr_arg
#تعریف_منسوخ form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(ساخت curl_httppost **httppost,
                          ساخت curl_httppost **آخری_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, آخری_post);
  form = formadd(httppost, آخری_post, arg);
  va_end(arg);
  واپس form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
صحیح curl_formget(ساخت curl_httppost *form, خالی *arg,
                 curl_formget_callback append)
{
  CURLcode نتیجہ;
  curl_mimepart toppart;

  /* Validate callback is provided */
  اگر(!append)
    واپس (صحیح)CURLE_BAD_FUNCTION_دلیل;

  Curl_mime_initpart(&toppart); /* default form is empty */
  نتیجہ = Curl_getformdata(تہی, &toppart, form, تہی);
  اگر(!نتیجہ)
    نتیجہ = Curl_mime_prepare_سرخیاں(تہی, &toppart, "multipart/form-data",
                                       تہی, MIMESTRATEGY_FORM);

  جب(!نتیجہ) {
    حرف بفر[8192];
    size_t nread = Curl_mime_read(بفر, 1, حجم(بفر), &toppart);

    اگر(!nread)
      توڑو;

    اگر(nread > حجم(بفر) || append(arg, بفر, nread) != nread) {
      نتیجہ = CURLE_READ_ERROR;
      اگر(nread == CURL_READFUNC_ABORT)
        نتیجہ = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  واپس (صحیح)نتیجہ;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
خالی curl_formfree(ساخت curl_httppost *form)
{
  ساخت curl_httppost *اگلا;

  اگر(!form)
    /* no form to free, get out of this */
    واپس;

  کرو {
    اگلا = form->اگلا;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->مزید);

    اگر(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    اگر(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_بفر | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = اگلا;
  } جب(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
ساکن CURLcode setname(curl_mimepart *part, ثابت حرف *name, size_t len)
{
  حرف *zname;
  CURLcode نتیجہ;

  اگر(!name || !len)
    واپس curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  اگر(!zname)
    واپس CURLE_برآمد_کا_حافظہ;
  نتیجہ = curl_mime_name(part, zname);
  curlx_free(zname);
  واپس نتیجہ;
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

CURLcode Curl_getformdata(CURL *ڈیٹا,
                          curl_mimepart *finalform,
                          ساخت curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode نتیجہ = CURLE_OK;
  curl_mime *form = تہی;
  curl_mimepart *part;
  ساخت curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  اگر(!post)
    واپس نتیجہ; /* no input => no output! */

  form = curl_mime_init(ڈیٹا);
  اگر(!form)
    نتیجہ = CURLE_برآمد_کا_حافظہ;

  اگر(!نتیجہ)
    نتیجہ = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  برائے(; !نتیجہ && post; post = post->اگلا) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    اگر(post->مزید) {
      part = curl_mime_addpart(form);
      اگر(!part)
        نتیجہ = CURLE_برآمد_کا_حافظہ;
      اگر(!نتیجہ)
        نتیجہ = setname(part, post->name, post->namelength);
      اگر(!نتیجہ) {
        multipart = curl_mime_init(ڈیٹا);
        اگر(!multipart)
          نتیجہ = CURLE_برآمد_کا_حافظہ;
      }
      اگر(!نتیجہ)
        نتیجہ = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    برائے(file = post; !نتیجہ && file; file = file->مزید) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      اگر(!part)
        نتیجہ = CURLE_برآمد_کا_حافظہ;

      /* Set the headers. */
      اگر(!نتیجہ)
        نتیجہ = curl_mime_سرخیاں(part, file->contentheader, 0);

      /* Set the content type. */
      اگر(!نتیجہ && file->contenttype)
        نتیجہ = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      اگر(!نتیجہ && !post->مزید)
        نتیجہ = setname(part, post->name, post->namelength);

      /* Process contents. */
      اگر(!نتیجہ) {
        curl_off_t clen = post->contentslength;

        اگر(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        اگر(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          اگر(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wcast-function-type-strict"
#اگر_ختم
            نتیجہ = curl_mime_ڈیٹا_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       تہی, (خالی *)stdin);
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic نکالو
#اگر_ختم
          }
          ورنہ
            نتیجہ = curl_mime_filedata(part, file->contents);
          اگر(!نتیجہ && (post->flags & HTTPPOST_READFILE))
            نتیجہ = curl_mime_filename(part, تہی);
        }
        ورنہ اگر(post->flags & HTTPPOST_بفر)
          نتیجہ = curl_mime_ڈیٹا(part, post->بفر,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        ورنہ اگر(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          اگر(!clen)
            clen = -1;
          نتیجہ = curl_mime_ڈیٹا_cb(part, clen,
                                     fread_func, تہی, تہی, post->userp);
        }
        ورنہ {
          size_t uclen;
          اگر(!clen)
            uclen = CURL_ZERO_TERMINATED;
          ورنہ
            uclen = (size_t)clen;
          نتیجہ = curl_mime_ڈیٹا(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      اگر(!نتیجہ && post->showfilename &&
         (post->مزید || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_بفر |
                                        HTTPPOST_CALLBACK))))
        نتیجہ = curl_mime_filename(part, post->showfilename);
    }
  }

  اگر(نتیجہ)
    Curl_mime_cleanpart(finalform);

  واپس نتیجہ;
}

#ورنہ /* if disabled */
CURLFORMcode curl_formadd(ساخت curl_httppost **httppost,
                          ساخت curl_httppost **آخری_post, ...)
{
  (خالی)httppost;
  (خالی)آخری_post;
  واپس CURL_FORMADD_DISABLED;
}

صحیح curl_formget(ساخت curl_httppost *form, خالی *arg,
                 curl_formget_callback append)
{
  (خالی)form;
  (خالی)arg;
  (خالی)append;
  واپس CURL_FORMADD_DISABLED;
}

خالی curl_formfree(ساخت curl_httppost *form)
{
  (خالی)form;
  /* Nothing to do. */
}

#اگر_ختم /* if disabled */
