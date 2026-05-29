#pragma once
#include <string>
#include <vector>
#include "Vector2.h"
#include "Vector2F.h"

class AsoUtility
{

public:

	// ˆÚ“®•ûŒü
	enum class DIR
	{
		UP,
		RIGHT,
		DOWN,
		LEFT,
		MAX1
	};

	// •¶Žš—ñ‚Ì•ªŠ„
	static std::vector<std::string> Split(std::string& line, char delimiter);

	// ŽlŽÌŒÜ“ü
	static int Round(float value);

	// ŽlŽÌŒÜ“ü
	static Vector2 Round(Vector2F value);

	// ŽlŽÌŒÜ“ü
	static Vector2 Round(float x, float y);

};


