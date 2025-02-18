# Acrolinx Sidebar Windows C++ SDK

This is a library for integrating the [Acrolinx](https://www.acrolinx.com/) Sidebar into a Windows C++ application.

See: [Build With Acrolinx](https://support.acrolinx.com/hc/en-us/categories/10209837818770-Build-With-Acrolinx)

## Live Demo

[Acrolinx Sidebar Web Live Demo](https://acrolinx.github.io/acrolinx-sidebar-demo/samples/index.html)

## The Acrolinx Sidebar

The Acrolinx Sidebar is designed to show up beside the window where you edit your content.
You use it for checking, reviewing, and correcting your content.
To get an impression what the Sidebar looks like in existing integrations, have a look at
[Sidebar Quick Start](https://support.acrolinx.com/hc/en-us/articles/10252588984594-Sidebar-Quick-Start).

## Prerequisites

Please contact [Acrolinx SDK support](https://github.com/acrolinx/acrolinx-coding-guidance/blob/master/topics/sdk-support.md)
for consulting and getting your integration certified.
This sample works with a test license on an internal Acrolinx URL.
This license is only meant for demonstration and developing purposes.
Once you finished your integration, you'll have to get a license for your integration from Acrolinx.
  
Acrolinx offers different other SDKs, and examples for developing integrations.

Before you start developing your own integration, you might benefit from looking into:

* [Build With Acrolinx](https://support.acrolinx.com/hc/en-us/categories/10209837818770-Build-With-Acrolinx),
* the [Guidance for the Development of Acrolinx Integrations](https://github.com/acrolinx/acrolinx-coding-guidance),
* the [Acrolinx SDKs](https://github.com/acrolinx?q=sdk), and
* the [Acrolinx Demo Projects](https://github.com/acrolinx?q=demo).

Also note that "Visual C++ 2017 redistributable x86" is a run time requirement.
Download from [Microsoft](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170)

## Requirements

This SDK leverages Microsoft WebView2, built on Microsoft Edge Chromium, to render the Sidebar. Starting with Windows 11, Microsoft includes the Evergreen WebView2 Runtime by default.

For earlier versions of Windows, please install the WebView2 Evergreen Runtime manually.

You can download and install the Evergreen Standalone [Installer](https://developer.microsoft.com/en-us/microsoft-edge/webview2/) from Microsoft's official site.

## Using the SDK

For simplified development experience get SDK from [Nuget](https://www.nuget.org/packages/Acrolinx.Sidebar.SDK.CPP)

Make sure to register the DLL before executing your application.

For handy registration you can use this [utility](https://github.com/acrolinx/sidebar-demo-cpp/blob/main/scripts/RegisterAcrolinx.bat)

## Getting Started

See: [Sidebar demo](https://github.com/acrolinx/sidebar-demo-cpp/blob/master/README.md).

## SDK Features

1. Provides lookup functionality.
2. **Start page**: Provides interactive way to sign in to Acrolinx with built-in error handling.
3. Provides logging using [Plog v1.1.4](https://github.com/SergiusTheBest/plog).
4. **Acrolinx Storage**: The SDK uses its own storage mechanism using the Windows registry.
   + **Registry path**: `HKCU\Software\Acrolinx\Plugins\Storage\AcrolinxStorage\[KEY]`
   + **Fallback path**: `HKLM\Software\Acrolinx\Plugins\Storage\AcrolinxStorage\[KEY]`
5. **Acrolinx Adapter**: Provides reusable functionality for all integrations.

### Deprecation

> :warning: **Sidebar v14 is getting deprecated**

The process to update your existing integration is straightforward and involves the following steps:

1. Update to the Latest SDK Version: Please ensure that you’ve updated to the latest SDK version.
The most recent version can be found on our Nuget package page [here](https://www.nuget.org/packages/Acrolinx.Sidebar.SDK.CPP).

1. Remove the Initialization Config (Optional): If your current implementation includes the initialization config
`MinimumSidebarVersion=14.x.x`, we recommend removing this configuration.

## References

* The [Sidebar DEMO C++](https://github.com/acrolinx/sidebar-demo-cpp) is built based on this SDK.
* The Sidebar SDKs are based on the [Acrolinx Sidebar Interface](https://acrolinx.github.io/sidebar-interface/).
* RapidJSON for JSON utility. RapidJSON is licensed under the [MIT License](https://github.com/Tencent/rapidjson/blob/master/license.txt)
  and Copyright (C) 2015 THL A29 Limited, a Tencent company, and Milo Yip. You can get the source [here](https://github.com/Tencent/rapidjson).
* Plog v1.1.4 for logging utility. Plog is licensed under the [MPL-2.0 License](https://github.com/SergiusTheBest/plog/blob/master/LICENSE)
  and Copyright (c) 2016 Sergey Podobry (<sergey.podobry@gmail.com>). You can get the source [here]( https://github.com/SergiusTheBest/plog).
* Wix Toolset v3.11.2 for simple installer. Wix Toolset is licensed under the [MS-RL](https://wixtoolset.org/about/license/).
  The Wix Toolset website is at [wixtoolset.org](https://wixtoolset.org/).

## License

Copyright 2018-present Acrolinx GmbH

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at:

[http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

For more information visit: [https://www.acrolinx.com](https://www.acrolinx.com)
