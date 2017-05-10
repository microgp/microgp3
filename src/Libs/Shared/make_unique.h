#include <memory>

/*
 * Implementation of make_unique from Herb Sutter.
 * 
 * http://herbsutter.com/gotw/_102/
 * 
 * To remove when moving to C++14.
 */
template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}