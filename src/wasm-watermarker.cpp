#include <iostream>

int main(int argc, char *argv[])
{
	try
	{
		for (int i = 0; i < argc; i++)
		{
			std::cout << argv[i] << std::endl;
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';

		return 1;
	}
}
