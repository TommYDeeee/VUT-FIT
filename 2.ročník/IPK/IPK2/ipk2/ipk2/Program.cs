using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using CommandLine;
using CommandLine.Text;
using ipk2;
using static System.Int32;
using SharpPcap;
using SharpPcap.LibPcap;
using SharpPcap.Npcap;
using PacketDotNet;
using SharpPcap.WinPcap;

class Program
{

    public static void Main(string[] args)
    {
        main test = new main();
        test.Main(args);
    }
}