using System;
using System.Net;
using PacketDotNet;
using SharpPcap;


namespace ipk2
{
    //processing arrived packets, getting source/ destination IP address, port number and all necessary info
    //each layer is extracted to get important info. Data from raw packets are then being handled to further process
    //! https://github.com/chmorgan/sharppcap/blob/master/Examples/Example6.DumpTCP/Example6.DumpTCP.cs !
    //! while writing this part, i was inspired this official example from documentation. I did not just copy any part of that, by there could be some 
    // similarity, it helped me to understand how this should be done. !
    public class PacketProcessing
    {
        //default values
        private DateTime _time;
        private int _srcPort = 0, _dstPort = 0;
        private string _srcIp = "", _dstIp = "", _text = "", _hex = "";

        //arrival packet processing
        public void device_OnPacketArrival(RawCapture packet)
        {
            //time value get from packet
            _time = packet.Timeval.Date;
            //parse raw packet
            var parsedRawPacked = Packet.ParsePacket(packet.LinkLayerType, packet.Data);
            //get IP addresses of (IP layer) packet
            var ipPacket = parsedRawPacked.Extract<PacketDotNet.IPPacket>();
            _srcIp = ipPacket.SourceAddress.ToString();
            _dstIp = ipPacket.DestinationAddress.ToString();

            
            //try to get host name from local DNS cache
            _srcIp = IpToHost(_srcIp);
            _dstIp = IpToHost(_dstIp);
            

            //extraction of TCP packet
            var tcp = parsedRawPacked.Extract<PacketDotNet.TcpPacket>();
            if (tcp != null)
            {
                //get source and destination ports for tcp packet
                _srcPort = tcp.SourcePort;
                _dstPort = tcp.DestinationPort;
                //write header with necessary info
                WriteHeader();
                //process whole TCP packet
                var headerLength = (packet.Data.Length - tcp.PayloadData.Length);
                PacketsBytesProcess(packet, headerLength);
            }

            //extraction of UDP packet
            var udp = parsedRawPacked.Extract<PacketDotNet.UdpPacket>();
            if (udp != null)
            {
                //get source and destination ports for udp packet
                _srcPort = udp.SourcePort;
                _dstPort = udp.DestinationPort;
                //write header with necessary info
                WriteHeader();
                //process whole UDP packet
                var headerLength = (packet.Data.Length - udp.PayloadData.Length);
                PacketsBytesProcess(packet, headerLength);
            }
            
            //Additional part for IGMP/ICMP/ICMPv6 packet processing
            //for IGMP/ICMP/ICMPv6 packets we use default 0 port, because they dont work with port numbers
            //IGMP packet processing
            var igmp = parsedRawPacked.Extract<PacketDotNet.IgmpV2Packet>();
            if (igmp != null)
            {
                //write header with necessary info
                WriteHeader();
                //process whole IGMP packet
                var headerLength = (packet.Data.Length - igmp.Bytes.Length);
                PacketsBytesProcess(packet, headerLength);
            }
            
            //ICMP packet processing
            var icmp = parsedRawPacked.Extract<PacketDotNet.IcmpV4Packet>();
            if (icmp != null)
            {
                //write header with necessary info
                WriteHeader();
                //process whole ICMP packet
                var headerLength = (packet.Data.Length - icmp.Bytes.Length);
                PacketsBytesProcess(packet, headerLength);
            }

            //ICMPv6 packet processing
            var icmp6 = parsedRawPacked.Extract<PacketDotNet.IcmpV6Packet>();
            if (icmp6 != null)
            {
                //write header with necessary info
                WriteHeader();
                //process whole ICMP6 packet
                var headerLength = (packet.Data.Length - icmp6.Bytes.Length);    
                PacketsBytesProcess(packet, headerLength);
            }

        }

        //write output message header in correct format
        private void WriteHeader()
        {
            Console.WriteLine("{0:00}:{1:00}:{2:00}.{3} {4} : {5} > {6} : {7}", _time.Hour, _time.Minute, _time.Second,
                _time.Millisecond,
                _srcIp, _srcPort, _dstIp, _dstPort);
            Console.WriteLine();
        }

        //process packet bytes
        private void PacketsBytesProcess(RawCapture packet, int headerLength)
        {
            //default values initialized for counters and header bool value
            var counterHex16 = 0;
            var index = 0;
            var header = false;
            //loop over individual bytes
            foreach (var dataToPrint in packet.Data)
            {
                //convert to hex format
                var data = dataToPrint.ToString("X2").ToLower();
                //every 16 bytes write output line with necessary info and clear values
                if ((counterHex16 % 16 == 0) && counterHex16 != 0)
                {
                    WriteString(index, counterHex16);
                    _hex = "";
                    index += 16;
                }
                //append hex bytes into string
                if (counterHex16 % 8 == 0)
                {
                    _hex += " ";
                    _text += " ";
                }
                _hex = _hex + data + " ";
                //try to convert hex to ASCII, non-printable chars exchange for "."
                if (Convert.ToInt32(data, 16) >= 127 || Convert.ToInt32(data, 16) < 32)
                {
                    _text += ".";
                }
                else
                {
                    _text += (char) Convert.ToInt32(data, 16);
                }
                //increment line counter
                counterHex16++;
                //process last line of header
                if ((headerLength == counterHex16) && (header == false))
                {
                    AlignGenerate(counterHex16, index);
                    counterHex16 = 0;
                    index = headerLength;
                    header = true;
                    //if packet contains more then just a header, print newline to differentiate parts
                    if (packet.Data.Length != headerLength)
                    {
                        Console.WriteLine();
                    }
                }

                //if last line was process, don't forget to print it
                if ((counterHex16 + headerLength == packet.Data.Length) && header)
                {
                    if (_hex == "")
                    {
                        continue;
                    }
                    AlignGenerate(counterHex16, index);
                }
            }
            Console.Write("\n");
        }

        //Alignment generation for last line in header segment or body segment
        private void AlignGenerate(int data, int index)
        {
            //generated alignment for header (if number of bytes is <= 8 in that row)
            if ((((data / 8) % 2 == 0) || (data % 8 == 0)) && data % 16 != 0)
            {
                _hex += " ";
            }
            WriteString(index, data);
        }
        
        //append provided info to one string and print it on output. If needed, generate necessary alignment for better readability
        private void WriteString(int data, int index)
        {
            var alignment = "";
            //alignment generation
            if (index % 16 != 0)
            {
                for (var i = 0; i < 16 - (index % 16); i++)
                {
                    alignment += "   ";
                }
            }
            //final output line
            var dataCount = "0x" + data.ToString("X4") + ": " + _hex + alignment + _text;
            Console.WriteLine(dataCount);
            _text = "";
            _hex = "";
        }
        
        //method to resolve hostname from given IP address from local DNS cache or from DNS function GetHostEntry()
        private static string IpToHost(string ip)
        {
            string host;
            //try to get hostname from local DNS if it already contains given IP
            if (DnsCache.GetDnsRecord(ip) != null)
            {
                host = DnsCache.GetDnsRecord(ip).ToString();
            }
            //if the record does not exist try to get hostname from DNS function
            else
            {
                host = ip;
                try
                {
                    host = Dns.GetHostEntry(ip).HostName;
                }
                catch
                {
                    // ignored
                }
                //if successful add to local DNS (IP->hostname)
                //if not add (IP->IP) to also avoid loops for unknown IPs
                DnsCache.AddDnsRecord(ip, host);
            }
            return host;
        }
    }
    
}