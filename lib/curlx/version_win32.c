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

#यदि_परिभाषितम् _WIN32

#अन्तर्भाव "curlx/version_win32.h"

#यदि_अपरिभाषितम् CURL_WINDOWS_UWP
/* This Unicode version struct works for VerifyVersionInfoW (OSVERSIONINFOEXW)
   and RtlVerifyVersionInfo (RTLOSVERSIONINFOEXW) */
संरचना OUR_OSVERSIONINFOEXW {
  ULONG  dw_OSVersion_Info_आकार;
  ULONG  dw_Major_संस्करणम्;
  ULONG  dw_Minor_संस्करणम्;
  ULONG  dw_Build_सङ्ख्या;
  ULONG  dwPlatformId;
  WCHAR  szCSDVersion[128];
  USHORT wServicePackMajor;
  USHORT wServicePackMinor;
  USHORT w_Suite_आवरणम्;
  UCHAR  w_गुणनफलम्_Type;
  UCHAR  wReserved;
};

प्रकारनिर्देश LONG (APIENTRY *RTLVERIFYVERSIONINFO_FN)
  (संरचना OUR_OSVERSIONINFOEXW *, ULONG, ULONGLONG);
स्थिर RTLVERIFYVERSIONINFO_FN s_p_Rtl_Verify_संस्करणम्_Info;

