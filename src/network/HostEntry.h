#ifndef Net_HostEntry_INCLUDED
#define Net_HostEntry_INCLUDED


#include "Net.h"
#include "SocketDefs.h"
#include "IPAddress.h"
#include <vector>


namespace pi {


class PIL_API HostEntry
    /// This class stores information about a host
    /// such as host name, alias names and a list
    /// of IP addresses.
{
public:
    typedef std::vector<std::string> AliasList;
    typedef std::vector<IPAddress>   AddressList;

    HostEntry();
        /// Creates an empty HostEntry.

    HostEntry(struct hostent* entry);
        /// Creates the HostEntry from the data in a hostent structure.

#if defined(PIL_HAVE_IPv6) || defined(PIL_HAVE_ADDRINFO)
    HostEntry(struct addrinfo* info);
        /// Creates the HostEntry from the data in an addrinfo structure.
#endif

#if defined(PIL_VXWORKS)
    HostEntry(const std::string& name, const IPAddress& addr);
#endif

    HostEntry(const HostEntry& entry);
        /// Creates the HostEntry by copying another one.

    HostEntry& operator = (const HostEntry& entry);
        /// Assigns another HostEntry.

    void swap(HostEntry& hostEntry);
        /// Swaps the HostEntry with another one.

    ~HostEntry();
        /// Destroys the HostEntry.

    const std::string& name() const;
        /// Returns the canonical host name.

    const AliasList& aliases() const;
        /// Returns a vector containing alias names for
        /// the host name.

    const AddressList& addresses() const;
        /// Returns a vector containing the IPAddresses
        /// for the host.

private:
    std::string _name;
    AliasList   _aliases;
    AddressList _addresses;
};


//
// inlines
//
inline const std::string& HostEntry::name() const
{
    return _name;
}


inline const HostEntry::AliasList& HostEntry::aliases() const
{
    return _aliases;
}


inline const HostEntry::AddressList& HostEntry::addresses() const
{
    return _addresses;
}


inline void swap(HostEntry& h1, HostEntry& h2)
{
    h1.swap(h2);
}


}  // namespace pi


#endif // Net_HostEntry_INCLUDED
