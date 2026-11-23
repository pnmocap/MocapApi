using JetBrains.Annotations;
using UnityEngine;

public class NormalCommandProcess : AbstractCommandProcess 
{

  AbstractReporter _reporter;
  public NormalCommandProcess(ulong command, string title, AbstractReporter reporter) : base(command, title) { _reporter = reporter; }

  public override void handleRunning() { }

  public override void handleResult() {
    uint retCode = 0;
    MocapApi.IMCPCommand.Command.GetCommandResultCode(ref retCode, command);
    if (retCode != 0)
    {
      string msg = null;
      MocapApi.IMCPCommand.Command.GetCommandResultMessage(ref msg, command);
      _reporter.ReportError($"{title} error: \'{msg}\'!");
    }
    else {
      _reporter.ReportInfo($"{title} done.");
    }
    DestroyCommand();
  }


  public override bool isComplete() { return command ==0; }

}
