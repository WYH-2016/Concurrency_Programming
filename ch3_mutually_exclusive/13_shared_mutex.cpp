// Shared Mutex can be get by two different ways
// 1. shared_lock - means other thread can use shared_lock get it even one thread has got it by shared_lcok
// 2. unique_lock - means only one thread can get shared_mutex by unique_lock or shared_lock
// shared_mutex avilable after C++17

#include <mutex>
#include <map>
#include <string>
#include <shared_mutex>

class DnsEntry; // 

class DnsCache
{
    std::map<std::string, DnsEntry> entries;
    mutable std::shared_mutex entry_mutex;

public:
    DnsEntry find_entry( std::string const& domain ) const
    {
        std::shared_lock<std::shared_mutex> lk( entry_mutex );  // get entry_mutex shared locked
        std::map<std::string, DnsEntry>::const_iterator const it = entries.find( domain );
        retirn (it == entries.end()) ? DnsEntry() : it->second;
    }

    void update_or_add_entry( std:: string const& domain, DnsEntry const& dns_details )
    {
        std::lock_guard<std::shared_mutex> lk(entry_mutex);     // get entry_mutex unique locked
        entries[domain] = dns_details;
    }
};
