

#include <boost/dll/export.hpp>


BOOST_SYMBOL_EXPORT extern int unscoped_var;
int unscoped_var = 42;

BOOST_SYMBOL_EXPORT extern const double unscoped_c_var;
const double unscoped_c_var = 1.234;


namespace some_space {

BOOST_SYMBOL_EXPORT extern double variable;
double variable =  0.2;
BOOST_SYMBOL_EXPORT const int & scoped_fun()
{
	static int x = 0xDEADBEEF;
	return x;
}

}


BOOST_SYMBOL_EXPORT void overloaded(int i)
{
	unscoped_var = i;
}

BOOST_SYMBOL_EXPORT void overloaded(double d)
{
	some_space::variable = d;
}

namespace some_space
{

struct BOOST_SYMBOL_EXPORT some_class
{
	BOOST_MEMBER_EXPORT static int value ;
	BOOST_MEMBER_EXPORT static void set_value(int i) { value = i;}


	BOOST_MEMBER_EXPORT virtual double  func(double i, double j) {return i*j;}
	BOOST_MEMBER_EXPORT virtual int 	func(int i, 	int j) 	 {return i+j;}


	BOOST_MEMBER_EXPORT some_class() 	  {value = 23;}
	BOOST_MEMBER_EXPORT some_class(int i) {value = i;}

	BOOST_MEMBER_EXPORT virtual ~some_class()
	{
		value = 0;
	}

};

int some_class::value = -1;

}
