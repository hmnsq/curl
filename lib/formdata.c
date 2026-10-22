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
#అన్తర్భావ "curl_setup.h"

నిర్మాణం Curl_easy;

#అన్తర్భావ "formdata.h"

#ఒకవేళ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#అన్తర్భావ "urldata.h" /* for struct Curl_easy */
#అన్తర్భావ "mime.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "bufref.h"
#అన్తర్భావ "curlx/win32-fopen.h"


#నిర్వచించు HTTPPOST_PTRNAME     CURL_HTTPPOST_PTRNAME
#నిర్వచించు HTTPPOST_FILENAME    CURL_HTTPPOST_FILENAME
#నిర్వచించు HTTPPOST_PTRCONTENTS CURL_HTTPPOST_PTRCONTENTS
#నిర్వచించు HTTPPOST_READFILE    CURL_HTTPPOST_READFILE
#నిర్వచించు HTTPPOST_PTRBUFFER   CURL_HTTPPOST_PTRBUFFER
#నిర్వచించు HTTPPOST_CALLBACK    CURL_HTTPPOST_CALLBACK
#నిర్వచించు HTTPPOST_బఫరు      CURL_HTTPPOST_బఫరు

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
స్థిర నిర్మాణం curl_httppost *httppost_కూడు(నిర్మాణం FormInfo *src,
                                          నిర్మాణం curl_httppost *మాతృ_post,
                                          నిర్మాణం curl_httppost **httppost,
                                          నిర్మాణం curl_httppost **చివరి_post)
{
  నిర్మాణం curl_httppost *post;
  size_t namelength = src->namelength;
  ఒకవేళ(!namelength && Curl_bufref_ptr(&src->name))
    namelength = అక్షరశ్రేణి_పొడవు(Curl_bufref_ptr(&src->name));
  ఒకవేళ((src->bufferlength > LONG_MAX) || (namelength > LONG_MAX))
    /* avoid overflow in typecasts below */
    తిరిగి ఖాళీ;
  post = curlx_calloc(1, పరిమాణం(నిర్మాణం curl_httppost));
  ఒకవేళ(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (దీర్ఘ)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->విలువ));
    post->contentlen = src->contentslength;
    post->బఫరు = src->బఫరు;
    post->bufferlength = (దీర్ఘ)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  లేకపోతే
    తిరిగి ఖాళీ;

  ఒకవేళ(మాతృ_post) {
    /* now, point our 'more' to the original 'more' */
    post->మరిన్ని = మాతృ_post->మరిన్ని;

    /* then move the original 'more' to point to ourselves */
    మాతృ_post->మరిన్ని = post;
  }
  లేకపోతే {
    /* make the previous point to this */
    ఒకవేళ(*చివరి_post)
      (*చివరి_post)->తదుపరి = post;
    లేకపోతే
      (*httppost) = post;

    (*చివరి_post) = post;
  }
  తిరిగి post;
}

/* Allocate and initialize a new FormInfo structure. */
స్థిర నిర్మాణం FormInfo *forminfo_new(శూన్య)
{
  నిర్మాణం FormInfo *form_info = curlx_calloc(1, పరిమాణం(నిర్మాణం FormInfo));

  ఒకవేళ(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->విలువ);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  తిరిగి form_info;
}

