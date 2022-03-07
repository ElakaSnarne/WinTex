#pragma once

class CCaption
{
public:
	CCaption() { _frame = -1; _text = NULL; _tex = FALSE; _processed = FALSE; }
	CCaption(int frame, char* text, BOOL tex) { _frame = frame; _text = text; _tex = tex; _processed = FALSE; }
	~CCaption() { }

	int Frame() { return _frame; }
	char* Text() { return _text; }
	BOOL Tex() { return _tex; }
	BOOL Processed() { return _processed; }
	void SetProcessed(BOOL processed) { _processed = processed; }

protected:
	int _frame;
	char* _text;
	BOOL _tex;
	BOOL _processed;
};
