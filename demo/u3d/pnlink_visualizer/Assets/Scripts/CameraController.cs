using System.Collections.Generic;
using UnityEngine;
using static UnityEngine.Rendering.HDROutputUtils;

public class CameraController : MonoBehaviour
{
  // Start is called once before the first execution of Update after the MonoBehaviour is created
  public GameObject focusObject = null;
  enum EOperation{ 
    None,
    Rotate,
    Transpose,
  };
  EOperation operation = EOperation.None;
  Vector3 lastMousePosition;
  public RectTransform[] GUITransforms;
  void Start()
  {

  }

  bool inWindow(Vector3 mousePosition){
    return mousePosition.x >= 0 && mousePosition.x <= Screen.width &&
               mousePosition.y >= 0 && mousePosition.y <= Screen.height;
  }

  bool canHandleMouseEvent() {
    if (UnityEngine.EventSystems.EventSystem.current != null && UnityEngine.EventSystems.EventSystem.current.IsPointerOverGameObject()) return false;
    var pos = Input.mousePosition;
    if (!inWindow(pos)) {
      return false;
    }
    for (int i = 0; i < GUITransforms.Length; i++)
    {
      if (!GUITransforms[i].gameObject.activeSelf) {
        continue;
      }
      if (RectTransformUtility.RectangleContainsScreenPoint(GUITransforms[i], pos)) {
        return false;
      }
    }
    return true;
  }
  // Update is called once per frame
  void Update()
  {


    var scroll = Input.GetAxis("Mouse ScrollWheel");
    if (scroll != 0 && canHandleMouseEvent()) {
      var dir = transform.position - focusObject.transform.position;
      transform.position = focusObject.transform.position + dir.normalized * (dir.magnitude + scroll);
    }
    

    if (Input.GetMouseButtonDown(0))
    {
      if (canHandleMouseEvent()) {
        operation = EOperation.Transpose;
        lastMousePosition = Input.mousePosition;
      }
    }
    else if (Input.GetMouseButtonDown(1))
    {
      if (canHandleMouseEvent()) {
        operation = EOperation.Rotate;
        lastMousePosition = Input.mousePosition;
      }
    }
    else if (Input.GetMouseButton(0))
    {
      if (operation == EOperation.Transpose) {
        var t = Input.mousePosition - lastMousePosition;
        transform.position -= (transform.right * t.x / 100.0f + transform.up * t.y / 100.0f);
        lastMousePosition = Input.mousePosition;
      }
    }
    else if (Input.GetMouseButton(1))
    {
      if (operation == EOperation.Rotate) {
        var r = Input.mousePosition - lastMousePosition;
        transform.RotateAround(focusObject.transform.position, Vector3.up, r.x/3.0f);
        transform.RotateAround(focusObject.transform.position, transform.right, r.y/5.0f);
        lastMousePosition = Input.mousePosition;
      }
    }
    else if (Input.GetMouseButtonUp(0) || Input.GetMouseButtonUp(1)) {
      operation = EOperation.None;
    }

    //

  }
}
