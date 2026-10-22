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
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_WINDOWS_SSPI
ಅಕ್ಷರ *Curl_auth_build_spn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *realm)
{
  ಅಕ್ಷರ *spn = ಖಾಲಿ;

  /* Generate our SPN */
  ಆದರೆ(host && realm)
    spn = curl_maprintf("%s/%s@%s", service, host, realm);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(host)
    spn = curl_maprintf("%s/%s", service, host);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(realm)
    spn = curl_maprintf("%s@%s", service, realm);

  /* Return our newly allocated SPN */
  ಹಿಂತಿರುಗಿ spn;
}
#ಇಲ್ಲದಿದ್ದರೆ
TCHAR *Curl_auth_build_spn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *service, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *realm)
{
  ಅಕ್ಷರ *utf8_spn = ಖಾಲಿ;
  TCHAR *tchar_spn = ಖಾಲಿ;

  (ಶೂನ್ಯ)realm;

  /* Note: We could use DsMakeSPN() or DsClientMakeSpnForTargetServer() rather
     than doing this ourselves but the first is only available in Windows XP
     and Windows Server 2003 and the latter is only available in Windows 2000
     but not Windows95/98/ME or Windows NT4.0 unless the Active Directory
     Client Extensions are installed. As such it is far simpler for us to
     formulate the SPN instead. */

  /* Generate our UTF8 based SPN */
  utf8_spn = curl_maprintf("%s/%s", service, host);
  ಆದರೆ(!utf8_spn)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  /* Allocate and return a TCHAR based SPN. */
  tchar_spn = curlx_convert_UTF8_ಗಮ್ಯ_tchar(utf8_spn);
  curlx_free(utf8_spn);

  ಹಿಂತಿರುಗಿ tchar_spn;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_WINDOWS_SSPI */

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
bool Curl_auth_user_ಒಳಗೊಂಡಿದೆ_domain(ರಚನೆ Curl_creds *creds)
{
  bool valid = FALSE;

  ಆದರೆ(Curl_creds_has_user(creds)) {
    /* Check we have a domain name or UPN present */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = strpbrk(creds->user, "\\/@");

    valid = p && (p > creds->user) &&
            (p < (creds->user + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(creds->user) - 1));
  }
#ಆದರೆ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ಇಲ್ಲದಿದ್ದರೆ
    /* User and domain are obtained from the GSS-API credentials cache or the
       currently logged in user from Windows */
    valid = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ valid;
}

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can be sent to the connection's origin.
 */
bool Curl_auth_allowed_ಗಮ್ಯ_host(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ Curl_auth_allowed_ಗಮ್ಯ_origin(ದತ್ತಾಂಶ, ದತ್ತಾಂಶ->state.origin);
}

bool Curl_auth_allowed_ಗಮ್ಯ_origin(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ Curl_peer *origin)
{
  ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ->ಗಣ.allow_auth_ಗಮ್ಯ_other_hosts ||
         Curl_peer_equal(ದತ್ತಾಂಶ->state.initial_origin, origin);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_NTLM
ಸ್ಥಿರ ಶೂನ್ಯ ntlm_conn_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ ntlmdata *ntlm = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  DEBUGASSERT(ntlm);
  Curl_auth_cleanup_ntlm(ntlm);
  curlx_free(ntlm);
}

ರಚನೆ ntlmdata *Curl_auth_ntlm_get(ರಚನೆ connectdata *conn, bool proxy)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ = proxy ? CURL_META_NTLM_PROXY_CONN : CURL_META_NTLM_CONN;
  ರಚನೆ ntlmdata *ntlm = Curl_conn_meta_get(conn, ಕೀಲಿ);
  ಆದರೆ(!ntlm) {
    ntlm = curlx_calloc(1, ಗಾತ್ರ(*ntlm));
    ಆದರೆ(!ntlm || Curl_conn_meta_ಗಣ(conn, ಕೀಲಿ, ntlm, ntlm_conn_dtor))
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }
  ಹಿಂತಿರುಗಿ ntlm;
}

ಶೂನ್ಯ Curl_auth_ntlm_remove(ರಚನೆ connectdata *conn, bool proxy)
{
  Curl_conn_meta_remove(conn, proxy ? CURL_META_NTLM_PROXY_CONN
                                    : CURL_META_NTLM_CONN);
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_NTLM */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_KERBEROS5
ಸ್ಥಿರ ಶೂನ್ಯ krb5_conn_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ kerberos5data *krb5 = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  DEBUGASSERT(krb5);
  Curl_auth_cleanup_gssapi(krb5);
  curlx_free(krb5);
}

ರಚನೆ kerberos5data *Curl_auth_krb5_get(ರಚನೆ connectdata *conn)
{
  ರಚನೆ kerberos5data *krb5 = Curl_conn_meta_get(conn, CURL_META_KRB5_CONN);
  ಆದರೆ(!krb5) {
    krb5 = curlx_calloc(1, ಗಾತ್ರ(*krb5));
    ಆದರೆ(!krb5 ||
       Curl_conn_meta_ಗಣ(conn, CURL_META_KRB5_CONN, krb5, krb5_conn_dtor))
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }
  ಹಿಂತಿರುಗಿ krb5;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_KERBEROS5 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GSASL
ಸ್ಥಿರ ಶೂನ್ಯ gsasl_conn_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ gsasldata *gsasl = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  DEBUGASSERT(gsasl);
  Curl_auth_gsasl_cleanup(gsasl);
  curlx_free(gsasl);
}

ರಚನೆ gsasldata *Curl_auth_gsasl_get(ರಚನೆ connectdata *conn)
{
  ರಚನೆ gsasldata *gsasl = Curl_conn_meta_get(conn, CURL_META_GSASL_CONN);
  ಆದರೆ(!gsasl) {
    gsasl = curlx_calloc(1, ಗಾತ್ರ(*gsasl));
    ಆದರೆ(!gsasl ||
       Curl_conn_meta_ಗಣ(conn, CURL_META_GSASL_CONN, gsasl, gsasl_conn_dtor))
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }
  ಹಿಂತಿರುಗಿ gsasl;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_GSASL */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SPNEGO
ಸ್ಥಿರ ಶೂನ್ಯ nego_conn_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ negotiatedata *nego = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  DEBUGASSERT(nego);
  Curl_auth_cleanup_spnego(nego);
  curlx_free(nego);
}

ರಚನೆ negotiatedata *Curl_auth_nego_get(ರಚನೆ connectdata *conn, bool proxy)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ = proxy ? CURL_META_NEGO_PROXY_CONN : CURL_META_NEGO_CONN;
  ರಚನೆ negotiatedata *nego = Curl_conn_meta_get(conn, ಕೀಲಿ);
  ಆದರೆ(!nego) {
    nego = curlx_calloc(1, ಗಾತ್ರ(*nego));
    ಆದರೆ(!nego || Curl_conn_meta_ಗಣ(conn, ಕೀಲಿ, nego, nego_conn_dtor))
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }
  ಹಿಂತಿರುಗಿ nego;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_SPNEGO */
