using System.Collections.Specialized;

namespace ipk2
{
    //DNS cache class, implemented to avoid unwanted DNS request loop packets
    //circular dictionary to avoid high memory load if program is running long time
    //oldest value its removed and new value is added if the cache is full (FIFO)
    public static class DnsCache
    {
        //Ordered dictionary to know the insertion order, so its possible to remove oldest value
        private static readonly OrderedDictionary Cache = new OrderedDictionary();
        
        //add new dns record (IP->Hostname)
        //if hostname is unknown (IP->IP)
        public static void AddDnsRecord(string key, string value)
        {
            //max size of cache (primary number), should be totally enough even if long run and also memory friendly
            //if its full remove oldest
            if (Cache.Count == 101)
            {
                Cache.RemoveAt(0);
                Cache.Add(key, value);
            }
            //else just add value
            else
            {
                Cache.Add(key, value);
            }
        }

        //get DnsRecord from dictionary by key
        public static object GetDnsRecord(string key)
        {
            return Cache[key];
        }
    }
}