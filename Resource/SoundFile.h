#pragma once
#include "FileSuper.h"
#include <DxLib.h>

class SoundFile : public FileSuper
{
public:
	SoundFile(const std::string& path, int handle)
		: FileSuper(path, handle)
	{
	}

	~SoundFile() override
	{
		if (handle_ >= 0)
		{
			DeleteSoundMem(handle_);
		}
	}
};
