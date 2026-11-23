using TMPro;
using UnityEngine;
using static Unity.Burst.Intrinsics.X86.Avx;

public class InformationPanel : PopupPanelBehaviour
{
  public TextMeshProUGUI MocapApiInterfaceVersionText;
  public TextMeshProUGUI MocapApiLibraryVersionText;
  public TextMeshProUGUI MasterFirmwareVersionText;
  public TextMeshProUGUI MasterSerialNumberText;
  // Start is called once before the first execution of Update after the MonoBehaviour is created
  void Start()
  {
    MocapApiInterfaceVersionText.text = MocapApi.Version.mocapApiString;
    MocapApiLibraryVersionText.text = MocapApi.Version.libMocapApiString;
  }

  public void SetMasterInfo(string ver, string sn)
  {
    MasterFirmwareVersionText.SetText(ver);
    MasterSerialNumberText.SetText(sn);
  }
}
