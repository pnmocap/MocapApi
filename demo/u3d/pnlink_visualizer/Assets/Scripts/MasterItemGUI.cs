using MocapApi;
using TMPro;
using UnityEngine;

public class MasterItemGUI : MonoBehaviour
{
  public TextMeshProUGUI FirmwareVersion;
  public TextMeshProUGUI SerialNumber;
  // Start is called once before the first execution of Update after the MonoBehaviour is created
  void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

  public void updateGUI(ulong systemHandle) {
    string serialNumber = "";
    IMCPSystem.System.GetMasterSerialNumber(ref serialNumber, systemHandle);
    SerialNumber.SetText(serialNumber);
    string version = "";
    IMCPSystem.System.GetMasterVersion(ref version, systemHandle);
    FirmwareVersion.SetText(version);
  }
}
