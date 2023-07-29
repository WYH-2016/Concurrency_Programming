// lookup table usally has three way to implement
//  - binary tree, such as RB-Tree
//  - ordered array
//  - Hash table
// In order to realize a thread safe lookup table, Hash table is suitable

#include <mutex>
#include <list>
#include <vector>
#include <map>

template< typename Key, typename Value, typename Hash = std::hash<Key> >
class threadsafe_lookup_table
{
private:
    class bucket_type
    {
    private:
        typedef std::pair<Key, Value>   bucket_value;
        typedef std::list<bucket_value> bucket_data;
        typedef typename bucket_data::iterator bucket_iterator;

        bucket_data data;
        mutable std::shared_mutex bucket_mutex;

        bucket_iterator find_entry_for( Key const& key ) const
        {
            return std::find_if( data.begin(), 
                                 data.end(), 
                                 [&](bucket_value const& item){ return item.first == key; } );
        }
    public:
        Value value_for( Key const& key, Value const& defualt_value ) const
        {
            std::shared_lock< std::shared_mutex > lock( bucket_mutex );
            bucket_iterator const found_entry = find_entry_for( key );
            return ( found_entry == data.end() ? default_value : found_entry->second );
        }

        void add_or_update_mapping( Key const& key, Value const& value )
        {
            std::unique_lock< std::shared_mutex > lock( bucket_mutex );
            buket_iterator const found_entry = find_entry_for( key );
            if( found_entry == data.end() )
            {
                data.push_back( bucket_value( key, value ) );
            }
            else
            {
                found_entry->second = value;
            }
        }

        void remove_mapping( Key const& key )
        {
            std::unique_lock< std::shared_mutex > lock( bucket_mutex );
            bucket_iterator const found_entry = find_entry_for( key );
            if( found_entry != data.end() )
            {
                data.erase( found_entry );
            }
        }
    };

    std::vector< std::unique_ptr<bucket_type> > buckets;
    Hash hasher;
    bucket_type& get_bucket( Key const& key ) const
    {
        std::size_t const bucket_index = hasher( key ) % buckets.size();
        return *buckets[ bucket_index ];
    }

public:
    typedef     Key     key_type;
    typedef     Value   value_type;
    typedef     Hash    hash_type;

    threadsafe_lookup_table( unsigned num_buckets = 19, Hash const& _hasher = Hash()):
      buckets( num_buckets ), hasher( _hasher )
    {
        for( unsigned i = 0; i < num_buckets; i++)
        {
            buckets[i].reset( new bucket_type );
        }
    }
    threadsafe_lookup_table( threadsafe_lookup_table const & ) = delete;
    threadsafe_lookup_table& operator=( threadsafe_lookup_table const & ) = delete;

    Value value_for( Key const& key, Value const& default_value ) const
    {
        return get_bucket( key ).value_for( key, default_value );
    }

    void add_or_update_mpping( Key const& key, Value const& value )
    {
        return get_bucket( key ).add_or_update_mpping( key, value );
    }

    Value remove_mapping( Key const& key )
    {
        return get_bucket( key ).remove_mapping( key );
    }

    std::map< key_type, value_type> get_map() const
    {
        std::vector< std::unique_lock<std::shared_mutex> > locks;
        for( unsigned i = 0; i < buckets.size(); i++)
        {
            locks.push_back( std::shared_lock<std::shared_mutex> ( buckets[i].bucket_mutex ) );
        }
        std::map< key_type, value_type > res;
        for( unsigned i = 0; i < buckets.size(); i++ )
        {
            for( bucket_iterator it = buckets[i].data.begin(); it != bucket[i].date.end(); i++)
            {
                res.insert( *it );
            }
        }
        return res;
    }
};
