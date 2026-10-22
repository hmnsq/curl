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
#समावेश "curl_setup.h"

#समावेश "vauth/vauth.h"
#समावेश "creds.h"
#समावेश "curlx/multibyte.h"
#समावेश "url.h"

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
#अगर_अघोषित USE_WINDOWS_SSPI
अक्षर *Curl_auth_build_spn(स्थिर अक्षर *service, स्थिर अक्षर *host,
                          स्थिर अक्षर *realm)
{
  अक्षर *spn = शून्य;

  /* Generate our SPN */
  अगर(host && realm)
    spn = curl_maprintf("%s/%s@%s", service, host, realm);
  अन्यथा अगर(host)
    spn = curl_maprintf("%s/%s", service, host);
  अन्यथा अगर(realm)
    spn = curl_maprintf("%s@%s", service, realm);

  /* Return our newly allocated SPN */
  वापस spn;
}
#अन्यथा
TCHAR *Curl_auth_build_spn(स्थिर अक्षर *service, स्थिर अक्षर *host,
                           स्थिर अक्षर *realm)
{
  अक्षर *utf8_spn = शून्य;
  TCHAR *tchar_spn = शून्य;

  (व्योम)realm;

  /* Note: We could use DsMakeSPN() or DsClientMakeSpnForTargetServer() rather
     than doing this ourselves but the first is only available in Windows XP
     and Windows Server 2003 and the latter is only available in Windows 2000
     but not Windows95/98/ME or Windows NT4.0 unless the Active Directory
     Client Extensions are installed. As such it is far simpler for us to
     formulate the SPN instead. */

  /* Generate our UTF8 based SPN */
  utf8_spn = curl_maprintf("%s/%s", service, host);
  अगर(!utf8_spn)
    वापस शून्य;

  /* Allocate and return a TCHAR based SPN. */
  tchar_spn = curlx_convert_UTF8_to_tchar(utf8_spn);
  curlx_free(utf8_spn);

  वापस tchar_spn;
}
#पूर्ण_अगर /* USE_WINDOWS_SSPI */

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
bool Curl_auth_user_contains_domain(काष्ठा Curl_creds *creds)
{
  bool valid = FALSE;

  अगर(Curl_creds_has_user(creds)) {
    /* Check we have a domain name or UPN present */
    स्थिर अक्षर *p = strpbrk(creds->user, "\\/@");

    valid = p && (p > creds->user) &&
            (p < (creds->user + म_माप(creds->user) - 1));
  }
#अगर defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  अन्यथा
    /* User and domain are obtained from the GSS-API credentials cache or the
       currently logged in user from Windows */
    valid = TRUE;
#पूर्ण_अगर

  वापस valid;
}

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can be sent to the connection's origin.
 */
bool Curl_auth_allowed_to_host(काष्ठा Curl_easy *तथ्य)
{
  वापस Curl_auth_allowed_to_origin(तथ्य, तथ्य->state.origin);
}

bool Curl_auth_allowed_to_origin(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा Curl_peer *origin)
{
  वापस तथ्य->set.allow_auth_to_other_hosts ||
         Curl_peer_equal(तथ्य->state.initial_origin, origin);
}

#अगर_घोषित USE_NTLM
अटल व्योम ntlm_conn_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा ntlmdata *ntlm = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  DEBUGASSERT(ntlm);
  Curl_auth_cleanup_ntlm(ntlm);
  curlx_free(ntlm);
}

काष्ठा ntlmdata *Curl_auth_ntlm_get(काष्ठा connectdata *conn, bool proxy)
{
  स्थिर अक्षर *कुंजी = proxy ? CURL_META_NTLM_PROXY_CONN : CURL_META_NTLM_CONN;
  काष्ठा ntlmdata *ntlm = Curl_conn_meta_get(conn, कुंजी);
  अगर(!ntlm) {
    ntlm = curlx_calloc(1, माप(*ntlm));
    अगर(!ntlm || Curl_conn_meta_set(conn, कुंजी, ntlm, ntlm_conn_dtor))
      वापस शून्य;
  }
  वापस ntlm;
}

व्योम Curl_auth_ntlm_remove(काष्ठा connectdata *conn, bool proxy)
{
  Curl_conn_meta_remove(conn, proxy ? CURL_META_NTLM_PROXY_CONN
                                    : CURL_META_NTLM_CONN);
}
#पूर्ण_अगर /* USE_NTLM */

#अगर_घोषित USE_KERBEROS5
अटल व्योम krb5_conn_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा kerberos5data *krb5 = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  DEBUGASSERT(krb5);
  Curl_auth_cleanup_gssapi(krb5);
  curlx_free(krb5);
}

काष्ठा kerberos5data *Curl_auth_krb5_get(काष्ठा connectdata *conn)
{
  काष्ठा kerberos5data *krb5 = Curl_conn_meta_get(conn, CURL_META_KRB5_CONN);
  अगर(!krb5) {
    krb5 = curlx_calloc(1, माप(*krb5));
    अगर(!krb5 ||
       Curl_conn_meta_set(conn, CURL_META_KRB5_CONN, krb5, krb5_conn_dtor))
      वापस शून्य;
  }
  वापस krb5;
}
#पूर्ण_अगर /* USE_KERBEROS5 */

#अगर_घोषित USE_GSASL
अटल व्योम gsasl_conn_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा gsasldata *gsasl = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  DEBUGASSERT(gsasl);
  Curl_auth_gsasl_cleanup(gsasl);
  curlx_free(gsasl);
}

काष्ठा gsasldata *Curl_auth_gsasl_get(काष्ठा connectdata *conn)
{
  काष्ठा gsasldata *gsasl = Curl_conn_meta_get(conn, CURL_META_GSASL_CONN);
  अगर(!gsasl) {
    gsasl = curlx_calloc(1, माप(*gsasl));
    अगर(!gsasl ||
       Curl_conn_meta_set(conn, CURL_META_GSASL_CONN, gsasl, gsasl_conn_dtor))
      वापस शून्य;
  }
  वापस gsasl;
}
#पूर्ण_अगर /* USE_GSASL */

#अगर_घोषित USE_SPNEGO
अटल व्योम nego_conn_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा negotiatedata *nego = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  DEBUGASSERT(nego);
  Curl_auth_cleanup_spnego(nego);
  curlx_free(nego);
}

काष्ठा negotiatedata *Curl_auth_nego_get(काष्ठा connectdata *conn, bool proxy)
{
  स्थिर अक्षर *कुंजी = proxy ? CURL_META_NEGO_PROXY_CONN : CURL_META_NEGO_CONN;
  काष्ठा negotiatedata *nego = Curl_conn_meta_get(conn, कुंजी);
  अगर(!nego) {
    nego = curlx_calloc(1, माप(*nego));
    अगर(!nego || Curl_conn_meta_set(conn, कुंजी, nego, nego_conn_dtor))
      वापस शून्य;
  }
  वापस nego;
}
#पूर्ण_अगर /* USE_SPNEGO */