शून्य curlx_verify_windows_init(शून्य)
{
#यदि defined(__clang__) && __clang_major__ >= 16
#निर्देशः clang diagnostic प्रक्षिप
#निर्देशः clang diagnostic ignored "-Wcast-function-type-strict"
#यदि_अन्तः
  s_p_Rtl_Verify_संस्करणम्_Info = CURLX_FUNCTION_CAST(RTLVERIFYVERSIONINFO_FN,
    Get_Proc_स्थानाङ्कः(GetModuleHandle(TEXT("ntdll")), "RtlVerifyVersionInfo"));
#यदि defined(__clang__) && __clang_major__ >= 16
#निर्देशः clang diagnostic निष्कासय
#यदि_अन्तः
}
#यदि_अन्तः /* !CURL_WINDOWS_UWP */

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
bool curlx_verify_windows_संस्करणम्(ध्रुव अचिह्नित पूर्णाङ्क major_संस्करणम्,
                                  ध्रुव अचिह्नित पूर्णाङ्क minor_संस्करणम्,
                                  ध्रुव अचिह्नित पूर्णाङ्क build_संस्करणम्,
                                  ध्रुव PlatformIdentifier platform,
                                  ध्रुव संस्करणम्_Condition condition)
{
  bool matched = FALSE;

#यदि_परिभाषितम् CURL_WINDOWS_UWP
  /* We have no way to determine the Windows version from Windows apps,
     so let's assume we are running on the target Windows version. */
  ध्रुव WORD पूर्णम्_संस्करणम् = MAKEWORD(minor_संस्करणम्, major_संस्करणम्);
  ध्रुव WORD target_संस्करणम् = (WORD)_WIN32_WINNT;

  (शून्य)build_संस्करणम्;

  विकल्प(condition) {
  पक्ष संस्करणम्_LESS_THAN:
    matched = target_संस्करणम् < पूर्णम्_संस्करणम्;
    भङ्ग;

  पक्ष संस्करणम्_LESS_THAN_EQUAL:
    matched = target_संस्करणम् <= पूर्णम्_संस्करणम्;
    भङ्ग;

  पक्ष संस्करणम्_EQUAL:
    matched = target_संस्करणम् == पूर्णम्_संस्करणम्;
    भङ्ग;

  पक्ष संस्करणम्_GREATER_THAN_EQUAL:
    matched = target_संस्करणम् >= पूर्णम्_संस्करणम्;
    भङ्ग;

  पक्ष संस्करणम्_GREATER_THAN:
    matched = target_संस्करणम् > पूर्णम्_संस्करणम्;
    भङ्ग;
  }

  यदि(matched && (platform == PLATFORM_WINDOWS)) {
    /* we are always running on PLATFORM_WINNT */
    matched = FALSE;
  }
#अन्यथा
  ULONGLONG cm = 0;
  संरचना OUR_OSVERSIONINFOEXW osver;
  BYTE majorCondition;
  BYTE minorCondition;
  BYTE buildCondition;
  BYTE spMajorCondition;
  BYTE spMinorCondition;
  DWORD dw_Type_आवरणम् = VER_MAJORVERSION | VER_MINORVERSION |
                     VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR;

  विकल्प(condition) {
  पक्ष संस्करणम्_LESS_THAN:
    majorCondition = VER_LESS;
    minorCondition = VER_LESS;
    buildCondition = VER_LESS;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    भङ्ग;

  पक्ष संस्करणम्_LESS_THAN_EQUAL:
    majorCondition = VER_LESS_EQUAL;
    minorCondition = VER_LESS_EQUAL;
    buildCondition = VER_LESS_EQUAL;
    spMajorCondition = VER_LESS_EQUAL;
    spMinorCondition = VER_LESS_EQUAL;
    भङ्ग;

  पक्ष संस्करणम्_EQUAL:
    majorCondition = VER_EQUAL;
    minorCondition = VER_EQUAL;
    buildCondition = VER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    भङ्ग;

  पक्ष संस्करणम्_GREATER_THAN_EQUAL:
    majorCondition = VER_GREATER_EQUAL;
    minorCondition = VER_GREATER_EQUAL;
    buildCondition = VER_GREATER_EQUAL;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    भङ्ग;

  पक्ष संस्करणम्_GREATER_THAN:
    majorCondition = VER_GREATER;
    minorCondition = VER_GREATER;
    buildCondition = VER_GREATER;
    spMajorCondition = VER_GREATER_EQUAL;
    spMinorCondition = VER_GREATER_EQUAL;
    भङ्ग;

  स्वभाव:
    प्रत्यागम FALSE;
  }

  स्मृतिं_पूरय(&osver, 0, परिमाण(osver));
  osver.dw_OSVersion_Info_आकार = परिमाण(osver);
  osver.dw_Major_संस्करणम् = major_संस्करणम्;
  osver.dw_Minor_संस्करणम् = minor_संस्करणम्;
  osver.dw_Build_सङ्ख्या = build_संस्करणम्;
  यदि(platform == PLATFORM_WINDOWS)
    osver.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS;
  अन्यथा यदि(platform == PLATFORM_WINNT)
    osver.dwPlatformId = VER_PLATFORM_WIN32_NT;

  cm = Ver_समुच्चयः_Condition_आवरणम्(cm, VER_MAJORVERSION, majorCondition);
  cm = Ver_समुच्चयः_Condition_आवरणम्(cm, VER_MINORVERSION, minorCondition);
  cm = Ver_समुच्चयः_Condition_आवरणम्(cm, VER_SERVICEPACKMAJOR, spMajorCondition);
  cm = Ver_समुच्चयः_Condition_आवरणम्(cm, VER_SERVICEPACKMINOR, spMinorCondition);

  यदि(platform != PLATFORM_DONT_CARE) {
    cm = Ver_समुच्चयः_Condition_आवरणम्(cm, VER_PLATFORMID, VER_EQUAL);
    dw_Type_आवरणम् |= VER_PLATFORMID;
  }

  /* Later versions of Windows have version functions that may not return the
     real version of Windows unless the application is so manifested. We prefer
     the real version always, so we use the Rtl variant of the function when
     possible. Note though the function signatures have underlying fundamental
     types that are the same, the return values are different. */
  यदि(s_p_Rtl_Verify_संस्करणम्_Info)
    matched = !s_p_Rtl_Verify_संस्करणम्_Info(&osver, dw_Type_आवरणम्, cm);
  अन्यथा
    matched = !!Verify_संस्करणम्_Info_W((OSVERSIONINFOEXW *)&osver, dw_Type_आवरणम्, cm);

  /* Compare the build number separately. VerifyVersionInfo normally compares
     major.minor in hierarchical order (eg 1.9 is less than 2.0) but does not
     do the same for build (eg 1.9 build 222 is not less than 2.0 build 111).
     Build comparison is only needed when build numbers are equal (eg 1.9 is
     always less than 2.0 so build comparison is not needed). */
  यदि(matched && build_संस्करणम् &&
     (condition == संस्करणम्_EQUAL ||
      ((condition == संस्करणम्_GREATER_THAN_EQUAL ||
        condition == संस्करणम्_LESS_THAN_EQUAL) &&
        curlx_verify_windows_संस्करणम्(major_संस्करणम्, minor_संस्करणम्, 0,
                                     platform, संस्करणम्_EQUAL)))) {

    cm = Ver_समुच्चयः_Condition_आवरणम्(0, VER_BUILDNUMBER, buildCondition);
    dw_Type_आवरणम् = VER_BUILDNUMBER;
    यदि(s_p_Rtl_Verify_संस्करणम्_Info)
      matched = !s_p_Rtl_Verify_संस्करणम्_Info(&osver, dw_Type_आवरणम्, cm);
    अन्यथा
      matched = !!Verify_संस्करणम्_Info_W((OSVERSIONINFOEXW *)&osver,
                                     dw_Type_आवरणम्, cm);
  }

#यदि_अन्तः

  प्रत्यागम matched;
}

#यदि_अन्तः /* _WIN32 */
