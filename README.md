# Acrolinx Sidebar Windows C++ SDK

This is a library for integrating the [Acrolinx](http://www.acrolinx.com/) Sidebar into a Windows C++ application.

See: [Getting Started with Custom Integrations](https://support.acrolinx.com/hc/en-us/articles/205687652-Getting-Started-with-Custom-Integrations)

## Live Demo

[Acrolinx Sidebar Web Live Demo](https://acrolinx.github.io/acrolinx-sidebar-demo/samples/index.html)

## The Acrolinx Sidebar

The Acrolinx Sidebar is designed to show up beside the window where you edit your content.
You use it for checking, reviewing, and correcting your content.
To get an impression what the Sidebar looks like in existing integrations, have a look at
[Get Started With the Sidebar](https://support.acrolinx.com/hc/en-us/articles/205697451-Get-Started-With-the-Sidebar).

## Prerequisites

Please contact [Acrolinx SDK support](https://github.com/acrolinx/acrolinx-coding-guidance/blob/master/topics/sdk-support.md)
for consulting and getting your integration certified.
This sample works with a test license on an internal Acrolinx URL.
This license is only meant for demonstration and developing purposes.
Once you finished your integration, you'll have to get a license for your integration from Acrolinx.
  
Acrolinx offers different other SDKs, and examples for developing integrations.

Before you start developing your own integration, you might benefit from looking into:

* [Getting Started with Custom Integrations](https://support.acrolinx.com/hc/en-us/articles/205687652-Getting-Started-with-Custom-Integrations),
* the [Guidance for the Development of Acrolinx Integrations](https://github.com/acrolinx/acrolinx-coding-guidance),
* the [Acrolinx SDKs](https://github.com/acrolinx?q=sdk), and
* the [Acrolinx Demo Projects](https://github.com/acrolinx?q=demo).

## Getting Started

## Using the SDK

See: [Sidebar demo](https://github.com/acrolinx/sidebar-demo-cpp/blob/master/README.md).

## SDK Features

1. Provides [lookup](https://github.com/acrolinx/acrolinx-coding-guidance/blob/master/topics/lookup/diff.md) functionality.
2. **Start page**: Provides interactive way to sign in to Acrolinx with built-in error handling.
3. Provides logging using [EasyLogging++ v8.91](https://github.com/muflihun/easyloggingpp).
4. **Acrolinx Storage**: Applications using the IE web browser control may be denied to access LocalStorage. The SDK uses its own storage mechanism using the Windows registry.
   * **Registry path**: `HKCU\Software\Acrolinx\Plugins\Storage\AcrolinxStorage\[KEY]`
   * **Fallback path**: `HKLM\Software\Acrolinx\Plugins\Storage\AcrolinxStorage\[KEY]`
5. **Acrolinx Adapter**: Provides reusable functionality for all integrations.
  
## References

* The [Sidebar DEMO C++](https://github.com/acrolinx/sidebar-demo-cpp) is built based on this SDK.
* The Sidebar SDKs are based on the [Acrolinx Sidebar Interface](https://acrolinx.github.io/sidebar-sdk-js/).
* RapidJSON for JSON utility. RapidJSON is licensed under the [MIT License](https://github.com/Tencent/rapidjson/blob/master/license.txt) and Copyright (C) 2015 THL A29 Limited, a Tencent company, and Milo Yip. You can get the source [here](https://github.com/Tencent/rapidjson). The RapidJSON website is at [rapidjson.org](http://rapidjson.org/).
* EasyLogging++ v8.91 for logging utility. EasyLogging++ is licensed under the [EasyLogging++ License](https://github.com/muflihun/easyloggingpp/blob/v8.91/LICENCE.txt) and Copyright (c) 2012-2013 Majid Khan. You can get the source [here]( https://github.com/muflihun/easyloggingpp/tree/v8.91).

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

For more information visit: [http://www.acrolinx.com](http://www.acrolinx.com)