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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "vauth/vauth.h"
#அந்தர்பாவ "creds.h"
#அந்தர்பாவ "curlx/multibyte.h"
#அந்தர்பாவ "url.h"

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
எழுத்து *Curl_auth_build_spn(மாறிலி எழுத்து *service, மாறிலி எழுத்து *host,
                          மாறிலி எழுத்து *realm)
{
  எழுத்து *spn = வெற்று;

  /* Generate our SPN */
  எனில்(host && realm)
    spn = curl_maprintf("%s/%s@%s", service, host, realm);
  இல்லையெனில் எனில்(host)
    spn = curl_maprintf("%s/%s", service, host);
  இல்லையெனில் எனில்(realm)
    spn = curl_maprintf("%s@%s", service, realm);

  /* Return our newly allocated SPN */
  திரும்பு spn;
}
#இல்லையெனில்
TCHAR *Curl_auth_build_spn(மாறிலி எழுத்து *service, மாறிலி எழுத்து *host,
                           மாறிலி எழுத்து *realm)
{
  எழுத்து *utf8_spn = வெற்று;
  TCHAR *tchar_spn = வெற்று;

  (வெறுமை)realm;

  /* Note: We could use DsMakeSPN() or DsClientMakeSpnForTargetServer() rather
     than doing this ourselves but the first is only available in Windows XP
     and Windows Server 2003 and the latter is only available in Windows 2000
     but not Windows95/98/ME or Windows NT4.0 unless the Active Directory
     Client Extensions are installed. As such it is far simpler for us to
     formulate the SPN instead. */

  /* Generate our UTF8 based SPN */
  utf8_spn = curl_maprintf("%s/%s", service, host);
  எனில்(!utf8_spn)
    திரும்பு வெற்று;

  /* Allocate and return a TCHAR based SPN. */
  tchar_spn = curlx_convert_UTF8_to_tchar(utf8_spn);
  curlx_free(utf8_spn);

  திரும்பு tchar_spn;
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
bool Curl_auth_user_contains_domain(கட்டமைப்பு Curl_creds *creds)
{
  bool valid = FALSE;

  எனில்(Curl_creds_has_user(creds)) {
    /* Check we have a domain name or UPN present */
    மாறிலி எழுத்து *p = strpbrk(creds->user, "\\/@");

    valid = p && (p > creds->user) &&
            (p < (creds->user + strlen(creds->user) - 1));
  }
#எனில் defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  இல்லையெனில்
    /* User and domain are obtained from the GSS-API credentials cache or the
       currently logged in user from Windows */
    valid = TRUE;
#endif

  திரும்பு valid;
}

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can be sent to the connection's origin.
 */
bool Curl_auth_allowed_to_host(கட்டமைப்பு Curl_easy *data)
{
  திரும்பு Curl_auth_allowed_to_origin(data, data->state.origin);
}

bool Curl_auth_allowed_to_origin(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு Curl_peer *origin)
{
  திரும்பு data->set.allow_auth_to_other_hosts ||
         Curl_peer_equal(data->state.initial_origin, origin);
}

#ifdef USE_NTLM
நிலையான வெறுமை ntlm_conn_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு ntlmdata *ntlm = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  DEBUGASSERT(ntlm);
  Curl_auth_cleanup_ntlm(ntlm);
  curlx_free(ntlm);
}

கட்டமைப்பு ntlmdata *Curl_auth_ntlm_get(கட்டமைப்பு connectdata *conn, bool proxy)
{
  மாறிலி எழுத்து *சாவி = proxy ? CURL_META_NTLM_PROXY_CONN : CURL_META_NTLM_CONN;
  கட்டமைப்பு ntlmdata *ntlm = Curl_conn_meta_get(conn, சாவி);
  எனில்(!ntlm) {
    ntlm = curlx_calloc(1, அளவு(*ntlm));
    எனில்(!ntlm || Curl_conn_meta_set(conn, சாவி, ntlm, ntlm_conn_dtor))
      திரும்பு வெற்று;
  }
  திரும்பு ntlm;
}

வெறுமை Curl_auth_ntlm_remove(கட்டமைப்பு connectdata *conn, bool proxy)
{
  Curl_conn_meta_remove(conn, proxy ? CURL_META_NTLM_PROXY_CONN
                                    : CURL_META_NTLM_CONN);
}
#endif /* USE_NTLM */

#ifdef USE_KERBEROS5
நிலையான வெறுமை krb5_conn_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு kerberos5data *krb5 = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  DEBUGASSERT(krb5);
  Curl_auth_cleanup_gssapi(krb5);
  curlx_free(krb5);
}

கட்டமைப்பு kerberos5data *Curl_auth_krb5_get(கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு kerberos5data *krb5 = Curl_conn_meta_get(conn, CURL_META_KRB5_CONN);
  எனில்(!krb5) {
    krb5 = curlx_calloc(1, அளவு(*krb5));
    எனில்(!krb5 ||
       Curl_conn_meta_set(conn, CURL_META_KRB5_CONN, krb5, krb5_conn_dtor))
      திரும்பு வெற்று;
  }
  திரும்பு krb5;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
நிலையான வெறுமை gsasl_conn_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு gsasldata *gsasl = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  DEBUGASSERT(gsasl);
  Curl_auth_gsasl_cleanup(gsasl);
  curlx_free(gsasl);
}

கட்டமைப்பு gsasldata *Curl_auth_gsasl_get(கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு gsasldata *gsasl = Curl_conn_meta_get(conn, CURL_META_GSASL_CONN);
  எனில்(!gsasl) {
    gsasl = curlx_calloc(1, அளவு(*gsasl));
    எனில்(!gsasl ||
       Curl_conn_meta_set(conn, CURL_META_GSASL_CONN, gsasl, gsasl_conn_dtor))
      திரும்பு வெற்று;
  }
  திரும்பு gsasl;
}
#endif /* USE_GSASL */

#ifdef USE_SPNEGO
நிலையான வெறுமை nego_conn_dtor(மாறிலி வெறுமை *சாவி, size_t klen, வெறுமை *entry)
{
  கட்டமைப்பு negotiatedata *nego = entry;
  (வெறுமை)சாவி;
  (வெறுமை)klen;
  DEBUGASSERT(nego);
  Curl_auth_cleanup_spnego(nego);
  curlx_free(nego);
}

கட்டமைப்பு negotiatedata *Curl_auth_nego_get(கட்டமைப்பு connectdata *conn, bool proxy)
{
  மாறிலி எழுத்து *சாவி = proxy ? CURL_META_NEGO_PROXY_CONN : CURL_META_NEGO_CONN;
  கட்டமைப்பு negotiatedata *nego = Curl_conn_meta_get(conn, சாவி);
  எனில்(!nego) {
    nego = curlx_calloc(1, அளவு(*nego));
    எனில்(!nego || Curl_conn_meta_set(conn, சாவி, nego, nego_conn_dtor))
      திரும்பு வெற்று;
  }
  திரும்பு nego;
}
#endif /* USE_SPNEGO */
