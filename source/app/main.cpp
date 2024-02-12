#include <stdio.h>

#include <wv/Core/cApplication.h>

int main()
{
	wv::cApplication::getInstance().create();
	wv::cApplication::getInstance().run();
}