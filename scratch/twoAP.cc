#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"

using namespace ns3;

void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon);

int main (int argc, char **argv)
{
  NS_LOG_COMPONENT_DEFINE ("twoAP");
//  LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
//  LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);

  Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::POISON_REVERSE));

  NS_LOG_INFO ("Create nodes.");
  Ptr<Node> iotsrv = CreateObject<Node> ();
  Names::Add ("IoTServer", iotsrv);
  Ptr<Node> iotgw = CreateObject<Node> ();
  Names::Add ("IoTGateway", iotgw);
  Ptr<Node> websrv = CreateObject<Node> ();
  Names::Add ("WebServer", websrv);
  Ptr<Node> webusr = CreateObject<Node> ();
  Names::Add ("WebUser", webusr);
  Ptr<Node> a = CreateObject<Node> ();
  Names::Add ("RouterA", a);
  Ptr<Node> b = CreateObject<Node> ();
  Names::Add ("RouterB", b);
  Ptr<Node> c = CreateObject<Node> ();
  Names::Add ("RouterC", c);
  Ptr<Node> d = CreateObject<Node> ();
  Names::Add ("RouterD", d);
  Ptr<Node> e = CreateObject<Node> ();
  Names::Add ("RouterE", e);
  Ptr<Node> ap1 = CreateObject<Node> ();
  Names::Add ("AP1", ap1);
  Ptr<Node> ap2 = CreateObject<Node> ();
  Names::Add ("AP2", ap2);

  NodeContainer net1 (iotsrv, a);
  NodeContainer net2 (websrv, b);
  NodeContainer net3 (a, c);
  NodeContainer net4 (b, c);
  NodeContainer net5 (c, d);
  NodeContainer net6 (d, ap1);
  NodeContainer net7 (b, e);
  NodeContainer net8 (c, e);
  NodeContainer net9 (e, ap2);
  NodeContainer routers (a, b, c, d, e);
  NodeContainer aps (ap1, ap2);
  NodeContainer users (iotgw, webusr);
  NodeContainer servers (iotsrv,websrv);
  NodeContainer wifinodes (iotgw,webusr);


  NS_LOG_INFO ("Create core channels.");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
  NetDeviceContainer ndc1 = csma.Install (net1);
  NetDeviceContainer ndc2 = csma.Install (net2);
  NetDeviceContainer ndc3 = csma.Install (net3);
  NetDeviceContainer ndc4 = csma.Install (net4);
  NetDeviceContainer ndc5 = csma.Install (net5);
  NetDeviceContainer ndc6 = csma.Install (net6);
  NetDeviceContainer ndc7 = csma.Install (net7);
  NetDeviceContainer ndc8 = csma.Install (net8);
  NetDeviceContainer ndc9 = csma.Install (net9);

  NS_LOG_INFO ("Create IPv4 and routing");
  RipHelper ripRouting;

  Ipv4ListRoutingHelper listRH;
  listRH.Add (ripRouting, 0);

  InternetStackHelper internet;
  internet.SetIpv6StackInstall (false);
  internet.SetRoutingHelper (listRH);
  internet.Install (routers);
  internet.Install (aps);

  InternetStackHelper internetNodes;
  internetNodes.SetIpv6StackInstall (false);
  internetNodes.Install (servers);

  NS_LOG_INFO ("Assign IPv4 Addresses.");
  Ipv4AddressHelper ipv4;

  ipv4.SetBase (Ipv4Address ("10.0.0.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic1 = ipv4.Assign (ndc1);

  ipv4.SetBase (Ipv4Address ("10.0.1.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic2 = ipv4.Assign (ndc2);

  ipv4.SetBase (Ipv4Address ("10.0.2.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic3 = ipv4.Assign (ndc3);

  ipv4.SetBase (Ipv4Address ("10.0.3.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic4 = ipv4.Assign (ndc4);

  ipv4.SetBase (Ipv4Address ("10.0.4.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic5 = ipv4.Assign (ndc5);

  ipv4.SetBase (Ipv4Address ("10.0.5.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic6 = ipv4.Assign (ndc6);

  ipv4.SetBase (Ipv4Address ("10.0.6.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic7 = ipv4.Assign (ndc7);

  ipv4.SetBase (Ipv4Address ("10.0.7.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic8 = ipv4.Assign (ndc8);

  ipv4.SetBase (Ipv4Address ("10.0.8.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic9 = ipv4.Assign (ndc9);


  Ptr<Ipv4StaticRouting> staticRouting;
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (iotsrv->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.0.2", 1 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (websrv->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.1.2", 1 );

  NS_LOG_INFO ("Set WiFi Channel");
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac;
  //Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               /*"Ssid", SsidValue (ssid),*/
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifinodes);

  mac.SetType ("ns3::ApWifiMac"/*,
                            "Ssid", SsidValue (ssid)*/);

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, aps);

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (100.0),
                                 "DeltaY", DoubleValue (100.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (webusr);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (aps);
  mobility.Install (iotgw);

  //Alterando a posição fixa do iotgw
  Ptr<MobilityModel> mobi = iotgw->GetObject<MobilityModel> ();
  Vector pos = mobi->GetPosition();
  pos.x = 201.0;
  pos.y = 1.0;
  mobi->SetPosition(pos);


  internetNodes.Install (wifinodes);

/*Imprimir posições
  for (NodeContainer::Iterator j = aps.Begin ();
       j != aps.End (); ++j)
    {
      Ptr<Node> object = *j;
      Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
      NS_ASSERT (position != 0);
      Vector pos = position->GetPosition ();
      std::cout << "x=" << pos.x << ", y=" << pos.y << ", z=" << pos.z << std::endl;
    }


  for (NodeContainer::Iterator j = wifinodes.Begin ();
       j != wifinodes.End (); ++j)
    {
      Ptr<Node> object = *j;
      Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
      NS_ASSERT (position != 0);
      Vector pos = position->GetPosition ();
      std::cout << "x=" << pos.x << ", y=" << pos.y << ", z=" << pos.z << std::endl;
    }
*/

  ipv4.SetBase (Ipv4Address ("10.1.5.0"), Ipv4Mask ("255.255.255.0"));
  ipv4.Assign (apDevices.Get(0));

  ipv4.SetBase (Ipv4Address ("10.2.5.0"), Ipv4Mask ("255.255.255.0"));
  ipv4.Assign (apDevices.Get(1));

  Ipv4InterfaceContainer stas = ipv4.Assign (staDevices);

  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (iotgw->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->AddNetworkRouteTo ("10.0.0.0", "255.255.255.0","10.1.5.1",1,5 );
  staticRouting->AddNetworkRouteTo ("10.0.0.0", "255.255.255.0","10.2.5.1",1,5 );
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (webusr->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->AddNetworkRouteTo ("10.0.0.0", "255.255.255.0","10.1.5.1",1,5 );
  staticRouting->AddNetworkRouteTo ("10.0.0.0", "255.255.255.0","10.2.5.1",1,5 );


  RipHelper routingHelper;

  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> (&std::cout);



//  routingHelper.PrintRoutingTableAt (Seconds (90.0), iotgw, routingStream);
//  routingHelper.PrintRoutingTableAt (Seconds (90.0), webusr, routingStream);
//  routingHelper.PrintRoutingTableAt (Seconds (90.0), c, routingStream);
//  routingHelper.PrintRoutingTableAt (Seconds (90.0), d, routingStream);


  NS_LOG_INFO ("Create Web Application.");
  uint16_t port = 4000;
  UdpServerHelper serverWeb (port);
  ApplicationContainer apps = serverWeb.Install (webusr);
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (100.0));

  uint32_t MaxPacketSize = 1024;
  Time interPacketInterval = Seconds (0.008); //0.008 = 1Mbps
  uint32_t maxPacketCount = 1000000;
  UdpClientHelper clientWeb (stas.GetAddress(1), port);
  clientWeb.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  clientWeb.SetAttribute ("Interval", TimeValue (interPacketInterval));
  clientWeb.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  apps = clientWeb.Install (websrv);
  apps.Start (Seconds (40.0));
  apps.Stop (Seconds (100.0));
/*
  NS_LOG_INFO ("Create IoT Application.");
  UdpServerHelper serverIoT (port);
  ApplicationContainer apps2 = serverIoT.Install (iotsrv);
  apps2.Start (Seconds (1.0));
  apps2.Stop (Seconds (100.0));

  UdpClientHelper clientIoT (iic1.GetAddress(0), port);
  clientIoT.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  clientIoT.SetAttribute ("Interval", TimeValue (interPacketInterval));
  clientIoT.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  apps2 = clientIoT.Install (iotgw);
  apps2.Start (Seconds (50.0));
  apps2.Stop (Seconds (100.0));
*/
//  AsciiTraceHelper ascii;
//  csma.EnableAsciiAll (ascii.CreateFileStream ("firstAP.tr"));
//  csma.EnablePcapAll ("firstAP", true);

  FlowMonitorHelper fmHelper;

  Ptr<FlowMonitor> allMon = fmHelper.InstallAll();
  Simulator::Schedule(Seconds(3),&ThroughputMonitor,&fmHelper, allMon);




  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (101.0));
  Simulator::Run ();

  ThroughputMonitor(&fmHelper, allMon);

  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  return 0;

}


void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon)
	{
		std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats();
		Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());
		for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
		{
  			Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);
        if(fiveTuple.sourceAddress == "10.0.1.1" || fiveTuple.destinationAddress == "10.0.0.1"){
    			std::cout<<"Flow ID			: " << stats->first <<" ; "<< fiveTuple.sourceAddress <<" -----> "<<fiveTuple.destinationAddress<<std::endl;
    //			std::cout<<"Tx Packets = " << stats->second.txPackets<<std::endl;
    //			std::cout<<"Rx Packets = " << stats->second.rxPackets<<std::endl;
    			std::cout<<"Duration		: "<<stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds()<<std::endl;
    			std::cout<<"Last Received Packet	: "<< stats->second.timeLastRxPacket.GetSeconds()<<" Seconds"<<std::endl;
    			std::cout<<"Throughput: " << stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps"<<std::endl;
          std::cout<<"Lost: " << stats->second.lostPackets <<std::endl;
    			std::cout<<"---------------------------------------------------------------------------"<<std::endl;
        }
		}
			Simulator::Schedule(Seconds(1),&ThroughputMonitor, fmhelper, flowMon);

	}
