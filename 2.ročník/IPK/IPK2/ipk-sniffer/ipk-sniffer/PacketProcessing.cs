using System;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using PacketDotNet;
using SharpPcap;

namespace ipk2
{
    public static class PacketProcessing
    {
        //arrival packet processing
        public static void device_OnPacketArrival(RawCapture packet)
        {
            //default values initialized
            var time = packet.Timeval.Date;
            var len = packet.Data.Length;
            
            //parse raw packet
            var e = Packet.ParsePacket(packet.LinkLayerType, packet.Data);
            
            //extraction of TCP packet
            var tcp = e.Extract<PacketDotNet.TcpPacket>();
            if (tcp != null)
            {
                //get IP addresses of parent (IP layer) packet
                var ipPacket = (PacketDotNet.IPPacket)tcp.ParentPacket;
                var srcIp = ipPacket.SourceAddress.ToString();
                var dstIp = ipPacket.DestinationAddress.ToString();
                //try to resolve hostname, if not found use IP address, get source and destination ports
                try
                {
                    srcIp = Dns.GetHostEntry(srcIp).HostName;
                }
                catch
                {
                    // ignored
                }

                try
                {
                    dstIp = Dns.GetHostEntry(srcIp).HostName;
                }
                catch
                {
                    // ignored
                }
                
                int srcPort = tcp.SourcePort;
                int dstPort = tcp.DestinationPort;

                //write output message header in correct format
                Console.WriteLine("{0}:{1}:{2}.{3} {5}:{6} > {7}:{8}",
                    time.Hour, time.Minute, time.Second, time.Millisecond, len,
                    srcIp, srcPort, dstIp, dstPort);
                Console.WriteLine();

                //process tcp packet
                PacketsBytesProcess(tcp.ParentPacket);
            }
            
            //extraction of UDP packet
            var udp = e.Extract<PacketDotNet.UdpPacket>();
            if (udp != null)
            {
                //get IP addresses of parent (IP layer) packet
                var ipPacket = (PacketDotNet.IPPacket)udp.ParentPacket;
                var srcIp = ipPacket.SourceAddress.ToString();
                var dstIp = ipPacket.DestinationAddress.ToString();
                
                //try to resolve hostname, if not found use IP address, get source and destination ports
                try
                {
                    srcIp = Dns.GetHostEntry(srcIp).HostName;
                }
                catch
                {
                    // ignored
                }

                try
                {
                    dstIp = Dns.GetHostEntry(srcIp).HostName;
                }
                catch
                {
                    // ignored
                }

                int srcPort = udp.SourcePort;
                int dstPort = udp.DestinationPort;
                
                //write output message header in correct format
                Console.WriteLine("{0}:{1}:{2}.{3} {5}:{6} > {7}:{8}",
                    time.Hour, time.Minute, time.Second, time.Millisecond, len,
                    srcIp, srcPort, dstIp, dstPort);
                Console.WriteLine();
                
                //process UDP packet
                PacketsBytesProcess(udp.ParentPacket);
            }
        }

        //process packet bytes
        private static void PacketsBytesProcess(Packet packet)
        {
            //default values initialized
            var data10 = 0;
            var text = "";
            var hex = "";
            var index = 0;
            
            //loop over individual bytes
            foreach (var dataToPrint in packet.Bytes)
            {
                //convert to hex format
                var data = dataToPrint.ToString("X2");
                
                //every 16 bytes write output line with necessary info and clear values
                if ((data10 % 16 == 0) && data10 != 0)
                {
                    WriteString(index, text, hex, data10);
                    text = "";
                    hex = "";
                    index += 16;
                }
                
                //append hex bytes into string
                hex = hex + data + " ";
                
                //try to convert hex to ASCII, non-printable chars exchange for "."
                if (Convert.ToInt32(data, 16) > 127 || Convert.ToInt32(data, 16) < 32)
                {
                    text += ".";
                }
                else
                {
                    text += (char) Convert.ToInt32(data, 16);
                }

                //increment line counter
                data10++;

                //if last line was process, don't forget to print it
                if (data10 == packet.Bytes.Length)
                {
                    WriteString(index, text, hex, data10);
                    text = "";
                    hex = "";
                }
            }
            Console.Write("\n");
        }
        
        //append provided info to one string and print it on output. If needed, generate necessary alignment for better readability
        private static void WriteString(int data, string text, string hex, int index)
        {
            var alignment = "";
            if (index % 16 != 0)
            {
                for (var i = 0; i < 16 - (index % 16); i++)
                {
                    alignment += "   ";
                }
            }
            var dataCount = "0x" + data.ToString("X4")+ ": " + hex + alignment + text;
            Console.WriteLine(dataCount);
        }
    }
}