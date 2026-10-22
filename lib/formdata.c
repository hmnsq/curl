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
#समावेश "curl_setup.h"

काष्ठा Curl_easy;

#समावेश "formdata.h"

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_FORM_API)

#समावेश "urldata.h" /* for struct Curl_easy */
#समावेश "mime.h"
#समावेश "curlx/strdup.h"
#समावेश "bufref.h"
#समावेश "curlx/win32-fopen.h"


#घोषणा HTTPPOST_PTRNAME     CURL_HTTPPOST_PTRNAME
#घोषणा HTTPPOST_FILENAME    CURL_HTTPPOST_FILENAME
#घोषणा HTTPPOST_PTRCONTENTS CURL_HTTPPOST_PTRCONTENTS
#घोषणा HTTPPOST_READFILE    CURL_HTTPPOST_READFILE
#घोषणा HTTPPOST_PTRBUFFER   CURL_HTTPPOST_PTRBUFFER
#घोषणा HTTPPOST_CALLBACK    CURL_HTTPPOST_CALLBACK
#घोषणा HTTPPOST_बफर      CURL_HTTPPOST_बफर

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
अटल काष्ठा curl_httppost *httppost_add(काष्ठा FormInfo *src,
                                          काष्ठा curl_httppost *parent_post,
                                          काष्ठा curl_httppost **httppost,
                                          काष्ठा curl_httppost **last_post)
{
  काष्ठा curl_httppost *post;
  माप_प्रकार namelength = src->namelength;
  अगर(!namelength && Curl_bufref_ptr(&src->name))
    namelength = म_माप(Curl_bufref_ptr(&src->name));
  अगर((src->bufferlength > दीर्घ_उच्च) || (namelength > दीर्घ_उच्च))
    /* avoid overflow in typecasts below */
    वापस शून्य;
  post = curlx_calloc(1, माप(काष्ठा curl_httppost));
  अगर(post) {
    post->name = CURL_UNCONST(Curl_bufref_ptr(&src->name));
    post->namelength = (दीर्घ)namelength;
    post->contents = CURL_UNCONST(Curl_bufref_ptr(&src->मूल्य));
    post->contentlen = src->contentslength;
    post->बफर = src->बफर;
    post->bufferlength = (दीर्घ)src->bufferlength;
    post->contenttype = CURL_UNCONST(Curl_bufref_ptr(&src->contenttype));
    post->flags = src->flags | CURL_HTTPPOST_LARGE;
    post->contentheader = src->contentheader;
    post->showfilename = CURL_UNCONST(Curl_bufref_ptr(&src->showfilename));
    post->userp = src->userp;
  }
  अन्यथा
    वापस शून्य;

  अगर(parent_post) {
    /* now, point our 'more' to the original 'more' */
    post->more = parent_post->more;

    /* then move the original 'more' to point to ourselves */
    parent_post->more = post;
  }
  अन्यथा {
    /* make the previous point to this */
    अगर(*last_post)
      (*last_post)->अग्र = post;
    अन्यथा
      (*httppost) = post;

    (*last_post) = post;
  }
  वापस post;
}

/* Allocate and initialize a new FormInfo structure. */
अटल काष्ठा FormInfo *forminfo_new(व्योम)
{
  काष्ठा FormInfo *form_info = curlx_calloc(1, माप(काष्ठा FormInfo));

  अगर(form_info) {
    Curl_bufref_init(&form_info->name);
    Curl_bufref_init(&form_info->मूल्य);
    Curl_bufref_init(&form_info->contenttype);
    Curl_bufref_init(&form_info->showfilename);
  }

  वापस form_info;
}

/* Replace the target field data by a dynamic copy of it. */
अटल CURLcode forminfo_copyfield(काष्ठा bufref *field, माप_प्रकार len)
{
  स्थिर अक्षर *मूल्य = Curl_bufref_ptr(field);
  CURLcode परिणाम = CURLE_OK;

  अगर(मूल्य) {
    अगर(!len)
      len = म_माप(मूल्य);
    परिणाम = Curl_bufref_memdup0(field, मूल्य, len);
  }

  वापस परिणाम;
}

