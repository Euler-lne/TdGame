#ifndef _MANAGER_H_
#define _MANAGER_H_

template <typename T>
class Manager
{
public:
	static T* instance()
	{
		if (manager == nullptr)
			manager = new T();
		return manager;
	}

private:
	static T* manager;

protected:
	Manager() = default;
	~Manager() = default;
	Manager(const Manager&) = delete;
	Manager& operator=(const Manager&) = delete;
};

template <typename T>
T* Manager<T>::manager = nullptr;
/*
静态成员变量需要在类外进行定义和初始化。
这种定义和初始化不涉及访问控制权限，因此可以在类外进行。
访问控制主要是在类的成员函数和友元访问类成员时起作用。
*/

#endif // !_MANAGER_H_

