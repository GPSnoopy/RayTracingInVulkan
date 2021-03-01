#pragma once

#include <boost/exception/all.hpp>
#include <boost/stacktrace.hpp>
#include <stdexcept>
#undef APIENTRY

typedef boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace> traced;

template <class E>
[[noreturn]] inline void Throw(const E& e) noexcept(false)
{
	throw boost::enable_error_info(e) << traced(boost::stacktrace::stacktrace());
}
