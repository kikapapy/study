#include <iostream>

void noMoreMemory()
{
	std::cerr << "ERROR : Memory\n";
	abort();
}

int main(void)
{
	std::set_new_handler(noMoreMemory);
	int *pBigdataArray = new int[100000000000];

	std::cout << "Hello world\n";

	return 0;
}	
