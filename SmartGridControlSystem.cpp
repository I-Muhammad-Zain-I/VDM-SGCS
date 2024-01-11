#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include "VDM.h"
using namespace std;


class InvariantCheck {
public:
    virtual bool inv() = 0;
};
class UniqueExistCheck {
public:
    virtual bool isUnique(string Id) = 0;
};

enum DeviceType {
    CONSUMER,
    GENERATOR
};
enum PowerSignal {
  INC_CONSUMPTION,
  DEC_CONSUMPTION,
  INC_GENERATION,
  DEC_GENERATION
};

class Device {
  public:
    string deviceId;
    DeviceType deviceType;
    
  public:
    Device() {}
    Device(string devId, DeviceType devType) {
      deviceId = devId;
      deviceType = devType;
    }
};


void checkUnmatchedInput(int &choice) {
  if (!(cin >> choice)) {
    
    cout << "Invalid input. Please enter a number." << endl;

    cin.clear();
 
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
 }
}



int GCMenu() {

  int gcChoice = 0;
  cout << endl;
  cout << "Choose Any one following" << endl;
  cout << "1 - Display Devices" << endl;
  cout << "2 - Add Devices" << endl;
  cout << "3 - Remove Devices" << endl;
  cout << "4 - Exit" << endl;
  
  checkUnmatchedInput(gcChoice);

  while(gcChoice < 1 ||  gcChoice > 4) {
    cout << "Please Enter Correct Option (1-4): "; 
    checkUnmatchedInput(gcChoice);
  }
  
  return gcChoice;
}

string takeDeviceIdInput() {
  string deviceId = "";
  cout << "Enter Device ID: "; cin >> deviceId;
  return deviceId;
}

DeviceType takeDeviceTypeInput() {
  int devTypeInt = 0;
  DeviceType deviceType;

  cout << "Choose Device Type (1-2)" << endl;
  cout << "1 - Consumer" << endl;
  cout << "2 - Generator" << endl;
  cin >> devTypeInt;

  while(devTypeInt < 1 && devTypeInt > 2) {
    cout << "Please Choose Correct Option (1-2)" << endl;
  }

  switch (devTypeInt)
  {
    case 1:
        deviceType = DeviceType::CONSUMER;
      break;
    case 2:
        deviceType = DeviceType::GENERATOR;
      break;
    default:
      break; 
  }
  return deviceType;
};




class SmartGridControlSystem : public VDM, public InvariantCheck, public UniqueExistCheck {
  public:
    const int POWER_CONSUMPTION = 250;
    const int POWER_GENERATION = 500;
    int totalConsumption;
    int totalGeneration;
    vector<Device> connectedDevices;

  public:
    SmartGridControlSystem() {
      connectedDevices = vector<Device>(0);
      totalGeneration = 0;
      totalConsumption = 0;
      VDM::invTest(*this);
    }
     
    bool inv() override {
        bool result = (totalConsumption <= totalGeneration);
        if(!result) {
          cout << "Power Balance Invariant Violated" << endl;
        }
        return result;
    }

    bool isUnique(string devId) override {
      VDM::preTest(connectedDevices.size() >= 0);
      bool result = true;
      if(connectedDevices.size() > 0) {
        for(int i = 0; i < connectedDevices.size(); ++i) {
          if(connectedDevices[i].deviceId == devId) {
            result = false;
          }
        }
  
      }
      if(!result) {
        cout << "Device Id: " << devId << " already Exists" << endl;
      } 
      return result;
    }

   

    int getDeviceIndex(string devId) {
      VDM::preTest(!VDM::uniqueExists(*this, devId));
      // above mean id is not(unique and is non-existant) => id is not unique and is existant
      int result = -1;
      for(int i = 0; i < connectedDevices.size(); ++i) {
        if(connectedDevices[i].deviceId == devId) {
          result = i;
          cout << "Index for: " << devId << " is: " << i << endl;
          return result;
        }
      }
      return result;
    }
    
    PowerSignal addDevice(string devId, DeviceType devType) {

      bool result = (devType == DeviceType::CONSUMER);
      // below mean id is (unique and is non-existant)
      VDM::preTest(VDM::uniqueExists(*this, devId) && 
      ( 
        (devType == DeviceType::CONSUMER 
        ? (totalGeneration >= totalConsumption + POWER_CONSUMPTION)
        : true)
      ));

      PowerSignal powSignalOut;
      Device device = Device(devId, devType);
      connectedDevices.push_back(device);

      if(devType == DeviceType::CONSUMER) {
        totalConsumption += POWER_CONSUMPTION;
        powSignalOut =  PowerSignal::INC_CONSUMPTION;
      }
      else {
        totalGeneration += POWER_GENERATION;
        powSignalOut =  PowerSignal::INC_GENERATION;
      }
       
      VDM::invTest(*this);
      return powSignalOut;
    }

