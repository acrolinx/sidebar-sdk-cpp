/* Copyright Acrolinx GmbH */

#include "StdAfx.h"
#include "DiffMatchPatchWorker.h"

CDiffMatchPatchWorker::CDiffMatchPatchWorker(void)
{
}

void CDiffMatchPatchWorker::LookupMatches(IRange* originalRange, IRange* currentRange, CString originalContent,  CString currentContent)
{
    Diffs diffs = dmp.diff_main(originalContent.GetString(), currentContent.GetString());
    Diffs::iterator currentDiff;

    OffsetMappingSet offsetMappingSet;
    LONG offsetCountOld = 0;
    LONG currentDiffOffset = 0;

    for(currentDiff = diffs.begin(); currentDiff != diffs.end();)
    {
        OffsetDiff offsetMap;

        switch(currentDiff->operation)
        {
        case EQUAL:
            offsetCountOld += currentDiff->text.length();
            break;
        case DELETE:
            offsetCountOld += currentDiff->text.length();
            currentDiffOffset -= currentDiff->text.length();
            break;
        case INSERT:
            currentDiffOffset += currentDiff->text.length();
            break;
        }

        offsetMap.first = offsetCountOld;
        offsetMap.second = currentDiffOffset;
        offsetMappingSet.insert(offsetMap);
        ++currentDiff;
    }

    LONG startOffest = 0, endOffset = 0;

    originalRange->GetStart(&startOffest);
    originalRange->GetEnd(&endOffset);

    int alignedBegin = FindNewIndex(offsetMappingSet, startOffest);
    int alignedEnd = FindNewIndex(offsetMappingSet, endOffset);

    currentRange->InitInstance(alignedBegin, alignedEnd);
}

LONG CDiffMatchPatchWorker::FindNewIndex(OffsetMappingSet offsetMappingSet, LONG originalIndex)
{
    return originalIndex + FindDisplacement(offsetMappingSet, originalIndex);
}

LONG CDiffMatchPatchWorker::FindDisplacement(OffsetMappingSet offsetMappingSet, LONG originalIndex)
{
    OffsetDiff positionMarker;
    positionMarker.first = originalIndex + 0.1;
    positionMarker.second = 0;

    offsetMappingSet.insert(positionMarker);
    OffsetMappingSet::iterator findResult = offsetMappingSet.find(positionMarker);

    LONG index = std::distance(offsetMappingSet.begin(), findResult);

    LOGD << "Position of original range "<< index << " in diffs";

    if(index > 0)
    {
        findResult = offsetMappingSet.begin();
        std::advance(findResult, index - 1);
        LOGD << "Diff distance " << findResult->second;
        return (LONG)findResult->second;
    }

    return 0;
}
