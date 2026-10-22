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
#ضمن "curl_setup.h"

#ifdef _WIN32

#ضمن "curlx/version_win32.h"

#ifndef CURL_WINDOWS_UWP
/* This Unicode version struct works for VerifyVersionInfoW (OSVERSIONINFOEXW)
   and RtlVerifyVersionInfo (RTLOSVERSIONINFOEXW) */
بنية OUR_OSVERSIONINFOEXW {
  ULONG  dwOSVersionInfoSize;
  ULONG  dwMajorVersion;
  ULONG  dwMinorVersion;
  ULONG  dw_Build_ژمار;
  ULONG  dwPlatformId;
  WCHAR  szCSDVersion[128];
  USHORT wServicePackMajor;
  USHORT wServicePackMinor;
  USHORT wSuiteMask;
  UCHAR  wProductType;
  UCHAR  wReserved;
};

تعريفنوع LONG (APIENTRY *RTLVERIFYVERSIONINFO_FN)
  (بنية OUR_OSVERSIONINFOEXW *, ULONG, ULONGLONG);
ساكن RTLVERIFYVERSIONINFO_FN s_pRtlVerifyVersionInfo;

فراغ curlx_verify_windows_init(فراغ)
{
#إذا defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic پاڵنان
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  s_pRtlVerifyVersionInfo = CURLX_FUNCTION_CAST(RTLVERIFYVERSIONINFO_FN,
    GetProcAddress(GetModuleHandle(TEXT("ntdll")), "RtlVerifyVersionInfo"));
#إذا defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic دەرهێنان
#endif
}
#endif /* !CURL_WINDOWS_UWP */

/*
 * curlx_verify_windows_version()
 *
 * This is used to verify if we are running on a specific Windows version.
 *
 * Parameters:
 *
 * majorVersion [in] - The major version number.
 * minorVersion [in] - The minor version number.
 * buildVersion [in] - The build version number. If 0, this parameter is
 *                     ignored.
 * platform     [in] - The optional platform identifier.
 * condition    [in] - The test condition used to specifier whether we are
 *                     checking a version less than, equal to or greater than
 *                     what is specified in the major and minor version
 *                     numbers.
 *
 * Returns TRUE if matched; otherwise FALSE.
 */
