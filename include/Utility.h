#pragma once

#include <memory>
#include <variant>
#include <iostream>
#include <chrono>


#define BIND_EVENT_FN(fn) [this](auto&& ... args)->decltype(auto){return this->fn(std::forward<decltype(args)>(args)...);}

#define NONCOPYABLE(className) className(const className&) = delete;\
								className& operator=(const className&) = delete
	
#define NONMOVABLE(className) className(className&&) = delete;\
								className& operator=(className&&) = delete
	
#define NONCOPYMOVABLE(className)  NONCOPYABLE(className);\
									NONMOVABLE(className)

template<typename T, typename deleter = std::default_delete<T>>
using ScopedPtr = std::unique_ptr<T, deleter>;

//using default deleter
template<typename T, typename ... Args>
constexpr ScopedPtr<T> CreateScopedPtr(Args&& ... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

//using custom deleter
template<typename T, typename deleter, typename ... Args>
constexpr ScopedPtr<T, deleter> CreateScopedPtr(Args&& ... args)
{
    return std::unique_ptr<T, deleter>(new T{ std::forward<Args>(args)... });
}

template<typename T>
using RefPtr = std::shared_ptr<T>;

template<typename T, typename... Args>
constexpr RefPtr<T> CreateRefPtr(Args&& ... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T, typename deleter, typename... Args>
constexpr RefPtr<T> CreateRefPtr(Args&& ... args)
{
    return std::shared_ptr<T>(new T{ std::forward<Args>(args)... });
}

template<typename Enum, typename Value, typename = std::enable_if_t<std::is_enum_v<Enum>>>
	class BitFlags
	{
	public:
		template<typename ... Args>
		constexpr BitFlags(Args&& ... args) : m_Value{ Create<Enum>(std::forward<Args>(args)...) } {}

		template<typename T, typename ... Args>
		constexpr Value Create(T t, Args&&... args) const {
			return Create(t) | Create(std::forward<Args>(args)...);
		}

		template<typename T>
		constexpr Value Create(T t) const {
			return static_cast<Value>(t);
		}

		template<typename T>
		constexpr Value Create() const { return 0; }

		constexpr bool IsSet(Enum e) const
		{
			return (m_Value & static_cast<Value>(e)) != 0;
		}

		void Combine(const BitFlags& other)
		{
			m_Value |= other.m_Value;
		}

		operator Value()const { return m_Value; }


	private:
		Value m_Value;
	};


inline std::variant<std::false_type, std::true_type> bool_variant(bool condition)
{
    if (condition) return std::true_type{};
    else return std::false_type{};
}

//*****************************define universal hash function************************"""
template<typename T>
static inline void hash_combine(size_t& seed, const T& val)
{
    seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}


template<typename T>
static inline void hash_val(size_t& seed, const T& val)
{
    hash_combine(seed, val);
}

template<typename T, typename... Types>
static inline void hash_val(size_t& seed, const T& val, const Types&... args)
{
    hash_combine(seed, val);
    hash_val(seed, args...);
}

template<typename... Types>
static inline size_t hash_val(const Types&... args)
{
    size_t seed = 0;
    hash_val(seed, args...);
    return seed;
}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

class Timer
{
public:
    Timer()
    {
        Reset();
    }

    ~Timer()
    {
        
        std::cout << "Runs " << ElapsedMillis() << "ms" << std::endl;
    }

    void Reset()
    {
        m_Start = std::chrono::high_resolution_clock::now();
    }

    float Elapsed()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
    }

    float ElapsedMillis()
    {
        return Elapsed() * 1000.0f;
    }

    float ElapsedSeconds()
    {
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - m_Start).count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
};

template<typename ...>
struct Types;

template<typename T>
struct Types<T>
{
    constexpr static bool Is_All_Different_Type = true;
    constexpr static bool Is_In_Any_Type = true;
};


template<typename T1, typename T2>
struct Types<T1, T2>
{
    constexpr static bool Is_All_Different_Type = !std::is_same_v<std::decay_t<T1>, std::decay_t<T2>>;
    constexpr static bool Is_In_Any_Type = std::is_same_v<std::decay_t<T1>, std::decay_t<T2>>;
};


template<typename T, typename ... Ts>
struct Types<T, Ts...>
{
    constexpr static bool Is_All_Different_Type = (Types<T, Ts>::Is_All_Different_Type && ...) && Types<Ts...>::Is_All_Different_Type;
    constexpr static bool Is_In_Any_Type = (Types<T, Ts>::Is_In_Any_Type || ...);
};

template<typename ... Ts>
concept All_Different_Types = Types<Ts...>::Is_All_Different_Type;

template<typename T, typename ... Ts>
concept In_Any_Types = Types<T, Ts...>::Is_In_Any_Type;

template<typename ... components> 
requires(Types<components...>::Is_All_Different_Type)
struct ComponentGroup
{
   
    ComponentGroup() = default;
    ComponentGroup(components&&... comps) : List(std::forward<components>(comps)...), size(std::tuple_size_v<decltype(List)>) {}

    template<typename T>
    requires(Types<T, components...>::Is_In_Any_Type)
    T GetComponent()
    {
        return std::get<T>(List);
    }

    template<typename T>
    requires(Types<T, components...>::Is_In_Any_Type)
    T& GetComponent()
    {
        return std::get<T>(List);
    }

    template<size_t i>
    requires(i < sizeof...(components))
    auto GetComponent()
    {
        return std::get<i>(List);
    }

    template<size_t i>
    requires(i < sizeof...(components))
    auto& GetComponent()
    {
        return std::get<i>(List);
    }

    std::tuple<components...> List;
    int size;
};

template<typename Type>
constexpr uint32_t TypeID()
{
    return static_cast<uint32_t>(reinterpret_cast<std::uintptr_t>(&typeid(Type)));
}

template<int num>
std::string DecimalFormatTo(float value)
{   
    std::stringstream ss;
    ss << std::fixed << std::setprecision(num) << value;
    return ss.str();  
}

