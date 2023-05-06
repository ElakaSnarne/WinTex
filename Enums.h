#pragma once

enum class VideoMode
{
	Embedded = 0,
	FullScreen = 1
};

enum class ConversationOption
{
	None = 0,
	A = 1,
	B = 2,
	C = 3,

	AskAbout = 4,
	Offer = 7,
	Abort = 99,
};

enum class InteractionMode
{
	None = 0,
	AskAbout = 1,
	Offer = 2,
	Buy = 3
};

enum class ModuleType
{
	Video = 1,
	Location = 2,
	Picture = 3,
	CrimeLink = 4,
	Inventory = 5,
	NewsPaper = 6,
	Travel = 7,
	MainMenu = 8,
	UltraSafe = 9,
	TornNote = 10,
	EncodedMessage = 11,
	ShreddedNote = 12,
	CodePanel = 13,
	AAASafe = 14,
	ColonelsComputer = 15,
	GRSComputer = 16,
	Ferrelette = 17,
	Stasis = 18,
	ResumeGame = 19,
	Hints = 20,
	VidPhone = 21,
	RitzCodePanel = 22,
	Crossword = 23,
};
