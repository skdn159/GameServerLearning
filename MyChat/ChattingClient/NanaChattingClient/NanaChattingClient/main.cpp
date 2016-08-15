#include <iostream>
#include "MainForm.h"

int main()
{
	MainForm mainClient;

	mainClient.Init();

	mainClient.CreateGUI();

	mainClient.ShowModal();

	getchar();

	return 0;
}