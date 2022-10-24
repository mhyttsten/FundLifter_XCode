//
//  Utils.hpp
//  FundLifter
//
//  Created by Magnus Hyttsten on 10/10/22.
//  Copyright © 2022 Magnus Hyttsten. All rights reserved.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <stdio.h>
#include <unordered_map>

using namespace std;

extern const char charArchive;

int getLineNumber(string in, size_t idx_end);
bool isWhitespace(char c);
bool isIdentifierCharacter(char c);
bool isReturnTypeCharacter(char c);
size_t backwardsSkippingWhiteSpaces(string in, size_t idx);
size_t backwardsToNonIdentifier(string in, size_t idx);
size_t forwardToNonIdentifier(string in, size_t idx);
size_t forwardToNonWhiteSpace(string in, size_t idx);
void populateIdentifiersAfter(unordered_map<string, void*>& m, string fin, string key);



#endif /* Utils_hpp */
