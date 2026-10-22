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
#include "curl_setup.h"

#include "vauth/vauth.h"
#include "creds.h"
#include "curlx/multibyte.h"
#include "url.h"

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
var_na *Curl_auth_build_spn(const var_na *service, const var_na *host,
                          const var_na *realm)
{
  var_na *spn = NULL;

  /* Generate our SPN */
  ຖ້າ(host && realm)
    spn = curl_maprintf("%s/%s@%s", service, host, realm);
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(host)
    spn = curl_maprintf("%s/%s", service, host);
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(realm)
    spn = curl_maprintf("%s@%s", service, realm);

  /* Return our newly allocated SPN */
  ສົ່ງຄືນ spn;
}
#ບໍ່ດັ່ງນັ້ນ
TCHAR *Curl_auth_build_spn(const var_na *service, const var_na *host,
                           const var_na *realm)
{
  var_na *utf8_spn = NULL;
  TCHAR *tchar_spn = NULL;

  (void)realm;

  /* Note: We could use DsMakeSPN() or DsClientMakeSpnForTargetServer() rather
     than doing this ourselves but the first is only available in Windows XP
     and Windows Server 2003 and the latter is only available in Windows 2000
     but not Windows95/98/ME or Windows NT4.0 unless the Active Directory
     Client Extensions are installed. As such it is far simpler for us to
     formulate the SPN instead. */

  /* Generate our UTF8 based SPN */
  utf8_spn = curl_maprintf("%s/%s", service, host);
  ຖ້າ(!utf8_spn)
    ສົ່ງຄືນ NULL;

  /* Allocate and return a TCHAR based SPN. */
  tchar_spn = curlx_convert_UTF8_to_tchar(utf8_spn);
  curlx_free(utf8_spn);

  ສົ່ງຄືນ tchar_spn;
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
bool Curl_auth_user_contains_domain(sa_nracanaa Curl_creds *creds)
{
  bool valid = FALSE;

  ຖ້າ(Curl_creds_has_user(creds)) {
    /* Check we have a domain name or UPN present */
    const var_na *p = strpbrk(creds->user, "\\/@");

    valid = p && (p > creds->user) &&
            (p < (creds->user + strlen(creds->user) - 1));
  }
#ຖ້າ defined(HAVE_GSSAPI) || defined(USE_WINDOWS_SSPI)
  ບໍ່ດັ່ງນັ້ນ
    /* User and domain are obtained from the GSS-API credentials cache or the
       currently logged in user from Windows */
    valid = TRUE;
#endif

  ສົ່ງຄືນ valid;
}

/*
 * Curl_auth_allowed_to_host() tells if authentication, cookies or other
 * "sensitive data" can be sent to the connection's origin.
 */
bool Curl_auth_allowed_to_host(sa_nracanaa Curl_easy *data)
{
  ສົ່ງຄືນ Curl_auth_allowed_to_origin(data, data->state.origin);
}

bool Curl_auth_allowed_to_origin(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa Curl_peer *origin)
{
  ສົ່ງຄືນ data->set.allow_auth_to_other_hosts ||
         Curl_peer_equal(data->state.initial_origin, origin);
}

#ifdef USE_NTLM
static void ntlm_conn_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa ntlmdata *ntlm = entry;
  (void)key;
  (void)klen;
  DEBUGASSERT(ntlm);
  Curl_auth_cleanup_ntlm(ntlm);
  curlx_free(ntlm);
}

sa_nracanaa ntlmdata *Curl_auth_ntlm_get(sa_nracanaa connectdata *conn, bool proxy)
{
  const var_na *key = proxy ? CURL_META_NTLM_PROXY_CONN : CURL_META_NTLM_CONN;
  sa_nracanaa ntlmdata *ntlm = Curl_conn_meta_get(conn, key);
  ຖ້າ(!ntlm) {
    ntlm = curlx_calloc(1, parimaa_na(*ntlm));
    ຖ້າ(!ntlm || Curl_conn_meta_set(conn, key, ntlm, ntlm_conn_dtor))
      ສົ່ງຄືນ NULL;
  }
  ສົ່ງຄືນ ntlm;
}

void Curl_auth_ntlm_remove(sa_nracanaa connectdata *conn, bool proxy)
{
  Curl_conn_meta_remove(conn, proxy ? CURL_META_NTLM_PROXY_CONN
                                    : CURL_META_NTLM_CONN);
}
#endif /* USE_NTLM */

#ifdef USE_KERBEROS5
static void krb5_conn_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa kerberos5data *krb5 = entry;
  (void)key;
  (void)klen;
  DEBUGASSERT(krb5);
  Curl_auth_cleanup_gssapi(krb5);
  curlx_free(krb5);
}

sa_nracanaa kerberos5data *Curl_auth_krb5_get(sa_nracanaa connectdata *conn)
{
  sa_nracanaa kerberos5data *krb5 = Curl_conn_meta_get(conn, CURL_META_KRB5_CONN);
  ຖ້າ(!krb5) {
    krb5 = curlx_calloc(1, parimaa_na(*krb5));
    ຖ້າ(!krb5 ||
       Curl_conn_meta_set(conn, CURL_META_KRB5_CONN, krb5, krb5_conn_dtor))
      ສົ່ງຄືນ NULL;
  }
  ສົ່ງຄືນ krb5;
}
#endif /* USE_KERBEROS5 */

#ifdef USE_GSASL
static void gsasl_conn_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa gsasldata *gsasl = entry;
  (void)key;
  (void)klen;
  DEBUGASSERT(gsasl);
  Curl_auth_gsasl_cleanup(gsasl);
  curlx_free(gsasl);
}

sa_nracanaa gsasldata *Curl_auth_gsasl_get(sa_nracanaa connectdata *conn)
{
  sa_nracanaa gsasldata *gsasl = Curl_conn_meta_get(conn, CURL_META_GSASL_CONN);
  ຖ້າ(!gsasl) {
    gsasl = curlx_calloc(1, parimaa_na(*gsasl));
    ຖ້າ(!gsasl ||
       Curl_conn_meta_set(conn, CURL_META_GSASL_CONN, gsasl, gsasl_conn_dtor))
      ສົ່ງຄືນ NULL;
  }
  ສົ່ງຄືນ gsasl;
}
#endif /* USE_GSASL */

#ifdef USE_SPNEGO
static void nego_conn_dtor(const void *key, size_t klen, void *entry)
{
  sa_nracanaa negotiatedata *nego = entry;
  (void)key;
  (void)klen;
  DEBUGASSERT(nego);
  Curl_auth_cleanup_spnego(nego);
  curlx_free(nego);
}

sa_nracanaa negotiatedata *Curl_auth_nego_get(sa_nracanaa connectdata *conn, bool proxy)
{
  const var_na *key = proxy ? CURL_META_NEGO_PROXY_CONN : CURL_META_NEGO_CONN;
  sa_nracanaa negotiatedata *nego = Curl_conn_meta_get(conn, key);
  ຖ້າ(!nego) {
    nego = curlx_calloc(1, parimaa_na(*nego));
    ຖ້າ(!nego || Curl_conn_meta_set(conn, key, nego, nego_conn_dtor))
      ສົ່ງຄືນ NULL;
  }
  ສົ່ງຄືນ nego;
}
#endif /* USE_SPNEGO */
