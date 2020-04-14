using System;
using System.Collections.Generic;
using System.IO;
using CommandLine.Text;
using static System.Int32;
using SharpPcap;


namespace ipk2

{
    //argument parsing and loop over acquired packets
    public class Args
    {
        private int _n = 1;
        private ICaptureDevice _device;
        
        //arguments processing
        public void Main(IEnumerable<string> args)
        {
            //bool variables and default values initialized for further use
            bool tcp = false, udp = false, nVal = false, iVal = false, pVal = false;
            bool haveN = false, haveI = false, haveP = false;
            int? p = null;
            string i = null; 
            var filter = "";
            
            //loop over arguments and parsing them, catching errors
            foreach (var arg in args)
            {
                if(pVal)
                {
                    try
                    {
                        p = Parse(arg);
                        if(p <= 0 || p > 65535)
                        {
                            Exit("Port number not in valid range 1-65535");
                        }
                    }
                    catch
                    {
                        Exit("Invalid port number");
                    }
                    haveP = true;
                    pVal = false;
                    continue;
                }
                if(iVal)
                {
                    i = arg;
                    haveI = true;
                    iVal = false;
                    continue;
                }
                if(nVal)
                {
                    try
                    {
                        _n = Parse(arg);
                    }
                    catch
                    {
                        Exit("Invalid number of packets provided");
                    }
                    haveN = true;
                    nVal = false;
                    continue;
                }
                switch (arg)
                {
                    case "-h":
                    case "--help":
                        Console.WriteLine(Help());
                        return;
                    case "-i":
                        if (haveI)
                        {
                            Exit("Multiple interface arguments");
                        }
                        iVal = true;
                        continue;
                    case "-p":
                        if (haveP)
                        {
                            Exit("Multiple port arguments");
                        }
                        pVal = true;
                        continue;
                    case "-n":
                        if (haveN)
                        {
                            Exit("Multiple number of packets arguments");
                        }
                        nVal = true;
                        continue;
                    case "-u":
                    case "--udp":
                        udp = true;
                        continue;
                    case "-t":
                    case "--tcp":
                        tcp = true;
                        continue;
                    default:
                        Exit("Invalid argument");
                        break;
                }
            }

            //if either both or none of tcp/udp are provided, no filter is required for packet protocols. 
            //if they are then filter is used
            if ((!tcp & !udp) | tcp & udp)
            {
                filter += "";
            }
            else if(tcp)
            {
                filter += "tcp ";
            }
            else
            {
                filter += "udp ";

            }

            //filter for port number if required
            if (p == null)
            {
                filter += "";
            }
            else
            {
                filter += $"port {p}";
            }
            
            //if no interface provided, list of active devices is print. Otherwise  provided device is initialized for further use
            if (i == null)
            {
                Console.WriteLine("\nAVAILABLE INTERFACES:");
                foreach (var dev in CaptureDeviceList.Instance)
                {
                    Console.WriteLine($"{dev.Name}: {dev.Description}");
                }
                return;
            }
            else
            {
                try
                {
                    _device = CaptureDeviceList.Instance[i];
                }
                catch
                {
                    Exit("Invalid interface name");
                }
            }

            //device opening, filter application and main loop over acquired packets
            const int readtime = 1000;
            _device.Open(DeviceMode.Normal, readtime);
            _device.Filter = filter;
            for (var j = 0; j < _n; j++)
            {
                var packet = _device.GetNextPacket();
                PacketProcessing.device_OnPacketArrival(packet);
            }
        }

        //help for users with all necessary info
        private static string Help()
        {
            const string help = "\nINFO:\n" +
                                "simple C# script for sniffing, parsing and visualizing TCP or UDP packets\n" +
                                "Author: Tomáš Ďuriš (xduris05)\n" +
                                "FIT VUT, 17.4.2020\n\n" +
                                "USAGE:\n" +
                                "./ipk-sniffer -i interface [-p port] [--tcp|-t] [--udp|-u] [-n num]\n\n" +
                                "ARGUMENTS:\n" +
                                "-i : interface must be a valid interface, to see valid interfaces please use script without this argument, INTERFACE on which packets are being sniffed\n" +
                                "-p : port must be a valid number in range 0 - 65535, PORT on which are packets being sniffed\n" +
                                "-t|--tcp : only tcp packets are being showed\n" +
                                "-u|--udp : only udp packets are being showed\n" +
                                "if both -t|--tcp and -u|--udp or none are present, both tcp and udp packets are processed\n" +
                                "-n : number must be an integer, NUMBER of packets that are processed\n";
            return help;
        }

        //exit on error
        private static void Exit(string message)
        {
            var e = Console.Error;
            e.WriteLine($"\nERROR: {message}");
            e.WriteLine(Help());
            Environment.Exit(1);
        }
    }
}