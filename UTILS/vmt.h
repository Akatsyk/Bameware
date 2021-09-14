#pragma once
#include <Windows.h>
#include <map>

#define k_page_writeable (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)
#define k_page_readable (k_page_writeable|PAGE_READONLY|PAGE_WRITECOPY|PAGE_EXECUTE_READ|PAGE_EXECUTE_WRITECOPY)
#define k_page_offlimits (PAGE_GUARD|PAGE_NOACCESS)

namespace VMT
{
	class VMTHookManager
	{
	public:
		VMTHookManager();
		VMTHookManager(void* instance);
		~VMTHookManager();

		void Init(void* instance);
		void Restore();

		bool IsHooked(int index);

		template <class T>
		T HookFunction(int index, void* func)
		{
			/// index is out of range or null function pointer
			if (index < 0 || index >= m_size || !func)
				return reinterpret_cast<T>(nullptr);

			/// get the original (this assumes that you dont try to hook the function twice)
			auto original = reinterpret_cast<T>(m_vtable[index]);
			m_original_funcs[index] = original;

			unsigned long old_protection, tmp;
			VirtualProtect(&m_vtable[index], sizeof(void*), PAGE_EXECUTE_READWRITE, &old_protection);
			m_vtable[index] = func;
			VirtualProtect(&m_vtable[index], sizeof(void*), old_protection, &tmp);

			return original;
		}

		void UnHookFunction(int index);

		template <class T>
		static T GetFunction(void* instance, int index)
		{
			const auto vtable = *static_cast<void***>(instance);
			return reinterpret_cast<T>(vtable[index]);
		}

		template <class T>
		T GetOriginalFunction(int index)
		{
			/// index is out of range
			if (index < 0 || index >= m_size)
				return reinterpret_cast<T>(nullptr);

			/// function isn't even hooked - just return the one in the vtable
			if (m_original_funcs.find(index) == m_original_funcs.end())
				return reinterpret_cast<T>(m_vtable[index]);

			return reinterpret_cast<T>(m_original_funcs[index]);
		}

	private:
		std::map<int, void*> m_original_funcs;
		void* m_instance = nullptr;
		void** m_vtable = nullptr;
		int m_size = 0;
	};

	template <class T>
	static T GetVFunction(void* instance, int index)
	{
		const auto vtable = *static_cast<void***>(instance);
		return reinterpret_cast<T>(vtable[index]);
	}
}
