#pragma once

#include "Vulkan.hpp"
#include "Utilities/Exception.hpp"
#include <stdexcept>
#include <vector>

template <class TValue>
inline std::vector<TValue> GetEnumerateVector(VkResult(enumerate) (uint32_t*, TValue*))
{
	std::vector<TValue> initial;
	return GetEnumerateVector(enumerate, initial);
}

template <class THandle, class TValue>
inline std::vector<TValue> GetEnumerateVector(THandle handle, void(enumerate) (THandle, uint32_t*, TValue*))
{
	std::vector<TValue> initial;
	return GetEnumerateVector(handle, enumerate, initial);
}

template <class THandle, class TValue>
inline std::vector<TValue> GetEnumerateVector(THandle handle, VkResult(enumerate) (THandle, uint32_t*, TValue*))
{
	std::vector<TValue> initial;
	return GetEnumerateVector(handle, enumerate, initial);
}

template <class THandle1, class THandle2, class TValue>
inline std::vector<TValue> GetEnumerateVector(THandle1 handle1, THandle2 handle2, VkResult(enumerate) (THandle1, THandle2, uint32_t*, TValue*))
{
	std::vector<TValue> initial;
	return GetEnumerateVector(handle1, handle2, enumerate, initial);
}

template <class TValue>
inline std::vector<TValue> GetEnumerateVector(VkResult(enumerate) (uint32_t*, TValue*), std::vector<TValue>& vector)
{
	uint32_t count = 0;
	if (enumerate(&count, nullptr) != VK_SUCCESS)
	{
		Throw(std::runtime_error("enumerate call failed"));
	}

	vector.resize(count);
	if (enumerate(&count, vector.data()) != VK_SUCCESS)
	{
		Throw(std::runtime_error("enumerate call failed"));
	}

	return vector;
}

template <class THandle, class TValue>
inline std::vector<TValue> GetEnumerateVector(THandle handle, void(enumerate) (THandle, uint32_t*, TValue*), std::vector<TValue>& vector)
{
	uint32_t count = 0;
	enumerate(handle, &count, nullptr);

	vector.resize(count);
	enumerate(handle, &count, vector.data());

	return vector;
}

template <class THandle, class TValue>
inline std::vector<TValue> GetEnumerateVector(THandle handle, VkResult(enumerate) (THandle, uint32_t*, TValue*), std::vector<TValue>& vector)
{
	uint32_t count = 0;
	if (enumerate(handle, &count, nullptr) != VK_SUCCESS)
	{
		Throw(std::runtime_error("enumerate call failed"));
	}

	vector.resize(count);
	if (enumerate(handle, &count, vector.data()) != VK_SUCCESS)
	{
		Throw(std::runtime_error("enumerate call failed"));
	}

	return vector;
}

template <class THandle1, class THandle2, class TValue>
inline std::vector<TValue> GetEnumerateVector(THandle1 handle1, THandle2 handle2, VkResult(enumerate) (THandle1, THandle2, uint32_t*, TValue*), std::vector<TValue>& vector)
{
	uint32_t count = 0;
	if (enumerate(handle1, handle2, &count, nullptr) != VK_SUCCESS)
	{
		Throw(std::runtime_error("enumerate call failed"));
	}

	vector.resize(count);
	if (enumerate(handle1, handle2, &count, vector.data()) != VK_SUCCESS)
	{
		Throw(std::runtime_error("enumerate call failed"));
	}

	return vector;
}
