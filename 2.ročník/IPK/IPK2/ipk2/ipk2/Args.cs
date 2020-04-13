using System;
using System.Collections.Generic;
using static System.Int32;
using SharpPcap;


namespace ipk2

{
    public class Args
    {
        private int _n = 1;
        private ICaptureDevice _device;
        
        //Arguments processing
        public void Main(IEnumerable<string> args)
        {
            //Bool variables and default values initialized for further use
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
                    }
                    catch
                    {
                        Exit();
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
                        this._n = Parse(arg);
                    }
                    catch
                    {
                        Exit();
                    }
                    haveN = true;
                    nVal = false;
                    continue;
                }
                switch (arg)
                {
                    case "-i":
                        if (haveI)
                        {
                            Exit();
                        }
                        iVal = true;
                        continue;
                    case "-p":
                        if (haveP)
                        {
                            Exit();
                        }
                        pVal = true;
                        continue;
                    case "-n":
                        if (haveN)
                        {
                            Exit();
                        }
                        nVal = true;
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

            //If either both or none of tcp/udp are provided, no filter is required for packet protocols. 
            //If they are then filter is used
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

            //Filter for port number if required
            if (p == null)
            {
                filter += "";
            }
            else
            {
                filter += $"port {p}";
            }
            
            //If no interface provided, list of active devices is print. Otherwise  provided device is initialized for further use
            if (i == null)
            {
                foreach (var dev in CaptureDeviceList.Instance)
                {
                    Console.WriteLine(dev.Name);
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
                    Exit();
                }
            }

            //Device opening, filter application and main loop over acquired
            const int readtime = 1000;
            _device.Open(DeviceMode.Normal, readtime);
            _device.Filter = filter;
            for (var j = 0; j < this._n; j++)
            {
                var packet = _device.GetNextPacket();
                PacketProcessing.device_OnPacketArrival(packet);
            }
        }

        //Exit on error
        private static void Exit()
        {
            Environment.Exit(1);
        }
    }
}