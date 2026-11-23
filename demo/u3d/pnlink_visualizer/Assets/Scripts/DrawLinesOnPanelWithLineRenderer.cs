using System.IO;
using UnityEngine;

public class DrawLinesOnPanelWithLineRenderer : MonoBehaviour
{
  StreamReader streamReader;
  private Material lineMaterial;
  // Start is called once before the first execution of Update after the MonoBehaviour is created
  void Start()
  {
    lineMaterial = new Material(Shader.Find("Hidden/Internal-Colored"));
    lineMaterial.hideFlags = HideFlags.HideAndDontSave;
    lineMaterial.SetInt("_SrcBlend", (int)UnityEngine.Rendering.BlendMode.SrcAlpha);
    lineMaterial.SetInt("_DstBlend", (int)UnityEngine.Rendering.BlendMode.OneMinusSrcAlpha);
    lineMaterial.SetInt("_Cull", (int)UnityEngine.Rendering.CullMode.Off);
    lineMaterial.SetInt("_ZWrite", 0);

    string sample = Path.GetFullPath(Path.Combine(Application.dataPath, "../test_data/gps_reocrd_0.txt"));
    if (!File.Exists(sample))
    {
      Debug.LogWarning("Optional GPS fixture not found. The GPS sample is disabled: " + sample);
      enabled = false;
      return;
    }
    streamReader = new StreamReader(sample);
  }

  // Update is called once per frame
  void Update()
  {
    if (streamReader == null || streamReader.EndOfStream)
    {
      return;
    }
    var line = streamReader.ReadLine();
    var lines = line.Split(' ');
    var latitude = float.Parse(lines[1]); // 纬度
    var longitude = float.Parse(lines[3]);// 经度
    var altitude = float.Parse(lines[5].Split(':')[0]); // 海拔
    Debug.Log(string.Format("纬度: {0} 经度: {1} 海拔: {2}", lines[1], lines[3], lines[5]));
  }
  void OnDestroy() { if (streamReader != null) streamReader.Dispose(); }
  void OnGUI()
  {
    lineMaterial.SetPass(0);
    GL.LoadOrtho();
    GL.Begin(GL.LINES);
    GL.Color(Color.blue);
    GL.Vertex3(0, 0, 0);
    GL.Color(Color.red);
    GL.Vertex3(1, 1, 0);
    GL.End();
  }
}