/***************************************************************************
 *
 * forminfo_add()
 *
 * Adds a FormInfo structure to the list presented by parent.
 *
 ***************************************************************************/
अटल व्योम forminfo_add(काष्ठा FormInfo *form_info, काष्ठा FormInfo *parent)
{
  form_info->flags |= HTTPPOST_FILENAME;

  अगर(parent) {
    /* now, point our 'more' to the original 'more' */
    form_info->more = parent->more;

    /* then move the original 'more' to point to ourselves */
    parent->more = form_info;
  }
}

अटल व्योम free_formlist(काष्ठा FormInfo *ptr)
{
  क्रम(; ptr; ptr = ptr->more) {
    Curl_bufref_free(&ptr->name);
    Curl_bufref_free(&ptr->मूल्य);
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

अटल CURLFORMcode formadd_check(काष्ठा FormInfo *first_form,
                                  काष्ठा curl_httppost **httppost,
                                  काष्ठा curl_httppost **last_post)
{
  स्थिर अक्षर *prevtype = शून्य;
  काष्ठा FormInfo *form = शून्य;
  काष्ठा curl_httppost *post = शून्य;

  /* go through the list, check for completeness and if everything is
   * alright add the HttpPost item otherwise set retval accordingly */

  क्रम(form = first_form; form; form = form->more) {
    स्थिर अक्षर *name = Curl_bufref_ptr(&form->name);

    अगर(((!name || !Curl_bufref_ptr(&form->मूल्य)) && !post) ||
       (form->contentslength &&
        (form->flags & HTTPPOST_FILENAME)) ||
       ((form->flags & HTTPPOST_FILENAME) &&
        (form->flags & HTTPPOST_PTRCONTENTS)) ||

       (!form->बफर &&
        (form->flags & HTTPPOST_बफर) &&
        (form->flags & HTTPPOST_PTRBUFFER)) ||

       ((form->flags & HTTPPOST_READFILE) &&
        (form->flags & HTTPPOST_PTRCONTENTS))
      ) {
      वापस CURL_FORMADD_INCOMPLETE;
    }
    अगर(((form->flags & HTTPPOST_FILENAME) ||
        (form->flags & HTTPPOST_बफर)) &&
       !Curl_bufref_ptr(&form->contenttype)) {
      स्थिर अक्षर *f = Curl_bufref_ptr((form->flags & HTTPPOST_बफर) ?
                                      &form->showfilename : &form->मूल्य);
      स्थिर अक्षर *type = Curl_mime_contenttype(f);
      अगर(!type)
        type = prevtype;
      अगर(!type)
        type = FILE_CONTENTTYPE_DEFAULT;

      /* our contenttype is missing */
      अगर(Curl_bufref_memdup0(&form->contenttype, type, म_माप(type)))
        वापस CURL_FORMADD_MEMORY;
    }
    अगर(name && form->namelength && स_प्रथम(name, 0, form->namelength))
      वापस CURL_FORMADD_NULL;
    अगर(!(form->flags & HTTPPOST_PTRNAME) &&
       /* Note that there is small risk that form->name is NULL here if the app
          passed in a bad combo, so we check for that. */
       forminfo_copyfield(&form->name, form->namelength))
      वापस CURL_FORMADD_MEMORY;
    अगर(!(form->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE |
                        HTTPPOST_PTRCONTENTS | HTTPPOST_PTRBUFFER |
                        HTTPPOST_CALLBACK)) &&
       forminfo_copyfield(&form->मूल्य, (माप_प्रकार)form->contentslength))
      वापस CURL_FORMADD_MEMORY;
    post = httppost_add(form, post, httppost, last_post);

    अगर(!post)
      वापस CURL_FORMADD_MEMORY;

    अगर(Curl_bufref_ptr(&form->contenttype))
      prevtype = Curl_bufref_ptr(&form->contenttype);
  }

  वापस CURL_FORMADD_OK;
}

/* Shallow cleanup. Remove the newly created chain, the structs only and not
   the content they point to */
अटल व्योम free_chain(काष्ठा curl_httppost *c)
{
  जबतक(c) {
    काष्ठा curl_httppost *अग्र = c->अग्र;
    अगर(c->more)
      free_chain(c->more);
    curlx_free(c);
    c = अग्र;
  }
}

अटल CURLFORMcode formadd(काष्ठा curl_httppost **httppost,
                            काष्ठा curl_httppost **last_post, बहु_सूची params)
{
  काष्ठा FormInfo *first_form, *curr, *form = शून्य;
  CURLFORMcode retval = CURL_FORMADD_OK;
  CURLformoption option;
  स्थिर काष्ठा curl_forms *forms = शून्य;
  अक्षर *avalue = शून्य;
  काष्ठा curl_httppost *newchain = शून्य;
  काष्ठा curl_httppost *lastnode = शून्य;

#घोषणा form_ptr_arg(t) (forms ? (t)(व्योम *)avalue : बहु_तर्क(params, t))
#अगर_घोषित HAVE_UINTPTR_T
#घोषणा form_int_arg(t) (forms ? (t)(uintptr_t)avalue : बहु_तर्क(params, t))
#अन्यथा
#घोषणा form_int_arg(t) (forms ? (t)(व्योम *)avalue : बहु_तर्क(params, t))
#पूर्ण_अगर

  /*
   * We need to allocate the first struct to fill in.
   */
  first_form = forminfo_new();
  अगर(!first_form)
    वापस CURL_FORMADD_MEMORY;

  curr = first_form;

  /*
   * Loop through all the options set. Break if we have an error to report.
   */
  जबतक(retval == CURL_FORMADD_OK) {

    /* first see if we have more parts of the array param */
    अगर(forms) {
      /* get the upcoming option from the given array */
      option = forms->option;
      avalue = (अक्षर *)CURL_UNCONST(forms->मूल्य);

      forms++; /* advance this to next entry */
      अगर(CURLFORM_END == option) {
        /* end of array state */
        forms = शून्य;
        जारी;
      }
    }
    अन्यथा {
      /* This is not array-state, get next option. This gets an 'int' with
         va_arg() because CURLformoption might be a smaller type than int and
         might cause compiler warnings and wrong behavior. */
      option = (CURLformoption)बहु_तर्क(params, पूर्णांक);
      अगर(CURLFORM_END == option)
        अवरोध;
    }

    चयन(option) {
    हाल CURLFORM_ARRAY:
      अगर(forms)
        /* we do not support an array from within an array */
        retval = CURL_FORMADD_ILLEGAL_ARRAY;
      अन्यथा {
        forms = बहु_तर्क(params, काष्ठा curl_forms *);
        अगर(!forms)
          retval = CURL_FORMADD_NULL;
      }
      अवरोध;

      /*
       * Set the Name property.
       */
    हाल CURLFORM_PTRNAME:
      curr->flags |= HTTPPOST_PTRNAME;
      FALLTHROUGH();
    हाल CURLFORM_COPYNAME:
      अगर(Curl_bufref_ptr(&curr->name))
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा {
        avalue = form_ptr_arg(अक्षर *);
        अगर(avalue)
          Curl_bufref_set(&curr->name, avalue, 0, शून्य); /* No copy yet. */
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      अवरोध;
    हाल CURLFORM_NAMELENGTH:
      अगर(curr->namelength)
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा
        curr->namelength = (माप_प्रकार)form_int_arg(दीर्घ);
      अवरोध;

      /*
       * Set the contents property.
       */
    हाल CURLFORM_PTRCONTENTS:
      curr->flags |= HTTPPOST_PTRCONTENTS;
      FALLTHROUGH();
    हाल CURLFORM_COPYCONTENTS:
      अगर(Curl_bufref_ptr(&curr->मूल्य))
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा {
        avalue = form_ptr_arg(अक्षर *);
        अगर(avalue)
          Curl_bufref_set(&curr->मूल्य, avalue, 0, शून्य); /* No copy yet. */
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      अवरोध;
    हाल CURLFORM_CONTENTSLENGTH: {
      दीर्घ clen = form_int_arg(दीर्घ);
      अगर(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      अन्यथा
        curr->contentslength = (curl_off_t)clen;
      अवरोध;
    }

    हाल CURLFORM_CONTENTLEN: {
      curl_off_t clen = form_int_arg(curl_off_t);
      अगर(clen < 0)
        retval = CURL_FORMADD_INCOMPLETE;
      अन्यथा {
        curr->flags |= CURL_HTTPPOST_LARGE;
        curr->contentslength = clen;
      }
      अवरोध;
    }

      /* Get contents from a given filename */
    हाल CURLFORM_FILECONTENT:
      अगर(curr->flags & (HTTPPOST_PTRCONTENTS | HTTPPOST_READFILE))
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा {
        avalue = form_ptr_arg(अक्षर *);
        अगर(avalue) {
          अगर(Curl_bufref_memdup0(&curr->मूल्य, avalue, म_माप(avalue)))
            retval = CURL_FORMADD_MEMORY;
          अन्यथा
            curr->flags |= HTTPPOST_READFILE;
        }
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      अवरोध;

      /* We upload a file */
    हाल CURLFORM_FILE:
      avalue = form_ptr_arg(अक्षर *);
      अगर(Curl_bufref_ptr(&curr->मूल्य)) {
        अगर(curr->flags & HTTPPOST_FILENAME) {
          अगर(avalue) {
            form = forminfo_new();
            अगर(!form ||
               Curl_bufref_memdup0(&form->मूल्य, avalue, म_माप(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            अन्यथा {
              forminfo_add(form, curr);
              curr = form;
              form = शून्य;
            }
          }
          अन्यथा
            retval = CURL_FORMADD_NULL;
        }
        अन्यथा
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      अन्यथा {
        अगर(avalue) {
          अगर(Curl_bufref_memdup0(&curr->मूल्य, avalue, म_माप(avalue)))
            retval = CURL_FORMADD_MEMORY;
          अन्यथा
            curr->flags |= HTTPPOST_FILENAME;
        }
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      अवरोध;

    हाल CURLFORM_BUFFERPTR:
      curr->flags |= HTTPPOST_PTRBUFFER | HTTPPOST_बफर;
      अगर(curr->बफर)
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा {
        avalue = form_ptr_arg(अक्षर *);
        अगर(avalue) {
          curr->बफर = avalue; /* store for the moment */
          /* Make value non-NULL to be accepted as fine */
          Curl_bufref_set(&curr->मूल्य, avalue, 0, शून्य);
        }
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      अवरोध;

    हाल CURLFORM_BUFFERLENGTH:
      अगर(curr->bufferlength)
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा
        curr->bufferlength = (माप_प्रकार)form_int_arg(दीर्घ);
      अवरोध;

    हाल CURLFORM_STREAM:
      curr->flags |= HTTPPOST_CALLBACK;
      अगर(curr->userp)
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा {
        avalue = form_ptr_arg(अक्षर *);
        अगर(avalue) {
          curr->userp = avalue;
          /* The following line is not strictly true but we derive a value
             from this later on and we need this non-NULL to be accepted as
             a fine form part */
          Curl_bufref_set(&curr->मूल्य, avalue, 0, शून्य);
        }
        अन्यथा
          retval = CURL_FORMADD_NULL;
      }
      अवरोध;

    हाल CURLFORM_CONTENTTYPE:
      avalue = form_ptr_arg(अक्षर *);
      अगर(Curl_bufref_ptr(&curr->contenttype)) {
        अगर(curr->flags & HTTPPOST_FILENAME) {
          अगर(avalue) {
            form = forminfo_new();
            अगर(!form || Curl_bufref_memdup0(&form->contenttype, avalue,
                                            म_माप(avalue))) {
              curlx_free(form);
              retval = CURL_FORMADD_MEMORY;
            }
            अन्यथा {
              forminfo_add(form, curr);
              curr = form;
              form = शून्य;
            }
          }
          अन्यथा
            retval = CURL_FORMADD_NULL;
        }
        अन्यथा
          retval = CURL_FORMADD_OPTION_TWICE;
      }
      अन्यथा अगर(avalue) {
        अगर(Curl_bufref_memdup0(&curr->contenttype, avalue, म_माप(avalue)))
          retval = CURL_FORMADD_MEMORY;
      }
      अन्यथा
        retval = CURL_FORMADD_NULL;
      अवरोध;

    हाल CURLFORM_CONTENTHEADER: {
      /* this "cast increases required alignment of target type" but
         we consider it OK anyway */
      काष्ठा curl_slist *list = form_ptr_arg(काष्ठा curl_slist *);

      अगर(curr->contentheader)
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा
        curr->contentheader = list;

      अवरोध;
    }
    हाल CURLFORM_FILENAME:
    हाल CURLFORM_बफर:
      avalue = form_ptr_arg(अक्षर *);
      अगर(!avalue)
        retval = CURL_FORMADD_NULL;
      अन्यथा अगर(Curl_bufref_ptr(&curr->showfilename))
        retval = CURL_FORMADD_OPTION_TWICE;
      अन्यथा अगर(Curl_bufref_memdup0(&curr->showfilename, avalue, म_माप(avalue)))
        retval = CURL_FORMADD_MEMORY;
      अवरोध;

    शेष:
      retval = CURL_FORMADD_UNKNOWN_OPTION;
      अवरोध;
    }
  }

  अगर(!retval)
    retval = formadd_check(first_form, &newchain, &lastnode);

  अगर(retval)
    /* On error, free allocated fields for all nodes of the FormInfo linked
       list without deallocating nodes. List nodes are deallocated later on */
    free_formlist(first_form);

  /* Always deallocate FormInfo linked list nodes without touching node
     fields given that these have either been deallocated or are owned
     now by the httppost linked list */
  जबतक(first_form) {
    काष्ठा FormInfo *ptr = first_form->more;
    curlx_free(first_form);
    first_form = ptr;
  }

  अगर(!retval) {
    /* Only if all is fine, link the new chain into the provided list */
    अगर(*last_post)
      (*last_post)->अग्र = newchain;
    अन्यथा
      (*httppost) = newchain;

    (*last_post) = lastnode;
  }
  अन्यथा
    free_chain(newchain);

  वापस retval;
#अघोषित form_ptr_arg
#अघोषित form_int_arg
}

/*
 * curl_formadd() is a public API to add a section to the multipart formpost.
 *
 * @unittest: 1308
 */

CURLFORMcode curl_formadd(काष्ठा curl_httppost **httppost,
                          काष्ठा curl_httppost **last_post, ...)
{
  बहु_सूची arg;
  CURLFORMcode form;
  बहु_शुरू(arg, last_post);
  form = formadd(httppost, last_post, arg);
  बहु_पूर्ण(arg);
  वापस form;
}

/*
 * curl_formget()
 * Serialize a curl_httppost struct.
 * Returns 0 on success.
 *
 * @unittest: 1308
 */
पूर्णांक curl_formget(काष्ठा curl_httppost *form, व्योम *arg,
                 curl_formget_callback append)
{
  CURLcode परिणाम;
  curl_mimepart toppart;

  /* Validate callback is provided */
  अगर(!append)
    वापस (पूर्णांक)CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_mime_initpart(&toppart); /* default form is empty */
  परिणाम = Curl_getformdata(शून्य, &toppart, form, शून्य);
  अगर(!परिणाम)
    परिणाम = Curl_mime_prepare_headers(शून्य, &toppart, "multipart/form-data",
                                       शून्य, MIMESTRATEGY_FORM);

  जबतक(!परिणाम) {
    अक्षर बफर[8192];
    माप_प्रकार nread = Curl_mime_read(बफर, 1, माप(बफर), &toppart);

    अगर(!nread)
      अवरोध;

    अगर(nread > माप(बफर) || append(arg, बफर, nread) != nread) {
      परिणाम = CURLE_READ_ERROR;
      अगर(nread == CURL_READFUNC_ABORT)
        परिणाम = CURLE_ABORTED_BY_CALLBACK;
    }
  }

  Curl_mime_cleanpart(&toppart);
  वापस (पूर्णांक)परिणाम;
}

/*
 * curl_formfree() is an external function to free up a whole form post
 * chain
 */
व्योम curl_formfree(काष्ठा curl_httppost *form)
{
  काष्ठा curl_httppost *अग्र;

  अगर(!form)
    /* no form to free, get out of this */
    वापस;

  करो {
    अग्र = form->अग्र;  /* the following form line */

    /* recurse to sub-contents */
    curl_formfree(form->more);

    अगर(!(form->flags & HTTPPOST_PTRNAME))
      curlx_free(form->name); /* free the name */
    अगर(!(form->flags &
         (HTTPPOST_PTRCONTENTS | HTTPPOST_बफर | HTTPPOST_CALLBACK)))
      curlx_free(form->contents); /* free the contents */
    curlx_free(form->contenttype); /* free the content type */
    curlx_free(form->showfilename); /* free the faked filename */
    curlx_free(form);       /* free the struct */
    form = अग्र;
  } जबतक(form); /* continue */
}

/* Set mime part name, taking care of non null-terminated name string. */
अटल CURLcode setname(curl_mimepart *part, स्थिर अक्षर *name, माप_प्रकार len)
{
  अक्षर *zname;
  CURLcode परिणाम;

  अगर(!name || !len)
    वापस curl_mime_name(part, name);
  zname = curlx_memdup0(name, len);
  अगर(!zname)
    वापस CURLE_OUT_OF_MEMORY;
  परिणाम = curl_mime_name(part, zname);
  curlx_free(zname);
  वापस परिणाम;
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

CURLcode Curl_getformdata(CURL *तथ्य,
                          curl_mimepart *finalform,
                          काष्ठा curl_httppost *post,
                          curl_read_callback fread_func)
{
  CURLcode परिणाम = CURLE_OK;
  curl_mime *form = शून्य;
  curl_mimepart *part;
  काष्ठा curl_httppost *file;

  Curl_mime_cleanpart(finalform); /* default form is empty */

  अगर(!post)
    वापस परिणाम; /* no input => no output! */

  form = curl_mime_init(तथ्य);
  अगर(!form)
    परिणाम = CURLE_OUT_OF_MEMORY;

  अगर(!परिणाम)
    परिणाम = curl_mime_subparts(finalform, form);

  /* Process each top part. */
  क्रम(; !परिणाम && post; post = post->अग्र) {
    /* If we have more than a file here, create a mime subpart and fill it. */
    curl_mime *multipart = form;
    अगर(post->more) {
      part = curl_mime_addpart(form);
      अगर(!part)
        परिणाम = CURLE_OUT_OF_MEMORY;
      अगर(!परिणाम)
        परिणाम = setname(part, post->name, post->namelength);
      अगर(!परिणाम) {
        multipart = curl_mime_init(तथ्य);
        अगर(!multipart)
          परिणाम = CURLE_OUT_OF_MEMORY;
      }
      अगर(!परिणाम)
        परिणाम = curl_mime_subparts(part, multipart);
    }

    /* Generate all the part contents. */
    क्रम(file = post; !परिणाम && file; file = file->more) {
      /* Create the part. */
      part = curl_mime_addpart(multipart);
      अगर(!part)
        परिणाम = CURLE_OUT_OF_MEMORY;

      /* Set the headers. */
      अगर(!परिणाम)
        परिणाम = curl_mime_headers(part, file->contentheader, 0);

      /* Set the content type. */
      अगर(!परिणाम && file->contenttype)
        परिणाम = curl_mime_type(part, file->contenttype);

      /* Set field name. */
      अगर(!परिणाम && !post->more)
        परिणाम = setname(part, post->name, post->namelength);

      /* Process contents. */
      अगर(!परिणाम) {
        curl_off_t clen = post->contentslength;

        अगर(post->flags & CURL_HTTPPOST_LARGE)
          clen = post->contentlen;

        अगर(post->flags & (HTTPPOST_FILENAME | HTTPPOST_READFILE)) {
          अगर(!म_भेद(file->contents, "-")) {
            /* There are a few cases where the code below will not work; in
               particular, freopen(stdin) by the caller is not guaranteed
               to result as expected. This feature has been kept for backward
               compatibility: use of "-" pseudo filename should be avoided. */
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic धकेलो
#आशय clang diagnostic ignored "-Wcast-function-type-strict"
#पूर्ण_अगर
            परिणाम = curl_mime_तथ्य_cb(part, (curl_off_t)-1,
                                       (curl_read_callback)ख_पढ़ो,
                                       curlx_fseek,
                                       शून्य, (व्योम *)मानक_निवेश);
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic निकालो
#पूर्ण_अगर
          }
          अन्यथा
            परिणाम = curl_mime_filedata(part, file->contents);
          अगर(!परिणाम && (post->flags & HTTPPOST_READFILE))
            परिणाम = curl_mime_filename(part, शून्य);
        }
        अन्यथा अगर(post->flags & HTTPPOST_बफर)
          परिणाम = curl_mime_तथ्य(part, post->बफर,
                                  post->bufferlength ?
                                  post->bufferlength : -1);
        अन्यथा अगर(post->flags & HTTPPOST_CALLBACK) {
          /* the contents should be read with the callback and the size is set
             with the contentslength */
          अगर(!clen)
            clen = -1;
          परिणाम = curl_mime_तथ्य_cb(part, clen,
                                     fread_func, शून्य, शून्य, post->userp);
        }
        अन्यथा {
          माप_प्रकार uclen;
          अगर(!clen)
            uclen = CURL_ZERO_TERMINATED;
          अन्यथा
            uclen = (माप_प्रकार)clen;
          परिणाम = curl_mime_तथ्य(part, post->contents, uclen);
        }
      }

      /* Set fake filename. */
      अगर(!परिणाम && post->showfilename &&
         (post->more || (post->flags & (HTTPPOST_FILENAME | HTTPPOST_बफर |
                                        HTTPPOST_CALLBACK))))
        परिणाम = curl_mime_filename(part, post->showfilename);
    }
  }

  अगर(परिणाम)
    Curl_mime_cleanpart(finalform);

  वापस परिणाम;
}

#अन्यथा /* if disabled */
CURLFORMcode curl_formadd(काष्ठा curl_httppost **httppost,
                          काष्ठा curl_httppost **last_post, ...)
{
  (व्योम)httppost;
  (व्योम)last_post;
  वापस CURL_FORMADD_DISABLED;
}

पूर्णांक curl_formget(काष्ठा curl_httppost *form, व्योम *arg,
                 curl_formget_callback append)
{
  (व्योम)form;
  (व्योम)arg;
  (व्योम)append;
  वापस CURL_FORMADD_DISABLED;
}

व्योम curl_formfree(काष्ठा curl_httppost *form)
{
  (व्योम)form;
  /* Nothing to do. */
}

#पूर्ण_अगर /* if disabled */