bool curlx_verify_windows_version(ثابت unsigned صحيح majorVersion,
                                  ثابت unsigned صحيح minorVersion,
                                  ثابت unsigned صحيح buildVersion,
                                  ثابت PlatformIdentifier platform,
                                  ثابت VersionCondition condition)
{
  bool matched = FALSE;

#ifdef CURL_WINDOWS_UWP
  /* We have no way to determine the Windows version from Windows apps,
     so let's assume we are running on the target Windows version. */
  ثابت WORD fullVersion = MAKEWORD(minorVersion, majorVersion);
  ثابت WORD targetVersion = (WORD)_WIN32_WINNT;

  (فراغ)buildVersion;

  بدل(condition) {
  حالة VERSION_LESS_THAN:
    matched = targetVersion < fullVersion;
    اكسر;

  حالة VERSION_LESS_THAN_EQUAL:
    matched = targetVersion <= fullVersion;
    اكسر;

  حالة VERSION_EQUAL:
    matched = targetVersion == fullVersion;
    اكسر;

  حالة VERSION_GREATER_THAN_EQUAL:
    matched = targetVersion >= fullVersion;
    اكسر;

  حالة VERSION_GREATER_THAN:
    matched = targetVersion > fullVersion;
    اكسر;
  }

  إذا(matched && (platform == PLATFORM_WINDOWS)) {
    /* we are always running on PLATFORM_WINNT */
    matched = FALSE;
  }
#وإلا
  ULONGLONG cm = 0;
  بنية OUR_OSVERSIONINFOEXW osver;
  BYTE majorCondition;
  BYTE minorCondition;
  BYTE buildCondition;
  BYTE spMajorCondition;
  BYTE spMinorCondition;
  DWORD dwTypeMask = VER_MAJORVERSION | VER_MINORVERSION |
                     VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR;

  بدل(condition) {
  حالة VERSION_LESS_THAN:
    majorCondition = VER_LESS;
    minorCondition = VER_LESS;
    buildCondition = VER_LESS;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    اكسر;

  حالة VERSION_LESS_THAN_EQUAL:
    majorCondition = VER_LESS_EQUAL;
    minorCondition = VER_LESS_EQUAL;
    buildCondition = VER_LESS_EQUAL;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    اكسر;

  حالة VERSION_EQUAL:
    majorCondition = VER_EQUAL;
    minorCondition = VER_EQUAL;
    buildCondition = VER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    اكسر;

  حالة VERSION_GREATER_THAN_EQUAL:
    majorCondition = VER_GREATER_EQUAL;
    minorCondition = VER_GREATER_EQUAL;
    buildCondition = VER_GREATER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    اكسر;

  حالة VERSION_GREATER_THAN:
    majorCondition = VER_GREATER;
    minorCondition = VER_GREATER;
    buildCondition = VER_GREATER;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    اكسر;

  افتراضي:
    أرجع FALSE;
  }

  memset(&osver, 0, حجم(osver));
  osver.dwOSVersionInfoSize = حجم(osver);
  osver.dwMajorVersion = majorVersion;
  osver.dwMinorVersion = minorVersion;
  osver.dw_Build_ژمار = buildVersion;
  إذا(platform == PLATFORM_WINDOWS)
    osver.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
  وإلا إذا(platform == PLATFORM_WINNT)
    osver.dwPlatformId = VER_PLATFORM_WIN32_NT;

  cm = VerSetConditionMask(cm, VER_MAJORVERSION, majorCondition);
  cm = VerSetConditionMask(cm, VER_MINORVERSION, minorCondition);
  cm = VerSetConditionMask(cm, VER_SERVICEPACKMAJOR, spMajorCondition);
  cm = VerSetConditionMask(cm, VER_SERVICEPACKMINOR, spMinorCondition);

  إذا(platform != PLATFORM_DONT_CARE) {
    cm = VerSetConditionMask(cm, VER_PLATFORMID, VER_EQUAL);
    dwTypeMask |= VER_PLATFORMID;
  }

  /* Later versions of Windows have version functions that may not return the
     real version of Windows unless the application is so manifested. We prefer
     the real version always, so we use the Rtl variant of the function when
     possible. Note though the function signatures have underlying fundamental
     types that are the same, the return values are different. */
  إذا(s_pRtlVerifyVersionInfo)
    matched = !s_pRtlVerifyVersionInfo(&osver, dwTypeMask, cm);
  وإلا
    matched = !!VerifyVersionInfoW((OSVERSIONINFOEXW *)&osver, dwTypeMask, cm);

  /* Compare the build number separately. VerifyVersionInfo normally compares
     major.minor in hierarchical order (eg 1.9 is less than 2.0) but does not
     do the same for build (eg 1.9 build 222 is not less than 2.0 build 111).
     Build comparison is only needed when build numbers are equal (eg 1.9 is
     always less than 2.0 so build comparison is not needed). */
  إذا(matched && buildVersion &&
     (condition == VERSION_EQUAL ||
      ((condition == VERSION_GREATER_THAN_EQUAL ||
        condition == VERSION_LESS_THAN_EQUAL) &&
        curlx_verify_windows_version(majorVersion, minorVersion, 0,
                                     platform, VERSION_EQUAL)))) {

    cm = VerSetConditionMask(0, VER_BUILDNUMBER, buildCondition);
    dwTypeMask = VER_BUILDNUMBER;
    إذا(s_pRtlVerifyVersionInfo)
      matched = !s_pRtlVerifyVersionInfo(&osver, dwTypeMask, cm);
    وإلا
      matched = !!VerifyVersionInfoW((OSVERSIONINFOEXW *)&osver,
                                     dwTypeMask, cm);
  }

#endif

  أرجع matched;
}

#endif /* _WIN32 */
