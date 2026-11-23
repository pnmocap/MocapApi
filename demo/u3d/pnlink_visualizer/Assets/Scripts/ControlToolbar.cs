using UnityEngine;

public class ControlToolbar : MonoBehaviour
{
  public GameObject StatusColor;
  // Start is called once before the first execution of Update after the MonoBehaviour is created
  void Start()
  {
    StatusColor.GetComponent<StatusColor>().SetColor(Color.red);
  }

  // Update is called once per frame
  void Update()
  {

  }

  public void SetOnline()
  {
    StatusColor.GetComponent<StatusColor>().SetColor(Color.green);
  }
}
