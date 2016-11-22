/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Tuple
	
classes:
        Tuple<data_type>                                        :元组对象
***********************************************************************/
#ifndef VCZH_TUPLE
#define VCZH_TUPLE
namespace vl
{
    template<typename ...TArgs>    class Tuple;

    template<typename T, typename ...TArgs>
    class Tuple<T, TArgs...> : public Object, public Tuple<TArgs...>
    {
    public:
        T value;
        Tuple<TArgs...>    TupleList;

        Tuple()
        {
        }

        Tuple(const T& v, const TArgs& ...args)
            :TupleList(args...), value(v)
        {
        }

        static int Compare(const Tuple& lhs, const Tuple& rhs)
        {
            if (lhs.value < rhs.value)
            {
                return -1;
            }
            else if (lhs.value > rhs.value)
            {
                return 1;
            }
            return lhs.TupleList.Compare(lhs.TupleList, rhs.TupleList);
        }

        bool operator==(const Tuple& value)const { return Compare(*this, value) == 0; }
        bool operator!=(const Tuple& value)const { return Compare(*this, value) != 0; }
        bool operator< (const Tuple& value)const { return Compare(*this, value) < 0; }
        bool operator<=(const Tuple& value)const { return Compare(*this, value) <= 0; }
        bool operator> (const Tuple& value)const { return Compare(*this, value) > 0; }
        bool operator>=(const Tuple& value)const { return Compare(*this, value) >= 0; }
    };

    template<>
    class Tuple<> : public Object
    {
    public:
        static int Compare(const Tuple& lhs, const Tuple& rhs)
        {
            return 0;
        }
    };
}
#endif
