/*
 * msvc_tokenizer.hpp
 *
 *  Created on: 30.09.2015
 *      Author: Klemens
 */

#ifndef BOOST_PLUGIN_DETAIL_MSVC_TOKENIZER_HPP_
#define BOOST_PLUGIN_DETAIL_MSVC_TOKENIZER_HPP_

#include <boost/tokenizer.hpp>
#include <functional>

namespace boost {
namespace dll {
namespace detail {

/*! This is an implementation for boost::tokenizer,
 * to allow the tokenizing of a function name.
 * ### Itanium Api
 * Given a Itanium function name, it tokenizes as follows:
 * \code
 * std::function<void(double)> some_class::some_function(double, std::function<double()>) const
 * \endcode
 * Tokens:
 *  - `std::function<void(double)>`
 *  - `some_class::some_function`
 *  - `(`
 *  - `double`
 *  - `std::function<double()>`
 *  - `const`
 *
 *  ### MSVC
 *  The same function looks differently when demangled with MSVC, so the tokenizer gives a different result.
 *
 *  \code
 *  public: class std::function<void(double)> __cdecl some_class::some_function(double, class std::function<double()>) const
 *  \endcode
 *
 *  Which results in a different sequence for the tokens:
 * Tokens:
 *  - `public:`
 *  - `class`
 *  - `std::function<void(double)>`
 *  - `some_class::some_function`
 *  - `(`
 *  - `double`
 *  - `class std::function<double()>`
 *  - `const`
 *
 */

struct tokenize_function
{
	int in_template_args = 0;//to avoid a token at ',' if in
	int in_function_args = 0;//used for function pointers
	bool argument_list = false;

	template<typename Iterator, typename Token>
	bool in_argument_list(Iterator &next, const Iterator &end, Token&tok)
	{
		if (next == end)
		{
			return false;
		}
		else if (*next == '(')
		{
			tok += '(';
			next++;
			return true;
		}
		else if (*next == ')')
		{
			tok += ')';
			argument_list = false;
			next++;
			return true;
		}

		while (next != end)
		{
			if (in_template_args > 0)
			{
				if (*next == '>')
					in_template_args--;
				else if (*next == '<')
					in_template_args++;
				tok += *next;
				next++;
			}
			else if (in_function_args > 0)
			{
				if (*next == ')')
					in_function_args--;
				else if (*next == '(')
					in_function_args++;
				tok += *next;
				next++;
			}
			else if (*next == '(')
			{
				in_function_args++;
				next++;
				return true;
			}
			else if (*next == '<')
			{
				in_template_args++;
				next++;
				return true;
			}
			else if (*next == ',')
			{
				next++;
				if (*next == ' ') //if there's a space after comma.
					next++;

				return true;
			}
			else if (*next == ')') //end of argument list.
			{
				return !tok.empty();
			}
			else
			{
				tok += *next;
				next++;

			}
		}
		return !tok.empty();

	}

	template<typename Iterator, typename Token>
	bool out_argument_list(Iterator &next, const Iterator &end, Token&tok)
	{

		//stry ( => function pointer.
		if (next == end)
			return false;
		if (*next == '(')
		{
			in_function_args++;
			tok += *next;
			next++;
		}

		while (next != end)
		{
			if (in_template_args > 0)
			{
				if (*next == '>')
					in_template_args--;
				else if (*next == '<')
					in_template_args++;
				tok += *next;
				next++;
			}
			else if (in_function_args > 0)
			{
				if (*next == ')')
					in_function_args--;
				else if (*next == '(')
					in_function_args++;
				tok += *next;
				next++;
			}
			else if (*next == '(')
			{
				argument_list = true;
				return true;
			}
			else if (*next == '<')
			{
				in_template_args++;
				tok += *next;
				next++;
				return true;
			}
			else if (*next == ' ')
			{
				next++;
				return true;
			}
			else
			{
				tok += *next;
				next++;
			}
		}
		return !tok.empty();
	}



	template<typename Iterator, typename Token>
	bool operator()(Iterator &next, const Iterator &end, Token&tok)
	{
		tok = Token();

		return argument_list ? in_argument_list(next, end, tok) : out_argument_list(next, end, tok);

	}
	void reset()
	{
		in_template_args = 0;
		in_function_args = 0;
		argument_list = false;
	}
};


}}}


#endif /* MSVC_TOKENIZER_HPP_ */
