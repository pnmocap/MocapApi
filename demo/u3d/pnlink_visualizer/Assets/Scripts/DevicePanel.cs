using MocapApi;
using System.Collections.Generic;
using UnityEngine;

public class DevicePanel : PopupPanelBehaviour
{
  public GameObject SensorItem;
  public GameObject MasterItem;
  Dictionary<uint, SensorItemGUI> SensorItemMap;
  // Start is called once before the first execution of Update after the MonoBehaviour is created
  void Start()
  {

  }
  public void handleSensorItem() {
    foreach (var sensor in SensorItemMap.Values)
    {
      sensor.gameObject.SetActive(!sensor.gameObject.activeSelf);
    }
  }
  public void handleMasterItem() {
    MasterItem.SetActive(!MasterItem.activeSelf);
  }
  public void updateGUI(ulong systemHandle)
  {
    MasterItem.GetComponent<MasterItemGUI>().updateGUI(systemHandle);

    uint countOfSensorModules = 0;
    IMCPSystem.System.GetSensorModules(null, ref countOfSensorModules, systemHandle);
    ulong[] sensorModules = new ulong[countOfSensorModules];
    IMCPSystem.System.GetSensorModules(sensorModules, ref countOfSensorModules, systemHandle);
    if (null == SensorItemMap)
    {
      SensorItemMap = new Dictionary<uint, SensorItemGUI>();
    }
    float topOffset = 0.0f;
    for (uint i = 0; i < countOfSensorModules; ++i)
    {
      uint id = 0;
      var err = IMCPSensorModule.SensorModule.GetSensorModuleId(ref id, sensorModules[i]);
      if (err != EMCPError.Error_None)
      {
        continue;
      }
      SensorItemGUI sensorModule = null;
      if (SensorItemMap.ContainsKey(id))
      {
        sensorModule = SensorItemMap[id];
      }
      else
      {
        sensorModule = SensorItem.GetComponent<SensorItemGUI>();
        RectTransform rectTransform = null;
        if (!SensorItemMap.ContainsValue(sensorModule))
        {
          rectTransform = SensorItem.GetComponent<RectTransform>();
        }
        else
        {
          var gameObj = GameObject.Instantiate(SensorItem);
          gameObj.transform.SetParent(SensorItem.transform.parent);
          rectTransform = gameObj.GetComponent<RectTransform>();
          rectTransform.anchoredPosition = SensorItem.GetComponent<RectTransform>().anchoredPosition;
          rectTransform.sizeDelta = SensorItem.GetComponent<RectTransform>().sizeDelta;
          sensorModule = gameObj.GetComponent<SensorItemGUI>();
        }
        var pos = rectTransform.anchoredPosition;
        pos.y -= topOffset;
        rectTransform.anchoredPosition = pos;
        topOffset += rectTransform.sizeDelta.y;
        SensorItemMap.Add(id, sensorModule);
        sensorModule.setId(id);
        rectTransform.gameObject.name = $"Sensor_Item{id}";
      }
      sensorModule.updateGUI(sensorModules[i]);
    }
  }
}
