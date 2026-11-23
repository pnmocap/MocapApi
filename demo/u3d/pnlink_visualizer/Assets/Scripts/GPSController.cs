using System.Collections;
using System.IO;
using System.Text.RegularExpressions;
using UnityEngine;
using UnityEngine.Android;

public class GPSController : MonoBehaviour
{
  public GPSTracker GPSTracker;
  bool capture = true;
  Coroutine gpsCoroutine;
  StreamWriter recordStrm;
  float updateInterval = 0.1f; // 更新间隔时间（秒）
  uint reocrdIndex = 0;
  public MainPanel mainPanel;
  // Start is called once before the first execution of Update after the MonoBehaviour is created
  void Start()
    {
        
    }
  private string[] permissions = new string[] {
        "android.permission.READ_EXTERNAL_STORAGE",
        "android.permission.WRITE_EXTERNAL_STORAGE",
        "android.permission.ACCESS_FINE_LOCATION",
        "android.permission.ACCESS_COARSE_LOCATION",
    };
  public void StartUpdateGPS() {
    // 检查是否支持定位服务
    if (!Input.location.isEnabledByUser)
    {
      mainPanel.ReportError("定位服务未启用，请在设置中开启。");
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
    GPSTracker.ClearData();
    mainPanel.ReportInfo("定位服务启用。");
  }
  public void StopUpdateGPS() {
    if (null != recordStrm) {
      recordStrm.Flush();
      recordStrm.Close();
      recordStrm = null;
    }
    
    Input.location.Stop();
    capture = false;
    if (null != gpsCoroutine) {
      StopCoroutine(gpsCoroutine);
    }
    reocrdIndex += 1;
  }

    // Update is called once per frame
    void Update()
    {
        
    }

  IEnumerator UpdateLocation()
  {
    while (Input.location.status == LocationServiceStatus.Initializing)
    {
      yield return new WaitForSeconds(1);
    }
    if (Input.location.status == LocationServiceStatus.Failed)
    {
      mainPanel.ReportError("无法确定设备位置。");
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
}
