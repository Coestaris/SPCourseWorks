#include"TokenType.h"

int main()
{
	input();
	
	TokenType zalypa;
	zalypa = numcheck("\"bdbdb\"");
	if (zalypa == BinNumber) cout << "bin";
	if (zalypa == DecNumber) cout << "dec";
	if (zalypa == HexNumber) cout << "hex";
	if (zalypa == Text)cout << "text"<<endl;
	
	
	
	delimeter();
	output();


	return 0;
	
}