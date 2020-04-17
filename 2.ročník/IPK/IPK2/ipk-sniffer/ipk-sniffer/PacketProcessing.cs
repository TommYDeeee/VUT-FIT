using System;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using PacketDotNet;
using SharpPcap;


namespace ipk2
{
    public class PacketProcessing
    {
        private DateTime _time;
        private int _len = 0, _srcPort = 0, _dstPort = 0;
        private string _srcIp = "", _dstIp = "";

        //arrival packet processing
        public void device_OnPacketArrival(RawCapture packet)
        {
            //default values initialized
            _time = packet.Timeval.Date;
            _len = packet.Data.Length;

            //parse raw packet
            var e = Packet.ParsePacket(packet.LinkLayerType, packet.Data);
            //get IP addresses of (IP layer) packet
            var ipPacket = e.Extract<PacketDotNet.IPPacket>();
            _srcIp = ipPacket.SourceAddress.ToString();
            _dstIp = ipPacket.DestinationAddress.ToString();
            
            //try to resolve hostname, if not found use IP address
            try
            {
                _srcIp = Dns.GetHostEntry(_srcIp).HostName;
            }
            catch
            {
                // ignored
            }

            try
            {
                _dstIp = Dns.GetHostEntry(_dstIp).HostName;
            }
            catch
            {
                // ignored
            }

            //extraction of TCP packet
            var tcp = e.Extract<PacketDotNet.TcpPacket>();
            if (tcp != null)
            {
                //get source and destination ports for tcp packet
                _srcPort = tcp.SourcePort;
                _dstPort = tcp.DestinationPort;
                
                //write header with necessary info
                WriteHeader();
                
                //process whole TCP packet
                PacketsBytesProcess(e);
            }
            
            //extraction of UDP packet
            var udp = e.Extract<PacketDotNet.UdpPacket>();
            if (udp != null)
            {
                //get source and destination ports for udp packet
                int srcPort = udp.SourcePort;
                int dstPort = udp.DestinationPort;
                
                //write header with necessary info
                WriteHeader();
                
                //process whole UDP packet
                PacketsBytesProcess(e);
            }
        }

        //write output message header in correct format
        private void WriteHeader()
        {
            Console.WriteLine("{0}:{1}:{2}.{3} {5}:{6} > {7}:{8}",
                _time.Hour, _time.Minute, _time.Second, _time.Millisecond, _len,
                _srcIp, _srcPort, _dstIp, _dstPort);
            Console.WriteLine();
        }
        
        //process packet bytes
        private void PacketsBytesProcess(Packet packet)
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
        private void WriteString(int data, string text, string hex, int index)
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