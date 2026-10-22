/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Steve Holme, <steve_holme@hotmail.com>.
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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "creds.h"
#अन्तर्भाव "curlx/multibyte.h"
#अन्तर्भाव "url.h"

/*
 * Curl_auth_build_spn()
 *
 * This is used to build an SPN string in the following formats:
 *
 * service/host@realm (Not currently used)
 * service/host       (Not used by GSS-API)
 * service@realm      (Not used by Windows SSPI)
 *
 * Parameters:
 *
 * service  [in] - The service type such as http, smtp, pop or imap.
 * host     [in] - The hostname.
 * realm    [in] - The realm.
 *
 * Returns a pointer to the newly allocated SPN.
 */
#ifndef USE_WINDOWS_SSPI
वर्ण *Curl_auth_build_spn(अचर वर्ण *service, अचर वर्ण *host,
                          अचर वर्ण *realm)
{
  वर्ण *spn = रिक्त;

  /* Generate our SPN */
  यदि(host && realm)
    spn = curl_maprintf("%s/%s@%s", service, host, realm);
  अन्यथा यदि(host)
    spn = curl_maprintf("%s/%s", service, host);
  अन्यथा यदि(realm)
    spn = curl_maprintf("%s@%s", service, realm);

  /* Return our newly allocated SPN */
  फर्काऊ spn;
}
#अन्यथा
TCHAR *Curl_auth_build_spn(अचर वर्ण *service, अचर वर्ण *host,
                           अचर वर्ण *realm)
{
  वर्ण *utf8_spn = रिक्त;
  TCHAR *tchar_spn = रिक्त;

  (शून्य)realm;

  /* Note: We could use DsMakeSPN() or DsClientMakeSpnForTargetServer() rather
     than doing this ourselves but the first is only available in Windows XP
     and Windows Server 2003 and the latter is only available in Windows 2000
     but not Windows95/98/ME or Windows NT4.0 unless the Active Directory
     Client Extensions are installed. As such it is far simpler for us to
     formulate the SPN instead. */

  /* Generate our UTF8 based SPN */
  utf8_spn = curl_maprintf("%s/%s", service, host);
  यदि(!utf8_spn)
    फर्काऊ रिक्त;

  /* Allocate and return a TCHAR based SPN. */
  tchar_spn = curlx_convert_UTF8_to_tchar(utf8_spn);
  curlx_free(utf8_spn);

  फर्काऊ tchar_spn;
}
#endif /* USE_WINDOWS_SSPI */

/*
 * Curl_auth_user_contains_domain()
 *
 * This is used to test if the specified user contains a Windows domain name as
 * follows:
 *
 * Domain\User (Down-level Logon Name)
 * Domain/User (curl Down-level format - for compatibility with existing code)
 * User@Domain (User Principal Name)
 *
 * Note: The username may be empty when using a GSS-API library or Windows
 * SSPI as the user and domain are either obtained from the credentials cache
 * when using GSS-API or via the currently logged in user's credentials when
 * using Windows SSPI.
 *
 * Parameters:
 *
 * user  [in] - The username.
 *
 * Returns TRUE on success; otherwise FALSE.
 */
bool Curl_auth_user_contains_domain(संरचना Curl_creds *creds)
{
  bool valid = FALSE;

  यदि(Curl_creds_has_user(creds)) {
    /* Check we have a domain name or UPN present */
    अचर वर्ण *p = strpbrk(creds->user, "\\/@");

    valid = p && (p > creds->user) &&
            (p < (creds->user + म_माप(creds->user) - 1));
  }
#यदि defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  अन्यथा
    /* User and domain are obtained from the GSS-API credentials cache or the
       currently logged in user from Windows */
    valid = TRUE;
#endif

  फर्काऊ valid;
}

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can be sent to the connection's origin.
 */
bool Curl_auth_allowed_to_host(संरचना Curl_easy *data)
{
  फर्काऊ Curl_auth_allowed_to_origin(data, data->state.origin);
}

bool Curl_auth_allowed_to_origin(संरचना Curl_easy *data,
                                 संरचना Curl_peer *origin)
{
  फर्काऊ data->set.allow_auth_to_other_hosts ||
         Curl_peer_equal(data->state.initial_origin, origin);
}

