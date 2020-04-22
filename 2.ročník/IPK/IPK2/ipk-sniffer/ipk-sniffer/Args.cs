using System;
using System.Collections.Generic;
using static System.Int32;
using SharpPcap;


namespace ipk2

{
    //argument parsing, filter setting, help and exit methods
    public class Args
    {
        public int N = 1;
        public ICaptureDevice Device;
        
        //arguments processing
        public string Main(IEnumerable<string> args)
        {
            //bool variables and default values initialized for further use
            bool tcp = false, udp = false, igmp = false, icmp = false, icmp6 = false, nVal = false, iVal = false, pVal = false;
            bool haveN = false, haveI = false, haveP = false;
            int? p = null;
            string i = null;
            var filter = "";

            //loop over arguments and parsing them, catching errors
            //each argument could be given only once
            foreach (var arg in args)
            {
                if (pVal)
                {
                    try
                    {
                        p = Parse(arg);
                        //should be valid port number
                        if (p <= 0 || p > 65535)
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

                if (iVal)
                {
                    i = arg;
                    haveI = true;
                    iVal = false;
                    continue;
                }

                if (nVal)
                {
                    try
                    {
                        N = Parse(arg);
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
                        Environment.Exit(0);
                        break;
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
                        if (udp)
                        {
                            Exit("Parameter udp was already given");
                        }
                        udp = true;
                        continue;
                    case "-t":
                    case "--tcp":
                        if (tcp)
                        {
                            Exit("Parameter tcp was already given");
                        }
                        tcp = true;
                        continue;
                    case "-ig":
                    case "--igmp":
                        if (igmp)
                        {
                            Exit("Parameter igmp was already given");
                        }
                        igmp = true;
                        continue;
                    case "-ic":
                    case "--icmp":
                        if (icmp)
                        {
                            Exit("Parameter icmp was already given");
                        }
                        icmp = true;
                        continue;
                    case "-ic6":
                    case "--icmp6":
                        if (icmp6)
                        {
                            Exit("Parameter icmp was already given");
                        }
                        icmp6 = true;
                        continue;
                    default:
                        Exit("Invalid argument");
                        break;
                }
            }
            
            //Additional IGMP/ICMP processing
            
            //complex argument managing by hand, because of additional IGMP or ICMP/ICMPv6 packet processing and multiple combination 
            //with UDP or TCP packets and specified ports (just for TCP or UDP packets) to implement complex and easy to use sniffing application
            
            //regular assignment
            
            //if either both or none of tcp/udp are provided, combined filter for processing TCP or UDP packets is used 
            //if they are then specific filter is used
            // This part is just for igmp/icmp with or without tcp/udp with or without ports combinations
            if (igmp || icmp || icmp6)
            {
                var igmpIcmpFilter = "";
                var icmpFilter = "";
                if (!udp && !tcp)
                {
                    if (p != null)
                    {
                        Exit("You cant have port number filter just on igmp/icmp/icmpv6 packet");
                    }
                    igmpIcmpFilter += "(";
                }
                if (icmp || icmp6)
                {
                    if (icmp && icmp6)
                    {
                        icmpFilter += "(icmp or icmp6";
                    }
                    else if (icmp)
                    {
                        icmpFilter += "(icmp";
                    }
                    else
                    {
                        icmpFilter += "(icmp6";
                    }
                }
                if((icmp || icmp6) && igmp)
                {
                    igmpIcmpFilter += icmpFilter + " or igmp";
                }
                else if(igmp)
                {
                    igmpIcmpFilter += "(igmp";
                }
                else
                {
                    igmpIcmpFilter += icmpFilter;
                }
                
                if (!tcp && !udp)
                {
                    filter += igmpIcmpFilter + ""; 
                }
                else
                {
                    if (tcp && udp)
                    {
                        filter += igmpIcmpFilter + " or ((tcp or udp)";
                    }
                    else if (udp)
                    {
                        filter += igmpIcmpFilter + " or (udp";
                    }
                    else
                    {
                        filter += igmpIcmpFilter + " or (tcp";
                    }
                }
            }
            //End of additional part
            //Regular assignment
            else
            {
                if ((!tcp & !udp) | tcp & udp)
                {
                    filter += "(((tcp or udp)";
                }
                else if (tcp)
                {
                    filter += "((tcp";
                }
                else if(udp)
                {
                    filter += "((udp";
                }
            }
            
            //filter for port number if required
            if (p == null)
            {
                filter += "))";
            }
            else
            {
                filter += $" and port {p}))";
            }

            //if no interface provided, list of active devices is print. Otherwise  provided device is initialized for further use
            if (i == null)
            {
                Console.WriteLine("\nAVAILABLE INTERFACES:");
                foreach (var dev in CaptureDeviceList.Instance)
                {
                    Console.WriteLine($"{dev.Name}: {dev.Description}");
                }
                Environment.Exit(0);
            }
            else
            {
                try
                {
                    Device = CaptureDeviceList.Instance[i];
                }
                catch
                {
                    Exit("Invalid interface name");
                }
            }
            return filter;
        }

        //help for users with all necessary info
        private static string Help()
        {
            const string help = "\nINFO:\n" +
                                "simple C# script for sniffing, parsing and visualizing TCP,UDP, IGMP and ICMP (v4 or v6) packets\n" +
                                "Author: Tomáš Ďuriš (xduris05)\n" +
                                "FIT VUT, 17.4.2020\n\n" +
                                "USAGE:\n" +
                                "./ipk-sniffer -i interface [-p port] [--tcp|-t] [--udp|-u] [-n num] [--icmp|-ic] [--icmp6|-ic6] [--igmp|-ig]\n\n" +
                                "ARGUMENTS:\n" +
                                "-i : interface must be a valid interface, to see valid interfaces please use script without this argument, INTERFACE on which packets are being sniffed\n" +
                                "-p : port must be a valid number in range 0 - 65535, PORT on which are packets being sniffed\n" +
                                "-t|--tcp : only tcp packets are being shown\n" +
                                "-u|--udp : only udp packets are being shown\n" +
                                "if both -t|--tcp and -u|--udp or none are present, both tcp and udp packets are processed\n" +
                                "-n : number must be an integer, NUMBER of packets that are processed\n\n" +
                                "*ADDITIONAL (BONUS) ASSIGNMENT*\n" +
                                "-ig|--igmp : additional igmp packets are being shown\n" +
                                "-ic|--icmp : additional icmp packets are being shown\n" +
                                "-ic6|--icmp6 : additional icmp6 packets are being shown\n" +
                                "additional arguments could be combined with regular arguments and even with regular arguments with port filters (just tcp or udp packets will be filtered with port number)\n";

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