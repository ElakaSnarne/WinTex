#pragma once

#ifndef __ENUMS__
#define __ENUMS__

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
	Crossword = 22,
};

enum class ActionType
{
	None = 0x00000000,
	Look = 0x00000001,
	Move = 0x00000002,
	Get = 0x00000004,
	OnOff = 0x00000008,
	Talk = 0x00000010,
	Open = 0x00000020,
	Use = 0x00000040,
	Terminate = 0x00008000,
};

enum class AnimationStatus
{
	NotStarted = 0,
	Running = 1,
	OnHold = 2,
	Completed = 4
};

enum class Alignment
{
	Default = 0x00,
	Left = 0x01,
	Right = 0x02,
	CenterX = 0x04,
	Top = 0x08,
	Bottom = 0x10,
	CenterY = 0x20,
	ScaleX = 0x40,
	ScaleY = 0x80,
	Scale = ScaleX | ScaleY,
	Crop = 0x100
};

#endif
