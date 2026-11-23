using UnityEngine;
using MocapApi;
using TMPro;
public class MocapApiVersion : MonoBehaviour
{
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
    var tmp0 = gameObject.GetComponent<TextMeshProUGUI>();
    var tmp1 = GetComponent<TextMeshProUGUI>().text = string.Format("MocapApi Version:{0}", Version.libMocapApiString);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