#ifdef USE_NTLM
स्थिर शून्य ntlm_conn_dtor(अचर शून्य *साँचो, माप_प्रकार klen, शून्य *entry)
{
  संरचना ntlmdata *ntlm = entry;
  (शून्य)साँचो;
  (शून्य)klen;
  DEBUGASSERT(ntlm);
  Curl_auth_cleanup_ntlm(ntlm);
  curlx_free(ntlm);
}

संरचना ntlmdata *Curl_auth_ntlm_get(संरचना connectdata *conn, bool proxy)
{
  अचर वर्ण *साँचो = proxy ? CURL_META_NTLM_PROXY_CONN : CURL_META_NTLM_CONN;
  संरचना ntlmdata *ntlm = Curl_conn_meta_get(conn, साँचो);
  यदि(!ntlm) {
    ntlm = curlx_calloc(1, परिमाण(*ntlm));
    यदि(!ntlm || Curl_conn_meta_set(conn, साँचो, ntlm, ntlm_conn_dtor))
      फर्काऊ रिक्त;
  }
  फर्काऊ ntlm;
}

शून्य Curl_auth_ntlm_remove(संरचना connectdata *conn, bool proxy)
{
  Curl_conn_meta_remove(conn, proxy ? CURL_META_NTLM_PROXY_CONN
                                    : CURL_META_NTLM_CONN);
}
#endif /* USE_NTLM */

#ifdef USE_KERBEROS5
स्थिर शून्य krb5_conn_dtor(अचर शून्य *साँचो, माप_प्रकार klen, शून्य *entry)
{
  संरचना kerberos5data *krb5 = entry;
  (शून्य)साँचो;
  (शून्य)klen;
  DEBUGASSERT(krb5);
  Curl_auth_cleanup_gssapi(krb5);
  curlx_free(krb5);
}

संरचना kerberos5data *Curl_auth_krb5_get(संरचना connectdata *conn)
{
  संरचना kerberos5data *krb5 = Curl_conn_meta_get(conn, CURL_META_KRB5_CONN);
  यदि(!krb5) {
    krb5 = curlx_calloc(1, परिमाण(*krb5));
    यदि(!krb5 ||
       Curl_conn_meta_set(conn, CURL_META_KRB5_CONN, krb5, krb5_conn_dtor))
      फर्काऊ रिक्त;
  }
  फर्काऊ krb5;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
स्थिर शून्य gsasl_conn_dtor(अचर शून्य *साँचो, माप_प्रकार klen, शून्य *entry)
{
  संरचना gsasldata *gsasl = entry;
  (शून्य)साँचो;
  (शून्य)klen;
  DEBUGASSERT(gsasl);
  Curl_auth_gsasl_cleanup(gsasl);
  curlx_free(gsasl);
}

संरचना gsasldata *Curl_auth_gsasl_get(संरचना connectdata *conn)
{
  संरचना gsasldata *gsasl = Curl_conn_meta_get(conn, CURL_META_GSASL_CONN);
  यदि(!gsasl) {
    gsasl = curlx_calloc(1, परिमाण(*gsasl));
    यदि(!gsasl ||
       Curl_conn_meta_set(conn, CURL_META_GSASL_CONN, gsasl, gsasl_conn_dtor))
      फर्काऊ रिक्त;
  }
  फर्काऊ gsasl;
}
#endif /* USE_GSASL */

#ifdef USE_SPNEGO
स्थिर शून्य nego_conn_dtor(अचर शून्य *साँचो, माप_प्रकार klen, शून्य *entry)
{
  संरचना negotiatedata *nego = entry;
  (शून्य)साँचो;
  (शून्य)klen;
  DEBUGASSERT(nego);
  Curl_auth_cleanup_spnego(nego);
  curlx_free(nego);
}

संरचना negotiatedata *Curl_auth_nego_get(संरचना connectdata *conn, bool proxy)
{
  अचर वर्ण *साँचो = proxy ? CURL_META_NEGO_PROXY_CONN : CURL_META_NEGO_CONN;
  संरचना negotiatedata *nego = Curl_conn_meta_get(conn, साँचो);
  यदि(!nego) {
    nego = curlx_calloc(1, परिमाण(*nego));
    यदि(!nego || Curl_conn_meta_set(conn, साँचो, nego, nego_conn_dtor))
      फर्काऊ रिक्त;
  }
  फर्काऊ nego;
}
#endif /* USE_SPNEGO */
