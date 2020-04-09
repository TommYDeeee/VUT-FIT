using System;
using System.Collections.Generic;
using System.Linq;
using CommandLine;
using CommandLine.Text;
using static System.Int32;

class Program
{

    static void Main(string[] args)
    {
        bool tcp = false, udp = false, n_val = false, i_val = false, p_val = false;
        bool have_n = false, have_i = false, have_p = false;
        int? p = null;
        int n = 1;
        string i = null;
        
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
                    n = Parse(arg);
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
            Console.WriteLine("TCP&UDP");
        }
        else if(tcp)
        {
            Console.WriteLine("tcp");
        }
        else
        {
            Console.WriteLine("udp");

        }

        if (i == null)
        {
            Console.WriteLine("ALL INTERFACES");
        }
        else
        {
            Console.WriteLine(i);
        }
        Console.WriteLine(n);


        if (have_p)
        {
            Console.WriteLine(p);
        }
        else
        {
            Console.WriteLine("ALL PORTS");
        }
    }

    static void Exit()
    {
        Environment.Exit(1);
    }
}