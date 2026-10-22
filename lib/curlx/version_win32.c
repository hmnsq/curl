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
#অন্তর্ভাব "curl_setup.h"

#ifdef _WIN32

#অন্তর্ভাব "curlx/version_win32.h"

#ifndef CURL_WINDOWS_UWP
/* This Unicode version struct works for VerifyVersionInfoW (OSVERSIONINFOEXW)
   and RtlVerifyVersionInfo (RTLOSVERSIONINFOEXW) */
সংরচনা OUR_OSVERSIONINFOEXW {
  ULONG  dwOSVersionInfoSize;
  ULONG  dwMajorVersion;
  ULONG  dwMinorVersion;
  ULONG  dw_Build_সংখ্যা;
  ULONG  dwPlatformId;
  WCHAR  szCSDVersion[128];
  USHORT wServicePackMajor;
  USHORT wServicePackMinor;
  USHORT wSuiteMask;
  UCHAR  wProductType;
  UCHAR  wReserved;
};

প্রকারনির্দেশ LONG (APIENTRY *RTLVERIFYVERSIONINFO_FN)
  (সংরচনা OUR_OSVERSIONINFOEXW *, ULONG, ULONGLONG);
স্থির RTLVERIFYVERSIONINFO_FN s_pRtlVerifyVersionInfo;

