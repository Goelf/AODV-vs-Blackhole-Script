/* Blackhole Attack Simulation with AODV Routing Protocol
 * 
 * Network topology is like 2D grid
 * 
 *     n0   n1   n2   n3   n4
 *     n5   n6   n7   n8   n9
 *     n10  n11  n12  n13  n14
 *     n15  n16  n17  n18  n19
 *     n20  n21  n22  n23  n24

 * 
 * Each node is in the range of its immediate adjacent.The number of nodes by default is 25
 * 
 * Output of this file:
 * 1. Generates blackhole.routes file for routing table information and
 * 2. blackhole_aodv.xml file for viewing animation in NetAnim.
 * 
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/applications-module.h"

// routing protocols
#include "ns3/aodv-module.h"

// header file specific to this code
#include "myapp.h"

// added for flow monitoring
#include "ns3/gnuplot.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/flow-monitor-helper.h"

// animator
#include "ns3/netanim-module.h"
//
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>


NS_LOG_COMPONENT_DEFINE ("Blackhole");

using namespace ns3;
void ThroughputMonitor(FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> monitor, Gnuplot2dDataset DataSet, Gnuplot2dDataset DataSet1, Gnuplot2dDataset DataSet2);


int main(int argc, char* argv[])
{
  std::string phyMode ("DsssRate1Mbps");//IEEE 802.11b
  double interval = 0.001; //seconds
  std::string rtslimit = "1500";//bytes
  double distance = 100;
  int m_Xsize = 5;//5 nodes in a row
  int m_Ysize = 5;//5 colums in total


  //enabled for mobility
  int nodeSpeed = 10; // m/s
  int nodePause = 0; // secs

  CommandLine cmd;
  cmd.AddValue ("x-size", "The number of nodes in a row grid", m_Xsize);
  cmd.AddValue ("y-size", "The number of rows in a grid", m_Ysize);
  cmd.AddValue ("distance", "distance (m)", distance);
  cmd.AddValue ("rtslimit", "RTS/CTS threshold in bytes", rtslimit);
  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  cmd.Parse (argc, argv);

  //load the topology
  NS_LOG_DEBUG ("Grid:" << m_Xsize << "*" << m_Ysize);
  // Convert to time object
  Time interPacketInterval = Seconds (interval);
  // turn off RTS/CTS for frames below 2200 bytes
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue (rtslimit));
  // Fix non-unicast data rate to be the same as that of unicast
  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (phyMode));

  //create malicious nodes and legitimate nodes
  NS_LOG_INFO ("Create nodes.");

  NodeContainer c; // ALL Nodes
  NodeContainer not_malicious;
  NodeContainer malicious;
  c.Create(m_Ysize*m_Xsize);
  using namespace std;

//All nodes are not malicious
/*
  int n = 0;
  while(n<=24){
	  not_malicious.Add(c.Get(n));
      n++;
  }
*/
/*
// Only one attacker
  int n = 3;
  while(n<=24){
  	  not_malicious.Add(c.Get(n));
      n++;
    }
  not_malicious.Add(c.Get(0));
  not_malicious.Add(c.Get(1));
  malicious.Add(c.Get(2));
*/

/*
  // three attackers
  int n1 = 0;
  while(n1<=4){
    not_malicious.Add(c.Get(n1));
    n1++;
   }
   int n2 = 14;
    while(n2<=24){
  	  not_malicious.Add(c.Get(n2));
  	  n2++;
   }
    not_malicious.Add(c.Get(6));
    not_malicious.Add(c.Get(7));
    not_malicious.Add(c.Get(8));
    not_malicious.Add(c.Get(9));
    not_malicious.Add(c.Get(11));
    not_malicious.Add(c.Get(12));

    malicious.Add(c.Get(5));
    malicious.Add(c.Get(10));
    malicious.Add(c.Get(13));
*/

//Five attackers

 not_malicious.Add(c.Get(8));
 not_malicious.Add(c.Get(10));
 not_malicious.Add(c.Get(11));
 not_malicious.Add(c.Get(12));
 not_malicious.Add(c.Get(13));
 not_malicious.Add(c.Get(22));
 not_malicious.Add(c.Get(23));
 not_malicious.Add(c.Get(24));

 int n1 = 0;
 while(n1<=6){
   not_malicious.Add(c.Get(n1));
   n1++;
  }


  int n2 = 15;
   while(n2<=19){
 	  not_malicious.Add(c.Get(n2));
 	  n2++;
  }

