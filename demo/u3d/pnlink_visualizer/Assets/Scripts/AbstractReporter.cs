using UnityEngine;

public interface AbstractReporter
{
  public static Color ErrorColor = new Color(1.0f, 0.0f, 0.0f, 1.0f);
  public static Color WarningColor = new Color(0.0f, 1.0f, 1.0f, 0.5f);
  public static Color InfoColor = new Color(1.0f, 1.0f, 1.0f, 0.5f);
  public abstract void ReportError(string msg);
  public abstract void ReportWarning(string msg);

  public abstract void ReportInfo(string msg);
}
