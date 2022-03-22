#pragma once

struct CSVSeparator
{
	static bool IsBeginSeparator( unsigned char c)
	{
		if (c == ',' || c == '\"' )
			return true;
		return false;
	}
	static bool IsBeginSeparator( char c)
	{
		if (c == ',' || c == '\"')
			return true;
		return false;
	}
	static bool IsBeginSeparator( wchar_t c)
	{
		if (c == L',' || c == L'\"')
			return true;
		return false;
	}
	static bool IsSeparator(unsigned char c)
	{
		if (c == ',' || c == '\"' || c == '\r')
			return true;
		return false;
	}
	static bool IsSeparator(char c)
	{
		if (c == ',' || c == '\"' || c == '\r')
			return true;
		return false;
	}
	static bool IsSeparator(wchar_t c)
	{
		if (c == L',' || c == L'\"' || c == L'\r')
			return true;
		return false;
	}
	static bool IsQuote(unsigned char c)
	{
		if ( c == '\"')
			return true;
		return false;
	}
	static bool IsQuote(char c)
	{
		if ( c == '\"')
			return true;
		return false;
	}
	static bool IsQuote(wchar_t c)
	{
		if ( c == L'\"')
			return true;
		return false;
	}
	static bool IsNewLine(unsigned char c)
	{
		if (c == '\n' || c == '\r')
			return true;
		return false;
	}
	static bool IsNewLine(char c)
	{
		if (c == '\n' || c == '\r')
			return true;
		return false;
	}
	static bool IsNewLine(wchar_t c)
	{
		if (c == L'\n' || c == L'\r')
			return true;
		return false;
	}
};

// S supports separating the stream into tokens, T classifies the tokens
// C is the character type
//
// doesn't support the escaping of separator characters
//
template <typename C, typename S, typename T> class LexicalAnalyser
{
private:
	// the input
	C const* b_;
	C const* e_;

	// the current token
	C const* bt_;
	C const* et_;

	// the current token's details
	size_t type_;
	std::basic_string<C> value_;

	// set the first token up.
	void First()
	{
		bt_ = b_;
		et_ = b_;
		type_ = -1;
		if (bt_ == e_)
			return;
		Advance();
		type_ = T::Classify(value_);
	}

	void Advance()
	{
		bool bquote = false;
		if (S::IsNewLine(*bt_)) // special case
		{
			et_ = bt_;
			while (et_ != e_ && S::IsNewLine(*et_))
				++et_;
		}
		else
		{
			while (bt_ != e_ && S::IsBeginSeparator(*bt_))
			{
				bquote = S::IsQuote(*bt_);
				++bt_;
			}
			et_ = bt_;
			if (bquote)
			{
				while (et_ != e_ && !S::IsQuote(*et_))
					++et_;
			}
			if (S::IsNewLine(*et_)) // special case
			{
				while (et_ != e_ && S::IsNewLine(*et_))
					++et_;
			}
			else
			{
				while (et_ != e_ && !S::IsSeparator(*et_))
				{
					++et_;
				}
			}
		}
		value_.assign(bt_, et_);
	}

public:
	LexicalAnalyser(C const* b, C const* e) : b_(b), e_( e)
	{
		First();
	}

	bool Next()
	{
		if (et_ == e_)
			return false;
		// advance the pointers and call the classifier
		bt_ = et_;
		Advance();
		type_ = T::Classify(value_);

		return true;
	}
	size_t Current(C const** bt, C const** et)
	{
		*bt = bt_;
		*et = et_;
		return type_;
	}
	size_t Current() const
	{
		return type_;
	}
	C const * B() const
	{
		return bt_;
	}
	std::basic_string<C> const& Value() const
	{
		return value_;
	}
};

// S supports separating the stream into tokens, T classifies the tokens
// I is an iterator
//
// doesn't support the escaping of separator characters
//
template <typename I, typename S, typename T> class LexicalAnalyserI
{
private:
	// the input
	I b_;
	I e_;

	// the current token
	I bt_;
	I et_;

	// the current token's details
	size_t type_;
	std::basic_string<typename I::value_type> value_;

	// set the first token up.
	void First()
	{
		bt_ = b_;
		et_ = b_;
		type_ = -1;
		if (bt_ == e_)
			return;
		Advance();
		type_ = T::Classify(value_);
	}

	void Advance()
	{
		bool bquote = false;
		if (S::IsNewLine(*bt_)) // special case
		{
			et_ = bt_;
			while (et_ != e_ && S::IsNewLine(*et_))
				++et_;
		}
		else
		{
			while (bt_ != e_ && S::IsBeginSeparator(*bt_))
			{
				bquote = S::IsQuote(*bt_);
				++bt_;
			}
			et_ = bt_;
			if (bquote)
			{
				while (et_ != e_ && !S::IsQuote(*et_))
					++et_;
			}
			if (et_ != e_)
			{
				if (S::IsNewLine(*et_)) // special case
				{
					while (et_ != e_ && S::IsNewLine(*et_))
						++et_;
				}
				else
				{
					while (et_ != e_ && !S::IsSeparator(*et_))
					{
						++et_;
					}
				}
			}
		}
		value_.assign(bt_, et_);
	}

public:
	LexicalAnalyserI(I b, I e) : b_(b), e_(e)
	{
		First();
	}

	bool Next()
	{
		if (et_ == e_)
			return false;
		// advance the pointers and call the classifier
		bt_ = et_;
		Advance();
		type_ = T::Classify(value_);

		return true;
	}
#if 0
	size_t Current(C const** bt, C const** et)
	{
		*bt = bt_;
		*et = et_;
		return type_;
	}
#endif
	size_t Current() const
	{
		return type_;
	}
	I B() const
	{
		return bt_;
	}
	I E() const
	{
		return et_;
	}
	std::basic_string<typename I::value_type> const& Value() const
	{
		return value_;
	}
};

