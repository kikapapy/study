#include <iostream>

class X
{
public:
	X (void){}
	~X (void){}

private:
	int *pBigdataArray;

public:	
	static std::new_handler set_new_handler (std::new_handler p);
	static void* operator new (size_t size);
private:
	static std::new_handler currentHandler;
};

std::new_handler X::currentHandler;

std::new_handler X::set_new_handler (std::new_handler p)
{
	std::new_handler oldHandler = currentHandler;
	currentHandler = p;
	return oldHandler;
}

void * X::operator new (size_t size)
{
	std::new_handler globalHandler = std::set_new_handler(currentHandler);

	void *memory;
	
	try {
		memory = ::operator new(size);
	}
	catch (std::bad_alloc) {
		std::set_new_handler(globalHandler);
		throw;
	}

	std::set_new_handler(globalHandler);

	return memory;
}

void g_noMoreMemory()
{
	std::cerr << "ERROR : Memory Global\n";
	abort();
}

void noMoreMemory()
{
	std::cerr << "ERROR : Memory\n";
	abort();
}

int main(void)
{
	std::cout << "std::set_new_handler(g_noMoreMemory)\n";
	std::set_new_handler(g_noMoreMemory);

	std::cout << "X::set_new_handler(noMoreMemory)\n";
	X::set_new_handler(noMoreMemory);

	std::cout << "X *px1 = new X\n";
	X *px1 = new X;

	std::cout << "int *pBigdataArray = new int[100000000000]\n";
	int *pBigdataArray = new int[100000000000];

	std::cout << "X::set_new_handler(0)\n";
	X::set_new_handler(0);

	std::cout << "X *px2 = new X\n";
	X *px2 = new X;

	std::cout << "Hello world\n";

	return 0;
}	
