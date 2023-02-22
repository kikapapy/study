#include <iostream>

class CTest
{
public:
	CTest(void)
	{
		std::cout << "Create\n";
	}

	~CTest(void)
	{
		std::cout << "Delete\n";
	}
};

int main(void)
{
	CTest *hTest = new CTest[5];

	std::cout << "Hello world\n";

	//delete hTest;	
	delete [] hTest;
	
	typedef CTest AddressLines[4];
	CTest *pal = new AddressLines;
	std::cout << "Test2\n";
	//delete pal;
	delete [] pal;

	return 0;
}	