//add malcious nodes
 malicious.Add(c.Get(7));
 malicious.Add(c.Get(9));
 malicious.Add(c.Get(14));
 malicious.Add(c.Get(20));
 malicious.Add(c.Get(21));

  // Set up WiFi
  WifiHelper wifi;

  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);

  YansWifiChannelHelper wifiChannel ;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::TwoRayGroundPropagationLossModel",
	  	  	  	  	  	  	  	    "SystemLoss", DoubleValue(1),
		  	  	  	  	  	  	    "HeightAboveZ", DoubleValue(1.5));
  //For range near 250m
   wifiPhy.Set ("TxPowerStart", DoubleValue(33));
   wifiPhy.Set ("TxPowerEnd", DoubleValue(33));
   wifiPhy.Set ("TxPowerLevels", UintegerValue(1));
   wifiPhy.Set ("TxGain", DoubleValue(0));
   wifiPhy.Set ("RxGain", DoubleValue(0));
   wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue(-61.8));//The energy of a received signal should be higher than this threshold (dbm) to allow the PHY layer to detect the signal.
   wifiPhy.Set ("CcaMode1Threshold", DoubleValue(-64.8));



  wifiPhy.SetChannel (wifiChannel.Create ());

  // Add a non-QoS upper MAC
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  wifiMac.SetType ("ns3::AdhocWifiMac");

  // Set 802.11b standard and disable the rate control
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);

  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue(phyMode),
                                "ControlMode",StringValue(phyMode));


  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, c);
  //Create pcap file to capture packets
  wifiPhy.EnablePcapAll(std::string("aodv"));

//  Enable AODV and blackhole attack
  AodvHelper aodv;
  AodvHelper malicious_aodv;
  //AodvHelper secure_aodv;

  // Set up Internet stack
  InternetStackHelper internet;
  internet.SetRoutingHelper (aodv);
  internet.Install (not_malicious);
  
  malicious_aodv.Set("IsMalicious",BooleanValue(true)); // putting *false* instead of *true* would disable the malicious behavior of the node
  internet.SetRoutingHelper (malicious_aodv);
  internet.Install (malicious);

  // Set up Addresses
  Ipv4AddressHelper ipv4;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer ifcont = ipv4.Assign (devices);

  MobilityHelper mobility;

//Set random waypoint mobility model

   int64_t streamIndex = 0; // used to get consistent mobility across scenarios

   ObjectFactory p;

       p.SetTypeId ("ns3::GridPositionAllocator");
       p.Set ("MinX", DoubleValue (0.0));
       p.Set ("MinY", DoubleValue (0.0));
       p.Set ("DeltaX", DoubleValue (distance));
       p.Set ("DeltaY", DoubleValue (distance));
       p.Set ("GridWidth", UintegerValue (m_Xsize));
       p.Set ("LayoutType", StringValue ("RowFirst"));


       Ptr<PositionAllocator> PositionAlloc = p.Create ()->GetObject<GridPositionAllocator> ();
       streamIndex += PositionAlloc->AssignStreams (streamIndex);

       std::stringstream ssSpeed;
       ssSpeed << "ns3::UniformRandomVariable[Min=0.0|Max=" << nodeSpeed << "]";
       std::stringstream ssPause;
       ssPause << "ns3::ConstantRandomVariable[Constant=" << nodePause << "]";
       mobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel",
        								"Speed", StringValue (ssSpeed.str()),
        		                        "Pause", StringValue (ssPause.str()),
                                        "PositionAllocator", PointerValue (PositionAlloc));
       mobility.SetPositionAllocator (PositionAlloc);
       mobility.Install (c);
       streamIndex += mobility.AssignStreams (c, streamIndex);


  NS_LOG_INFO ("Create Applications.");

  // UDP connection from N0 to N24

  uint16_t sinkPort = 6;

  Address sinkAddress (InetSocketAddress (ifcont.GetAddress (24), sinkPort)); // interface of n24
  PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install (c.Get (24)); //n24 as sink
  sinkApps.Start (Seconds (0.));
  sinkApps.Stop (Seconds (100.));

  Ptr<Socket> ns3UdpSocket = Socket::CreateSocket (c.Get (0), UdpSocketFactory::GetTypeId ()); //source at n0

  // Create UDP application at n0
  Ptr<MyApp> app = CreateObject<MyApp> ();
  app->Setup (ns3UdpSocket, sinkAddress, 512, 1000000, DataRate ("500Kbps"));
  c.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (31.));
  app->SetStopTime (Seconds (100.));

  // Create file for throughput performance
  // used for create gnuplot file to show performance figure of Throughput
  std::string fileName = "Throughput";
  std::string graphic = fileName + ".png";
  std::string plotfilename = fileName + ".plt";
  std::string plottitle = "Throughput";
  std::string datatitle = "Throughput";

  Gnuplot gnuplot (graphic);
  gnuplot.SetTitle (plottitle);
  gnuplot.SetTerminal("png");
  gnuplot.SetLegend("Simulation time in seconds", "Throughput");//set labels for each axis
  Gnuplot2dDataset dataset;
  dataset.SetTitle (datatitle);
  dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

