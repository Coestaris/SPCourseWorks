#include"TokenType.h"
#include"Delimeter.h"
#include"Lexem.h"

int main()
{
   setupTokenTypeDict();

   // ET2 routines
   createTokens("test.txt");
   proceedTokens();

   printTokenList();

   return 0;
}