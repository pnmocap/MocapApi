using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;

[RequireComponent(typeof(CanvasRenderer))]
public class CurveUI : Graphic
{
  [Header("曲线控制点")]
  public List<Vector2> controlPoints = new List<Vector2>
    {
        new Vector2(0, 0),
        new Vector2(0.5f, 1),
        new Vector2(1, 0)
    };

  [Header("曲线参数")]
  [Range(3, 100)] public int segments = 30;  // 曲线分段数
  public float thickness = 5f;               // 曲线粗细
  public bool closePath = false;             // 是否闭合路径

  protected override void OnPopulateMesh(VertexHelper vh)
  {
    vh.Clear();
    if (controlPoints.Count < 2) return;

    // 生成曲线路径点
    List<Vector2> pathPoints = new List<Vector2>();
    for (int i = 0; i <= segments; i++)
    {
      float t = (float)i / segments;
      pathPoints.Add(CalculateBezierPoint(t));
    }

    // 生成曲线网格
    GenerateCurveMesh(vh, pathPoints);
  }

  // 计算贝塞尔曲线点（支持多阶）
  private Vector2 CalculateBezierPoint(float t)
  {
    List<Vector2> points = new List<Vector2>(controlPoints);
    while (points.Count > 1)
    {
      for (int i = 0; i < points.Count - 1; i++)
      {
        points[i] = Vector2.Lerp(points[i], points[i + 1], t);
      }
      points.RemoveAt(points.Count - 1);
    }
    return points[0];
  }

  // 生成曲线网格（带厚度）
  private void GenerateCurveMesh(VertexHelper vh, List<Vector2> path)
  {
    UIVertex vert = UIVertex.simpleVert;
    vert.color = color;

    // 遍历路径点生成两侧顶点
    for (int i = 0; i < path.Count; i++)
    {
      Vector2 dir = GetDirection(path, i);
      Vector2 normal = new Vector2(-dir.y, dir.x).normalized;

      // 计算左右顶点位置
      Vector2 leftPos = path[i] + normal * thickness * 0.5f;
      Vector2 rightPos = path[i] - normal * thickness * 0.5f;

      // 转换到 Panel 的局部坐标系
      leftPos = TransformPointToRect(leftPos);
      rightPos = TransformPointToRect(rightPos);

      // 添加顶点
      vert.position = leftPos;
      vh.AddVert(vert);
      vert.position = rightPos;
      vh.AddVert(vert);
    }

    // 生成三角形
    for (int i = 0; i < path.Count - 1; i++)
    {
      int start = i * 2;
      vh.AddTriangle(start, start + 1, start + 2);
      vh.AddTriangle(start + 1, start + 3, start + 2);
    }

    // 闭合路径
    if (closePath && path.Count > 1)
    {
      int last = (path.Count - 1) * 2;
      vh.AddTriangle(0, 1, last);
      vh.AddTriangle(1, last + 1, last);
    }
  }

  // 将标准化坐标 (0~1) 转换为 RectTransform 的局部坐标
  private Vector2 TransformPointToRect(Vector2 normalizedPoint)
  {
    Rect rect = rectTransform.rect;
    return new Vector2(
        Mathf.Lerp(rect.xMin, rect.xMax, normalizedPoint.x),
        Mathf.Lerp(rect.yMin, rect.yMax, normalizedPoint.y)
    );
  }

  // 计算路径点的切线方向
  private Vector2 GetDirection(List<Vector2> path, int index)
  {
    if (path.Count <= 1) return Vector2.right;
    if (index == 0) return (path[1] - path[0]).normalized;
    if (index == path.Count - 1) return (path[index] - path[index - 1]).normalized;
    return (path[index + 1] - path[index - 1]).normalized;
  }

  // 外部调用刷新曲线
  public void UpdateCurve()
  {
    SetVerticesDirty();
  }
}