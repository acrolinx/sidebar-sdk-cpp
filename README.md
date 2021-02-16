# Acrolinx Sidebar Windows C++ SDK

This is a library for integrating the [Acrolinx](https://www.acrolinx.com/) Sidebar into a Windows C++ application.

See: [Getting Started with Custom Integrations](https://docs.acrolinx.com/customintegrations)

## Live Demo

[Acrolinx Sidebar Web Live Demo](https://acrolinx.github.io/acrolinx-sidebar-demo/samples/index.html)

## The Acrolinx Sidebar

The Acrolinx Sidebar is designed to show up beside the window where you edit your content.
You use it for checking, reviewing, and correcting your content.
To get an impression what the Sidebar looks like in existing integrations, have a look at
[Get Started With the Sidebar](https://docs.acrolinx.com/coreplatform/latest/en/the-sidebar/get-started-with-the-sidebar).

## Prerequisites

Please contact [Acrolinx SDK support](https://github.com/acrolinx/acrolinx-coding-guidance/blob/master/topics/sdk-support.md)
for consulting and getting your integration certified.
This sample works with a test license on an internal Acrolinx URL.
This license is only meant for demonstration and developing purposes.
Once you finished your integration, you'll have to get a license for your integration from Acrolinx.
  
Acrolinx offers different other SDKs, and examples for developing integrations.

Before you start developing your own integration, you might benefit from looking into:

* [Getting Started with Custom Integrations](https://docs.acrolinx.com/customintegrations),
* the [Guidance for the Development of Acrolinx Integrations](https://github.com/acrolinx/acrolinx-coding-guidance),
* the [Acrolinx SDKs](https://github.com/acrolinx?q=sdk), and
* the [Acrolinx Demo Projects](https://github.com/acrolinx?q=demo).

Also note that "Visual C++ 2010 redistributable x86" is a run time requirement.

## Getting Started

See: [Sidebar demo](https://github.com/acrolinx/sidebar-demo-cpp/blob/master/README.md).

## SDK Features

1. Provides lookup functionality.
2. **Start page**: Provides interactive way to sign in to Acrolinx with built-in error handling.
3. Provides logging using [Plog v1.1.4](https://github.com/SergiusTheBest/plog).
4. **Acrolinx Storage**: Applications using the Internet Explorer web browser control may be denied to access LocalStorage.
   The SDK uses its own storage mechanism using the Windows registry.
   + **Registry path**: `HKCU\Software\Acrolinx\Plugins\Storage\AcrolinxStorage\[KEY]`
   + **Fallback path**: `HKLM\Software\Acrolinx\Plugins\Storage\AcrolinxStorage\[KEY]`
5. **Acrolinx Adapter**: Provides reusable functionality for all integrations.
  
## References

* The [Sidebar DEMO C++](https://github.com/acrolinx/sidebar-demo-cpp) is built based on this SDK.
* The Sidebar SDKs are based on the [Acrolinx Sidebar Interface](https://acrolinx.github.io/sidebar-interface/).
* RapidJSON for JSON utility. RapidJSON is licensed under the [MIT License](https://github.com/Tencent/rapidjson/blob/master/license.txt)
  and Copyright (C) 2015 THL A29 Limited, a Tencent company, and Milo Yip. You can get the source [here](https://github.com/Tencent/rapidjson).
* Plog v1.1.4 for logging utility. Plog is licensed under the [MPL-2.0 License](https://github.com/SergiusTheBest/plog/blob/master/LICENSE)
  and Copyright (c) 2016 Sergey Podobry (sergey.podobry@gmail.com). You can get the source [here]( https://github.com/SergiusTheBest/plog).
* Wix Toolset v3.10.4 for simple installer. Wix Toolset is licensed under the [MS-RL](https://wixtoolset.org/about/license/).
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
