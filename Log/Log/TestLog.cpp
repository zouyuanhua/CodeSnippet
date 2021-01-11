
#include <iostream>
#include "Log.h"

using namespace FLog;
using namespace std;


int main(int argc, char ** argv)
{
	Log log;
	log.SetVerbosity(Log::Info);
	logInfo(TestLog, "test!!!!!!!!!!" << 10);
	log.Flush();

	//auto x = [](int v){ std::cout << v << std::endl; }(123);
	int a = 123;
	auto x = [](int a) { cout << a << endl; };
	x(a);

	char ch;
	int n;
	scanf_s("%d\n", &n);
	scanf_s(" %c", &ch);
	printf("get char = %c.", ch);


	//getchar();

	fflush(stdin);

	do
	{
		std::cout << '\n' << "Press the enter key to continue.";
	} while (getchar() != '\n' );
	return 0;
}

