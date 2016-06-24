#ifndef PIL_MetaProgramming_INCLUDED
#define PIL_MetaProgramming_INCLUDED

namespace pi {


template <typename T>
struct IsReference
    /// Use this struct to determine if a template type is a reference.
{
    enum
    {
        VALUE = 0
    };
};


template <typename T>
struct IsReference<T&>
{
    enum
    {
        VALUE = 1
    };
};


template <typename T>
struct IsReference<const T&>
{
    enum
    {
        VALUE = 1
    };
};


template <typename T>
struct IsConst
    /// Use this struct to determine if a template type is a const type.
{
    enum
    {
        VALUE = 0
    };
};


template <typename T>
struct IsConst<const T&>
{
    enum
    {
        VALUE = 1
    };
};


template <typename T>
struct IsConst<const T>
{
    enum
    {
        VALUE = 1
    };
};


template <typename T, int i>
struct IsConst<const T[i]>
    /// Specialization for const char arrays
{
    enum
    {
        VALUE = 1
    };
};


template <typename T>
struct TypeWrapper
    /// Use the type wrapper if you want to decouple constness and references from template types.
{
    typedef T TYPE;
    typedef const T CONSTTYPE;
    typedef T& REFTYPE;
    typedef const T& CONSTREFTYPE;
};


template <typename T>
struct TypeWrapper<const T>
{
    typedef T TYPE;
    typedef const T CONSTTYPE;
    typedef T& REFTYPE;
    typedef const T& CONSTREFTYPE;
};


template <typename T>
struct TypeWrapper<const T&>
{
    typedef T TYPE;
    typedef const T CONSTTYPE;
    typedef T& REFTYPE;
    typedef const T& CONSTREFTYPE;
};


template <typename T>
struct TypeWrapper<T&>
{
    typedef T TYPE;
    typedef const T CONSTTYPE;
    typedef T& REFTYPE;
    typedef const T& CONSTREFTYPE;
};


} // namespace pi


#endif // PIL_MetaProgramming_INCLUDED
