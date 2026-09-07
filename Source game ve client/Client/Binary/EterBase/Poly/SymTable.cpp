#include "StdAfx.h"
#include "SymTable.h"

using namespace std;

CSymTable::CSymTable(int aTok, string aStr) : dVal(0), token(aTok), strlex(aStr)
{
}

CSymTable::~CSymTable()
{
}
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