/* Replace the target field data by a dynamic copy of it. */
స్థిర CURLcode forminfo_copyfield(నిర్మాణం bufref *క్షేత్రం, size_t len)
{
  స్థిరరాశి అక్షరం *విలువ = Curl_bufref_ptr(క్షేత్రం);
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(విలువ) {
    ఒకవేళ(!len)
      len = అక్షరశ్రేణి_పొడవు(విలువ);
    ఫలితం = Curl_bufref_memdup0(క్షేత్రం, విలువ, len);
  }

  తిరిగి ఫలితం;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
స్థిర శూన్య forminfo_కూడు(నిర్మాణం FormInfo *form_info, నిర్మాణం FormInfo *మాతృ)
{
  form_info->flags |= HTTPPOST_FILENAME;

  ఒకవేళ(మాతృ) {
    /* now, point our 'more' to the original 'more' */
    form_info->మరిన్ని = మాతృ->మరిన్ని;

    /* then move the original 'more' to point to ourselves */
    మాతృ->మరిన్ని = form_info;
  }
}

స్థిర శూన్య free_formlist(నిర్మాణం FormInfo *ptr)
{
  కొరకు(; ptr; ptr = ptr->మరిన్ని) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->విలువ);
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

స్థిర CURLFORMcode formadd_check(నిర్మాణం FormInfo *మొదటి_form,
                                  నిర్మాణం curl_httppost **httppost,
                                  నిర్మాణం curl_httppost **చివరి_post)
{
  స్థిరరాశి అక్షరం *prevtype = ఖాళీ;
  నిర్మాణం FormInfo *form = ఖాళీ;
  నిర్మాణం curl_httppost *post = ఖాళీ;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  కొరకు(form = మొదటి_form; form; form = form->మరిన్ని) {
    స్థిరరాశి అక్షరం *name = Curl_bufref_ptr(&form->name);

    ఒకవేళ(((!name || !Curl_bufref_ptr(&form->విలువ)) && !post) ||
       (form->contentslength &&
        (form->flags & HTTPPOST_FILENAME)) ||
       ((form->flags & HTTPPOST_FILENAME) &&
        (form->flags & HTTPPOST_PTRCONTENTS)) ||

       (!form->బఫరు &&
        (form->flags & HTTPPOST_బఫరు) &&
        (form->flags & HTTPPOST_PTRBUFFER)) ||

       ((form->flags & HTTPPOST_READFILE) &&
        (form->flags & HTTPPOST_PTRCONTENTS))
      ) {
      తిరిగి CURL_FORMADD_INCOMPLETE;
    }
    ఒకవేళ(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_బఫరు)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      స్థిరరాశి అక్షరం *f = Curl_bufref_ptr((form->flags & HTTPPOST_బఫరు) ?
                                      &form->showfilename : &form->విలువ);
      స్థిరరాశి అక్షరం *type = Curl_mime_contenttype(f);
      ఒకవేళ(!type)
        type = prevtype;
      ఒకవేళ(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      ఒకవేళ(Curl_bufref_memdup0(&form->contenttype, type, అక్షరశ్రేణి_పొడవు(type)))
        తిరిగి CURL_FORMADD_జ్ఞాపకం;
    }
    ఒకవేళ(name && form->namelength && memchr(name, 0, form->namelength))
      తిరిగి CURL_FORMADD_NULL;
    ఒకవేళ(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      తిరిగి CURL_FORMADD_జ్ఞాపకం;
    ఒకవేళ(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->విలువ, (size_t)form->contentslength))
      తిరిగి CURL_FORMADD_జ్ఞాపకం;
    post = httppost_కూడు(form, post, httppost, చివరి_post);

    ఒకవేళ(!post)
      తిరిగి CURL_FORMADD_జ్ఞాపకం;

    ఒకవేళ(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  తిరిగి CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
స్థిర శూన్య free_chain(నిర్మాణం curl_httppost *c)
{
  అయితే(c) {
    నిర్మాణం curl_httppost *తదుపరి = c->తదుపరి;
    ఒకవేళ(c->మరిన్ని)
      free_chain(c->మరిన్ని);
    curlx_free(c);
    c = తదుపరి;
  }
}

స్థిర CURLFORMcode formadd(నిర్మాణం curl_httppost **httppost,
                            నిర్మాణం curl_httppost **చివరి_post, va_list params)
{
  నిర్మాణం FormInfo *మొదటి_form, *curr, *form = ఖాళీ;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  స్థిరరాశి నిర్మాణం curl_forms *forms = ఖాళీ;
  అక్షరం *avalue = ఖాళీ;
  నిర్మాణం curl_httppost *newchain = ఖాళీ;
  నిర్మాణం curl_httppost *lastnode = ఖాళీ;

#నిర్వచించు form_ptr_arg(t) (forms ? (t)(శూన్య *)avalue : va_arg(params, t))
#నిర్వచితమైతే HAVE_UINTPTR_T
#నిర్వచించు form_int_arg(t) (forms ? (t)(uintptr_t)avalue : va_arg(params, t))
#లేకపోతే
#నిర్వచించు form_int_arg(t) (forms ? (t)(శూన్య *)avalue : va_arg(params, t))
#అయితే_ముగింపు

  /*
   * We need to allocate the first struct to fill in.
   */
  మొదటి_form = forminfo_new();
  ఒకవేళ(!మొదటి_form)
    తిరిగి CURL_FORMADD_జ్ఞాపకం;

  curr = మొదటి_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  అయితే(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    ఒకవేళ(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (అక్షరం *)CURL_UNCONST(forms->విలువ);

      forms++; /* advance this to next entry */
      ఒకవేళ(CURLFORM_చివర == option) {
        /* end of array state */
        forms = ఖాళీ;
        కొనసాగు;
      }
    }
    లేకపోతే {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)va_arg(params, పూర్ణాంకం);
      ఒకవేళ(CURLFORM_చివర == option)
        ఆపు;
    }

    ఎంపిక(option) {
    స్థితి CURLFORM_ARRAY:
      ఒకవేళ(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      లేకపోతే {
        forms = va_arg(params, నిర్మాణం curl_forms *);
        ఒకవేళ(!forms)
          retval = CURL_FORMADD_NULL;
      }
      ఆపు;

      /*
       * Set the Name property.
       */
    స్థితి CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    స్థితి CURLFORM_COPYNAME:
      ఒకవేళ(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      లేకపోతే {
        avalue = form_ptr_arg(అక్షరం *);
        ఒకవేళ(avalue)
          Curl_bufref_సమితి(&curr->name, avalue, 0, ఖాళీ); /* No copy yet. */
        లేకపోతే
          retval = CURL_FORMADD_NULL;
      }
      ఆపు;
    స్థితి CURLFORM_NAMELENGTH:
      ఒకవేళ(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      లేకపోతే
        curr->namelength = (size_t)form_int_arg(దీర్ఘ);
      ఆపు;

      /*
       * Set the contents property.
       */
    స్థితి CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    స్థితి CURLFORM_COPYCONTENTS:
      ఒకవేళ(Curl_bufref_ptr(&curr->విలువ))
        retval = CURL_FORMADD_OPTION_TWICE;
      లేకపోతే {
        avalue = form_ptr_arg(అక్షరం *);
        ఒకవేళ(avalue)
          Curl_bufref_సమితి(&curr->విలువ, avalue, 0, ఖాళీ); /* No copy yet. */
        లేకపోతే
          retval = CURL_FORMADD_NULL;
      }
      ఆపు;
    స్థితి CURLFORM_CONTENTSLENGTH: {
      దీర్ఘ clen = form_int_arg(దీర్ఘ);
      ఒకవేళ(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      లేకపోతే
        curr->contentslength = (curl_off_t)clen;
      ఆపు;
    }

    స్థితి CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      ఒకవేళ(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      లేకపోతే {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      ఆపు;
    }

      /* Get contents from a given filename */
    స్థితి CURLFORM_FILECONTENT:
      ఒకవేళ(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      లేకపోతే {
        avalue = form_ptr_arg(అక్షరం *);
        ఒకవేళ(avalue) {
          ఒకవేళ(Curl_bufref_memdup0(&curr->విలువ, avalue, అక్షరశ్రేణి_పొడవు(avalue)))
            retval = CURL_FORMADD_జ్ఞాపకం;
          లేకపోతే
            curr->flags |= HTTPPOST_READFILE;
        }
        లేకపోతే
          retval = CURL_FORMADD_NULL;
      }
      ఆపు;

      /* We upload a file */
    స్థితి CURLFORM_FILE:
      avalue = form_ptr_arg(అక్షరం *);
      ఒకవేళ(Curl_bufref_ptr(&curr->విలువ)) {
        ఒకవేళ(curr->flags & HTTPPOST_FILENAME) {
          ఒకవేళ(avalue) {
            form = forminfo_new();
            ఒకవేళ(!form ||
               Curl_bufref_memdup0(&form->విలువ, avalue, అక్షరశ్రేణి_పొడవు(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_జ్ఞాపకం;
            }
            లేకపోతే {
              forminfo_కూడు(form, curr);
              curr = form;
              form = ఖాళీ;
            }
          }
          లేకపోతే
            retval = CURL_FORMADD_NULL;
        }
        లేకపోతే
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      లేకపోతే {
        ఒకవేళ(avalue) {
          ఒకవేళ(Curl_bufref_memdup0(&curr->విలువ, avalue, అక్షరశ్రేణి_పొడవు(avalue)))
            retval = CURL_FORMADD_జ్ఞాపకం;
          లేకపోతే
            curr->flags |= HTTPPOST_FILENAME;
        }
        లేకపోతే
          retval = CURL_FORMADD_NULL;
      }
      ఆపు;

    స్థితి CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_బఫరు;
      ఒకవేళ(curr->బఫరు)
        retval = CURL_FORMADD_OPTION_TWICE;
      లేకపోతే {
        avalue = form_ptr_arg(అక్షరం *);
        ఒకవేళ(avalue) {
          curr->బఫరు = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_సమితి(&curr->విలువ, avalue, 0, ఖాళీ);
        }
        లేకపోతే
          retval = CURL_FORMADD_NULL;
      }
      ఆపు;

    స్థితి CURLFORM_BUFFERLENGTH:
      ఒకవేళ(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      లేకపోతే
        curr->bufferlength = (size_t)form_int_arg(దీర్ఘ);
      ఆపు;

    స్థితి CURLFORM_ప్రవాహం:
      curr->flags |= HTTPPOST_CALLBACK;
      ఒకవేళ(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      లేకపోతే {
        avalue = form_ptr_arg(అక్షరం *);
        ఒకవేళ(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_సమితి(&curr->విలువ, avalue, 0, ఖాళీ);
        }
        లేకపోతే
          retval = CURL_FORMADD_NULL;
      }
      ఆపు;

    స్థితి CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(అక్షరం *);
      ఒకవేళ(Curl_bufref_ptr(&curr->contenttype)) {
        ఒకవేళ(curr->flags & HTTPPOST_FILENAME) {
          ఒకవేళ(avalue) {
            form = forminfo_new();
            ఒకవేళ(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            అక్షరశ్రేణి_పొడవు(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_జ్ఞాపకం;
            }
            లేకపోతే {
              forminfo_కూడు(form, curr);
              curr = form;
              form = ఖాళీ;
            }
          }
          లేకపోతే
            retval = CURL_FORMADD_NULL;
        }
        లేకపోతే
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      లేకపోతే ఒకవేళ(avalue) {
        ఒకవేళ(Curl_bufref_memdup0(&curr->contenttype, avalue, అక్షరశ్రేణి_పొడవు(avalue)))
          retval = CURL_FORMADD_జ్ఞాపకం;
      }
      లేకపోతే
        retval = CURL_FORMADD_NULL;
      ఆపు;

    స్థితి CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      నిర్మాణం curl_slist *list = form_ptr_arg(నిర్మాణం curl_slist *);

      ఒకవేళ(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      లేకపోతే
        curr->contentheader = list;

      ఆపు;
    }
    స్థితి CURLFORM_FILENAME:
    స్థితి CURLFORM_బఫరు:
      avalue = form_ptr_arg(అక్షరం *);
      ఒకవేళ(!avalue)
        retval = CURL_FORMADD_NULL;
      లేకపోతే ఒకవేళ(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      లేకపోతే ఒకవేళ(Curl_bufref_memdup0(&curr->showfilename, avalue, అక్షరశ్రేణి_పొడవు(avalue)))
        retval = CURL_FORMADD_జ్ఞాపకం;
      ఆపు;

    అప్రమేయ:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      ఆపు;
    }
  }

  ఒకవేళ(!retval)
    retval = formadd_check(మొదటి_form, &newchain, &lastnode);

  ఒకవేళ(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(మొదటి_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  అయితే(మొదటి_form) {
    నిర్మాణం FormInfo *ptr = మొదటి_form->మరిన్ని;
    curlx_free(మొదటి_form);
    మొదటి_form = ptr;
  }

  ఒకవేళ(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    ఒకవేళ(*చివరి_post)
      (*చివరి_post)->తదుపరి = newchain;
    లేకపోతే
      (*httppost) = newchain;

    (*చివరి_post) = lastnode;
  }
  లేకపోతే
    free_chain(newchain);

  తిరిగి retval;
#నిర్వచనం_రద్దు form_ptr_arg
#నిర్వచనం_రద్దు form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(నిర్మాణం curl_httppost **httppost,
                          నిర్మాణం curl_httppost **చివరి_post, ...)
{
  va_list arg;
  CURLFORMcode form;
  va_start(arg, చివరి_post);
  form = formadd(httppost, చివరి_post, arg);
  va_end(arg);
  తిరిగి form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
పూర్ణాంకం curl_formget(నిర్మాణం curl_httppost *form, శూన్య *arg,
                 curl_formget_callback append)
{
  CURLcode ఫలితం;
  curl_mimepart toppart;

  /* Validate callback is provided */
  ఒకవేళ(!append)
    తిరిగి (పూర్ణాంకం)CURLE_BAD_FUNCTION_వాదం;

  Curl_mime_initpart(&toppart); /* default form is empty */
  ఫలితం = Curl_getformdata(ఖాళీ, &toppart, form, ఖాళీ);
  ఒకవేళ(!ఫలితం)
    ఫలితం = Curl_mime_prepare_శీర్షికలు(ఖాళీ, &toppart, "multipart/form-data",
                                       ఖాళీ, MIMESTRATEGY_FORM);

  అయితే(!ఫలితం) {
    అక్షరం బఫరు[8192];
    size_t nread = Curl_mime_read(బఫరు, 1, పరిమాణం(బఫరు), &toppart);

    ఒకవేళ(!nread)
      ఆపు;

    ఒకవేళ(nread > పరిమాణం(బఫరు) || append(arg, బఫరు, nread) != nread) {
      ఫలితం = CURLE_READ_ERROR;
      ఒకవేళ(nread == CURL_READFUNC_ABORT)
        ఫలితం = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  తిరిగి (పూర్ణాంకం)ఫలితం;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
శూన్య curl_formfree(నిర్మాణం curl_httppost *form)
{
  నిర్మాణం curl_httppost *తదుపరి;

  ఒకవేళ(!form)
    /* no form to free, get out of this */
    తిరిగి;

  చేయి {
    తదుపరి = form->తదుపరి;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->మరిన్ని);

    ఒకవేళ(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    ఒకవేళ(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_బఫరు | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = తదుపరి;
  } అయితే(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
స్థిర CURLcode setname(curl_mimepart *part, స్థిరరాశి అక్షరం *name, size_t len)
{
  అక్షరం *zname;
  CURLcode ఫలితం;

  ఒకవేళ(!name || !len)
    తిరిగి curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  ఒకవేళ(!zname)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  ఫలితం = curl_mime_name(part, zname);
  curlx_free(zname);
  తిరిగి ఫలితం;
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

CURLcode Curl_getformdata(CURL *దత్తాంశం,
                          curl_mimepart *finalform,
                          నిర్మాణం curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode ఫలితం = CURLE_OK;
  curl_mime *form = ఖాళీ;
  curl_mimepart *part;
  నిర్మాణం curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  ఒకవేళ(!post)
    తిరిగి ఫలితం; /* no input => no output! */

  form = curl_mime_init(దత్తాంశం);
  ఒకవేళ(!form)
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(!ఫలితం)
    ఫలితం = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  కొరకు(; !ఫలితం && post; post = post->తదుపరి) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    ఒకవేళ(post->మరిన్ని) {
      part = curl_mime_addpart(form);
      ఒకవేళ(!part)
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      ఒకవేళ(!ఫలితం)
        ఫలితం = setname(part, post->name, post->namelength);
      ఒకవేళ(!ఫలితం) {
        multipart = curl_mime_init(దత్తాంశం);
        ఒకవేళ(!multipart)
          ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
      ఒకవేళ(!ఫలితం)
        ఫలితం = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    కొరకు(file = post; !ఫలితం && file; file = file->మరిన్ని) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      ఒకవేళ(!part)
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

      /* Set the headers. */
      ఒకవేళ(!ఫలితం)
        ఫలితం = curl_mime_శీర్షికలు(part, file->contentheader, 0);

      /* Set the content type. */
      ఒకవేళ(!ఫలితం && file->contenttype)
        ఫలితం = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      ఒకవేళ(!ఫలితం && !post->మరిన్ని)
        ఫలితం = setname(part, post->name, post->namelength);

      /* Process contents. */
      ఒకవేళ(!ఫలితం) {
        curl_off_t clen = post->contentslength;

        ఒకవేళ(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        ఒకవేళ(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          ఒకవేళ(!strcmp(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic నెట్టు
#సూచన clang diagnostic ignored "-Wcast-function-type-strict"
#అయితే_ముగింపు
            ఫలితం = curl_mime_దత్తాంశం_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)fread,
                                       curlx_fseek,
                                       ఖాళీ, (శూన్య *)stdin);
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic తీయి
#అయితే_ముగింపు
          }
          లేకపోతే
            ఫలితం = curl_mime_filedata(part, file->contents);
          ఒకవేళ(!ఫలితం && (post->flags & HTTPPOST_READFILE))
            ఫలితం = curl_mime_filename(part, ఖాళీ);
        }
        లేకపోతే ఒకవేళ(post->flags & HTTPPOST_బఫరు)
          ఫలితం = curl_mime_దత్తాంశం(part, post->బఫరు,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        లేకపోతే ఒకవేళ(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          ఒకవేళ(!clen)
            clen = -1;
          ఫలితం = curl_mime_దత్తాంశం_cb(part, clen,
                                     fread_func, ఖాళీ, ఖాళీ, post->userp);
        }
        లేకపోతే {
          size_t uclen;
          ఒకవేళ(!clen)
            uclen = CURL_ZERO_TERMINATED;
          లేకపోతే
            uclen = (size_t)clen;
          ఫలితం = curl_mime_దత్తాంశం(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      ఒకవేళ(!ఫలితం && post->showfilename &&
         (post->మరిన్ని || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_బఫరు |
                                        HTTPPOST_CALLBACK))))
        ఫలితం = curl_mime_filename(part, post->showfilename);
    }
  }

  ఒకవేళ(ఫలితం)
    Curl_mime_cleanpart(finalform);

  తిరిగి ఫలితం;
}

#లేకపోతే /* if disabled */
CURLFORMcode curl_formadd(నిర్మాణం curl_httppost **httppost,
                          నిర్మాణం curl_httppost **చివరి_post, ...)
{
  (శూన్య)httppost;
  (శూన్య)చివరి_post;
  తిరిగి CURL_FORMADD_DISABLED;
}

పూర్ణాంకం curl_formget(నిర్మాణం curl_httppost *form, శూన్య *arg,
                 curl_formget_callback append)
{
  (శూన్య)form;
  (శూన్య)arg;
  (శూన్య)append;
  తిరిగి CURL_FORMADD_DISABLED;
}

శూన్య curl_formfree(నిర్మాణం curl_httppost *form)
{
  (శూన్య)form;
  /* Nothing to do. */
}

#అయితే_ముగింపు /* if disabled */
