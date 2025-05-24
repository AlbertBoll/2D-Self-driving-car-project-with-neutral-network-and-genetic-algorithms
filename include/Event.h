#pragma once

#include"Utility.h"
#include <unordered_map>
#include <queue>
#include <functional>


struct MouseButtonParam
{
    unsigned int ID;
    int32_t X;
    int32_t Y;
    uint8_t Button;
    uint8_t Clicks;
};

class IEvent
{

public:
	virtual const std::string& GetName() const = 0;
	virtual ~IEvent(){}

};


template<typename> class Events;

template<typename R, typename... Args>
class Events<R(Args...)>: public IEvent

{

	using Callbacks = std::vector<std::function<R(Args...)>>;

	using iterator = typename Callbacks::iterator;

	using const_iterator = typename Callbacks::const_iterator;


public:
	Events() = default;
    virtual ~Events() = default;
	Events(const std::string& eventName): m_EventName(eventName){}

	const std::string& GetName() const override { return m_EventName; }

	void Reserve(size_t size) { m_Callbacks.reserve(size); }

	auto& Subscribe(const std::function<R(Args...)>& function) { m_Callbacks.push_back(function); return function; }

	void Unsubscribe(const std::function<R(Args...)>& function)

	{

		m_Callbacks.erase(std::remove_if(m_Callbacks.begin(), m_Callbacks.end(), [&](const std::function<R(Args...)>& f)

			{

				return function.target_type().hash_code() == f.target_type().hash_code();

			}), m_Callbacks.end());

	}


	iterator begin() { return m_Callbacks.begin(); }

	iterator end() { return m_Callbacks.end(); }

	[[nodiscard]] const_iterator begin() const { return m_Callbacks.begin(); }

	[[nodiscard]] const_iterator end() const { return m_Callbacks.end(); }

	[[nodiscard]] const_iterator cbegin() const { return m_Callbacks.cbegin(); }

	[[nodiscard]] const_iterator cend() const { return m_Callbacks.cend(); }


	template<typename ... Args>
	void Fire(Args&&... args)const
	{

		this->operator()(std::forward<Args>(args)...);

	}

	void operator()(Args&&... args)const
	{

		for (auto&& event_func : m_Callbacks)
		{
			event_func(std::forward<Args>(args)...);
		}

	}


private:
	std::string m_EventName;
	Callbacks m_Callbacks;

};


class EventDispatcher
{

public:

	~EventDispatcher() 
	{
		
		for (auto& eventList : m_EventCallBackList)
		{
            //std::cout<<"delete event: "<<eventList.first<<std::endl;
			delete eventList.second;
		}
	};
	
	void UnregisterEvent(const std::string& eventName)
	{
		//auto events = m_EventCallBackList.find(eventName);
		//m_EventCallBackList.find(eventName);
		
		for (auto [itr, rangeEnd] = m_EventCallBackList.equal_range(eventName); itr != rangeEnd; ++itr)
		{
			delete itr->second;
		}

		m_EventCallBackList.erase(eventName);
		
	}


	void RegisterEvent(IEvent* callBackList)
	{
		m_EventCallBackList.emplace(callBackList->GetName(), callBackList);
	}

	template <typename R = void, typename ... Args>
	R DispatchEvent(const std::string& eventName, Args&&...args)
	{
		auto range = m_EventCallBackList.equal_range(eventName);

		for (auto it = range.first; it != range.second; ++it)
		{
			if (Events<R(Args...)>* events = dynamic_cast<Events<R(Args...)>*>(it->second))
				events->Fire(std::forward<Args>(args)...);
		}

	}


private:
	std::unordered_multimap<std::string, IEvent*> m_EventCallBackList;
};







#if 0
struct WindowEvent
{
    WindowEvent(){std::cout<<"Window Event constructed"<<std::endl;}
    ~WindowEvent()
    {
        std::cout<<"Window Event destroyed"<<std::endl;
    }
    int i = 8;
};

template<typename Event>
struct EventListener 
{
    using CallBack = std::function<void(const Event&)>;

    EventListener(CallBack&& callback, uint32_t listener_id): m_CallBackFn(std::move(callback)), m_ListenerID(listener_id) {};

    CallBack m_CallBackFn;
    uint32_t m_ListenerID;

};

template<typename Event>
struct EventRegistry
{
    using Listener = ScopedPtr<EventListener<Event>>;
    std::queue<ScopedPtr<Event>> m_EventQueue;
    std::vector<Listener> m_Listeners;
    //~EventRegistry(){}
    
};

struct EventDispatcher
{

    ~EventDispatcher()
    {
        for(auto& [_, ptr]: m_Registry)
        {
            auto registry = CastRegistry<char>(ptr);
            delete registry; 
        }
        m_Registry.clear();
        m_Tasks = {};
        //std::cout<<"Event Dispatcher Destroyed\n";
    }

    template<typename Event, typename Callback>
    void AttachCallback(Callback&& callback, uint32_t listener_id)
    {
        auto listener = CreateScopedPtr<EventListener<Event>>(std::move(callback), listener_id);
        GetRegistry<Event>()->m_Listeners.push_back(std::move(listener));
    }

    template<typename Event>
    void DetachCallback(uint32_t listener_id)
    {
        auto& listeners = GetRegistry<Event>()->m_Listeners;
        listeners.erase(std::remove_if(listeners.begin(), listeners.end(), [&](auto& listener)
        {
            return listener->m_m_ListenerID == listener_id;
        }), listeners.end());
        
    }

    void EraseListener(uint32_t listener_id)
    {
        for(auto& [_, registry]: m_Registry)
        {
            auto& listeners = CastRegistry<int8_t>(registry)->m_Listeners;
            listeners.erase(std::remove_if(listeners.begin(), listeners.end(), [&](auto& listener)
            {
                return listener->m_ListenerID == listener_id;
            }), listeners.end());
        }
    }

    template<typename Event, typename ... Args>
    void PostEvent(Args&& ... args)
    {
        auto registry = GetRegistry<Event>();
        if(registry-> m_Listeners.empty())return;
        std::cout<<"Event posted start\n";
        registry->m_EventQueue.push(CreateScopedPtr<Event>(std::forward<Args>(args)...));
        std::cout<<"Event posted end\n";
    }

    template<typename Task>
    void PostTask(Task&& task)
    {
        m_Tasks.push(std::move(task));
    }

    void PollEvents()
    {
        for(auto& [_, pointer]: m_Registry)
        {
            auto registry = CastRegistry<char>(pointer);
            while(!registry->m_EventQueue.empty())
            {
                for(auto& listener: registry->m_Listeners)
                {
                    listener->m_CallBackFn(*registry->m_EventQueue.front());
                }
                registry->m_EventQueue.pop();
            }
            
        }
    }

private:
    template<typename Event>
    EventRegistry<Event>* CastRegistry(void* p)
    {
        return static_cast<EventRegistry<Event>*>(p);
    }

    template<typename Event>
    EventRegistry<Event>* GetRegistry()
    {
        auto it = m_Registry.find(TypeID<Event>());
        if (it != m_Registry.end())
        {
            return CastRegistry<Event>(it->second);
        }

        auto registry = new EventRegistry<Event>();
        m_Registry[TypeID<Event>()] = registry;
        return registry;
    }


private:
    std::unordered_map<uint32_t, void*> m_Registry;
    std::queue<std::function<void()>> m_Tasks;


};
#endif