    PowerSignal removeDevice(int index) {
      VDM::preTest((index <= (connectedDevices.size() - 1) && index >= 0) && ( 
        connectedDevices[index].deviceType == DeviceType::GENERATOR 
        ? (totalConsumption <= totalGeneration - POWER_GENERATION)
        : true
      ));

      PowerSignal powSignalOut;
      
      if(connectedDevices[index].deviceType == DeviceType::CONSUMER) {
        totalConsumption -= POWER_CONSUMPTION;
        powSignalOut = PowerSignal::DEC_CONSUMPTION;
      }
      else {
        totalGeneration -= POWER_GENERATION;
        powSignalOut = PowerSignal::DEC_GENERATION;
      }
      connectedDevices.erase(connectedDevices.begin() + index);
      VDM::invTest(*this);
      return powSignalOut;
    }

    void displaySystemData() {
      cout << endl;
      if(connectedDevices.size() > 0) {
       
        cout << setw(20) << "Device Num" << setw(20) << "Device ID" << setw(20) << "DeviceType" << setw(20) << endl;
        for(int i = 0; i < connectedDevices.size(); ++i) {
          cout << setw(20) << i+1 
                << setw(20) << connectedDevices[i].deviceId 
                << setw(20) << (connectedDevices[i].deviceType == DeviceType::GENERATOR ? "Generator" : "Consumer" )
                << endl;
        }
        cout << "---------------------------------------------------------------------------------" << endl;
        cout << "Total Consumption: " << totalConsumption << endl;
        cout << "Total Generation: " << totalGeneration << endl;
        
        }
      else {
        cout << "No Devices Connected Yet" << endl;
      }
    } 
};


class SGCSTest {
  private:
    vector<string> input;
    vector<string> expectedOutput;
    vector<string> actualOutput;
    vector<string> status;
    vector<string> testName;
    SmartGridControlSystem sgcs;
    const string PASSED = "|        PASSED        |";
    const string FAILED = "|        FAILED        |";
  public:
    SGCSTest() : sgcs(SmartGridControlSystem()) {
        input =  vector<string>(0);
        expectedOutput =  vector<string>(0);
        actualOutput =  vector<string>(0);
        status =  vector<string>(0);
        testName =  vector<string>(0);
    
    }
    void deviceIdisUnique_test_1() {
      // Test: 1 -> 'D-1', device added, device added, passed give existingId and check existence
      sgcs.addDevice("D-1", DeviceType::GENERATOR);
      testName.push_back("Existing-ID & Check Presence");
      string inp = "D-1";
      string expOut = "|     ID Not Unique    |";
      bool result = sgcs.isUnique(inp);
      input.push_back("|          " + inp + "          |");
      expectedOutput.push_back(expOut);
      if(result == false) {
        actualOutput.push_back(expOut);
        status.push_back(PASSED);
      }
      else {
        actualOutput.push_back("|       Id Unique      |");
        status.push_back(FAILED);
      }
    }
    void deviceIdisUnique_test_2() {
      // Test: 2 -> 'D-2'  give Non-existingId and check absence
      testName.push_back("Non-Existing-ID & Check Absence");
      string inp = "D-2";
      string expOut = "|       ID Unique      |";
      bool result = sgcs.isUnique(inp);
      input.push_back("|          " + inp + "          |");
      expectedOutput.push_back(expOut);
      if(result == true) {
        actualOutput.push_back(expOut);
        status.push_back(PASSED);
      }
      else {
        actualOutput.push_back("|     ID Not Unique     ");
        status.push_back(FAILED);
      }
    }
    void getDeviceIndex_test_1() {
      // Test: 1 -> returns -1 for non-existing Id
      testName.push_back("Non-Existing-ID & Invalid index returned");
      string inp = "D-2";
      string expOut = "|Invalid Index Returned|";

      string actOutput = "| Valid Index Returned |";
      string stat = FAILED;

     
      input.push_back("|          " + inp + "          |");
      expectedOutput.push_back(expOut);
      try {
        bool result = sgcs.getDeviceIndex(inp);


      } catch (VDMException &ex) {
        actOutput = expOut;
        stat = PASSED;
      }

      actualOutput.push_back(actOutput);
      status.push_back(stat);
    }
    void getDeviceIndex_test_2() {
      // Test: 1 -> returns -1 for non-existing Id
      testName.push_back("Existing-ID & returned valid index");
      string inp = "D-1";
      string expOut = "| Valid Index Returned |";

      string actOutput = "";
      string stat = "";

      input.push_back("|          " + inp + "          |");
      expectedOutput.push_back(expOut);
      try {
        bool result = sgcs.getDeviceIndex(inp);
        actOutput = expOut;
        stat = PASSED;
      } catch (VDMException &ex) {
        actOutput = "|Invalid Index Returned|";
        stat = FAILED;
      }

      actualOutput.push_back(actOutput);
      status.push_back(stat);
    }
    void addDevice_Test_1() {
      // Test: 1 -> 1st Consumer => powerSignal: increaseConsumption
      testName.push_back("For 1 Generaor Add 1st Consumer");
      
      string expOut = "|    INC_CONSUMPTION   |";
      string actOutput = "";
      string stat = "";
      PowerSignal pS;

      input.push_back("|    D-2, CONSUMER     |");
      expectedOutput.push_back(expOut);

      try {
        pS = sgcs.addDevice("D-2", DeviceType::CONSUMER);
      } catch (VDMException &ex) {
        actOutput = "|      No Signal       |";
        stat = FAILED;
      }
      if(pS == PowerSignal::INC_CONSUMPTION) {
        actOutput = expOut;
        stat = PASSED;
      }
      actualOutput.push_back(actOutput);
      status.push_back(stat);
    }

