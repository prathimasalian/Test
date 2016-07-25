

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TcpExample");

class MyApp : public Application
{
public:
  MyApp ();
  virtual ~MyApp ();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp ()
{
  m_socket = 0;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}


int
main (int argc, char *argv[])
{

 
  std::string tcp_variant = "TcpNewReno";
 
  std::string bandwidth = "100Mbps";
  std::string delay = "6560ns";
double error_rate = 0.001;
  int simulation_time = 50; //seconds
Config::SetDefault ("ns3::RateErrorModel::ErrorRate", DoubleValue (0.01));
  Config::SetDefault ("ns3::RateErrorModel::ErrorUnit", StringValue ("EU_PKT"));
 //
  
   if (tcp_variant.compare("TcpNewReno") == 0)
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId()));
  else
    {
      fprintf (stderr, "Invalid TCP version\n");
      exit (1);
    }
//creating nodes

  NodeContainer csmaNodes;
  csmaNodes.Create (10);

  NetDeviceContainer devices;
 
  CsmaHelper csma;
csma.SetChannelAttribute ("DataRate", StringValue (bandwidth));
  csma.SetChannelAttribute ("Delay", StringValue (delay));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);


 
 InternetStackHelper stack;

  stack.Install (csmaNodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
 
 csmaInterfaces = address.Assign (csmaDevices);

  uint16_t sinkPort = 8080;
  Address sinkAddress (InetSocketAddress (csmaInterfaces.GetAddress (5), sinkPort));
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install (csmaNodes.Get (9));
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (simulation_time));

  Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (csmaNodes.Get (7), TcpSocketFactory::GetTypeId ());

  Ptr<MyApp> app = CreateObject<MyApp> ();
  app->Setup (ns3TcpSocket, sinkAddress, 1460, 1000000, DataRate ("100Mbps"));
  csmaNodes.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (1.0));
  app->SetStopTime (Seconds (simulation_time));
 
  
 // AsciiTraceHelper asciiTraceHelper;
 // Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("tcp-example.cwnd");
 // ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));

 // AsciiTraceHelper ascii;

  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (error_rate));
 std::list<uint32_t> sampleList;
 Ptr<ListErrorModel> pem = CreateObject<ListErrorModel> ();
  pem->SetList (sampleList);
  devices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

std::ofstream ascii;
  ascii.open ("trialp.tr");

AnimationInterface anim ("tcp4.xml");
  anim.SetConstantPosition(csmaNodes.Get(0), 0.0, 4.0);
  anim.SetConstantPosition(csmaNodes.Get(1), 1.0, 4.0);
anim.SetConstantPosition(csmaNodes.Get(2), 2.0, 4.0);
  anim.SetConstantPosition(csmaNodes.Get(3), 3.0, 4.0);

anim.SetConstantPosition(csmaNodes.Get(4), 4.0, 4.0);
  anim.SetConstantPosition(csmaNodes.Get(5), 5.0, 4.0);

anim.SetConstantPosition(csmaNodes.Get(6), 6.0, 4.0);
  anim.SetConstantPosition(csmaNodes.Get(7), 7.0, 4.0);

anim.SetConstantPosition(csmaNodes.Get(8), 8.0, 4.0);
  anim.SetConstantPosition(csmaNodes.Get(9), 9.0, 4.0);



  Simulator::Stop (Seconds (simulation_time));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}

