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
