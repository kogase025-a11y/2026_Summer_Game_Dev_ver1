#pragma once
#include "ResourceManager.h"
#include "../Resource/ImageFile.h"
#include "../Resource/SoundFile.h"

#include <DxLib.h>
#include <memory>
#include <string>

class FileManager
{
public:
    std::shared_ptr<ImageFile> LoadImageFM(const std::string& path)
    {
        return image_.Load(path,
            [](const std::string& p)
            {
                return LoadGraph(p.c_str());
            });
    }

    std::shared_ptr<SoundFile> LoadSoundFM(const std::string& path)
    {
        return sound_.Load(path,
            [](const std::string& p)
            {
                return LoadSoundMem(p.c_str());
            });
    }

private:
    ResourceManager<ImageFile> image_;
    ResourceManager<SoundFile> sound_;
};
