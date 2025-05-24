#pragma once

template<typename T>
class Singleton 
{   
public:

    static T& GetInstance()
    {
        static T instance;
        return instance;
    }

protected:
    Singleton() = default;
    ~Singleton() = default;

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

};