//used for create gnuplot file to show performance figure of Packet loss
  std::string fileName1 = "Packetloss";
  std::string graphic1 = fileName1 + ".png";
  std::string plotfilename1 = fileName1 + ".plt";
  std::string plottitle1 = "Packetloss";
  std::string datatitle1 = "Packetloss";

  Gnuplot gnuplot1 (graphic1);
  gnuplot1.SetTitle (plottitle1);
  gnuplot1.SetTerminal("png");
  gnuplot1.SetLegend("Simulation time in seconds", "Number of packet loss");//set labels for each axis
  Gnuplot2dDataset dataset1;
  dataset1.SetTitle (datatitle1);
  dataset1.SetStyle (Gnuplot2dDataset::LINES_POINTS);

//used for create gnuplot file to show performance figure of end-to-end delay
  std::string fileName2 = "delay";
  std::string graphic2 = fileName2 + ".png";
  std::string plotfilename2 = fileName2 + ".plt";
  std::string plottitle2 = "End-to-End delay";
  std::string datatitle2 = "End-to-End delay";

  Gnuplot gnuplot2 (graphic2);
  gnuplot2.SetTitle (plottitle2);
  gnuplot2.SetTerminal("png");
  gnuplot2.SetLegend("Simulation time in seconds", "Average End-to-End delay for each flow(ns)");//set labels for each axis
  Gnuplot2dDataset dataset2;
  dataset2.SetTitle (datatitle2);
  dataset2.SetStyle (Gnuplot2dDataset::LINES_POINTS);

  //print routing table
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("blackhole.routes", std::ios::out);
  aodv.PrintRoutingTableAllAt (Seconds (10), routingStream);

  // install Flowmonitor on all nodes
  FlowMonitorHelper fmHelper;
  Ptr<FlowMonitor> flow = fmHelper.InstallAll();

  ThroughputMonitor(&fmHelper,flow,dataset,dataset1,dataset2);

//
// do the actual simulation.
//
  NS_LOG_INFO ("Run Simulation.");
  AnimationInterface anim ("blackhole_aodv.xml");
  Simulator::Stop (Seconds(100.0));
  Simulator::Run ();

  flow->CheckForLostPackets ();

  //add the Throughput dataset to the plot
    gnuplot.AddDataset(dataset);
  // Open the plot file.
    std::ofstream plotFile (plotfilename.c_str());
  // Write the plot file.
    gnuplot.GenerateOutput (plotFile);
  // Close the plot file.
    plotFile.close ();

  //add the Packet loss dataset to the plot
    gnuplot1.AddDataset(dataset1);
  // Open the plot file.
    std::ofstream plotFile1 (plotfilename1.c_str());
  // Write the plot file.
    gnuplot1.GenerateOutput (plotFile1);
  // Close the plot file.
    plotFile1.close ();

  //add the end-to-end delay dataset to the plot
    gnuplot2.AddDataset(dataset2);
  // Open the plot file.
    std::ofstream plotFile2 (plotfilename2.c_str());
  // Write the plot file.
    gnuplot2.GenerateOutput (plotFile2);
  // Close the plot file.
    plotFile2.close ();

  Simulator::Destroy();

  return 0;
}

void ThroughputMonitor(FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> monitor, Gnuplot2dDataset DataSet, Gnuplot2dDataset DataSet1, Gnuplot2dDataset DataSet2)
{
  double Throughput = 0;
  double packetloss = 0;
  double delay = 0;
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier ());
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
	  Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
	  if(t.sourceAddress==Ipv4Address("10.1.1.1")&&t.destinationAddress==Ipv4Address("10.1.1.25"))

	  {
      std::cout << "Flow ID:    " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
      std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
      std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
      std::cout <<"Duration		: "<<(i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds())<<std::endl;
      std::cout <<"Last Received Packet	: "<< i->second.timeLastRxPacket.GetSeconds()<<" Seconds"<<std::endl;
      std::cout <<"Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
      std::cout <<"Average delay: "<< (i->second.delaySum.GetSeconds()/i->second.rxPackets)<<std::endl;
      std::cout <<"Packet drop: "<<i->second.lostPackets<<"\n";
      std::cout<<"---------------------------------------------------------------------------"<<std::endl;
      Throughput = (i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds())/1024/1024  );
      packetloss = (i->second.lostPackets);
      delay = (i->second.delaySum.GetDouble()/i->second.rxPackets);
      //update gnuplot file data
      DataSet.Add((double)Simulator::Now().GetSeconds(), (double) Throughput);
      DataSet1.Add((double)Simulator::Now().GetSeconds(), (double) packetloss);
      DataSet2.Add((double)Simulator::Now().GetSeconds(), (double) delay);
	  }

    }
  Simulator::Schedule(Seconds(1),&ThroughputMonitor, fmhelper, monitor, DataSet, DataSet1, DataSet2);
  {
  monitor->SerializeToXmlFile("blackholeaodv.xml", true, true);
  }

}
