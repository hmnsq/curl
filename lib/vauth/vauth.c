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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "vauth/vauth.h"
#അന്തര്ഭാവ "creds.h"
#അന്തര്ഭാവ "curlx/multibyte.h"
#അന്തര്ഭാവ "url.h"

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
അക്ഷരം *Curl_auth_build_spn(സ്ഥിരം അക്ഷരം *service, സ്ഥിരം അക്ഷരം *host,
                          സ്ഥിരം അക്ഷരം *realm)
{
  അക്ഷരം *spn = ശൂന്യ;

  /* Generate our SPN */
  എങ്കിൽ(host && realm)
    spn = curl_maprintf("%s/%s@%s", service, host, realm);
  അല്ലെങ്കിൽ എങ്കിൽ(host)
    spn = curl_maprintf("%s/%s", service, host);
  അല്ലെങ്കിൽ എങ്കിൽ(realm)
    spn = curl_maprintf("%s@%s", service, realm);

  /* Return our newly allocated SPN */
  തിരികെ spn;
}
#അല്ലെങ്കിൽ
TCHAR *Curl_auth_build_spn(സ്ഥിരം അക്ഷരം *service, സ്ഥിരം അക്ഷരം *host,
                           സ്ഥിരം അക്ഷരം *realm)
{
  അക്ഷരം *utf8_spn = ശൂന്യ;
  TCHAR *tchar_spn = ശൂന്യ;

  (ശൂന്യം)realm;

  /* Note: We could use DsMakeSPN() or DsClientMakeSpnForTargetServer() rather
     than doing this ourselves but the first is only available in Windows XP
     and Windows Server 2003 and the latter is only available in Windows 2000
     but not Windows95/98/ME or Windows NT4.0 unless the Active Directory
     Client Extensions are installed. As such it is far simpler for us to
     formulate the SPN instead. */

  /* Generate our UTF8 based SPN */
  utf8_spn = curl_maprintf("%s/%s", service, host);
  എങ്കിൽ(!utf8_spn)
    തിരികെ ശൂന്യ;

  /* Allocate and return a TCHAR based SPN. */
  tchar_spn = curlx_convert_UTF8_to_tchar(utf8_spn);
  curlx_free(utf8_spn);

  തിരികെ tchar_spn;
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
bool Curl_auth_user_contains_domain(ഘടന Curl_creds *creds)
{
  bool valid = FALSE;

  എങ്കിൽ(Curl_creds_has_user(creds)) {
    /* Check we have a domain name or UPN present */
    സ്ഥിരം അക്ഷരം *p = strpbrk(creds->user, "\\/@");

    valid = p && (p > creds->user) &&
            (p < (creds->user + strlen(creds->user) - 1));
  }
#എങ്കിൽ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  അല്ലെങ്കിൽ
    /* User and domain are obtained from the GSS-API credentials cache or the
       currently logged in user from Windows */
    valid = TRUE;
#endif

  തിരികെ valid;
}

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can be sent to the connection's origin.
 */
bool Curl_auth_allowed_to_host(ഘടന Curl_easy *data)
{
  തിരികെ Curl_auth_allowed_to_origin(data, data->state.origin);
}

bool Curl_auth_allowed_to_origin(ഘടന Curl_easy *data,
                                 ഘടന Curl_peer *origin)
{
  തിരികെ data->set.allow_auth_to_other_hosts ||
         Curl_peer_equal(data->state.initial_origin, origin);
}

#ifdef USE_NTLM
സ്ഥിതം ശൂന്യം ntlm_conn_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന ntlmdata *ntlm = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  DEBUGASSERT(ntlm);
  Curl_auth_cleanup_ntlm(ntlm);
  curlx_free(ntlm);
}

ഘടന ntlmdata *Curl_auth_ntlm_get(ഘടന connectdata *conn, bool proxy)
{
  സ്ഥിരം അക്ഷരം *താക്കോൽ = proxy ? CURL_META_NTLM_PROXY_CONN : CURL_META_NTLM_CONN;
  ഘടന ntlmdata *ntlm = Curl_conn_meta_get(conn, താക്കോൽ);
  എങ്കിൽ(!ntlm) {
    ntlm = curlx_calloc(1, വലുപ്പം(*ntlm));
    എങ്കിൽ(!ntlm || Curl_conn_meta_set(conn, താക്കോൽ, ntlm, ntlm_conn_dtor))
      തിരികെ ശൂന്യ;
  }
  തിരികെ ntlm;
}

ശൂന്യം Curl_auth_ntlm_remove(ഘടന connectdata *conn, bool proxy)
{
  Curl_conn_meta_remove(conn, proxy ? CURL_META_NTLM_PROXY_CONN
                                    : CURL_META_NTLM_CONN);
}
#endif /* USE_NTLM */

#ifdef USE_KERBEROS5
സ്ഥിതം ശൂന്യം krb5_conn_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന kerberos5data *krb5 = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  DEBUGASSERT(krb5);
  Curl_auth_cleanup_gssapi(krb5);
  curlx_free(krb5);
}

ഘടന kerberos5data *Curl_auth_krb5_get(ഘടന connectdata *conn)
{
  ഘടന kerberos5data *krb5 = Curl_conn_meta_get(conn, CURL_META_KRB5_CONN);
  എങ്കിൽ(!krb5) {
    krb5 = curlx_calloc(1, വലുപ്പം(*krb5));
    എങ്കിൽ(!krb5 ||
       Curl_conn_meta_set(conn, CURL_META_KRB5_CONN, krb5, krb5_conn_dtor))
      തിരികെ ശൂന്യ;
  }
  തിരികെ krb5;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
സ്ഥിതം ശൂന്യം gsasl_conn_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന gsasldata *gsasl = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  DEBUGASSERT(gsasl);
  Curl_auth_gsasl_cleanup(gsasl);
  curlx_free(gsasl);
}

ഘടന gsasldata *Curl_auth_gsasl_get(ഘടന connectdata *conn)
{
  ഘടന gsasldata *gsasl = Curl_conn_meta_get(conn, CURL_META_GSASL_CONN);
  എങ്കിൽ(!gsasl) {
    gsasl = curlx_calloc(1, വലുപ്പം(*gsasl));
    എങ്കിൽ(!gsasl ||
       Curl_conn_meta_set(conn, CURL_META_GSASL_CONN, gsasl, gsasl_conn_dtor))
      തിരികെ ശൂന്യ;
  }
  തിരികെ gsasl;
}
#endif /* USE_GSASL */

#ifdef USE_SPNEGO
സ്ഥിതം ശൂന്യം nego_conn_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന negotiatedata *nego = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  DEBUGASSERT(nego);
  Curl_auth_cleanup_spnego(nego);
  curlx_free(nego);
}

ഘടന negotiatedata *Curl_auth_nego_get(ഘടന connectdata *conn, bool proxy)
{
  സ്ഥിരം അക്ഷരം *താക്കോൽ = proxy ? CURL_META_NEGO_PROXY_CONN : CURL_META_NEGO_CONN;
  ഘടന negotiatedata *nego = Curl_conn_meta_get(conn, താക്കോൽ);
  എങ്കിൽ(!nego) {
    nego = curlx_calloc(1, വലുപ്പം(*nego));
    എങ്കിൽ(!nego || Curl_conn_meta_set(conn, താക്കോൽ, nego, nego_conn_dtor))
      തിരികെ ശൂന്യ;
  }
  തിരികെ nego;
}
#endif /* USE_SPNEGO */
