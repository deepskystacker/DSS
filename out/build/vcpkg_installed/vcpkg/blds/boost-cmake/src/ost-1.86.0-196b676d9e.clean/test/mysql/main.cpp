// Copyright 2023 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt


#include <boost/mysql/tcp.hpp>

#include <boost/asio/system_executor.hpp>

using namespace boost::mysql;

int main()
{
    tcp_connection conn(boost::asio::system_executor{});
    
    // This is always false for a non-connected connection
    return static_cast<int>(conn.uses_ssl()); 
}