    void addDevice_Test_2() {
    // Test: 2 -> 3rd Consumer Addition for 1 Generator: invariant Violated
      sgcs.addDevice("D-3", DeviceType::CONSUMER);
      testName.push_back("For 1 Generator Add 3rd Consumer");
      
      string expOut = "|      No Signal       |";
      string actOutput = "";
      string stat = "";
      PowerSignal pS = PowerSignal::DEC_CONSUMPTION;

      input.push_back("|    D-4, CONSUMER     |");
      expectedOutput.push_back(expOut);
      
      try {
        pS = sgcs.addDevice("D-4", DeviceType::CONSUMER);
      } catch (VDMException &ex) {

        cout << "Hello" << endl;
        cout << pS << endl;
        actOutput = expOut;
        stat = PASSED;
      }
      if(pS == PowerSignal::INC_CONSUMPTION) {
        cout << "INC_CONS RUN" << endl;
        actOutput = "|   INC_CONSUMPTION    |";
        stat = FAILED;
      }
      actualOutput.push_back(actOutput);
      status.push_back(stat);
    }

    void addDevice_Test_3() {
      // Test: 3 -> 2nd Generator => powerSignal: increaseGeneration
      testName.push_back("Adding 2nd Generator");
      
      string expOut = "|    INC_GENERATION    |";
      string actOutput = "";
      string stat = "";
      PowerSignal pS;

      input.push_back("|    D-4, GENERATOR    |");
      expectedOutput.push_back(expOut);

      try {
        pS = sgcs.addDevice("D-4", DeviceType::GENERATOR);
      } catch (VDMException &ex) {
        actOutput = "|      No Signal       |";
        stat = FAILED;
      }
      if(pS == PowerSignal::INC_GENERATION) {
        actOutput = expOut;
        stat = PASSED;
      }
      actualOutput.push_back(actOutput);
      status.push_back(stat);
    }
    void removeDevice_Test_1() {
      // Test: 1 -> 2nd Generator remove => powerSignal: decreaseGeneration
      testName.push_back("Removing 2nd Generator");
      
      string expOut = "|    DEC_GENERATION    |";
      string actOutput = "";
      string stat = "";
      PowerSignal pS;

      int index = sgcs.getDeviceIndex("D-4");
      input.push_back("|      Index: " + to_string(index) + "        |");
      expectedOutput.push_back(expOut);

      try {
        pS = sgcs.removeDevice(index);
      } catch (VDMException &ex) {
        actOutput = "|      No Signal       |";
        stat = FAILED;
      }
      if(pS == PowerSignal::DEC_GENERATION) {
        actOutput = expOut;
        stat = PASSED;
      }
      actualOutput.push_back(actOutput);
      status.push_back(stat);
    }
    void removeDevice_Test_2() {
      // Test: 2 -> 2nd Consumer remove => powerSignal: decreaseConsumption
      testName.push_back("Removing 2nd Consumer");
      
      string expOut = "|   DEC_CONSUMPTION    |";
      string actOutput = "";
      string stat = "";
      PowerSignal pS;

      int index = sgcs.getDeviceIndex("D-3");
      input.push_back("|      Index: " + to_string(index) + "        |");
      expectedOutput.push_back(expOut);

      try {
        pS = sgcs.removeDevice(index);
      } catch (VDMException &ex) {
        actOutput = "|       No Signal      |";
        stat = FAILED;
      }
      if(pS == PowerSignal::DEC_CONSUMPTION) {
        actOutput = expOut;
        stat = PASSED;
      }
      actualOutput.push_back(actOutput);
      status.push_back(stat);
    }
    void removeDevice_Test_3() {
      // Test: 3 -> 1st Generator remove => no_Signal
     testName.push_back("Remove 1st Generator in presence of 1st Consumer");
      
      string expOut = "|       No Signal      |";
      string actOutput = "";
      string stat = "";
      PowerSignal pS;

      int index = sgcs.getDeviceIndex("D-1");
      input.push_back("|      Index: " + to_string(index) + "        |");
      expectedOutput.push_back(expOut);

      try {
        pS = sgcs.removeDevice(index);
      } catch (VDMException &ex) {
        actOutput = expOut;
        stat = PASSED;
      }
      if(pS == PowerSignal::DEC_GENERATION) {
        actOutput = expOut;
        stat = FAILED;
      }
      actualOutput.push_back(actOutput);
      status.push_back(stat);
    }
    void executeTests() {
      this->deviceIdisUnique_test_1();
      this->deviceIdisUnique_test_2();
      this->getDeviceIndex_test_1();
      this->getDeviceIndex_test_2();
      this->addDevice_Test_1();
      this->addDevice_Test_2();
      this->addDevice_Test_3();
      this->removeDevice_Test_1();
      this->removeDevice_Test_2();
      this->removeDevice_Test_3();

      this->displayTestResult();
    }
    void displayTestResult() {
      cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
      cout << "                                                   Test Result                                                    "  << endl;
      cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl << endl;

      cout << "------------------------------------------------------------------------------------------------------------------" << endl;
      cout << left << setw(30) << "|        Input         |"
              << setw(30) << "|   Expected Output    |" << setw(30) << "|    Actual Output     |"
              << setw(30) << "|        STATUS        |" << endl ;
      cout << "------------------------------------------------------------------------------------------------------------------" << endl<< endl;
    // Table content
    for (int i = 0; i < status.size(); ++i) {
        cout << "=> " << testName[i] << endl;
        cout << "------------------------------------------------------------------------------------------------------------------" << endl;
        cout  << left << setw(30) << input[i]
                  << setw(30) << expectedOutput[i] << setw(30) << actualOutput[i]
                  << setw(30) << status[i] << endl;
         cout << "------------------------------------------------------------------------------------------------------------------" << endl << endl;
    }
    }
};






