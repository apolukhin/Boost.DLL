

#include <boost/dll/export.hpp>
#include <boost/variant.hpp>

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


BOOST_SYMBOL_EXPORT void overloaded(const volatile int i)
{
	unscoped_var = i;
}

BOOST_SYMBOL_EXPORT void overloaded(const double d)
{
	some_space::variable = d;
}

BOOST_SYMBOL_EXPORT void use_variant(boost::variant<int, double> & v)
{
	v = 42;
}

BOOST_SYMBOL_EXPORT void use_variant(boost::variant<double, int> & v)
{
	v = 3.124;
}



namespace some_space
{

BOOST_SYMBOL_EXPORT extern int father_value;
int father_value = 12;

struct some_father
{
	some_father() {  father_value = 24; };
	~some_father() { father_value = 112; };
};



struct BOOST_SYMBOL_EXPORT some_class : some_father
{
	BOOST_DLL_MEMBER_EXPORT static int value ;
	BOOST_DLL_MEMBER_EXPORT static void set_value(const int &i) { value = i;}

	static some_class* dummy() {return new some_class();}//so it implements an allocating ctor.

	BOOST_DLL_MEMBER_EXPORT virtual double  func(double i, double j) {return i*j;}
	BOOST_DLL_MEMBER_EXPORT virtual int 	func(int i, 	int j) 	 {return i+j;}
	BOOST_DLL_MEMBER_EXPORT int func(int i, 	int j)   		  volatile {return i-j;;}
	BOOST_DLL_MEMBER_EXPORT double func(double i, double j) const volatile {return i/j;}

	int mem_val;
	BOOST_DLL_MEMBER_EXPORT int  get() const {return mem_val;}
	BOOST_DLL_MEMBER_EXPORT void set(int i)  {mem_val = i;}

	BOOST_DLL_MEMBER_EXPORT some_class() 	  : mem_val(123) {value = 23;}
	BOOST_DLL_MEMBER_EXPORT some_class(int i) : mem_val(456) {value = i;}

	BOOST_DLL_MEMBER_EXPORT virtual ~some_class()
	{
		value = 0;
	}

};

int some_class::value = -1;

}