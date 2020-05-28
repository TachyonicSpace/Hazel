#pragma once

#include <memory>

#ifdef HZ_PLATFORM_WINDOWS
	#ifdef HZ_DYNAMIC_LINK
		#ifdef HZ_BUILD_DLL
			#define HAZEL_API __declspec(dllexport)
		#else
			#define HAZEL_API __declspec(dllimport)
		#endif // HZ_BUILD_DLL
	#else
		#define HAZEL_API

#endif // HZ_DYNAMIC_LINK
#else
	#error Hazel only supports Windows!
#endif

#ifdef HZ_DEBUG
	#define HZ_ENABLE_ASSERTS
#endif // HZ_DEBUG


#ifdef HZ_ENABLE_ASSERTS
	#define HZ_ASSERT(x, ...) {if(!(x)) {HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
	#define HZ_CORE_ASSERT(x, ...) {if(!(x)) {HZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
	#define HZ_ASSERT(x, ...)
	#define HZ_CORE_ASSERT(x, ...)
#endif // HZ_ENABLE_ASSERTS


#define BIT(x) (1 << x)

#define HZ_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

//make sure to use dispatch as the name of the dispatcher
#define HZ_DISPATCH_EVENT(class, eType) dispatch.Dispatch<eType##Event>(HZ_BIND_EVENT_FN(class##::##On##eType))

namespace Hazel
{
	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T>
	using Ref = std::shared_ptr<T>;


	//template <typename T>
	//using newScope = std::make_unique<T>;

	//template <typename T>
	//using newRef = std::make_shared<T>;

}