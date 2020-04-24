using SharpPcap;

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
            var filter  = process.Main(args);
            
            //device opening, filter application and main loop over acquired packets, buffer time is set to 1s
            const int readtime = 1000;
            process.Device.Open(DeviceMode.Promiscuous, readtime);
            process.Device.Filter = filter;
            for (var j = 0; j < process.N; j++)
            {
                var packet = process.Device.GetNextPacket();
                //acquired packet is not compatible type of packet
                if (packet == null)
                { 
                    j--; 
                    continue; 
                }
                var packetProcess = new PacketProcessing();
                packetProcess.device_OnPacketArrival(packet);
            }
            process.Device.Close();
        }
    }
}