using MocapApi;
using UnityEngine;
using UnityEngine.UI;

public class Settings : MonoBehaviour
{
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    public TMPro.TMP_InputField UDPRemoteIP;
    public TMPro.TMP_InputField UDPRemotePort;
    public TMPro.TMP_InputField UDPLoacalIP;
    public TMPro.TMP_InputField UDPLoacalPort;
    public TMPro.TMP_Text RotationOrderText;
    public GameObject RotationOrderPanel;
    EMCPBvhRotation Rotation;
    public delegate void AppliedDelegate();
    AppliedDelegate _appliedDelegate;
    public Toggle WithPTP;
    public TMPro.TMP_InputField RecordTakeName;
    void Start()
    {
        load();

    }

    void load()
    {
        UDPLoacalIP.text = PlayerPrefs.GetString("UDP.Local.IP", "127.0.0.1");
        UDPLoacalPort.text = PlayerPrefs.GetInt("UDP.Local.Port", 8002).ToString();
        UDPRemoteIP.text = PlayerPrefs.GetString("UDP.Remote.IP", "127.0.0.1");
        UDPRemotePort.text = PlayerPrefs.GetInt("UDP.Remote.Port", 8080).ToString();
        WithPTP.isOn = (PlayerPrefs.GetInt("StartCapture.EnablePTP", 0) != 0);
        RecordTakeName.text = PlayerPrefs.GetString("Record.TakeName", "");
        Rotation = (EMCPBvhRotation)PlayerPrefs.GetInt("Bvh.Rotation.Order", (int)(EMCPBvhRotation.BvhRotation_YZX));
        updateRotationOrder();
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetMouseButtonDown(0) || Input.GetMouseButtonDown(1) || Input.GetMouseButtonDown(2))
        {
            if (RectTransformUtility.RectangleContainsScreenPoint(
              RotationOrderPanel.GetComponent<RectTransform>(),
              Input.mousePosition
          ) && RotationOrderPanel.activeSelf)
            {
                return;
            }
            RotationOrderPanel.SetActive(false);
            if (gameObject.activeSelf && !RectTransformUtility.RectangleContainsScreenPoint((RectTransform)transform, Input.mousePosition))
            {
                load();
                gameObject.SetActive(false);
            }
        }
    }

    public void setAppliedDelegate(AppliedDelegate appliedDelegate)
    {
        _appliedDelegate = appliedDelegate;
    }

    public void okButtonHandle()
    {
        int localPort, remotePort;
        string error;
        if (!int.TryParse(UDPLoacalPort.text.Trim(), out localPort) ||
            !int.TryParse(UDPRemotePort.text.Trim(), out remotePort))
        {
            GetComponentInParent<MainPanel>().ReportError("UDP 端口必须是 1 到 65535 的整数。");
            return;
        }
        if (!MocapConnectionSettings.Validate(UDPLoacalIP.text.Trim(), localPort,
            UDPRemoteIP.text.Trim(), remotePort, out error))
        {
            GetComponentInParent<MainPanel>().ReportError(error);
            return;
        }
        PlayerPrefs.SetString("UDP.Local.IP", UDPLoacalIP.text.Trim());
        PlayerPrefs.SetInt("UDP.Local.Port", localPort);
        PlayerPrefs.SetString("UDP.Remote.IP", UDPRemoteIP.text.Trim());
        PlayerPrefs.SetInt("UDP.Remote.Port", remotePort);
        PlayerPrefs.SetInt("Bvh.Rotation.Order", (int)Rotation);
        PlayerPrefs.SetInt("StartCapture.EnablePTP", WithPTP.isOn ? 1 : 0);
        PlayerPrefs.SetString("Record.TakeName", RecordTakeName.text.Trim());
        PlayerPrefs.Save();
        gameObject.SetActive(false);
        _appliedDelegate();
    }
    public void rotationOrderButtonHandle()
    {
        if (RotationOrderPanel.activeSelf)
        {
            RotationOrderPanel.SetActive(false);
        }
        else
        {
            RotationOrderPanel.SetActive(true);
        }
    }
    public void xyzButtonHandle()
    {
        Rotation = EMCPBvhRotation.BvhRotation_XYZ;
        updateRotationOrder();
        RotationOrderPanel.SetActive(false);
    }
    public void xzyButtonHandle()
    {
        Rotation = EMCPBvhRotation.BvhRotation_XZY;
        updateRotationOrder();
        RotationOrderPanel.SetActive(false);
    }
    public void yxzButtonHandle()
    {
        Rotation = EMCPBvhRotation.BvhRotation_YXZ;
        updateRotationOrder();
        RotationOrderPanel.SetActive(false);
    }
    public void yzxButtonHandle()
    {
        Rotation = EMCPBvhRotation.BvhRotation_YZX;
        updateRotationOrder();
        RotationOrderPanel.SetActive(false);
    }
    public void zxyButtonHandle()
    {
        Rotation = EMCPBvhRotation.BvhRotation_ZXY;
        updateRotationOrder();
        RotationOrderPanel.SetActive(false);
    }
    public void zyxButtonHandle()
    {
        Rotation = EMCPBvhRotation.BvhRotation_ZYX;
        updateRotationOrder();
        RotationOrderPanel.SetActive(false);
    }
    void updateRotationOrder()
    {
        switch (Rotation)
        {
            case EMCPBvhRotation.BvhRotation_XYZ:
                RotationOrderText.SetText("XYZ");
                break;
            case EMCPBvhRotation.BvhRotation_XZY:
                RotationOrderText.SetText("XZY");
                break;
            case EMCPBvhRotation.BvhRotation_YXZ:
                RotationOrderText.SetText("YXZ");
                break;
            case EMCPBvhRotation.BvhRotation_YZX:
                RotationOrderText.SetText("YZX");
                break;
            case EMCPBvhRotation.BvhRotation_ZXY:
                RotationOrderText.SetText("ZXY");
                break;
            case EMCPBvhRotation.BvhRotation_ZYX:
                RotationOrderText.SetText("ZYX");
                break;
            default:
                break;
        }
    }
}
