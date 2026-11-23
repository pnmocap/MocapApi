using System;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class OutputPanel : PopupPanelBehaviour, AbstractReporter
{
  public TextMeshProUGUI Text;
  public ScrollRect ScrollRect;

  public void ReportError(string msg)
  {
    Text.text += msg;
    Text.text += "\n";
    Canvas.ForceUpdateCanvases();
    ScrollRect.verticalNormalizedPosition = 0f;
  }

  public void ReportInfo(string msg)
  {
    Text.text += msg;
    Text.text += "\n";
    Canvas.ForceUpdateCanvases();
    ScrollRect.verticalNormalizedPosition = 0f;
  }

  public void ReportWarning(string msg)
  {
    Text.text += msg;
    Text.text += "\n";
    Canvas.ForceUpdateCanvases();
    ScrollRect.verticalNormalizedPosition = 0f;
  }

  // Start is called once before the first execution of Update after the MonoBehaviour is created
  void Start()
  {

  }
}
