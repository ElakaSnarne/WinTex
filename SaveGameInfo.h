#pragma once

#include <string>

struct SaveGameInfo
{
	std::wstring FileName;
	std::string Player;
	std::string Location;
	std::string DayInGame;
	std::string DateTime;
	std::string Comment;

	bool operator < (const SaveGameInfo& info) const
	{
		return (DateTime > info.DateTime);
	}
};