শূন্য curlx_verify_windows_init(শূন্য)
{
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  s_pRtlVerifyVersionInfo = CURLX_FUNCTION_CAST(RTLVERIFYVERSIONINFO_FN,
    GetProcAddress(GetModuleHandle(TEXT("ntdll")), "RtlVerifyVersionInfo"));
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic বের_করো
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
bool curlx_verify_windows_version(ধ্রুব unsigned পূর্ণাঙ্ক majorVersion,
                                  ধ্রুব unsigned পূর্ণাঙ্ক minorVersion,
                                  ধ্রুব unsigned পূর্ণাঙ্ক buildVersion,
                                  ধ্রুব PlatformIdentifier platform,
                                  ধ্রুব VersionCondition condition)
{
  bool matched = FALSE;

#ifdef CURL_WINDOWS_UWP
  /* We have no way to determine the Windows version from Windows apps,
     so let's assume we are running on the target Windows version. */
  ধ্রুব WORD fullVersion = MAKEWORD(minorVersion, majorVersion);
  ধ্রুব WORD targetVersion = (WORD)_WIN32_WINNT;

  (শূন্য)buildVersion;

  বিকল্প(condition) {
  পক্ষ VERSION_LESS_THAN:
    matched = targetVersion < fullVersion;
    ভঙ্গ;

  পক্ষ VERSION_LESS_THAN_EQUAL:
    matched = targetVersion <= fullVersion;
    ভঙ্গ;

  পক্ষ VERSION_EQUAL:
    matched = targetVersion == fullVersion;
    ভঙ্গ;

  পক্ষ VERSION_GREATER_THAN_EQUAL:
    matched = targetVersion >= fullVersion;
    ভঙ্গ;

  পক্ষ VERSION_GREATER_THAN:
    matched = targetVersion > fullVersion;
    ভঙ্গ;
  }

  যদি(matched && (platform == PLATFORM_WINDOWS)) {
    /* we are always running on PLATFORM_WINNT */
    matched = FALSE;
  }
#অন্যথা
  ULONGLONG cm = 0;
  সংরচনা OUR_OSVERSIONINFOEXW osver;
  BYTE majorCondition;
  BYTE minorCondition;
  BYTE buildCondition;
  BYTE spMajorCondition;
  BYTE spMinorCondition;
  DWORD dwTypeMask = VER_MAJORVERSION | VER_MINORVERSION |
                     VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR;

  বিকল্প(condition) {
  পক্ষ VERSION_LESS_THAN:
    majorCondition = VER_LESS;
    minorCondition = VER_LESS;
    buildCondition = VER_LESS;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    ভঙ্গ;

  পক্ষ VERSION_LESS_THAN_EQUAL:
    majorCondition = VER_LESS_EQUAL;
    minorCondition = VER_LESS_EQUAL;
    buildCondition = VER_LESS_EQUAL;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    ভঙ্গ;

  পক্ষ VERSION_EQUAL:
    majorCondition = VER_EQUAL;
    minorCondition = VER_EQUAL;
    buildCondition = VER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    ভঙ্গ;

  পক্ষ VERSION_GREATER_THAN_EQUAL:
    majorCondition = VER_GREATER_EQUAL;
    minorCondition = VER_GREATER_EQUAL;
    buildCondition = VER_GREATER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    ভঙ্গ;

  পক্ষ VERSION_GREATER_THAN:
    majorCondition = VER_GREATER;
    minorCondition = VER_GREATER;
    buildCondition = VER_GREATER;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    ভঙ্গ;

  স্বভাব:
    প্রত্যাগম FALSE;
  }

  memset(&osver, 0, পরিমাণ(osver));
  osver.dwOSVersionInfoSize = পরিমাণ(osver);
  osver.dwMajorVersion = majorVersion;
  osver.dwMinorVersion = minorVersion;
  osver.dw_Build_সংখ্যা = buildVersion;
  যদি(platform == PLATFORM_WINDOWS)
    osver.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
  অন্যথা যদি(platform == PLATFORM_WINNT)
    osver.dwPlatformId = VER_PLATFORM_WIN32_NT;

  cm = VerSetConditionMask(cm, VER_MAJORVERSION, majorCondition);
  cm = VerSetConditionMask(cm, VER_MINORVERSION, minorCondition);
  cm = VerSetConditionMask(cm, VER_SERVICEPACKMAJOR, spMajorCondition);
  cm = VerSetConditionMask(cm, VER_SERVICEPACKMINOR, spMinorCondition);

  যদি(platform != PLATFORM_DONT_CARE) {
    cm = VerSetConditionMask(cm, VER_PLATFORMID, VER_EQUAL);
    dwTypeMask |= VER_PLATFORMID;
  }

  /* Later versions of Windows have version functions that may not return the
     real version of Windows unless the application is so manifested. We prefer
     the real version always, so we use the Rtl variant of the function when
     possible. Note though the function signatures have underlying fundamental
     types that are the same, the return values are different. */
  যদি(s_pRtlVerifyVersionInfo)
    matched = !s_pRtlVerifyVersionInfo(&osver, dwTypeMask, cm);
  অন্যথা
    matched = !!VerifyVersionInfoW((OSVERSIONINFOEXW *)&osver, dwTypeMask, cm);

  /* Compare the build number separately. VerifyVersionInfo normally compares
     major.minor in hierarchical order (eg 1.9 is less than 2.0) but does not
     do the same for build (eg 1.9 build 222 is not less than 2.0 build 111).
     Build comparison is only needed when build numbers are equal (eg 1.9 is
     always less than 2.0 so build comparison is not needed). */
  যদি(matched && buildVersion &&
     (condition == VERSION_EQUAL ||
      ((condition == VERSION_GREATER_THAN_EQUAL ||
        condition == VERSION_LESS_THAN_EQUAL) &&
        curlx_verify_windows_version(majorVersion, minorVersion, 0,
                                     platform, VERSION_EQUAL)))) {

    cm = VerSetConditionMask(0, VER_BUILDNUMBER, buildCondition);
    dwTypeMask = VER_BUILDNUMBER;
    যদি(s_pRtlVerifyVersionInfo)
      matched = !s_pRtlVerifyVersionInfo(&osver, dwTypeMask, cm);
    অন্যথা
      matched = !!VerifyVersionInfoW((OSVERSIONINFOEXW *)&osver,
                                     dwTypeMask, cm);
  }

#endif

  প্রত্যাগম matched;
}

#endif /* _WIN32 */
