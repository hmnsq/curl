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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "creds.h"
#ಅನ್ತರ್ಭಾವ "curlx/multibyte.h"
#ಅನ್ತರ್ಭಾವ "url.h"

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
ವರ್ಣ *Curl_auth_build_spn(ಧ್ರುವ ವರ್ಣ *service, ಧ್ರುವ ವರ್ಣ *host,
                          ಧ್ರುವ ವರ್ಣ *realm)
{
  ವರ್ಣ *spn = ರಿಕ್ತ;

  /* Generate our SPN */
  ಯದಿ(host && realm)
    spn = curl_maprintf("%s/%s@%s", service, host, realm);
  ಅನ್ಯಥಾ ಯದಿ(host)
    spn = curl_maprintf("%s/%s", service, host);
  ಅನ್ಯಥಾ ಯದಿ(realm)
    spn = curl_maprintf("%s@%s", service, realm);

  /* Return our newly allocated SPN */
  ಪ್ರತ್ಯಾಗಮ spn;
}
#ಅನ್ಯಥಾ
TCHAR *Curl_auth_build_spn(ಧ್ರುವ ವರ್ಣ *service, ಧ್ರುವ ವರ್ಣ *host,
                           ಧ್ರುವ ವರ್ಣ *realm)
{
  ವರ್ಣ *utf8_spn = ರಿಕ್ತ;
  TCHAR *tchar_spn = ರಿಕ್ತ;

  (ಶೂನ್ಯ)realm;

  /* Note: We could use DsMakeSPN() or DsClientMakeSpnForTargetServer() rather
     than doing this ourselves but the first is only available in Windows XP
     and Windows Server 2003 and the latter is only available in Windows 2000
     but not Windows95/98/ME or Windows NT4.0 unless the Active Directory
     Client Extensions are installed. As such it is far simpler for us to
     formulate the SPN instead. */

  /* Generate our UTF8 based SPN */
  utf8_spn = curl_maprintf("%s/%s", service, host);
  ಯದಿ(!utf8_spn)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  /* Allocate and return a TCHAR based SPN. */
  tchar_spn = curlx_convert_UTF8_to_tchar(utf8_spn);
  curlx_free(utf8_spn);

  ಪ್ರತ್ಯಾಗಮ tchar_spn;
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
bool Curl_auth_user_contains_domain(ಸಂರಚನಾ Curl_creds *creds)
{
  bool valid = FALSE;

  ಯದಿ(Curl_creds_has_user(creds)) {
    /* Check we have a domain name or UPN present */
    ಧ್ರುವ ವರ್ಣ *p = strpbrk(creds->user, "\\/@");

    valid = p && (p > creds->user) &&
            (p < (creds->user + strlen(creds->user) - 1));
  }
#ಯದಿ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ಅನ್ಯಥಾ
    /* User and domain are obtained from the GSS-API credentials cache or the
       currently logged in user from Windows */
    valid = TRUE;
#endif

  ಪ್ರತ್ಯಾಗಮ valid;
}

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can be sent to the connection's origin.
 */
bool Curl_auth_allowed_to_host(ಸಂರಚನಾ Curl_easy *data)
{
  ಪ್ರತ್ಯಾಗಮ Curl_auth_allowed_to_origin(data, data->state.origin);
}

bool Curl_auth_allowed_to_origin(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ Curl_peer *origin)
{
  ಪ್ರತ್ಯಾಗಮ data->set.allow_auth_to_other_hosts ||
         Curl_peer_equal(data->state.initial_origin, origin);
}

#ifdef USE_NTLM
ಸ್ಥಿರ ಶೂನ್ಯ ntlm_conn_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ ntlmdata *ntlm = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  DEBUGASSERT(ntlm);
  Curl_auth_cleanup_ntlm(ntlm);
  curlx_free(ntlm);
}

ಸಂರಚನಾ ntlmdata *Curl_auth_ntlm_get(ಸಂರಚನಾ connectdata *conn, bool proxy)
{
  ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ = proxy ? CURL_META_NTLM_PROXY_CONN : CURL_META_NTLM_CONN;
  ಸಂರಚನಾ ntlmdata *ntlm = Curl_conn_meta_get(conn, ಕೀಲಿ);
  ಯದಿ(!ntlm) {
    ntlm = curlx_calloc(1, ಪರಿಮಾಣ(*ntlm));
    ಯದಿ(!ntlm || Curl_conn_meta_set(conn, ಕೀಲಿ, ntlm, ntlm_conn_dtor))
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }
  ಪ್ರತ್ಯಾಗಮ ntlm;
}

ಶೂನ್ಯ Curl_auth_ntlm_remove(ಸಂರಚನಾ connectdata *conn, bool proxy)
{
  Curl_conn_meta_remove(conn, proxy ? CURL_META_NTLM_PROXY_CONN
                                    : CURL_META_NTLM_CONN);
}
#endif /* USE_NTLM */

#ifdef USE_KERBEROS5
ಸ್ಥಿರ ಶೂನ್ಯ krb5_conn_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ kerberos5data *krb5 = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  DEBUGASSERT(krb5);
  Curl_auth_cleanup_gssapi(krb5);
  curlx_free(krb5);
}

ಸಂರಚನಾ kerberos5data *Curl_auth_krb5_get(ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ kerberos5data *krb5 = Curl_conn_meta_get(conn, CURL_META_KRB5_CONN);
  ಯದಿ(!krb5) {
    krb5 = curlx_calloc(1, ಪರಿಮಾಣ(*krb5));
    ಯದಿ(!krb5 ||
       Curl_conn_meta_set(conn, CURL_META_KRB5_CONN, krb5, krb5_conn_dtor))
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }
  ಪ್ರತ್ಯಾಗಮ krb5;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
ಸ್ಥಿರ ಶೂನ್ಯ gsasl_conn_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ gsasldata *gsasl = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  DEBUGASSERT(gsasl);
  Curl_auth_gsasl_cleanup(gsasl);
  curlx_free(gsasl);
}

ಸಂರಚನಾ gsasldata *Curl_auth_gsasl_get(ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ gsasldata *gsasl = Curl_conn_meta_get(conn, CURL_META_GSASL_CONN);
  ಯದಿ(!gsasl) {
    gsasl = curlx_calloc(1, ಪರಿಮಾಣ(*gsasl));
    ಯದಿ(!gsasl ||
       Curl_conn_meta_set(conn, CURL_META_GSASL_CONN, gsasl, gsasl_conn_dtor))
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }
  ಪ್ರತ್ಯಾಗಮ gsasl;
}
#endif /* USE_GSASL */

#ifdef USE_SPNEGO
ಸ್ಥಿರ ಶೂನ್ಯ nego_conn_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ negotiatedata *nego = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  DEBUGASSERT(nego);
  Curl_auth_cleanup_spnego(nego);
  curlx_free(nego);
}

ಸಂರಚನಾ negotiatedata *Curl_auth_nego_get(ಸಂರಚನಾ connectdata *conn, bool proxy)
{
  ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ = proxy ? CURL_META_NEGO_PROXY_CONN : CURL_META_NEGO_CONN;
  ಸಂರಚನಾ negotiatedata *nego = Curl_conn_meta_get(conn, ಕೀಲಿ);
  ಯದಿ(!nego) {
    nego = curlx_calloc(1, ಪರಿಮಾಣ(*nego));
    ಯದಿ(!nego || Curl_conn_meta_set(conn, ಕೀಲಿ, nego, nego_conn_dtor))
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }
  ಪ್ರತ್ಯಾಗಮ nego;
}
#endif /* USE_SPNEGO */
