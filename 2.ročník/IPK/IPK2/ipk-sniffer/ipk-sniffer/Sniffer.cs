namespace ipk2
{
    internal static class Sniffer
    {
        //Process arguments and main loop over packets
        public static void Main(string[] args)
        {
            var process = new Args();
            process.Main(args);
        }
    }
}