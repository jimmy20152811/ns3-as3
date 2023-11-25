
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/netanim-module.h"
#include "ns3/config-store.h"
//#include "ns3/random-walk2d-mobility-model.h"


using namespace ns3;


int main(int argc, char *argv[]) {
  // 创建仿真场景
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();

  // 创建一个节点容器用于放置基站和UE
  NodeContainer nodes;
  nodes.Create(19); 
  
  // 设置正方形区域的边界坐标
  double xMin = -5000.0;
  double xMax = 5000.0;
  double yMin = -5000.0;
  double yMax = 5000.0;

  // 创建位置分配器并分配基站和UE的位置
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
  
  // add macro cells in 9 pos
  positionAlloc->Add(Vector(0.0, 5000.0, 0.0));  // (0, 5)
  positionAlloc->Add(Vector(5000.0, 5000.0, 0.0));  // (5, 5)
  positionAlloc->Add(Vector(-5000.0, 5000.0, 0.0));  // (-5, 5)
  positionAlloc->Add(Vector(0.0, 0.0, 0.0));  // (0, 0)
  positionAlloc->Add(Vector(5000.0, 0.0, 0.0));  // (5, 0)
  positionAlloc->Add(Vector(-5000.0, 0.0, 0.0));  // (-5, 0)
  positionAlloc->Add(Vector(0.0, -5000.0, 0.0));  // (0, -5)
  positionAlloc->Add(Vector(5000.0, -5000.0, 0.0));  // (5, -5)
  positionAlloc->Add(Vector(-5000.0, -5000.0, 0.0));  // (-5, -5)

  // random n micro cells
  std::vector<Vector> microCellPositions;
  Ptr<UniformRandomVariable> randX = CreateObject<UniformRandomVariable>();
  Ptr<UniformRandomVariable> randY = CreateObject<UniformRandomVariable>();
  randX->SetAttribute ("Min", DoubleValue (xMin + 1000.0));
  randX->SetAttribute ("Max", DoubleValue (xMax - 1000.0));
  randY->SetAttribute ("Min", DoubleValue (yMin + 1000.0));
  randY->SetAttribute ("Max", DoubleValue (yMax - 1000.0));

  for (int i = 0; i < 10; i++) {
    Vector newPos;
    bool validPosition = false;
    
    while (!validPosition) {
      newPos = Vector(randX->GetValue(), randY->GetValue(), 0.0);
      validPosition = true;

      for (const Vector& existingPos : microCellPositions) {
        if ((newPos - existingPos).GetLength() < 1000.0) { // 检查最小距离
          validPosition = false;
          break;
        }
      }
    }

    microCellPositions.push_back(newPos);
    positionAlloc->Add(newPos);
  }
  MobilityHelper mobility;
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(nodes);
  

  // 随机分配50个UE
  NodeContainer movingNodes;
  movingNodes.Create(5);
  MobilityHelper mobilityMovingNodes;
  mobilityMovingNodes.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
  "X", StringValue("0.0"),
  "Y", StringValue("0.0"),
  "Rho", StringValue("ns3::UniformRandomVariable[Min=-4600|Max=4600]"));
  mobilityMovingNodes.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
  "Mode", StringValue("Time"),
  "Time", StringValue("1s"),
  "Speed", StringValue("ns3::ConstantRandomVariable[Constant=10.0]"),
  "Bounds", StringValue("-5000|5000|-5000|5000"));
  mobilityMovingNodes.Install(movingNodes);
  //Install an LTE protocol stack on the eNB(s)
  NetDeviceContainer enbDevs;
  enbDevs = lteHelper->InstallEnbDevice(nodes);
 for (uint32_t i = 0; i < enbDevs.GetN(); ++i) {
    // 获取eNB设备
    Ptr<NetDevice> enbNetDevice = enbDevs.Get(i);

    // 获取eNB设备的PHY层
    Ptr<LteEnbNetDevice> enbPhy = enbNetDevice->GetObject<LteEnbNetDevice>();

    // 设置发射功率（dBm）
    enbPhy->GetPhy()->SetTxPower(43.0);

    // 默认情况下，使用IsotropicAntennaModel作为天线模型
}

  //install an LTE protocol stack on the UEs
  NetDeviceContainer ueDevs;
  ueDevs = lteHelper->InstallUeDevice(movingNodes);

  //Attach the UEs to an eNB. This will configure each UE according to the eNB configuration, and create an RRC connection between them
  lteHelper->Attach(ueDevs, enbDevs.Get(3));

  //Activate a data radio bearer between each UE and the eNB it is attached to
  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer(q);
  lteHelper->ActivateDataRadioBearer(ueDevs, bearer);
  
  


  // 创建Traffic Model对象
//  Ptr<RandomVariableStream> randomStream = CreateObject<UniformRandomVariable>();
//  randomStream->SetAttribute("Min", DoubleValue(0.0));
//  randomStream->SetAttribute("Max", DoubleValue(1.0));

  // 将Traffic Model与UE节点关联
//for (uint32_t i = 0; i < movingNodes.GetN(); ++i) {
//  Ptr<UniformRandomVariable> trafficModel = CreateObject<UniformRandomVariable>();
//  trafficModel->SetAttribute("Min", DoubleValue(0.0));
//  trafficModel->SetAttribute("Max", DoubleValue(1.0));
//  movingNodes.Get(i)->AggregateObject(trafficModel);
//}


 
  // 将位置分配器设置为所有节点
  

 
  // 配置模拟的移动性模型和信道模型
  

  
  // 配置流量模型
  // ...

  // 运行仿真
  //Simulator::Run();
  Simulator::Stop (Seconds (5.0));
  AnimationInterface anim("hw2.xml");
  for (uint32_t i = 0; i < nodes.GetN(); ++i) {
    if (i < 9) { // 宏基站
      anim.UpdateNodeColor(nodes.Get(i), 255, 0, 0); // 红色
    } else if (i < 19) { // 微小基站
      anim.UpdateNodeColor(nodes.Get(i), 0, 0, 255); // 蓝色
    }  
  }
  for (uint32_t i = 0; i < movingNodes.GetN(); ++i) {
      anim.UpdateNodeColor(nodes.Get(i), 0, 255, 0); // 红色
  }  
  Simulator::Run();
  // 分析性能指标并绘制图表
  // ...

  Simulator::Destroy();
  return 0;
}
