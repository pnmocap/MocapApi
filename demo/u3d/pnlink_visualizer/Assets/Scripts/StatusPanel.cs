using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class StatusPanel : MonoBehaviour, AbstractReporter
{
  public TextMeshProUGUI Text;
  public void ReportError(string msg)
  {
    Text.SetText(msg);
    GetComponent<Image>().color = AbstractReporter.ErrorColor;
  }

  public void ReportInfo(string msg)
  {
    Text.SetText(msg);
    GetComponent<Image>().color = AbstractReporter.InfoColor;
  }

  public void ReportWarning(string msg)
  {
    Text.SetText(msg);
    GetComponent<Image>().color = AbstractReporter.WarningColor;
  }

  // Start is called once before the first execution of Update after the MonoBehaviour is created
  void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
