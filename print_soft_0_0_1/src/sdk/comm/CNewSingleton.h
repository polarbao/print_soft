#pragma once

#include <memory>
#include <mutex>
#include <iostream>

template <typename T>
class CNewSingleton
{

protected:
	// 禁用拷贝构造和赋值操作
	CNewSingleton() = default;
	CNewSingleton(const CNewSingleton<T>&) = delete;
	CNewSingleton& operator=(const CNewSingleton<T>& st) = delete;

	static std::shared_ptr<T> m_instance;

public:

	// 获取单例实例
	static std::shared_ptr<T> GetInstance()
	{
		static std::once_flag s_flag;
		std::call_once(s_flag, [&]()
		{
			m_instance = std::shared_ptr<T>(new T);
		});
		return m_instance;
	}

	~CNewSingleton() {};

};

template <typename T>
std::shared_ptr<T> CNewSingleton<T>::m_instance = nullptr;