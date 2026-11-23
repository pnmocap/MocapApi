using UnityEngine;
using UnityEngine.UI;

public class StatusColor : MonoBehaviour
{
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
  public void SetColor(Color clr) {
    Image img = GetComponent<Image>();
    img.material.SetColor("_Color", clr);
  }
}
