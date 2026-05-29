#pragma once
#include "FileSuper.h"
#include <DxLib.h>

class SoundFile final : public FileSuper
{
public:
	SoundFile(const std::string& path, int handle)
		: FileSuper(path, handle)
	{
	}

	~SoundFile() override
	{
		DeleteSoundMem(handle_);
	}

	void PlayLoop(int playType = DX_PLAYTYPE_LOOP) const
	{
		PlaySoundMem(handle_, playType);
	}

	void PlayOneShot(int playType = DX_PLAYTYPE_BACK) const
	{
		int frame = GetNowCount();
		if (frame != lastPlayFrame)
		{
			lastPlayFrame = frame;
			PlaySoundMem(handle_, playType);
		}
	}

	void Stop() const
	{
		StopSoundMem(handle_);
	}

	void SetVolume(int Volume) const
	{
		ChangeVolumeSoundMem(Volume, handle_);
	}

private:
	// constÉÅÉ\ÉbÉhì‡Ç≈àµÇ¶ÇÈÇÊÇ§Ç…mutableÇ…Ç∑ÇÈ
	mutable int lastPlayFrame = -1;
};
