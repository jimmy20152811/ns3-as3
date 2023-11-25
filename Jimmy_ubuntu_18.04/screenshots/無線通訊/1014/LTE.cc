#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"
//#include "ns3/building.h"

using namespace ns3;

int main(int argc, char *argv[]) {

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();

    // 定義仿真區域的大小
    double xSize = 10000.0;  // 10 km
    double ySize = 10000.0;  // 10 km
    // 設置中心點
    double centerX = 0.0;
    double centerY = 0.0;   
    // 創建仿真場景
    //Ptr<Building> building = CreateObject<Building> ();
    //building->SetBoundaries (Box (centerX - xSize / 2.0, centerX + xSize / 2.0, centerY - ySize / 2.0, centerY + ySize / 2.0));

    // 創建容器
    NodeContainer ueNodes;
    ueNodes.Create(10);  // 創建節點（UEs）
    NodeContainer gnbNodes;
    gnbNodes.Create(19);

    // 創建位置分配器
    Ptr<ListPositionAllocator> position = CreateObject<ListPositionAllocator>();

    // 增加9個 macro cells 
    position->Add(Vector(0.0, 5000.0, 0.0));  // (0, 5)
    position->Add(Vector(5000.0, 5000.0, 0.0));  // (5, 5)
    position->Add(Vector(-5000.0, 5000.0, 0.0));  // (-5, 5)
    position->Add(Vector(0.0, 0.0, 0.0));  // (0, 0)
    position->Add(Vector(5000.0, 0.0, 0.0));  // (5, 0)
    position->Add(Vector(-5000.0, 0.0, 0.0));  // (-5, 0)
    position->Add(Vector(0.0, -5000.0, 0.0));  // (0, -5)
    position->Add(Vector(5000.0, -5000.0, 0.0));  // (5, -5)
    position->Add(Vector(-5000.0, -5000.0, 0.0));  // (-5, -5)

    // 設置隨機數生成器
    RngSeedManager::SetSeed (1);
    Ptr<UniformRandomVariable> xRand = CreateObject<UniformRandomVariable> ();
    Ptr<UniformRandomVariable> yRand = CreateObject<UniformRandomVariable> ();

    // 創建10個 micro cells
    for (uint32_t i = 0; i < 10; ++i) {
        double x = xRand->GetValue (centerX - xSize / 2.0, centerX + xSize / 2.0);
        double y = yRand->GetValue (centerY - ySize / 2.0, centerY + ySize / 2.0);
        // 創建 micro cell，設置位置為 (x, y)
        position->Add(Vector(x, y, 0));
    }

    // mobility model 位置自己分配
    MobilityHelper mobilityGNB;
    mobilityGNB.SetPositionAllocator(position);
    mobilityGNB.Install(gnbNodes);

    // 創建位置分配器
    Ptr<ListPositionAllocator> positionUE = CreateObject<ListPositionAllocator>();
    // 創建10個 UEs
    for (uint32_t i = 0; i < 10; ++i) {
        double x = xRand->GetValue (centerX - xSize / 2.0, centerX + xSize / 2.0);
        double y = yRand->GetValue (centerY - ySize / 2.0, centerY + ySize / 2.0);
        // 創建 UE，設置位置為 (x, y)
        positionUE->Add(Vector(x, y, 0));
    }

    // 設定移動模型
    MobilityHelper mobilityUE;
    mobilityUE.SetPositionAllocator(positionUE);
    mobilityUE.Install(ueNodes);
    
    //Install an LTE protocol stack on the eNB(s)
    NetDeviceContainer enbDevs;
    enbDevs = lteHelper->InstallEnbDevice(gnbNodes);
    //install an LTE protocol stack on the UEs
    NetDeviceContainer ueDevs;
    ueDevs = lteHelper->InstallUeDevice(ueNodes);

    //Attach the UEs to an eNB. This will configure each UE according to the eNB configuration, and create an RRC connection between them
    lteHelper->Attach(ueDevs, enbDevs.Get(0));

    //Activate a data radio bearer between each UE and the eNB it is attached to
    enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    EpsBearer bearer(q);
    lteHelper->ActivateDataRadioBearer(ueDevs, bearer);
    
    Simulator::Stop(Seconds (5.0));

    AnimationInterface anim ("LTE.xml");

    // 執行模擬
    Simulator::Run();

    Simulator::Destroy();

    return 0;
}