using System.Collections;
using System.IO;
using System.Text.RegularExpressions;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.Android;
using UnityEngine.UI;
using static UnityEngine.Rendering.DebugUI;

public class GPSTest : MonoBehaviour
{
  float updateInterval = 0.01f; // 更新间隔时间（秒）
  bool capture = true;
  Coroutine gpsCoroutine;
  StreamWriter recordStrm;
  uint reocrdIndex = 0;
  public TMP_InputField inputField;
  public GameObject StartButton;
  public GameObject FinishButton;
  public GameObject MarkButton;
  public GPSTracker GPSTracker;
  public GameObject Message;
  void Start()
  {
    Debug.Log(Application.persistentDataPath);
    StartButton.GetComponent<UnityEngine.UI.Button>().interactable = true;
    FinishButton.GetComponent<UnityEngine.UI.Button>().interactable = false;
    MarkButton.GetComponent<UnityEngine.UI.Button>().interactable = false;
  }

  // Update is called once per frame
  void Update()
  {
    if (Input.GetMouseButtonDown(1) || Input.GetMouseButton(0)) {
      if (Message.activeSelf) {
        Message.SetActive(false);
      }      
    }
  }
  private string[] permissions = new string[] {
        "android.permission.READ_EXTERNAL_STORAGE",
        "android.permission.WRITE_EXTERNAL_STORAGE",
        "android.permission.ACCESS_FINE_LOCATION",
        "android.permission.ACCESS_COARSE_LOCATION",
    };
  void showMessage(string message) {
    Message.SetActive(true);
    var txt = Message.transform.GetChild(0);
    txt.GetComponent<TMPro.TextMeshProUGUI>().text = message;
  }
  public void StartRecord()
  {
    // 检查是否支持定位服务
    if (!Input.location.isEnabledByUser)
    {
      showMessage("定位服务未启用，请在设置中开启。");
      return;
    }
    if (!Permission.HasUserAuthorizedPermission(Permission.ExternalStorageRead) ||
           !Permission.HasUserAuthorizedPermission(Permission.ExternalStorageWrite) ||
           !Permission.HasUserAuthorizedPermission(Permission.FineLocation) ||
           !Permission.HasUserAuthorizedPermission(Permission.CoarseLocation))
    {
      Permission.RequestUserPermissions(permissions);
    }
    recordStrm = new StreamWriter(Path.Combine(Application.persistentDataPath, string.Format("gps_reocrd_{0}.txt", reocrdIndex)));
    // 启动定位服务
    Input.location.Start(1f, 1f);
    capture = true;
    gpsCoroutine = StartCoroutine(UpdateLocation());
    FinishButton.GetComponent<UnityEngine.UI.Button>().interactable = true;
    MarkButton.GetComponent<UnityEngine.UI.Button>().interactable = true;
    StartButton.GetComponent<UnityEngine.UI.Button>().interactable = false;
    GPSTracker.ClearData();
    showMessage("定位服务启用。");
  }
  IEnumerator UpdateLocation()
  {
    while (Input.location.status == LocationServiceStatus.Initializing)
    {
      yield return new WaitForSeconds(1);
    }
    if (Input.location.status == LocationServiceStatus.Failed)
    {
      showMessage("无法确定设备位置。");
      yield break;
    }
    while (capture)
    {
      var latitude = Input.location.lastData.latitude;
      var longitude = Input.location.lastData.longitude;
      var altitude = Input.location.lastData.altitude;
      if (latitude != 0 || longitude != 0 || altitude != 0)
      {
        recordStrm.Write("纬度: " + latitude + " 经度: " + longitude + " 海拔: " + altitude + "\n");
        GPSTracker.AddData(latitude, longitude, altitude);
      }
      yield return new WaitForSeconds(updateInterval);
    }
  }
  public void FinishRecord()
  {
    recordStrm.Flush();
    recordStrm.Close();
    recordStrm = null;
    Input.location.Stop();
    capture = false;
    StopCoroutine(gpsCoroutine);
    reocrdIndex += 1;
    StartButton.GetComponent<UnityEngine.UI.Button>().interactable = true;
    FinishButton.GetComponent<UnityEngine.UI.Button>().interactable = false;
    MarkButton.GetComponent<UnityEngine.UI.Button>().interactable = false;
  }
  public void Mark()
  {
    if (null != recordStrm)
    {
      double latitude = Input.location.lastData.latitude;
      double longitude = Input.location.lastData.longitude;
      var altitude = Input.location.lastData.altitude;

      recordStrm.Write("纬度: " + latitude + " 经度: " + longitude + " 海拔: " + altitude + ":" + inputField.text + "\n");
    }
  }

}
