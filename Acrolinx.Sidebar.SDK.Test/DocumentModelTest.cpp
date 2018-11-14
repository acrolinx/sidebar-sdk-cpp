#include "stdafx.h"

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualStudio::TestTools::UnitTesting;
using namespace AcrolinxSidebarSDKLib;


namespace AcrolinxSidebarSDKTest
{
    [TestClass]
    public ref class DocumentModelTest
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
        void OriginalToModifiedRangeSimpleTest()
        {

            DocumentModelClass^ documentModel = gcnew DocumentModelClass();
            documentModel->InitInstance("The memorry we used to share is no longer coherent.");

            RangeClass^ range = gcnew RangeClass();
            range->InitInstance(4, 11);
            documentModel->Update(range, "memory");

            RangeClass^ originalRange = gcnew RangeClass();
            originalRange->InitInstance(22, 27);
            Range^ modifiedRange = documentModel->GetOriginalToCurrentRange(originalRange);

            Assert::AreEqual(documentModel->GetCurrentContent()->Substring(modifiedRange->GetStart(), modifiedRange->GetLength()),
                documentModel->GetOriginalContent()->Substring(originalRange->GetStart(), originalRange->GetLength()));
        };

        [TestMethod]
        void InvalidRangeDoesNotCrash()
        {
            DocumentModelClass^ documentModel = gcnew DocumentModelClass();
            documentModel->InitInstance("If Purple People Eaters are real… where do they find purple people to eat?");

            RangeClass^ range = gcnew RangeClass();
            range->InitInstance(-3, 9);
            documentModel->Update(range, "purple");
        };

        [TestMethod]
        void MultipleRandomReplacements()
        {

            DocumentModelClass^ documentModel = gcnew DocumentModelClass();
            documentModel->InitInstance("Last Friday in three week’s time I saw a spotted striped blue worm shake hands with a legless lizard.");

            RangeClass^ range1 = gcnew RangeClass();
            range1->InitInstance(57, 61);
            documentModel->Update(range1, "green");

            RangeClass^ range2 = gcnew RangeClass();
            range2->InitInstance(41, 48);
            documentModel->Update(range2, "catched");

            RangeClass^ range3 = gcnew RangeClass();
            range3->InitInstance(5, 11);
            documentModel->Update(range3, "Tuesday");

            RangeClass^ originalRange = gcnew RangeClass();
            originalRange->InitInstance(10, 14);
            Range^ modifiedRange = documentModel->GetOriginalToCurrentRange(originalRange);

            Assert::AreEqual(documentModel->GetCurrentContent()->Substring(modifiedRange->GetStart(), modifiedRange->GetLength()),
                documentModel->GetOriginalContent()->Substring(originalRange->GetStart(), originalRange->GetLength()));
        };

        [TestMethod]
        void TextHasMoved()
        {
            DocumentModelClass^ documentModel = gcnew DocumentModelClass();
            documentModel->InitInstance("I'd rather be a bird than a fish.");
            documentModel->SetCurrentContent("Movement detected. I'd rather be an bird than a fish.");

            RangeClass^ range = gcnew RangeClass();
            range->InitInstance(0, 3);

            Range^ modifiedRange = documentModel->GetOriginalToCurrentRange(range);

            Assert::AreEqual(documentModel->GetCurrentContent()->Substring(modifiedRange->GetStart(), modifiedRange->GetLength()),
                documentModel->GetOriginalContent()->Substring(range->GetStart(), range->GetLength()));

        };

        [TestMethod]
        void MultiTextFoundAtCorrectPosition()
        {
            DocumentModelClass^ documentModel = gcnew DocumentModelClass();
            documentModel->InitInstance("A A");

            RangeClass^ range = gcnew RangeClass();
            range->InitInstance(0, 1);

            Range^ modifiedRange = documentModel->GetOriginalToCurrentRange(range);

            Assert::AreEqual(documentModel->GetCurrentContent()->Substring(modifiedRange->GetStart(), modifiedRange->GetLength()),
                documentModel->GetOriginalContent()->Substring(range->GetStart(), range->GetLength()));
        };
    };
}