int main() {
 
  SmartGridControlSystem gC = SmartGridControlSystem();
  
  cout << endl; cout << "-------------" << endl; cout << endl;
  cout << "SMART GRID CONTROL SYSTEM v1.0" << endl;
  cout << "Choose from following" << endl;

  int GCMenuChoice =  0;

  while(GCMenuChoice != 4) {
    GCMenuChoice = GCMenu();
    switch (GCMenuChoice) {
      case 1: {
        gC.displaySystemData(); break;
      }
      case 2: { 
        string deviceId = takeDeviceIdInput();
        DeviceType deviceType = takeDeviceTypeInput();
   
        try {
          gC.addDevice(deviceId,deviceType);
        } catch (VDMException &ex) {
          cout << "*****************************************************************" << endl;
          cout << ex.getMessage() << endl;
          std::cerr << "Exception caught at line: " << __LINE__ << endl;
          cout << "*****************************************************************" << endl;
        }
        break;
      }
      case 3:
        if(gC.connectedDevices.size() > 0) {
          string deviceId = takeDeviceIdInput();
          bool deviceIdIsExisting = gC.isUnique(deviceId);

          if(deviceIdIsExisting == false) {
            int index;
              index = gC.getDeviceIndex(deviceId);
            try {
              gC.removeDevice(index);
            } catch (VDMException &ex) {
              std::cerr << "Exception caught at line: " << __LINE__ << endl;
              cout << ex.getMessage() << endl;
            }
          }
          else {
            cout << "Device Id Does not Exist Please Enter a Unique Id" << endl;
          }
            
          break;
        }
        else {
          cout << "No Devices Added Yet" << endl;
        }
        case 4:
          break;
      }
    }

  // SGCSTest testSuite = SGCSTest();
  // testSuite.executeTests();
  
  return 0;
}


