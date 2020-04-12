using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;
using CommandLine;
using CommandLine.Text;
using ipk2;
using static System.Int32;
using SharpPcap;
using SharpPcap.LibPcap;
using SharpPcap.Npcap;
using PacketDotNet;
using SharpPcap.WinPcap;

namespace ipk2

{
    public class main
    {
        private int n = 1;
        private ICaptureDevice device = null;
        public void Main(string[] args)
        {
            main test = new main();
            bool tcp = false, udp = false, n_val = false, i_val = false, p_val = false;
            bool have_n = false, have_i = false, have_p = false;
            int? p = null;
            string i = null; 
            string filter = "";
            
            foreach (var arg in args)
            {
                if(p_val)
                {
                    try
                    {
                        p = Parse(arg);
                    }
                    catch
                    {
                        Exit();
                    }
                    have_p = true;
                    p_val = false;
                    continue;
                }
                if(i_val)
                {
                    i = arg;
                    have_i = true;
                    i_val = false;
                    continue;
                }
                if(n_val)
                {
                    try
                    {
                        this.n = Parse(arg);
                    }
                    catch (Exception)
                    {
                        Exit();
                    }
                    have_n = true;
                    n_val = false;
                    continue;
                }
                switch (arg)
                {
                    case "-i":
                        if (have_i)
                        {
                            Exit();
                        }
                        i_val = true;
                        continue;
                    case "-p":
                        if (have_p)
                        {
                            Exit();
                        }
                        p_val = true;
                        continue;
                    case "-n":
                        if (have_n)
                        {
                            Exit();
                        }
                        n_val = true;
                        continue;
                    case "-u":
                        udp = true;
                        continue;
                    case "--udp":
                        udp = true;
                        continue;
                    case "-t":
                        tcp = true;
                        continue;
                    case "--tcp":
                        tcp = true;
                        continue;
                    default:
                        Exit();
                        break;
                }
            }

            if ((!tcp & !udp) | tcp & udp)
            {
                filter = "";
            }
            else if(tcp)
            {
                filter += "tcp ";
            }
            else
            {
                filter += "udp ";

            }

            if (p == null)
            {
                filter = "";
            }
            else
            {
                filter += $"port {p}";
            }
            
            if (i == null)
            {
                device = CaptureDeviceList.Instance["any"];
            }
            else
            {
                try
                {
                    device = CaptureDeviceList.Instance[i];
                }
                catch
                {
                    Exit();
                }
            }

            int readtime = 1000;
            device.Open(DeviceMode.Normal, readtime);
            device.Filter = filter;
            Console.WriteLine(device.Filter);
            for (int j = 0; j < this.n; j++)
            {
                RawCapture packet = device.GetNextPacket();
                device_OnPacketArrival(packet);
            }
        }

        private void device_OnPacketArrival(RawCapture packet)
        {
            var time = packet.Timeval.Date;
            var len = packet.Data.Length;
            Packet e = Packet.ParsePacket(packet.LinkLayerType, packet.Data);
            var tcp = e.Extract<PacketDotNet.TcpPacket>();
            if (tcp != null)
            {
                var ipPacket = (PacketDotNet.IPPacket)tcp.ParentPacket;
                string srcIp = ipPacket.SourceAddress.ToString();
                string dstIp = ipPacket.DestinationAddress.ToString();
                try
                {
                    srcIp = Dns.GetHostEntry(srcIp).HostName.ToString();
                }
                catch{}

                try
                {
                    dstIp = Dns.GetHostEntry(srcIp).HostName.ToString();
                }
                catch {}
                int srcPort = tcp.SourcePort;
                int dstPort = tcp.DestinationPort;

                Console.WriteLine("{0}:{1}:{2}.{3} {5}:{6} > {7}:{8}",
                    time.Hour, time.Minute, time.Second, time.Millisecond, len,
                    srcIp, srcPort, dstIp, dstPort);
                int data_10 = 0;
                String text = "";
                String hex = "";
                int index = 0;
                foreach (var data_to_print in tcp.Bytes)
                {
                    var data = data_to_print.ToString("X2");
                    if ((data_10 % 16 == 0) && data_10 != 0)
                    {
                        WriteString(index, text, hex);
                        text = "";
                        hex = "";
                        index = index + 16;
                    }
                    hex = hex + data + " ";
                    if (Convert.ToInt32(data, 16) > 127 || Convert.ToInt32(data, 16) < 32)
                    {
                        text = text + ".";
                    }
                    else
                    {
                        text = text + (char) Convert.ToInt32(data, 16);
                    }
                    data_10++;
                    if (data_10 == 10)
                    {
                        Console.Write("\n");
                    }
                    if (data_10 == tcp.Bytes.Length)
                    {
                        WriteString(index, text, hex);
                        text = "";
                        hex = "";
                    }
                }
                Console.Write("\n");
            }
            var udp = e.Extract<PacketDotNet.UdpPacket>();
            if (udp != null)
            {
                var ipPacket = (PacketDotNet.IPPacket)udp.ParentPacket;
                string srcIp = ipPacket.SourceAddress.ToString();
                string dstIp = ipPacket.DestinationAddress.ToString();
                try
                {
                    srcIp = Dns.GetHostEntry(srcIp).HostName.ToString();
                }
                catch{}

                try
                {
                    dstIp = Dns.GetHostEntry(srcIp).HostName.ToString();
                }
                catch {}
                int srcPort = udp.SourcePort;
                int dstPort = udp.DestinationPort;

                Console.WriteLine("{0}:{1}:{2}.{3} {5}:{6} > {7}:{8}",
                    time.Hour, time.Minute, time.Second, time.Millisecond, len,
                    srcIp, srcPort, dstIp, dstPort);
                int data_10 = 0;
                String text = "";
                String hex = "";
                int index = 0;
                foreach (var data_to_print in udp.Bytes)
                {
                    var data = data_to_print.ToString("X2");
                    if ((data_10 % 16 == 0) && data_10 != 0)
                    {
                        WriteString(index, text, hex);
                        text = "";
                        hex = "";
                        index = index + 16;
                    }
                    hex = hex + data + " ";
                    if (Convert.ToInt32(data, 16) > 127 || Convert.ToInt32(data, 16) < 32)
                    {
                        text = text + ".";
                    }
                    else
                    {
                        text = text + (char) Convert.ToInt32(data, 16);
                    }
                    data_10++;
                    if (data_10 == 10)
                    {
                        Console.Write("\n");
                    }
                    if (data_10 == udp.Bytes.Length)
                    {
                        WriteString(index, text, hex);
                        text = "";
                        hex = "";
                    }
                }
                Console.Write("\n");
            }
        }

        private void WriteString(int data, string text, string hex)
        {
            var data_count = "0x" + data.ToString("X4")+ ": " + hex + text + "\n";
            Console.Write(data_count);
        }
        private void Exit()
        {
            Environment.Exit(1);
        }
    }
}