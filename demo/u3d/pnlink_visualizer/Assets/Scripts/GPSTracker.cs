using JetBrains.Annotations;
using System;
using System.Collections.Generic;
using System.Linq;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.UIElements;


[RequireComponent(typeof(CanvasRenderer))]
public class GPSTracker: Graphic
{
  // 地球平均半径（米）
  private const double EarthRadius = 6378137.0;
  [Header("线宽")]
  public float thickness = 0.01f;
  public static void ConvertToCartesian(double latitude, double longitude, double altitude, out double x, out double y, out double z)
  {
    double latRad = latitude * Math.PI / 180; // 将纬度转换为弧度
    double lonRad = longitude * Math.PI / 180; // 将经度转换为弧度
    double a = 6378137; // 地球椭球长半轴
    double f = 1 / 298.257223563; // 地球椭球扁率
    double e2 = 2 * f - Math.Pow(f, 2); // 第一偏心率的平方
    double n = a / Math.Sqrt(1 - e2 * Math.Pow(Math.Sin(latRad), 2)); // 卯酉圈曲率半径

    x = (n + altitude) * Math.Cos(latRad) * Math.Cos(lonRad);
    y = (n + altitude) * Math.Cos(latRad) * Math.Sin(lonRad);
    z = (n * (1 - e2) + altitude) * Math.Sin(latRad);
  }

  // 墨卡托投影公式
  private static double MercatorProjection(double latitudeRad)
  {
    return Math.Log(Math.Tan(Math.PI / 4 + latitudeRad / 2));
  }

  List<Vector3> gpsDatas = new List<Vector3>();
  public void AddData(float latitude, float longitude, float altitude) {
    gpsDatas.Add(new Vector3(latitude, longitude, altitude));
    SetVerticesDirty();
  }
  public void ClearData() {
    gpsDatas.Clear();
    SetVerticesDirty();
  }
  Vector3[] boundDatas(Vector3[] gpsDatas, float minLatitude, float minLongitude, float minAltitude,
      float maxLatitude, float maxLongitude, float maxAltitude) {
    // 计算各维度缩放系数
    const float TARGET_RANGE = 0.9f; // 目标区间总跨度
    float scaleX = TARGET_RANGE / (maxLatitude - minLatitude);
    float scaleY = TARGET_RANGE / (maxLongitude - minLongitude);
    float scaleZ = TARGET_RANGE / (maxAltitude - minAltitude);

    Vector3[] result = new Vector3[gpsDatas.Length];

    // 遍历处理每个坐标点
    for (int i = 0; i < gpsDatas.Length; i++)
    {
      float normalizedX = (gpsDatas[i].x - minLatitude) * scaleX + 0.05f;
      if (minLatitude == maxLatitude)
      {
        normalizedX = 0.5f;
      }
      float normalizedY = (gpsDatas[i].y - minLongitude) * scaleY + 0.05f;
      if (maxLongitude == minLongitude) {
        normalizedY = 0.5f;
      }
      float normalizedZ = (gpsDatas[i].z - minAltitude) * scaleZ + 0.05f;

      result[i] = new Vector3(normalizedX, normalizedY, normalizedZ);
    }

    return result;
  }
  Vector3[] mapGPSDatas() {
    float minLatitude = float.MaxValue, minLongitude = float.MaxValue, minAltitude = float.MaxValue,
      maxLatitude = float.MinValue, maxLongitude = float.MinValue, maxAltitude = float.MinValue;
    var datas = new Vector3[gpsDatas.Count];
    for (int i=0; i < gpsDatas.Count; ++i)
    {
      double x, y, z;
      ConvertToCartesian((double)gpsDatas[i].x, (double)gpsDatas[i].y, (double)gpsDatas[i].z, out x, out y, out z);
      datas[i].x = (float)x;
      datas[i].y = (float)y;
      datas[i].z = (float)z;

      if (x < minLatitude)
      {
        minLatitude = (float)x;
      }
      else if (x > maxLatitude)
      {
        maxLatitude = (float)x;
      }
      if (y < minLongitude)
      {
        minLongitude = (float)y;
      }
      else if (y > maxLongitude)
      {
        maxLongitude = (float)y;
      }
      if (z < minAltitude)
      {
        minAltitude = (float)z;
      }
      else if (z > maxAltitude)
      {
        maxAltitude = (float)z;
      }
    }
    return boundDatas(datas, minLatitude, minLongitude, minAltitude,
      maxLatitude, maxLongitude, maxAltitude);
  }

  private Vector3 GetDirection(Vector3[] datas, int index) {
    if (datas.Length <= 1) {
      return Vector3.right;
    }
    if (index == 0) { 
      return (datas[1] - datas[0]).normalized;
    }
    if (index == datas.Length - 1) { 
      return (datas[index] - datas[index - 1]).normalized;
    }
    return (datas[index + 1] - datas[index-1]).normalized;
  }

  private Vector2 TransformPointToRect(Vector3 normalizedPoint) { 
    Rect rect = rectTransform.rect;
    return new Vector2(Mathf.Lerp(rect.xMin, rect.xMax, normalizedPoint.x), 
      Mathf.Lerp(rect.yMin, rect.yMax, normalizedPoint.y));
  }

  protected override void OnPopulateMesh(VertexHelper vh)
  {
    vh.Clear();
    if (gpsDatas.Count < 2)
    {
      return;
    }
    var datas = mapGPSDatas();
    UIVertex vert = UIVertex.simpleVert;
    vert.color = color;
    for (int i = 0; i < datas.Length; ++i)
    {
      var direct = GetDirection(datas, i);
      var normal = new Vector3(-direct.y, direct.x).normalized;

      var leftPos = datas[i] + normal * thickness * 0.5f;
      var rightPos = datas[i] - normal * thickness * 0.5f;
      vert.position = TransformPointToRect(leftPos);
      Debug.LogErrorFormat("vert : {0}", vert.position);
      vh.AddVert(vert);
      vert.position = TransformPointToRect(rightPos);
      Debug.LogErrorFormat("vert : {0}", vert.position);
      vh.AddVert(vert);
    }
    for (int i = 0; i < datas.Length - 1; ++i)
    {
      int start = i * 2;
      vh.AddTriangle(start, start + 1, start + 2);
      vh.AddTriangle(start + 1, start + 3, start + 2);
    }
  }
}
