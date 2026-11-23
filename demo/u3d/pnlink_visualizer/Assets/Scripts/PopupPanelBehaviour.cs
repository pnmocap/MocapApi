using UnityEngine;

public class PopupPanelBehaviour : MonoBehaviour
{
  // Start is called once before the first execution of Update after the MonoBehaviour is created
  void Start()
  {

  }

  public delegate void CloseDelegate();
  public CloseDelegate _closeDelegate;
  public delegate bool MaybeCloseDelegate();
  public MaybeCloseDelegate _maybeCloseDelegate;

  public bool IsAnyButtonDown()
  {
    return Input.GetMouseButtonDown(0) || Input.GetMouseButtonDown(1) || Input.GetMouseButtonDown(2);
  }

  public bool MouseInSelfRect()
  {
    return RectTransformUtility.RectangleContainsScreenPoint((RectTransform)transform, Input.mousePosition);
  }
  public void Close()
  {
    gameObject.SetActive(false);
    if (null != _closeDelegate)
    {
      _closeDelegate();
    }
  }
  public void DefaultClickHandle()
  {
    if (IsAnyButtonDown())
    {
      if (null != _maybeCloseDelegate)
      {
        if (!_maybeCloseDelegate())
        {
          return;
        }
      }
      if (gameObject.activeSelf && !MouseInSelfRect())
      {
        Close();
      }
    }
  }

  // Update is called once per frame
  void Update()
  {
    DefaultClickHandle();
  }
}
