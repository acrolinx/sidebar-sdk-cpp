/* Copyright (c) 2018 Acrolinx GmbH */

#pragma once

//dmp Conflicting with windows.h
#undef DELETE
#undef max
#undef min
#include "diff-match-patch\diff_match_patch.h"
#include <map>
#include <set>
#include "Range.h"

using namespace std;
typedef pair<DOUBLE, DOUBLE> OffsetDiff;
typedef set<OffsetDiff> OffsetMappingSet;

#define dmp (*this)

class CDiffMatchPatchWorker : diff_match_patch<wstring> {
public:
    CDiffMatchPatchWorker();
    void LookupMatches(IRange* originalRange, IRange* currentRange, CString originalContent,  CString currentContent);
    LONG FindNewIndex(OffsetMappingSet offsetMappingSet, LONG originalIndex);
    LONG FindDisplacement(OffsetMappingSet offsetMappingSet, LONG originalIndex);
};