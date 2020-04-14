namespace ipk2
{
    /*
     simple C# script for sniffing, parsing and visualizing TCP or UDP packets
    "Author: Tomáš Ďuriš (xduris05)
    "FIT VUT, 17.4.2020
    */
    
    internal static class Sniffer
    {
        //process arguments and main loop over packets
        public static void Main(string[] args)
        {
            var process = new Args();
            process.Main(args);
        }
    }
}