using UnityEngine;

public class DynamicLineDrawer : MonoBehaviour
{
  private Mesh _mesh;
  private Vector3[] _vertices;
  private int[] _indices;
  private Material _material;
  // Start is called once before the first execution of Update after the MonoBehaviour is created
  void Start()
    {
    // 初始化 Mesh
    _mesh = new Mesh();
    _vertices = new Vector3[2]; // 初始两个顶点（一条线段）
    _indices = new int[] { 0, 1 }; // 线段连接顺序

    _mesh.vertices = _vertices;
    _mesh.SetIndices(_indices, MeshTopology.Lines, 0);

    // 创建材质并指定 Shader
    _material = new Material(Shader.Find("Unlit/UnlitLineShader"));
    _material.color = Color.red;
  }

    // Update is called once per frame
    void Update()
    {
    // 动态更新顶点数据（例如根据实时输入）
    _vertices[0] = new Vector3(100, -200, 10);//transform.position;
    _vertices[1] = new Vector3(100, 200, 10);//transform.position + Vector3.up * 200f;

    _mesh.vertices = _vertices;
    _mesh.RecalculateBounds();

    // 提交绘制命令
    Graphics.DrawMesh(_mesh, Matrix4x4.identity, _material, 0);
  }
}
