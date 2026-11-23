using System.IO;
using UnityEngine;

public class GPSDataLoader : MonoBehaviour
{
  StreamReader streamReader;
  int count = 0;
  public string FilePath;
  // Start is called once before the first execution of Update after the MonoBehaviour is created
  void Start()
    {
    streamReader = new StreamReader(FilePath);
  }

    // Update is called once per frame
    void Update()
    {
    if (++count < 10) {
      return;
    }
    count = 0;
    if (streamReader.EndOfStream)
    {
      return;
    }
    var line = streamReader.ReadLine();
    if (streamReader.EndOfStream)
    {
      Debug.Log("");
    }
    var lines = line.Split(' ');
    var latitude = float.Parse(lines[1]); // 纬度
    var longitude = float.Parse(lines[3]);// 经度
    var altitude = float.Parse(lines[5].Split(':')[0]); // 海拔
    GetComponent<GPSTracker>().AddData(latitude, longitude, altitude);
  }
}
