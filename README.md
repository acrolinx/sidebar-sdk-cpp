# Acrolinx Sidebar Windows C++ SDK

You can use the Acrolinx Sidebar C++ SDK COM Component to integrate Acrolinx in your Windows application. You can take deeper look into the SDK code in this repository.

## Prerequisites

Please contact Acrolinx SDK support (sdk-support@acrolinx.com) for initial consulting. We like to schedule a kickoff meeting to answer any questions about your integration project. After the meeting, we provide you with test server credentials and configuration settings you would need to get started.

## How to Start

Make sure that you have installed Microsoft Visual Studio with C++ support version 2010.
Open the solution file Acrolinx.Sidebar.SDK.sln with Visual Studio.

## How to Use the Sidebar in Your Integration

Refer to [Sidebar demo](https://github.com/acrolinx/sidebar-demo-cpp/blob/master/README.md).

## SDK Features

1. **Document Model** - Provides [lookup](https://github.com/acrolinx/acrolinx-coding-guidance/blob/master/topics/lookup/diff.md "Lookup") functionality.
2. **Start-page** - Provides interactive way to sign in to Acrolinx with in-built error handling.
3. **Logger** - Provides [logging](https://github.com/muflihun/easyloggingpp"logging") using easylogging++ v8.91.
4. **Acrolinx Storage**: Applications using IE web browser control may be denied to access LocalStorage.
	1. SDK uses its own storage mechanism using windows registry.
	2. **Registry path**: HKCU\Software\Acrolinx\Plugins\Storage\AcrolinxStorage\[KEY] **Fallback path**: HKLM\Software\Acrolinx\Plugins\Storage\AcrolinxStorage\[KEY]
5. **Acrolinx Adapter**: Provides reusable functionality for all integrations. Refer demo [here](https://github.com/acrolinx/sidebar-demo-cpp/tree/master/Acrolinx.Adapter.Demo.Sidebar.Cpp "Acrolinx.Adapter.Demo.Sidebar.Cpp").
  
## Server Dependency

To use the Acrolinx Sidebar, you need to connect to an Acrolinx version 5.3 or later. If your company has installed an Acrolinx, but you don't have an address yet, ask your administrator first.

## License

Copyright 2018 Acrolinx GmbH

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

For more information visit: http://www.acrolinx.com

## Dependencies

This project depends on -

RapidJSON for JSON utility. RapidJSON is licensed under the [MIT License](https://github.com/Tencent/rapidjson/blob/master/license.txt) and Copyright (C) 2015 THL A29 Limited, a Tencent company, and Milo Yip. You can get the source [here](https://github.com/Tencent/rapidjson). The RapidJSON website is at http://rapidjson.org/.

Easylogging++ v8.91 for logging utility. Easylogging++ is licensed under the [EasyLogging++ Licence](https://github.com/muflihun/easyloggingpp/blob/v8.91/LICENCE.txt) and Copyright (c) 2012-2013 Majid Khan. You can get the source [here]( https://github.com/muflihun/easyloggingpp/tree/v8.91).
