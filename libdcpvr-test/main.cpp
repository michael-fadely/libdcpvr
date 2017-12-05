#include "stdafx.h"

using namespace std;

int main()
{
	//ilInit();

	{
		printf("E_SAI.PVM:\n");
		PVMReader pvm("E_SAI.PVM");

		if (!pvm.is_open())
		{
			printf("E_SAI.PVM open failed.\n");
			return -1;
		}

		for (auto& i : pvm.get_entries())
		{
			cout << i.index << ": " << i.gbix << " - " << i.name << endl;
		}
	}

	printf("\n");

	{
		printf("CASINO04.PVM:\n");
		PVMReader pvm("CASINO04.PVM");

		if (!pvm.is_open())
		{
			printf("CASINO04.PVM open failed.\n");
			return -1;
		}

		for (auto& i : pvm.get_entries())
		{
			cout << i.index << ": " << i.gbix << " - " << i.name << " (format: " << i.format << ")" << endl;
		}
	}

	{
		PVRReader pvr("WINDY3_NBG2.PVR");

		if (!pvr.is_open())
		{
			printf("WINDY3_NBG2.PVR open failed.\n");
			return -2;
		}

		ofstream test_pvr("test_pvr.pvr", ios::binary);
		pvr.write(test_pvr);
	}

	{
		PVRReader result("test_pvr.pvr");
		if (!result.is_open())
		{
			printf("test_pvr.pvr is invalid.\n");
			return -3;
		}
	}

	return 0;
}
