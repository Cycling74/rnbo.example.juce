# On Windows, the WEBVIEW editor needs the Microsoft WebView2 SDK. JUCE looks for it in the local
# NuGet package folder, or in JUCE_WEBVIEW2_PACKAGE_LOCATION if that is set. If neither has it, we
# download the NuGet package into the build tree and point JUCE at it.
#
# To use your own copy (e.g. for offline builds), configure with
#   cmake -DJUCE_WEBVIEW2_PACKAGE_LOCATION=<dir containing Microsoft.Web.WebView2.*> ..

set(WEBVIEW2_VERSION "1.0.3485.44")
set(WEBVIEW2_SHA256 "bc09150b179246ac90189649b13be8e6b11b3ac200e817e18df106e1f3cf489e")

file(GLOB _webview2_nuget_packages "$ENV{USERPROFILE}/AppData/Local/PackageManagement/NuGet/Packages/*Microsoft.Web.WebView2*")

if(JUCE_WEBVIEW2_PACKAGE_LOCATION)
  message(STATUS "Using WebView2 SDK from ${JUCE_WEBVIEW2_PACKAGE_LOCATION}")
elseif(_webview2_nuget_packages)
  message(STATUS "Using WebView2 SDK from the local NuGet package folder")
else()
  set(_webview2_root "${CMAKE_BINARY_DIR}/_deps/webview2")
  set(_webview2_dir "${_webview2_root}/Microsoft.Web.WebView2.${WEBVIEW2_VERSION}")

  if(NOT EXISTS "${_webview2_dir}/build/native/include/WebView2.h")
    set(_webview2_nupkg "${_webview2_root}/Microsoft.Web.WebView2.${WEBVIEW2_VERSION}.nupkg")
    message(STATUS "Downloading WebView2 SDK ${WEBVIEW2_VERSION}")

    file(DOWNLOAD
      "https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/${WEBVIEW2_VERSION}"
      "${_webview2_nupkg}"
      EXPECTED_HASH SHA256=${WEBVIEW2_SHA256}
      STATUS _webview2_status)

    list(GET _webview2_status 0 _webview2_status_code)
    if(NOT _webview2_status_code EQUAL 0)
      list(GET _webview2_status 1 _webview2_status_message)
      message(FATAL_ERROR
        "Failed to download the WebView2 SDK: ${_webview2_status_message}\n"
        "Install it manually from a PowerShell prompt:\n"
        "> Register-PackageSource -provider NuGet -name nugetRepository -location https://www.nuget.org/api/v2\n"
        "> Install-Package Microsoft.Web.WebView2 -Scope CurrentUser -RequiredVersion ${WEBVIEW2_VERSION} -Source nugetRepository\n"
        "or pass -DJUCE_WEBVIEW2_PACKAGE_LOCATION=<dir> pointing at an existing copy.")
    endif()

    file(ARCHIVE_EXTRACT INPUT "${_webview2_nupkg}" DESTINATION "${_webview2_dir}")
    file(REMOVE "${_webview2_nupkg}")
  endif()

  set(JUCE_WEBVIEW2_PACKAGE_LOCATION "${_webview2_root}")
  message(STATUS "Using WebView2 SDK from ${_webview2_dir}")
endif()
