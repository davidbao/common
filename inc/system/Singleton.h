#ifndef SINGLETON_H
#define SINGLETON_H

#include <stdio.h>
#include <memory>
#include "thread/Locker.h"
#include "thread/Mutex.h"

namespace Common
{
	template <class T>
	class Singleton
	{
	public:
		static inline T* instance();

		static inline void initialize()
		{
			Singleton<T>::instance();
		}
		static inline void unInitialize()
		{
			delete Singleton<T>::instance();
			Singleton<T>::_instance = NULL;
		}

	private:
		Singleton(){}
		~Singleton(){}
		Singleton(const Singleton&){}
		Singleton & operator= (const Singleton &){return *this;}

		static T* _instance;
	};

	template <class T>
	T* Singleton<T>::_instance;

	template <class T>
	inline T* Singleton<T>::instance()
	{
        static Mutex m;
        
		Locker locker(&m);

		if(NULL == _instance)
		{
			_instance = new T;
		}
		return _instance;
	}

	//Class that will implement the singleton mode,
	//must use the macro in it's delare file
#define DECLARE_SINGLETON_CLASS( type )	friend class unique_ptr< type >; friend class Singleton< type >;
}
#endif // SINGLETON_H
