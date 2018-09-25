#include "stdafx.h"

int main()
{
	ilInit();

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
			std::cout << i.index << ": " << i.gbix << " - " << i.name << std::endl;
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
			std::cout << i.index << ": " << i.gbix << " - " << i.name << " (format: " << i.format << ")" << std::endl;
		}
	}

	{
		PVRReader pvr("WINDY3_NBG2.PVR");

		if (!pvr.is_open())
		{
			printf("WINDY3_NBG2.PVR open failed.\n");
			return -2;
		}

		auto decoded = pvr.decode();

		auto image = ilGenImage();
		ilBindImage(image);

		if (!ilTexImage(pvr.width, pvr.height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, decoded.data()))
		{
			auto error = ilGetError();
			throw;
		}

		ilEnable(IL_FILE_OVERWRITE);

		if (!ilSave(IL_PNG, reinterpret_cast<wchar_t const*>("WINDY3_NBG2.png"))) // this looks bad, but this is a bug in devil
		{
			auto error = ilGetError();
			throw;
		}

		ilDeleteImage(image);

		std::ofstream test_pvr("test_pvr.pvr", std::ios::binary);
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

	ilShutDown();
	return 0;
}
