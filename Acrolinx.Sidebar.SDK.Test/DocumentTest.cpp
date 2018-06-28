#include "stdafx.h"

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;
using namespace AcrolinxSidebarSDKLib;

namespace AcrolinxSidebarSDKTest
{
    [TestClass]
    public ref class DocumentTest
    {
    private:
        TestContext^ testContextInstance;

    public:
        /// <summary>
        ///Gets or sets the test context which provides
        ///information about and functionality for the current test run.
        ///</summary>
        property Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ TestContext
        {
            Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ get()
            {
                return testContextInstance;
            }
            System::Void set(Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ value)
            {
                testContextInstance = value;
            }
        };

#pragma region Additional test attributes
#pragma endregion

        [TestMethod]
        void GetFormatTest()
        {
            DokumentClass;
            DokumentClass^ document = gcnew DokumentClass();
            document->InitInstance("<x>test content</x>", "test.xml", Input_Format::IF_XML);
            Input_Format actualFormat = document->GetFormat();

            Assert::AreEqual(Input_Format::IF_XML, actualFormat);

        };

        [TestMethod]
        void GetFormatTestAuto()
        {
            DokumentClass;
            DokumentClass^ document = gcnew DokumentClass();
            document->InitInstance("<x>test content</x>", "test.xml", Input_Format::IF_AUTO);
            Input_Format actualFormat = document->GetFormat();

            Assert::AreEqual(Input_Format::IF_AUTO, actualFormat);

        };
    };
}
