#pragma once
#include "FileSuper.h"
#include <DxLib.h>

class ImageFile : public FileSuper
{
public:
	ImageFile(const std::string& path, int handle)
		: FileSuper(path, handle)
	{
	}

	~ImageFile() override
	{
		if (handle_ >= 0)
		{
			DeleteGraph(handle_);
		}
	}
};
